#include <iostream>
#include <string>
#include <curl/curl.h>
#include "nlohmann/json.hpp"
#include "timer.h"
#include "logger.h"

using json = nlohmann::json;

// Callback function for curl to write response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to make HTTP requests
std::string makeRequest(const std::string& url, const std::string& method = "GET", const std::string& data = "") {
    // Start timing the HTTP request
    Timer timer("HTTP_" + method + "_Request", LogComponent::CLIENT);
    
    Logger::debug("Making {} request to: {}", method, url);
    
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    
    curl = curl_easy_init();
    if (!curl) {
        Logger::error("curl_easy_init() failed");
        return "";
    }
    
    // Set URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    
    // Set callback function for reading the response
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    
    // Set HTTP method and data for POST/PUT requests
    if (method == "POST" || method == "PUT") {
        // Set custom request method if not POST (default)
        if (method == "PUT") {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        }
        
        // Set POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        
        // Set headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    } else if (method == "DELETE") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }
    
    // Perform the request
    res = curl_easy_perform(curl);
    
    // Check for errors
    if (res != CURLE_OK) {
        Logger::error("curl_easy_perform() failed: {}", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return "";
    }
    
    Logger::debug("HTTP request completed successfully, response size: {} bytes", readBuffer.size());
    
    // Cleanup
    curl_easy_cleanup(curl);
    
    return readBuffer;
}

// API functions
std::string getAllEmployees() {
    Logger::info("Requesting all employees from API");
    Timer timer("getAllEmployees", LogComponent::CLIENT);
    std::string response = makeRequest("http://localhost:8112/api/employees");
    
    // Parse response to get the number of employees if possible
    try {
        json jsonResponse = json::parse(response);
        if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
            Logger::info("Retrieved {} employees from API", jsonResponse["data"].size());
        }
    } catch (const std::exception& e) {
        Logger::warn("Could not parse API response: {}", e.what());
    }
    
    return response;
}

std::string getEmployeeById(const std::string& id) {
    return makeRequest("http://localhost:8112/api/employees/id/" + id);
}

std::string getEmployeesByName(const std::string& name) {
    return makeRequest("http://localhost:8112/api/employees/search/name/" + name);
}

std::string addEmployee(const json& employee) {
    return makeRequest(
        "http://localhost:8112/api/employees", 
        "POST", 
        employee.dump()
    );
}

std::string updateEmployee(const std::string& id, const json& updates) {
    return makeRequest(
        "http://localhost:8112/api/employees/update/" + id, 
        "PUT", 
        updates.dump()
    );
}

std::string deleteEmployee(const std::string& id) {
    return makeRequest(
        "http://localhost:8112/api/employees/id/" + id, 
        "DELETE"
    );
}

std::string getHighestSalary() {
    Logger::info("Requesting highest salary from API");
    Timer timer("getHighestSalary", LogComponent::CLIENT);
    std::string response = makeRequest("http://localhost:8112/api/employees/highestSalary");
    
    // Parse response to get the highest salary if possible
    try {
        json jsonResponse = json::parse(response);
        if (jsonResponse.contains("data")) {
            Logger::info("Highest salary retrieved: {}", jsonResponse["data"].dump());
        }
    } catch (const std::exception& e) {
        Logger::warn("Could not parse API response: {}", e.what());
    }
    
    return response;
}

std::string getTopEarners() {
    Logger::info("Requesting top earners from API");
    Timer timer("getTopEarners", LogComponent::CLIENT);
    std::string response = makeRequest("http://localhost:8112/api/employees/topEarners");
    
    // Parse response to get the number of top earners if possible
    try {
        json jsonResponse = json::parse(response);
        if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
            Logger::info("Retrieved {} top earning employees from API", jsonResponse["data"].size());
            
            // Log the highest salary employee's details if available
            if (!jsonResponse["data"].empty()) {
                auto& topEmployee = jsonResponse["data"][0];
                if (topEmployee.contains("employee_name") && topEmployee.contains("employee_salary")) {
                    Logger::info("Top earner: {} with salary {}", 
                                topEmployee["employee_name"].dump(), 
                                topEmployee["employee_salary"].dump());
                }
            }
        }
    } catch (const std::exception& e) {
        Logger::warn("Could not parse API response: {}", e.what());
    }
    
    return response;
}

std::string getEmployeeByTitle(const std::string& title) {
    Logger::info("Searching for employees with title: '{}'", title);
    Timer timer("searchEmployeesByTitle", LogComponent::CLIENT);
    std::string response = makeRequest("http://localhost:8112/api/employees/getEmployeesbyTitle/" + title);
    
    // Parse response to get the matching employees if possible
    try {
        json jsonResponse = json::parse(response);
        if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
            size_t matchCount = jsonResponse["data"].size();
            if (matchCount > 0) {
                Logger::info("Found {} employees with title matching '{}'", matchCount, title);
            } else {
                Logger::info("No employees found with title matching '{}'", title);
            }
        }
    } catch (const std::exception& e) {
        Logger::warn("Could not parse API response: {}", e.what());
    }
    
    return response;
}

void displayMenu() {
    std::cout << "\nEmployee API Client\n";
    std::cout << "----------------------------------------\n";
    std::cout << "1. Get all employees\n";
    std::cout << "2. Get employee by ID\n";
    std::cout << "3. Search employees by name\n";
    std::cout << "4. Add new employee\n";
    std::cout << "5. Update employee\n";
    std::cout << "6. Delete employee\n";
    std::cout << "7. Get highest salary\n";
    std::cout << "8. Get top 10 earners\n";
    std::cout << "9. Get employees by title\n";
    std::cout << "0. Exit\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Enter your choice: ";
}

int main() {
    // Initialize logger for the client component
    Logger::initForComponent(LogComponent::CLIENT);
    Logger::info("Starting Employee Management API Client");
    
    // Initialize curl globally
    curl_global_init(CURL_GLOBAL_DEFAULT);
    Logger::debug("CURL initialized globally");
    
    int choice;
    std::string id;
    std::string name;
    std::string title;
    std::string value;
    std::string response;
    bool running = true;
    
    while (running) {
        displayMenu();
        std::cin >> choice;
        
        // Clear any input errors
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        
        std::cin.ignore(); // Clear the newline character
        
        switch (choice) {
            case 0:
                running = false;
                std::cout << "Exiting program.\n";
                break;
                
            case 1: // Get all employees
                response = getAllEmployees();
                std::cout << "Response:\n" << response << std::endl;
                break;
                
            case 2: // Get employee by ID
                std::cout << "Enter employee ID: ";
                std::getline(std::cin, id);
                response = getEmployeeById(id);
                std::cout << "Response:\n" << response << std::endl;
                break;
                
            case 3: { // Search employees by name
                std::cout << "Enter employee name to search: ";
                std::getline(std::cin, name);
                
                // URL encode the name parameter
                CURL* curl = curl_easy_init();
                if (curl) {
                    char* encoded_name = curl_easy_escape(curl, name.c_str(), name.length());
                    if (encoded_name) {
                        std::string encodedNameStr(encoded_name);
                        response = getEmployeesByName(encodedNameStr);
                        curl_free(encoded_name);
                    } else {
                        std::cerr << "Failed to URL encode the name" << std::endl;
                        response = getEmployeesByName(name); // Fallback to non-encoded
                    }
                    curl_easy_cleanup(curl);
                } else {
                    std::cerr << "Failed to initialize curl for URL encoding" << std::endl;
                    response = getEmployeesByName(name); // Fallback to non-encoded
                }
                
                std::cout << "Response:\n" << response << std::endl;
                break;
            }
                
            case 4: { // Add new employee
                json newEmp;
                std::string name, salary, age, title, email;
                
                std::cout << "Enter employee name: ";
                std::getline(std::cin, name);
                
                std::cout << "Enter employee salary: ";
                std::getline(std::cin, salary);
                
                std::cout << "Enter employee age: ";
                std::getline(std::cin, age);
                
                std::cout << "Enter employee title: ";
                std::getline(std::cin, title);
                
                std::cout << "Enter employee email: ";
                std::getline(std::cin, email);
                
                newEmp["employee_name"] = name;
                newEmp["employee_salary"] = salary;
                newEmp["employee_age"] = age;
                newEmp["employee_title"] = title;
                newEmp["employee_email"] = email;
                
                response = addEmployee(newEmp);
                std::cout << "Response:\n" << response << std::endl;
                break;
            }
                
            case 5: { // Update employee
                json updates;
                std::string value;
                
                std::cout << "Enter employee ID to update: ";
                std::getline(std::cin, id);
                
                std::cout << "Enter new name (leave blank to keep current): ";
                std::getline(std::cin, value);
                if (!value.empty()) updates["employee_name"] = value;
                
                std::cout << "Enter new salary (leave blank to keep current): ";
                std::getline(std::cin, value);
                if (!value.empty()) updates["employee_salary"] = value;
                
                std::cout << "Enter new age (leave blank to keep current): ";
                std::getline(std::cin, value);
                if (!value.empty()) updates["employee_age"] = value;
                
                std::cout << "Enter new title (leave blank to keep current): ";
                std::getline(std::cin, value);
                if (!value.empty()) updates["employee_title"] = value;
                
                std::cout << "Enter new email (leave blank to keep current): ";
                std::getline(std::cin, value);
                if (!value.empty()) updates["employee_email"] = value;
                
                response = updateEmployee(id, updates);
                std::cout << "Response:\n" << response << std::endl;
                break;
            }
                
            case 6: // Delete employee
                std::cout << "Enter employee ID to delete: ";
                std::getline(std::cin, id);
                
                response = deleteEmployee(id);
                std::cout << "Response:\n" << response << std::endl;
                break;
                
            case 7: // Get highest salary
                response = getHighestSalary();
                std::cout << "Response:\n" << response << std::endl;
                break;
                
            case 8: // Get top 10 earners
                response = getTopEarners();
                std::cout << "Response:\n" << response << std::endl;
                break;
            
            case 9: // Get employees by title
                std::cout << "Enter employee title to search: ";
                std::getline(std::cin, title);
                response = getEmployeeByTitle(title);
                std::cout << "Response:\n" << response << std::endl;
                break;
            
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }
    
    // Cleanup curl global state
    curl_global_cleanup();
    Logger::debug("CURL global resources cleaned up");
    
    // Shutdown logger and flush any remaining logs
    Logger::info("Employee Management API Client shutting down");
    Logger::shutdown();
    
    return 0;
}
