#ifndef EMPLOYEE_API_H
#define EMPLOYEE_API_H

#include <unordered_map>
#include <vector>
#include <string>
#include "employee.h"

class EmployeeAPI {
private:
    std::unordered_map<std::string, Employee> employees;
    std::string dataFilePath;

public:
    EmployeeAPI(const std::string& dataFilePath);
    
    // API methods
    bool loadEmployees();
    bool saveEmployees();
    
    // Get all employees
    std::vector<Employee> getAllEmployees() const;
    
    // Get employee by ID
    Employee* getEmployeeById(const std::string& id);

    // Get employee by name
    std::vector<Employee> getEmployeesByName(const std::string& name) const;
    
    // Add new employee
    bool addEmployee(const Employee& employee);
    
    // Update employee
    bool updateEmployee(const Employee& employee);
    
    // Delete an employee by ID
    bool deleteEmployee(const std::string& id);
    
    // Get the highest salary amongst all employees
    int getHighestSalaryOfEmployees() const;
    
    // Get employees by title
    std::vector<Employee> getEmployeesByTitle(const std::string& title) const;
    
    // Get the top 10 highest earning employees
    std::vector<Employee> getTop10HighestEarningEmployees() const;
};

#endif // EMPLOYEE_API_H
