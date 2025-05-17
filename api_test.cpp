#include <iostream>
#include <string>
#include "nlohmann/json.hpp"
#include "employee.h"
#include "employee_api.h"

using json = nlohmann::json;

// Function to display the JSON response
void displayJsonResponse(const std::string& operation, const json& response) {
    std::cout << "\n------ " << operation << " ------\n";
    std::cout << response.dump() << std::endl;
    std::cout << "------------------------\n";
}

int main() {
    // Create an instance of the EmployeeAPI
    EmployeeAPI api("common/employees.json");
    
    // Test 1: Get all employees
    {
        auto employees = api.getAllEmployees();
        json response;
        json data = json::array();
        
        for (const auto& emp : employees) {
            json empObj;
            empObj["id"] = emp.getId();
            empObj["employee_name"] = emp.getName();
            empObj["employee_salary"] = emp.getSalary();
            empObj["employee_age"] = emp.getAge();
            empObj["employee_title"] = emp.getTitle();
            empObj["employee_email"] = emp.getEmail();
            data.push_back(empObj);
        }
        
        response["status"] = "success";
        response["data"] = data;
        
        displayJsonResponse("GET All Employees", response);
    }
    
    // Test 2: Get employee by ID
    {
        std::string id = "1";
        Employee* emp = api.getEmployeeById(id);
        json response;
        
        if (emp) {
            json empObj;
            empObj["id"] = emp->getId();
            empObj["employee_name"] = emp->getName();
            empObj["employee_salary"] = emp->getSalary();
            empObj["employee_age"] = emp->getAge();
            empObj["employee_title"] = emp->getTitle();
            empObj["employee_email"] = emp->getEmail();
            
            response["status"] = "success";
            response["data"] = empObj;
        } else {
            response["status"] = "error";
            response["message"] = "Employee not found";
        }
        
        displayJsonResponse("GET Employee by ID: " + id, response);
    }
    
    // Test 3: Get employees by name
    {
        std::string name = "a"; // This should match several employees
        auto employees = api.getEmployeesByName(name);
        json response;
        
        if (!employees.empty()) {
            json data = json::array();
            for (const auto& emp : employees) {
                json empObj;
                empObj["id"] = emp.getId();
                empObj["employee_name"] = emp.getName();
                empObj["employee_salary"] = emp.getSalary();
                empObj["employee_age"] = emp.getAge();
                empObj["employee_title"] = emp.getTitle();
                empObj["employee_email"] = emp.getEmail();
                data.push_back(empObj);
            }
            
            response["status"] = "success";
            response["data"] = data;
        } else {
            response["status"] = "error";
            response["message"] = "No employees found with the given name";
        }
        
        displayJsonResponse("GET Employees by Name: " + name, response);
    }
    
    // Test 4: Add new employee
    {
        Employee newEmp;
        newEmp.setId("8"); // Assuming 7 is the highest ID in the initial data
        newEmp.setName("John Doe");
        newEmp.setSalary("90000");
        newEmp.setAge("35");
        newEmp.setTitle("Software Engineer");
        newEmp.setEmail("jdoe@company.com");
        
        bool success = api.addEmployee(newEmp);
        json response;
        
        if (success) {
            json empObj;
            empObj["id"] = newEmp.getId();
            empObj["employee_name"] = newEmp.getName();
            empObj["employee_salary"] = newEmp.getSalary();
            empObj["employee_age"] = newEmp.getAge();
            empObj["employee_title"] = newEmp.getTitle();
            empObj["employee_email"] = newEmp.getEmail();
            
            response["status"] = "success";
            response["message"] = "Employee added successfully";
            response["data"] = empObj;
        } else {
            response["status"] = "error";
            response["message"] = "Failed to add employee";
        }
        
        displayJsonResponse("POST Add New Employee", response);
    }
    
    // Test 5: Update employee
    {
        std::string id = "2";
        Employee* emp = api.getEmployeeById(id);
        json response;
        
        if (emp) {
            // Store original values to show the difference
            std::string originalSalary = emp->getSalary();
            
            // Update the employee
            emp->setSalary("180000"); // Increase the salary
            
            if (api.updateEmployee(*emp)) {
                json empObj;
                empObj["id"] = emp->getId();
                empObj["employee_name"] = emp->getName();
                empObj["employee_salary"] = emp->getSalary();
                empObj["employee_age"] = emp->getAge();
                empObj["employee_title"] = emp->getTitle();
                empObj["employee_email"] = emp->getEmail();
                
                response["status"] = "success";
                response["message"] = "Employee updated successfully (Salary: " + originalSalary + " -> " + emp->getSalary() + ")";
                response["data"] = empObj;
            } else {
                response["status"] = "error";
                response["message"] = "Failed to update employee";
            }
        } else {
            response["status"] = "error";
            response["message"] = "Employee not found";
        }
        
        displayJsonResponse("PUT Update Employee ID: " + id, response);
    }
    
    // Test 6: Delete employee (let's add a temporary employee and then delete it)
    {
        // First, add a temporary employee
        Employee tempEmp;
        tempEmp.setId("999");
        tempEmp.setName("Temporary Employee");
        tempEmp.setSalary("50000");
        tempEmp.setAge("25");
        tempEmp.setTitle("Intern");
        tempEmp.setEmail("temp@company.com");
        
        api.addEmployee(tempEmp);
        
        // Now delete it
        std::string id = "999";
        bool success = api.deleteEmployee(id);
        json response;
        
        if (success) {
            response["status"] = "success";
            response["message"] = "Employee deleted successfully";
        } else {
            response["status"] = "error";
            response["message"] = "Failed to delete employee";
        }
        
        displayJsonResponse("DELETE Employee ID: " + id, response);
    }
    
    // Test 7: Get highest salary amongst all employees
    {
        int highestSalary = api.getHighestSalaryOfEmployees();
        json response;
        
        response["status"] = "success";
        response["data"] = highestSalary;
        
        displayJsonResponse("GET Highest Salary", response);
    }
    
    // Test 8: Get top 10 highest earning employees
    {
        auto topEmployees = api.getTop10HighestEarningEmployees();
        json response;
        
        json data = json::array();
        for (const auto& emp : topEmployees) {
            json empObj;
            empObj["id"] = emp.getId();
            empObj["employee_name"] = emp.getName();
            empObj["employee_salary"] = emp.getSalary();
            empObj["employee_age"] = emp.getAge();
            empObj["employee_title"] = emp.getTitle();
            empObj["employee_email"] = emp.getEmail();
            data.push_back(empObj);
        }
        
        response["status"] = "success";
        response["data"] = data;
        
        displayJsonResponse("GET Top 10 Earners", response);
    }
    
    // Test 9: Get employees by title
    {
        std::string title = "Engineer"; // This should match engineers
        auto employeesByTitle = api.getEmployeesByTitle(title);
        json response;
        
        if (!employeesByTitle.empty()) {
            json data = json::array();
            for (const auto& emp : employeesByTitle) {
                json empObj;
                empObj["id"] = emp.getId();
                empObj["employee_name"] = emp.getName();
                empObj["employee_salary"] = emp.getSalary();
                empObj["employee_age"] = emp.getAge();
                empObj["employee_title"] = emp.getTitle();
                empObj["employee_email"] = emp.getEmail();
                data.push_back(empObj);
            }
            
            response["status"] = "success";
            response["data"] = data;
        } else {
            response["status"] = "error";
            response["message"] = "No employees found with the given title";
        }
        
        displayJsonResponse("GET Employees by Title: " + title, response);
    }
    
    return 0;
}
