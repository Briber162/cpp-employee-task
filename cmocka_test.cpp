#include <string>
#include <vector>
#include <algorithm>  // for std::transform
#include <iostream>
#include "common/employee.h"
#include "common/employee_api.h"
#include "common/logger.h"
#include "common/timer.h"

// CMocka includes - with extern "C" for C++ compatibility
extern "C" {
    #include <stdarg.h>
    #include <stddef.h>
    #include <setjmp.h>
    #include <stdint.h>
    #include <cmocka.h>
}

// Global test data
static EmployeeAPI* api = nullptr;

// Setup function - called before each test
static int setup(void **state) {
    (void) state;
    
    // Initialize logger for test component
    static bool loggerInitialized = false;
    if (!loggerInitialized) {
        Logger::initForComponent(LogComponent::TEST);
        Logger::info("CMocka Test Suite Starting");
        loggerInitialized = true;
    }
    
    if (api == nullptr) {
        Logger::debug("Creating EmployeeAPI instance with data file: common/employees.json");
        api = new EmployeeAPI("common/employees.json");
    }
    return 0;
}


// Final teardown at the end of all tests
static int group_teardown(void **state) {
    (void) state;
    
    Logger::debug("Cleaning up test resources");
    delete api;
    api = nullptr;
    
    // Log test completion and shut down logger
    Logger::info("CMocka Test Suite Completed");
    Logger::shutdown();
    
    return 0;
}

// Test function: getAllEmployees
static void test_get_all_employees(void **state) {
    (void) state;
    auto employees = api->getAllEmployees();
    
    // Check if we have at least some employees
    assert_true(employees.size() > 0);
    
    // Verify some basic assumptions about the data
    for (const auto& emp : employees) {
        // Check that essential fields are not empty
        assert_false(emp.getId().empty());
        assert_false(emp.getName().empty());
    }
}

// Test function: getEmployeeById
static void test_get_employee_by_id(void **state) {
    (void) state;
    // Test with a valid ID (assuming ID 1 exists)
    Employee* emp = api->getEmployeeById("1");
    assert_non_null(emp);
    assert_string_equal(emp->getId().c_str(), "1");
    
    // Test with an invalid ID
    emp = api->getEmployeeById("999999");
    assert_null(emp);
}

// Test function: getEmployeesByName
static void test_get_employees_by_name(void **state) {
    (void) state;
    // Test with a common substring that should match multiple employees
    std::string searchName = "a";
    auto employees = api->getEmployeesByName(searchName);
    assert_true(employees.size() > 0);
    
    // Verify that the search is actually working
    for (const auto& emp : employees) {
        std::string empName = emp.getName();
        std::transform(empName.begin(), empName.end(), empName.begin(), ::tolower);
        
        std::string searchNameLower = searchName;
        std::transform(searchNameLower.begin(), searchNameLower.end(), searchNameLower.begin(), ::tolower);
        
        assert_true(empName.find(searchNameLower) != std::string::npos);
    }
    
    // Test with a non-existent name
    searchName = "XYZ123NonExistentName";
    employees = api->getEmployeesByName(searchName);
    assert_int_equal(employees.size(), 0);
}

// Test function: addEmployee and deleteEmployee
static void test_add_and_delete_employee(void **state) {
    (void) state;
    // Create a new test employee
    Employee testEmp;
    testEmp.setId("test_id_12345");
    testEmp.setName("Test Employee");
    testEmp.setSalary("100000");
    testEmp.setAge("30");
    testEmp.setTitle("Test Engineer");
    testEmp.setEmail("test@example.com");
    
    // Add the employee
    bool addResult = api->addEmployee(testEmp);
    assert_true(addResult);
    
    // Verify the employee was added
    Employee* addedEmp = api->getEmployeeById("test_id_12345");
    assert_non_null(addedEmp);
    assert_string_equal(addedEmp->getName().c_str(), "Test Employee");
    
    // Delete the employee
    bool deleteResult = api->deleteEmployee("test_id_12345");
    assert_true(deleteResult);
    
    // Verify the employee was deleted
    addedEmp = api->getEmployeeById("test_id_12345");
    assert_null(addedEmp);
}

// Test function: updateEmployee
static void test_update_employee(void **state) {
    (void) state;
    // Create and add a test employee first
    Employee testEmp;
    testEmp.setId("update_test_123");
    testEmp.setName("Update Test");
    testEmp.setSalary("75000");
    testEmp.setAge("35");
    testEmp.setTitle("Test Position");
    testEmp.setEmail("update@example.com");
    
    api->addEmployee(testEmp);
    
    // Get the employee and modify it
    Employee* emp = api->getEmployeeById("update_test_123");
    assert_non_null(emp);
    
    emp->setSalary("85000");
    emp->setTitle("Senior Test Position");
    
    // Update the employee
    bool updateResult = api->updateEmployee(*emp);
    assert_true(updateResult);
    
    // Verify the update worked
    Employee* updatedEmp = api->getEmployeeById("update_test_123");
    assert_non_null(updatedEmp);
    assert_string_equal(updatedEmp->getSalary().c_str(), "85000");
    assert_string_equal(updatedEmp->getTitle().c_str(), "Senior Test Position");
    
    // Clean up
    api->deleteEmployee("update_test_123");
}

// Test function: getHighestSalaryOfEmployees
static void test_highest_salary(void **state) {
    (void) state;
    Logger::info("Testing highest salary functionality");
    
    // Start timing the operation
    Timer timer("test_highest_salary", LogComponent::TEST);
    
    int highestSalary = api->getHighestSalaryOfEmployees();
    Logger::debug("Highest salary retrieved: {}", highestSalary);
    
    // Verify it's a positive value
    assert_true(highestSalary > 0);
    
    // Verify against the actual data by checking each employee
    auto employees = api->getAllEmployees();
    int expectedHighest = 0;
    
    for (const auto& emp : employees) {
        try {
            int salary = std::stoi(emp.getSalary());
            if (salary > expectedHighest) {
                expectedHighest = salary;
            }
        } catch (...) {
            // Skip invalid salaries
        }
    }
    
    assert_int_equal(highestSalary, expectedHighest);
}

// Test function: getTop10HighestEarningEmployees
static void test_top_earners(void **state) {
    (void) state;
    Logger::info("Testing top earners functionality");
    
    // Start timing the operation
    Timer timer("test_top_earners", LogComponent::TEST);
    
    auto topEmployees = api->getTop10HighestEarningEmployees();
    Logger::debug("Retrieved {} top earning employees", topEmployees.size());
    
    // Check that we got some results
    assert_true(topEmployees.size() > 0);
    assert_true(topEmployees.size() <= 10);
    
    // Verify that the results are properly sorted by salary in descending order
    for (size_t i = 1; i < topEmployees.size(); i++) {
        // Skip any employees with non-integer salaries
        try {
            int salaryPrev = std::stoi(topEmployees[i-1].getSalary());
            int salaryCurrent = std::stoi(topEmployees[i].getSalary());
            
            // Check each employee has lower or equal salary than the previous
            assert_true(salaryPrev >= salaryCurrent);
        } catch (...) {
            // Skip invalid salary comparisons
            continue;
        }
    }
    
    // If we have at least two employees, verify they have expected fields
    if (topEmployees.size() >= 1) {
        // Verify the first employee has required fields
        assert_false(topEmployees[0].getId().empty());
        assert_false(topEmployees[0].getName().empty());
        assert_false(topEmployees[0].getSalary().empty());
    }
    
    // We've already checked that the salaries are in descending order above
}

// Test function: getEmployeesByTitle
static void test_employees_by_title(void **state) {
    // Test with a common title substring that should match employees
    (void) state;
    std::string searchTitle = "sde";
    auto employees = api->getEmployeesByTitle(searchTitle);
    
    // Verify we got results
    assert_true(employees.size() > 0);
    
    // Verify that the search is working
    for (const auto& emp : employees) {
        std::string empTitle = emp.getTitle();
        std::transform(empTitle.begin(), empTitle.end(), empTitle.begin(), ::tolower);
        
        std::string searchTitleLower = searchTitle;
        std::transform(searchTitleLower.begin(), searchTitleLower.end(), searchTitleLower.begin(), ::tolower);
        
        assert_true(empTitle.find(searchTitleLower) != std::string::npos);
    }
    
    // Test with a non-existent title
    searchTitle = "NonExistentTitle12345";
    employees = api->getEmployeesByTitle(searchTitle);
    assert_int_equal(employees.size(), 0);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_get_all_employees),
        cmocka_unit_test(test_get_employee_by_id),
        cmocka_unit_test(test_get_employees_by_name),
        cmocka_unit_test(test_add_and_delete_employee),
        cmocka_unit_test(test_update_employee),
        cmocka_unit_test(test_highest_salary),
        cmocka_unit_test(test_top_earners),
        cmocka_unit_test(test_employees_by_title),
    };
    
    // Start timing the entire test suite execution
    Timer timer("complete_test_suite", LogComponent::TEST);
    
    // Log test count
    Logger::info("Running {} test cases", sizeof(tests) / sizeof(tests[0]));
    
    // Run tests with setup/teardown
    int result = cmocka_run_group_tests(tests, setup, group_teardown);
    
    // Log test results
    if (result == 0) {
        Logger::info("All tests passed successfully");
    } else {
        Logger::error("Test suite failed with {} failures", result);
    }
    
    return result;
}
