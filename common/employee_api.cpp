#include "employee_api.h"
#include "logger.h"
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

EmployeeAPI::EmployeeAPI(const std::string& dataFilePath) : dataFilePath(dataFilePath) {
    loadEmployees();
}

bool EmployeeAPI::loadEmployees() {
    Logger::get()->info("Loading employees from {}", dataFilePath);
    
    try {
        std::ifstream file(dataFilePath);
        if (!file.is_open()) {
            Logger::get()->error("Could not open file: {}", dataFilePath);
            return false;
        }

        json jsonData;
        file >> jsonData;
        file.close();

        employees.clear();
        Logger::get()->debug("Parsing {} employee records from JSON", jsonData.size());
        
        if (jsonData.contains("data") && jsonData["data"].is_array()) {
            for (const auto& item : jsonData["data"]) {
                try {
                    Employee emp;
                    emp.setId(item["id"].get<std::string>());
                    emp.setName(item["employee_name"].get<std::string>());
                    emp.setSalary(item["employee_salary"].is_string() ? 
                        item["employee_salary"].get<std::string>() : 
                        std::to_string(item["employee_salary"].get<int>()));
                    emp.setAge(item["employee_age"].is_string() ? 
                        item["employee_age"].get<std::string>() : 
                        std::to_string(item["employee_age"].get<int>()));
                    emp.setTitle(item["employee_title"].get<std::string>());
                    emp.setEmail(item["employee_email"].get<std::string>());
                    
                    employees[emp.getId()] = emp;
                } catch (const std::exception& e) {
                    Logger::get()->warn("Failed to parse employee record: {}", e.what());
                    // Continue with next record
                }
            }
        }
        
        Logger::get()->info("Successfully loaded {} employees", employees.size());
        return true;
    } catch (const std::exception& e) {
        Logger::get()->error("Error loading employees: {}", e.what());
        return false;
    }
}

bool EmployeeAPI::saveEmployees() {
    Logger::get()->info("Saving {} employees to {}", employees.size(), dataFilePath);
    try {
        json jsonData;
        json employeesArray = json::array();
        
        for (const auto& pair : employees) {
            const Employee& emp = pair.second;
            json employeeObj;
            employeeObj["id"] = emp.getId();
            employeeObj["employee_name"] = emp.getName();
            employeeObj["employee_salary"] = emp.getSalary();
            employeeObj["employee_age"] = emp.getAge();
            employeeObj["employee_title"] = emp.getTitle();
            employeeObj["employee_email"] = emp.getEmail();
            
            employeesArray.push_back(employeeObj);
        }
        
        jsonData["data"] = employeesArray;
        Logger::get()->debug("Created JSON with {} employee records", employeesArray.size());
        
        std::ofstream file(dataFilePath);
        if (!file.is_open()) {
            Logger::get()->error("Could not open file for writing: {}", dataFilePath);
            return false;
        }
        
        file << jsonData.dump(4);  // Pretty print with 4 spaces indentation
        file.close();
        
        Logger::get()->info("Successfully saved {} employees to {}", employees.size(), dataFilePath);
        return true;
    } catch (const std::exception& e) {
        Logger::get()->error("Error saving employees: {}", e.what());
        return false;
    }
}

std::vector<Employee> EmployeeAPI::getAllEmployees() const {
    Logger::get()->debug("Getting all employees, count: {}", employees.size());
    
    std::vector<Employee> result;
    for (const auto& pair : employees) {
        result.push_back(pair.second);
    }
    return result;
}

Employee* EmployeeAPI::getEmployeeById(const std::string& id) {
    Logger::get()->debug("Looking up employee with ID: {}", id);
    
    auto it = employees.find(id);
    if (it != employees.end()) {
        Logger::get()->debug("Found employee: {} ({})", it->second.getName(), id);
        return &(it->second);
    }
    
    Logger::get()->debug("Employee with ID {} not found", id);
    return nullptr;
}

std::vector<Employee> EmployeeAPI::getEmployeesByName(const std::string& name) const {
    Logger::get()->debug("Searching for employees with name containing: '{}'", name);
    
    std::vector<Employee> result;
    
    for (const auto& pair : employees) {
        const Employee& emp = pair.second;
        // Case-insensitive substring search
        std::string empName = emp.getName();
        std::string searchName = name;
      
        // Convert both strings to lowercase for case-insensitive comparison
        std::transform(empName.begin(), empName.end(), empName.begin(), ::tolower);
        std::transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);
        
        if (empName.find(searchName) != std::string::npos) {
            Logger::get()->trace("Match found: {} (ID: {})", emp.getName(), emp.getId());
            result.push_back(emp);
        }
    }
    
    Logger::get()->debug("Found {} employees matching name: '{}'", result.size(), name);
    return result;
}

bool EmployeeAPI::addEmployee(const Employee& employee) {
    std::string id = employee.getId();
    if (employees.find(id) != employees.end()) {
        return false;  // Employee with this ID already exists
    }
    
    employees[id] = employee;
    return saveEmployees();
}

bool EmployeeAPI::updateEmployee(const Employee& employee) {
    std::string id = employee.getId();
    if (employees.find(id) == employees.end()) {
        return false;  // Employee with this ID doesn't exist
    }
    
    employees[id] = employee;
    return saveEmployees();
}

bool EmployeeAPI::deleteEmployee(const std::string& id) {
    auto it = employees.find(id);
    if (it == employees.end()) {
        return false;  // Employee with this ID doesn't exist
    }
    
    employees.erase(it);
    return saveEmployees();
}

int EmployeeAPI::getHighestSalaryOfEmployees() const {
    int highestSalary = 0;
    
    for (const auto& [id, employee] : employees) {
        try {
            int salary = std::stoi(employee.getSalary());
            if (salary > highestSalary) {
                highestSalary = salary;
            }
        } catch (const std::exception& e) {
            // Skip if salary is not a valid integer
        }
    }
    
    return highestSalary;
}

std::vector<Employee> EmployeeAPI::getTop10HighestEarningEmployees() const {
    // Create a vector of employees for sorting
    std::vector<std::pair<std::string, int>> employeesWithSalary;
    
    // Convert salary strings to integers for sorting
    for (const auto& [id, employee] : employees) {
        try {
            int salary = std::stoi(employee.getSalary());
            employeesWithSalary.push_back({employee.getId(), salary});
        } catch (const std::exception& e) {
            // Skip if salary is not a valid integer
        }
    }
    
    // Sort employees by salary in descending order
    std::sort(employeesWithSalary.begin(), employeesWithSalary.end(), 
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    // Get the top 10 (or less if there aren't 10 employees)
    std::vector<Employee> topEmployees;
    int count = std::min(10, static_cast<int>(employeesWithSalary.size()));
    
    for (int i = 0; i < count; i++) {
        topEmployees.push_back(employees.at(employeesWithSalary[i].first));
    }
    
    return topEmployees;
}

std::vector<Employee> EmployeeAPI::getEmployeesByTitle(const std::string& title) const {
    std::vector<Employee> result;

    for (const auto& pair : employees) {
        const Employee& emp = pair.second;
        // Case-insensitive substring search
        std::string empTitle = emp.getTitle();
        std::string searchTtile = title;
      
        // Convert both strings to lowercase for case-insensitive comparison
        std::transform(empTitle.begin(), empTitle.end(), empTitle.begin(), ::tolower);
        std::transform(searchTtile.begin(), searchTtile.end(), searchTtile.begin(), ::tolower);
        
        if (empTitle.find(searchTtile) != std::string::npos) {
            result.push_back(emp);
        }
    }
    
    return result;
}
