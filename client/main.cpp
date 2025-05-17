#include <iostream>
#include <string>
#include <iomanip>
#include "employee_api.h"

// Function to display a single employee's information
void displayEmployee(const Employee& employee) {
    std::cout << "ID: " << employee.getId() << std::endl;
    std::cout << "Name: " << employee.getName() << std::endl;
    std::cout << "Age: " << employee.getAge() << std::endl;
    std::cout << "Title: " << employee.getTitle() << std::endl;
    std::cout << "Email: " << employee.getEmail() << std::endl;
    std::cout << "Salary: $" << employee.getSalary() << std::endl;
}

// Function to display all employees in a formatted table
void displayAllEmployees(const std::vector<Employee>& employees) {
    if (employees.empty()) {
        std::cout << "No employees found." << std::endl;
        return;
    }

    // Table header
    std::cout << std::setw(5) << "ID" << " | "
              << std::setw(20) << "Name" << " | "
              << std::setw(5) << "Age" << " | "
              << std::setw(30) << "Title" << " | "
              << std::setw(25) << "Email" << " | "
              << std::setw(10) << "Salary" << std::endl;
    
    std::cout << std::string(110, '-') << std::endl;
    
    // Table rows
    for (const auto& emp : employees) {
        std::cout << std::setw(5) << emp.getId() << " | "
                  << std::setw(20) << emp.getName() << " | "
                  << std::setw(5) << emp.getAge() << " | "
                  << std::setw(30) << emp.getTitle() << " | "
                  << std::setw(25) << emp.getEmail() << " | $"
                  << std::setw(9) << emp.getSalary() << std::endl;
    }
}

int main() {
    // Create an instance of EmployeeAPI with the path to the data file
    EmployeeAPI api("common/employees.json");
    
    int choice;
    bool running = true;
    
    while (running) {
        std::cout << "\nEmployee Management System\n";
        std::cout << "1. Get all employees\n";
        std::cout << "2. Get employee by ID\n";
        std::cout << "3. Search employees by name\n";
        std::cout << "4. Add new employee\n";
        std::cout << "5. Update employee\n";
        std::cout << "6. Delete employee\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        
        // Clear any input errors
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        
        switch (choice) {
            case 0:
                running = false;
                std::cout << "Exiting program.\n";
                break;
                
            case 1: { // Get all employees
                auto employees = api.getAllEmployees();
                std::cout << "\nAll Employees:\n";
                displayAllEmployees(employees);
                break;
            }
                
            case 2: { // Get employee by ID
                std::string id;
                std::cout << "Enter employee ID: ";
                std::cin >> id;
                
                Employee* emp = api.getEmployeeById(id);
                if (emp != nullptr) {
                    std::cout << "\nEmployee found:\n";
                    displayEmployee(*emp);
                } else {
                    std::cout << "Employee with ID " << id << " not found.\n";
                }
                break;
            }
                
            case 3: { // Search employees by name
                std::string name;
                std::cin.ignore(); // Clear the newline character
                std::cout << "Enter employee name to search: ";
                std::getline(std::cin, name);
                
                auto employees = api.getEmployeesByName(name);
                if (!employees.empty()) {
                    std::cout << "\nEmployees found:\n";
                    displayAllEmployees(employees);
                } else {
                    std::cout << "No employees found with name containing '" << name << "'.\n";
                }
                break;
            }
                
            case 4: { // Add new employee
                Employee newEmp;
                std::string name, salary, age, title, email;
                
                std::cin.ignore(); // Clear the newline character
                
                std::cout << "Enter employee name: ";
                std::getline(std::cin, name);
                
                std::cout << "Enter employee age: ";
                std::getline(std::cin, age);
                
                std::cout << "Enter employee title: ";
                std::getline(std::cin, title);
                
                std::cout << "Enter employee email: ";
                std::getline(std::cin, email);
                
                std::cout << "Enter employee salary: ";
                std::getline(std::cin, salary);
                
                // Auto-generate ID by finding max ID and incrementing
                auto employees = api.getAllEmployees();
                int maxId = 0;
                for (const auto& e : employees) {
                    try {
                        int id = std::stoi(e.getId());
                        if (id > maxId) maxId = id;
                    } catch (const std::exception& e) {
                        // Skip non-numeric IDs
                    }
                }
                
                newEmp.setId(std::to_string(maxId + 1));
                newEmp.setName(name);
                newEmp.setAge(age);
                newEmp.setTitle(title);
                newEmp.setEmail(email);
                newEmp.setSalary(salary);
                
                if (api.addEmployee(newEmp)) {
                    std::cout << "Employee added successfully with ID: " << newEmp.getId() << "\n";
                } else {
                    std::cout << "Failed to add employee.\n";
                }
                break;
            }
                
            case 5: { // Update employee
                std::string id;
                std::cout << "Enter employee ID to update: ";
                std::cin >> id;
                
                Employee* emp = api.getEmployeeById(id);
                if (emp == nullptr) {
                    std::cout << "Employee with ID " << id << " not found.\n";
                    break;
                }
                
                // Show current values
                std::cout << "\nCurrent employee details:\n";
                displayEmployee(*emp);
                
                // Get new values
                std::string name, age, title, email, salary;
                std::cin.ignore(); // Clear the newline character
                
                std::cout << "\nEnter new details (leave blank to keep current values):\n";
                
                std::cout << "Enter name [" << emp->getName() << "]: ";
                std::getline(std::cin, name);
                if (!name.empty()) emp->setName(name);
                
                std::cout << "Enter age [" << emp->getAge() << "]: ";
                std::getline(std::cin, age);
                if (!age.empty()) emp->setAge(age);
                
                std::cout << "Enter title [" << emp->getTitle() << "]: ";
                std::getline(std::cin, title);
                if (!title.empty()) emp->setTitle(title);
                
                std::cout << "Enter email [" << emp->getEmail() << "]: ";
                std::getline(std::cin, email);
                if (!email.empty()) emp->setEmail(email);
                
                std::cout << "Enter salary [" << emp->getSalary() << "]: ";
                std::getline(std::cin, salary);
                if (!salary.empty()) emp->setSalary(salary);
                
                if (api.updateEmployee(*emp)) {
                    std::cout << "Employee updated successfully.\n";
                } else {
                    std::cout << "Failed to update employee.\n";
                }
                break;
            }
                
            case 6: { // Delete employee
                std::string id;
                std::cout << "Enter employee ID to delete: ";
                std::cin >> id;
                
                Employee* emp = api.getEmployeeById(id);
                if (emp == nullptr) {
                    std::cout << "Employee with ID " << id << " not found.\n";
                    break;
                }
                
                char confirm;
                std::cout << "Are you sure you want to delete this employee? (y/n): ";
                std::cin >> confirm;
                
                if (confirm == 'y' || confirm == 'Y') {
                    if (api.deleteEmployee(id)) {
                        std::cout << "Employee deleted successfully.\n";
                    } else {
                        std::cout << "Failed to delete employee.\n";
                    }
                } else {
                    std::cout << "Delete operation cancelled.\n";
                }
                break;
            }
                
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }
    
    return 0;
}