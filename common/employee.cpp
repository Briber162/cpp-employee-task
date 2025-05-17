#include "employee.h"

Employee::Employee() {}

Employee::Employee(const std::string& id, const std::string& name, const std::string& salary, 
                   const std::string& age, const std::string& title, const std::string& email)
    : id(id), employee_name(name), employee_salary(salary), 
      employee_age(age), employee_title(title), employee_email(email) {}

// Getters
std::string Employee::getId() const {
    return id;
}

std::string Employee::getName() const {
    return employee_name;
}

std::string Employee::getSalary() const {
    return employee_salary;
}

std::string Employee::getAge() const {
    return employee_age;
}

std::string Employee::getTitle() const {
    return employee_title;
}

std::string Employee::getEmail() const {
    return employee_email;
}

// Setters
void Employee::setId(const std::string& id) {
    this->id = id;
}

void Employee::setName(const std::string& name) {
    this->employee_name = name;
}

void Employee::setSalary(const std::string& salary) {
    this->employee_salary = salary;
}

void Employee::setAge(const std::string& age) {
    this->employee_age = age;
}

void Employee::setTitle(const std::string& title) {
    this->employee_title = title;
}

void Employee::setEmail(const std::string& email) {
    this->employee_email = email;
}
