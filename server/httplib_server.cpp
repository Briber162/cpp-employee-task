#include <iostream>
#include <string>
#include "employee_api.h"
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "api_formatter.h"
#include "logger.h"
#include "timer.h"

using json = nlohmann::json;

// Convert Employee to JSON
json employeeToJson(const Employee& emp) {
    json j;
    j["id"] = emp.getId();
    j["employee_name"] = emp.getName();
    j["employee_salary"] = emp.getSalary();
    j["employee_age"] = emp.getAge();
    j["employee_title"] = emp.getTitle();
    j["employee_email"] = emp.getEmail();
    return j;
}

int main() {
    // Initialize logger for the server component
    Logger::initForComponent(LogComponent::SERVER);
    Logger::info("Initializing Employee Management HTTP Server");
    
    // Initialize Employee API
    EmployeeAPI api("common/employees.json");
    
    // Create a server instance
    httplib::Server svr;
    
    Logger::info("Setting up server routes and CORS");
    
    // Configure CORS headers
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
    });
    
    // Handle OPTIONS requests for CORS preflight
    svr.Options(".*", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204; // No content
    });

    // GET /api/employees - Get all employees
    svr.Get("/api/employees", [&api](const httplib::Request& req, httplib::Response& res) {
        (void) req;
        Logger::debug("GET /api/employees - Request for all employees");
        
        auto employees = api.getAllEmployees();
        Logger::info("GET /api/employees - Returning {} employees", employees.size());
        
        std::string responseStr = ApiFormatter::formatEmployeeListResponse(employees);
        
        res.set_content(responseStr, "application/json");
    });
    
    // Note: GET /api/employees/id/{id} endpoint is now defined at the bottom of the file using ApiFormatter
    
    // GET /api/employees/search/name/{name} - Get employees by name
    svr.Get(R"(/api/employees/search/name/(.+))", [&api](const httplib::Request& req, httplib::Response& res) {
        auto name = req.matches[1];
        Logger::debug("GET /api/employees/search/name/{} - Searching employees by name", name.str());
        
        // Start timing the operation
        Timer timer("searchEmployeesByName", LogComponent::SERVER);
        
        // Execute the search
        auto employees = api.getEmployeesByName(name.str());
        
        // Format the response
        std::string responseStr = ApiFormatter::formatEmployeeListResponse(employees);
        
        // Timer will automatically log the duration when it goes out of scope
        
        if (!employees.empty()) {
            res.set_content(responseStr, "application/json");
            Logger::info("GET /api/employees/search/name/{} - Found {} matching employees", name.str(), employees.size());
        } else {
            json response;
            response["data"] = json::array();
            response["status"] = "No employees found with the given name";
            res.status = 404;
            res.set_content(response.dump(4), "application/json");
        }
    });
    
    // POST /api/employees - Add new employee
    svr.Post("/api/employees", [&api](const httplib::Request& req, httplib::Response& res) {
        Logger::debug("POST /api/employees - Request to add a new employee");
        
        try {
            auto bodyJson = json::parse(req.body);
            Logger::debug("POST /api/employees - Successfully parsed request body");
            
            // Ensure all required fields are present
            if (!bodyJson.contains("employee_name") || !bodyJson.contains("employee_salary") ||
                !bodyJson.contains("employee_age") || !bodyJson.contains("employee_title") ||
                !bodyJson.contains("employee_email")) {
                Logger::warn("POST /api/employees - Request missing required fields");
                json error;
                error["status"] = "error";
                error["message"] = "Missing required fields";
                res.status = 400;
                res.set_content(error.dump(), "application/json");
                return;
            }
            
            // Create a new employee
            Employee emp;
            
            // Auto-generate ID by finding max ID and incrementing
            Logger::debug("POST /api/employees - Auto-generating employee ID");
            auto employees = api.getAllEmployees();
            int maxId = 0;
            for (const auto& e : employees) {
                try {
                    int id = std::stoi(e.getId());
                    if (id > maxId) maxId = id;
                } catch (const std::exception& ex) {
                    Logger::debug("POST /api/employees - Skipping non-numeric ID: {}", e.getId());
                    // Skip non-numeric IDs
                }
            }
            
            Logger::debug("POST /api/employees - Generated new ID: {}", maxId + 1);
            
            emp.setId(std::to_string(maxId + 1));
            emp.setName(bodyJson["employee_name"].get<std::string>());
            emp.setSalary(bodyJson["employee_salary"].is_string() ? 
                bodyJson["employee_salary"].get<std::string>() : 
                std::to_string(bodyJson["employee_salary"].get<int>()));
            emp.setAge(bodyJson["employee_age"].is_string() ? 
                bodyJson["employee_age"].get<std::string>() : 
                std::to_string(bodyJson["employee_age"].get<int>()));
            emp.setTitle(bodyJson["employee_title"].get<std::string>());
            emp.setEmail(bodyJson["employee_email"].get<std::string>());
            
            // Add employee
            Logger::info("POST /api/employees - Adding new employee: Name={}, Title={}", 
                          emp.getName(), emp.getTitle());
                          
            if (api.addEmployee(emp)) {
                Logger::info("POST /api/employees - Successfully added employee with ID: {}", emp.getId());
                std::string responseStr = ApiFormatter::formatSingleEmployeeResponse(
                    emp, "Employee added successfully");
                
                res.status = 201; // Created
                res.set_content(responseStr, "application/json");
            } else {
                Logger::error("POST /api/employees - Failed to add employee with ID: {}", emp.getId());
                json error;
                error["status"] = "error";
                error["message"] = "Failed to add employee";
                res.status = 500;
                res.set_content(error.dump(), "application/json");
            }
        } catch (const std::exception& e) {
            Logger::error("POST /api/employees - Exception while processing request: {}", e.what());
            json error;
            error["status"] = "error";
            error["message"] = std::string("Error: ") + e.what();
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });
    
    // PUT /api/employees/{id} - Update employee
    svr.Put(R"(/api/employees/update/([^/]+))", [&api](const httplib::Request& req, httplib::Response& res) {
        auto id = req.matches[1];
        
        // Check if employee exists
        Employee* emp = api.getEmployeeById(id);
        if (emp == nullptr) {
            json error;
            error["status"] = "error";
            error["message"] = "Employee not found";
            res.status = 404;
            res.set_content(error.dump(), "application/json");
            return;
        }
        
        try {
            auto bodyJson = json::parse(req.body);
            
            // Update only the fields that are provided
            if (bodyJson.contains("employee_name")) {
                emp->setName(bodyJson["employee_name"].get<std::string>());
            }
            
            if (bodyJson.contains("employee_salary")) {
                emp->setSalary(bodyJson["employee_salary"].is_string() ? 
                    bodyJson["employee_salary"].get<std::string>() : 
                    std::to_string(bodyJson["employee_salary"].get<int>()));
            }
            
            if (bodyJson.contains("employee_age")) {
                emp->setAge(bodyJson["employee_age"].is_string() ? 
                    bodyJson["employee_age"].get<std::string>() : 
                    std::to_string(bodyJson["employee_age"].get<int>()));
            }
            
            if (bodyJson.contains("employee_title")) {
                emp->setTitle(bodyJson["employee_title"].get<std::string>());
            }
            
            if (bodyJson.contains("employee_email")) {
                emp->setEmail(bodyJson["employee_email"].get<std::string>());
            }
            
            // Update employee
            if (api.updateEmployee(*emp)) {
                std::string responseStr = ApiFormatter::formatSingleEmployeeResponse(
                    *emp, "Employee updated successfully");
                
                res.set_content(responseStr, "application/json");
            } else {
                json error;
                error["status"] = "error";
                error["message"] = "Failed to update employee";
                res.status = 500;
                res.set_content(error.dump(), "application/json");
            }
        } catch (const std::exception& e) {
            json error;
            error["status"] = "error";
            error["message"] = std::string("Error: ") + e.what();
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });
    
    // DELETE /api/employees/{id} - Delete employee
    svr.Delete(R"(/api/employees/id/([^/]+))", [&api](const httplib::Request& req, httplib::Response& res) {
        auto id = req.matches[1];
        
        // Check if employee exists
        Employee* emp = api.getEmployeeById(id);
        if (emp == nullptr) {
            json error;
            error["status"] = "error";
            error["message"] = "Employee not found";
            res.status = 404;
            res.set_content(error.dump(), "application/json");
            return;
        }
        
        // Delete employee
        if (api.deleteEmployee(id)) {
            json data = {{
                "message", "Employee deleted successfully"
            }};
            std::string responseStr = ApiFormatter::formatApiResponse(data, "Success");
            res.set_content(responseStr, "application/json");
        } else {
            json error;
            error["status"] = "error";
            error["message"] = "Failed to delete employee";
            res.status = 500;
            res.set_content(error.dump(), "application/json");
        }
    });
    
    // GET /api/employees/highestSalary - Get highest salary amongst all employees
    svr.Get("/api/employees/highestSalary", [&api](const httplib::Request& req, httplib::Response& res) {
        (void) req;
        Logger::debug("GET /api/employees/highestSalary - Request for highest salary");
        
        // Start timing the operation
        Timer timer("getHighestSalaryOfEmployees", LogComponent::SERVER);
        
        // Execute the API call
        int highestSalary = api.getHighestSalaryOfEmployees();
        
        // Format the response
        json data = highestSalary;
        std::string responseStr = ApiFormatter::formatApiResponse(data, "Successfully retrieved highest salary");
        
        // Timer will automatically log the duration when it goes out of scope
        
        res.set_content(responseStr, "application/json");
        Logger::info("GET /api/employees/highestSalary - Returned highest salary: {}", highestSalary);
    });
    
    // GET /api/employees/topEarners - Get top 10 highest earning employees
    svr.Get("/api/employees/topEarners", [&api](const httplib::Request& req, httplib::Response& res) {
        (void) req;
        Logger::debug("GET /api/employees/topEarners - Request for top earning employees");
        
        // Start timing the operation
        Timer timer("getTop10HighestEarningEmployees", LogComponent::SERVER);
        
        // Execute the API call
        auto topEmployees = api.getTop10HighestEarningEmployees();
        
        // Format the response
        std::string responseStr = ApiFormatter::formatEmployeeListResponse(topEmployees, "Successfully retrieved top earners");
        
        // Timer will automatically log the duration when it goes out of scope
        
        res.set_content(responseStr, "application/json");
        Logger::info("GET /api/employees/topEarners - Returned {} top earning employees", topEmployees.size());
    });

    // GET /api/employees/getEmployeesbyTitle - Get employees by title
    svr.Get(R"(/api/employees/getEmployeesbyTitle/([^/]+))", [&api](const httplib::Request& req, httplib::Response& res) {
        auto title = req.matches[1];
        Logger::debug("GET /api/employees/getEmployeesbyTitle/{} - Searching employees by title", title.str());
        
        // Start timing the operation
        Timer timer("searchEmployeesByTitle", LogComponent::SERVER);
        
        // Execute the search
        auto employeesList = api.getEmployeesByTitle(title.str());
        
        // Timer will automatically log the duration when it goes out of scope
        
        if (!employeesList.empty()) {
            Logger::info("GET /api/employees/getEmployeesbyTitle/{} - Found {} matching employees", 
                       title.str(), employeesList.size());
            std::string responseStr = ApiFormatter::formatEmployeeListResponse(employeesList, 
                "Successfully retrieved employees by title");
            res.set_content(responseStr, "application/json");
        } else {
            Logger::info("GET /api/employees/getEmployeesbyTitle/{} - No employees found", title.str());
            json response;
            response["data"] = json::array();
            response["status"] = "No employees found with the given title";
            res.set_content(response.dump(4), "application/json");
        }
    });
    
    // GET /api/employees/{id} - Get employee by ID
    svr.Get(R"(/api/employees/id/(.+))", [&api](const httplib::Request& req, httplib::Response& res) {
        auto id = req.matches[1];
        Logger::debug("GET /api/employees/id/{} - Request for employee by ID", id.str());
        
        auto employee = api.getEmployeeById(id.str());
        
        if (employee) {
            Logger::info("GET /api/employees/id/{} - Employee found: {}", id.str(), employee->getName());
            std::string responseStr = ApiFormatter::formatSingleEmployeeResponse(*employee);
            res.set_content(responseStr, "application/json");
        } else {
            Logger::warn("GET /api/employees/id/{} - Employee not found", id.str());
            json response;
            response["data"] = json::object();
            response["status"] = "Error: Employee not found";
            res.set_content(response.dump(4), "application/json");
        }
    });
    
    // Display API information and log it
    Logger::info("Employee API Server initialized");
    
    std::cout << "Employee API Server\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Available endpoints:\n";
    std::cout << "GET    /api/employees                   - Get all employees\n";
    std::cout << "GET    /api/employees/id/{id}            - Get employee by ID\n";
    std::cout << "GET    /api/employees/search/name/{name} - Get employees by name\n";
    std::cout << "GET    /api/employees/highestSalary     - Get highest salary amongst all employees\n";
    std::cout << "GET    /api/employees/topEarners        - Get top 10 highest earning employees\n";
    std::cout << "POST   /api/employees                   - Add new employee\n";
    std::cout << "PUT    /api/employees/update/{id}       - Update employee\n";
    std::cout << "DELETE /api/employees/id/{id}           - Delete employee\n";
    std::cout << "GET    /api/employees/getEmployeesbyTitle/{title} - Get employees by title\n";
    std::cout << "----------------------------------------\n";
    
    Logger::info("Starting server on port 8112");

    // Start the server on port 8112
    if (!svr.listen("0.0.0.0", 8112)) {
        Logger::error("Failed to start server on port 8112");
        std::cerr << "Failed to start server" << std::endl;
        Logger::shutdown();
        return 1;
    }
    
    // This point is reached only when the server is stopped
    Logger::info("Server shutting down");
    Logger::shutdown();
    return 0;
}
