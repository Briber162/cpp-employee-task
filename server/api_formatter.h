#ifndef API_FORMATTER_H
#define API_FORMATTER_H

#include <string>
#include <vector>
#include "employee.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class ApiFormatter {
public:
    // Format a single employee to json
    static json formatEmployee(const Employee& emp) {
        json j;
        j["id"] = emp.getId();
        j["employee_name"] = emp.getName();
        j["employee_salary"] = emp.getSalary();
        j["employee_age"] = emp.getAge();
        j["employee_title"] = emp.getTitle();
        j["employee_email"] = emp.getEmail();
        return j;
    }

    // Format a vector of employees to json
    static json formatEmployees(const std::vector<Employee>& employees) {
        json data = json::array();
        for (const auto& emp : employees) {
            data.push_back(formatEmployee(emp));
        }
        return data;
    }

    // Format a generic API response
    static std::string formatApiResponse(const json& data, const std::string& status = "Successfully processed request.") {
        json response;
        response["data"] = data;
        response["status"] = status;
        
        // Pretty print with indentation of 4 spaces
        return response.dump(4);
    }

    // Convenience method for employee list response
    static std::string formatEmployeeListResponse(const std::vector<Employee>& employees, const std::string& status = "Successfully processed request.") {
        json data = formatEmployees(employees);
        return formatApiResponse(data, status);
    }
    
    // Convenience method for single employee response
    static std::string formatSingleEmployeeResponse(const Employee& employee, const std::string& status = "Successfully processed request.") {
        json data = formatEmployee(employee);
        return formatApiResponse(data, status);
    }
};

#endif // API_FORMATTER_H
