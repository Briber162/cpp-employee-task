#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>

class Employee {
private:
    std::string id;
    std::string employee_name;
    std::string employee_salary;
    std::string employee_age;
    std::string employee_title;
    std::string employee_email;

public:
    Employee();
    Employee(const std::string& id, const std::string& name, const std::string& salary, 
             const std::string& age, const std::string& title, const std::string& email);
    
    // Getters
    std::string getId() const;
    std::string getName() const;
    std::string getSalary() const;
    std::string getAge() const;
    std::string getTitle() const;
    std::string getEmail() const;
    
    // Setters
    void setId(const std::string& id);
    void setName(const std::string& name);
    void setSalary(const std::string& salary);
    void setAge(const std::string& age);
    void setTitle(const std::string& title);
    void setEmail(const std::string& email);
};

#endif // EMPLOYEE_H
