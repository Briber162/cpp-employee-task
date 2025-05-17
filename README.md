# C++ Employee Management System

A comprehensive C++ HTTP API for employee management with a modular architecture, featuring both a command-line interface and RESTful HTTP server implementations.

## Project Overview

This project implements a full-featured employee management system with the following components:

1. **Core Business Logic** - Manages employee data with CRUD operations
2. **HTTP API Server** - Exposes RESTful endpoints using httplib (header-only library)
3. **HTTP Client** - Provides API interaction capabilities
4. **CLI Interface** - Offers command line management
5. **Persistence Layer** - Stores data in JSON format

## Architecture

The project uses a modular, layered architecture:

```
┌────────────────────────────────────────────────────────────┐
│                      Client Layer                          │
│  ┌────────────────────┐         ┌─────────────────────┐   │
│  │    HTTP Client     │         │    CLI Interface    │   │
│  │   (client.cpp)     │         │     (main.cpp)      │   │
│  └────────────┬───────┘         └──────────┬──────────┘   │
└───────────────┼─────────────────────────────┼──────────────┘
                │                             │                    
                ▼                             ▼                    
┌────────────────────────────────────────────────────────────┐
│                      HTTP Layer                            │
│  ┌────────────────────────────────────────────────────┐   │
│  │                  HTTP Server                        │   │
│  │               (httplib_server.cpp)                  │   │
│  └─────────────────────────┬──────────────────────────┘   │
└──────────────────────────────┬─────────────────────────────┘
                               │                                
                               ▼                                
┌────────────────────────────────────────────────────────────┐
│                   Business Logic Layer                     │
│  ┌────────────────────────────────────────────────────┐   │
│  │                   Employee API                      │   │
│  │            (employee_api.h/cpp)                     │   │
│  └─────────────────────────┬──────────────────────────┘   │
└───────────────────────────────┬────────────────────────────┘
                               │                                
                               ▼                                
┌────────────────────────────────────────────────────────────┐
│                      Data Model Layer                      │
│  ┌────────────────────────────────────────────────────┐   │
│  │                Employee Class                       │   │
│  │              (employee.h/cpp)                       │   │
│  └─────────────────────────┬──────────────────────────┘   │
└───────────────────────────────┬────────────────────────────┘
                               │                                
                               ▼                                
┌────────────────────────────────────────────────────────────┐
│                    Persistence Layer                       │
│  ┌────────────────────────────────────────────────────┐   │
│  │                 JSON Storage                        │   │
│  │               (employees.json)                      │   │
│  └────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────┘
                                                             
┌────────────────────────────────────────────────────────────┐
│                Cross-Cutting Concerns                      │
│  ┌────────────────────────────────────────────────────┐   │
│  │                 Logging System                      │   │
│  │   ┌─────────────┐ ┌─────────┐ ┌────────┐ ┌──────┐ │   │
│  │   │ Server Log  │ │Client Log│ │Test Log│ │API Log│ │   │
│  │   └─────────────┘ └─────────┘ └────────┘ └──────┘ │   │
│  │                (logger.h/cpp + timer.h)             │   │
│  └────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────┘
```

## Project Structure

The project is organized into these key directories:

- **common/** - Core business logic and data models
  - `employee.h/cpp`: Employee class definition and implementation
  - `employee_api.h/cpp`: API for interacting with employee data
  - `employees.json`: JSON data store for employee information

- **server/** - HTTP server implementation
  - `httplib_server.cpp`: RESTful server using httplib
  - `api_formatter.h`: Formats API responses with consistent JSON structure

- **client/** - Client implementations
  - `client.cpp`: HTTP client for API interaction
  - `main.cpp`: Command-line interface for employee management

- **include/** - External dependencies
  - `nlohmann/json.hpp`: Single-header JSON library for C++
  - `httplib.h`: Header-only HTTP server library
  - `spdlog/`: High-performance logging library
  
## Third-Party Libraries

This project leverages several high-quality third-party libraries to enhance functionality and maintainability:

### 1. nlohmann/json (JSON for Modern C++)

**Purpose**: Provides seamless JSON parsing and serialization with a modern C++ API.

**Why we chose it**:
- Header-only implementation (no build complexity)
- Type safety through C++ templates
- Intuitive and clean API
- Extensive documentation
- Widely adopted in the industry

**Features we use**:
- JSON parsing from files and strings
- Automatic type conversion between JSON and C++ types
- JSON serialization with pretty-printing
- Error handling for malformed JSON
- Array and object manipulation

### 2. Httplib (C++ HTTP/HTTPS Server and Client Library)

**Purpose**: Provides HTTP server and client functionality with minimal setup.

**Why we chose it**:
- Header-only implementation
- Simple and intuitive API
- Cross-platform compatibility
- No external dependencies
- Supports modern C++ features

**Features we use**:
- HTTP server with route registration
- Request and response handling
- HTTP GET, POST, PUT, DELETE methods
- Request parameter extraction
- Response formatting
- CORS header management
- Error handling

### 3. CMocka (Unit Testing Framework)

**Purpose**: Enables comprehensive unit testing of the codebase.

**Why we chose it**:
- Lightweight and simple API
- Support for test fixtures, setup, and teardown
- C compatibility with C++ integration
- Detailed failure reporting

**Features we use**:
- Unit test organization
- Test fixtures with setup and teardown
- Assertions for verification
- Test grouping and execution

### 4. spdlog (Fast C++ Logging Library)

**Purpose**: Provides comprehensive, high-performance logging capabilities.

**Why we chose it**:
- Header-only with minimal setup
- Extremely fast performance
- Support for multiple log targets (console, files)
- Flexible formatting options
- Log rotation capabilities

**Features we use**:
- Multiple log levels (trace, debug, info, warn, error, critical)
- Component-specific logging (server, client, API, tests)
- Console and file logging simultaneously
- File rotation based on size
- Performance timing of critical operations
- Formatted string logging with parameter substitution

## Logging System

The project incorporates a comprehensive logging framework to facilitate debugging, performance monitoring, and operational visibility:

### Architecture

The logging system is built as a cross-cutting concern that spans all layers of the application:

- **Component-Specific Loggers**: Each component (server, client, API, tests) writes to its own log file
- **Multiple Log Levels**: Support for trace, debug, info, warn, error, and critical logging
- **Performance Monitoring**: Timing capabilities for critical operations
- **File Rotation**: Size-based log file rotation to prevent unbounded growth

### Log Files

All logs are stored in the `/logs` directory with component-specific files:

- `server.log`: HTTP server operations, requests, and responses
- `client.log`: Client API requests and responses
- `api.log`: Core business logic operations
- `test.log`: Unit test execution and results

### Performance Tracking

The system includes a Timer utility that measures and logs the execution time of critical operations:

```cpp
// Example: Measuring performance of an operation
Timer timer("getTop10HighestEarningEmployees");
// ... operation code ...
// Timer automatically logs duration when destroyed
```

This provides insights into potential bottlenecks and performance trends over time.

### Makefile Integration

The logging system integrates with the build process through dedicated make targets:

- `make clean-logs`: Removes all log files while preserving binaries
- `make clean-all`: Removes both logs and build artifacts

### Usage Example

Logs follow a consistent format with timestamp, level, component, and message:

```
[2025-05-18 03:17:25.062] [info] [test] PERF: 'test_top_earners' operation completed in 0 ms
```

## API Endpoints

The server exposes the following RESTful endpoints:

- `GET /api/employees` - Get all employees
- `GET /api/employees/id/{id}` - Get employee by ID
- `GET /api/employees/search/name/{name}` - Get employees by name (with case-insensitive search)
- `GET /api/employees/highestSalary` - Get highest salary among all employees
- `GET /api/employees/topEarners` - Get top 10 highest earning employees (complete employee objects sorted by salary)
- `GET /api/employees/getEmployeesbyTitle/{title}` - Get employees by title (with case-insensitive search) [Created by self - not in the original requirements]
- `POST /api/employees` - Add new employee
- `PUT /api/employees/update/{id}` - Update employee
- `DELETE /api/employees/id/{id}` - Delete employee

## API Response Format

All API responses are consistently formatted as JSON with this structure:

```json
{
    "data": [/* array of results or single object */],
    "status": "Successfully processed request."
}
```

For example, an employee object is formatted as:

```json
{
    "data": {
        "id": "1",
        "employee_name": "Tiger Nixon",
        "employee_salary": "320800",
        "employee_age": "61",
        "employee_title": "Vice Chair Executive Principal",
        "employee_email": "tnixon@company.com"
    },
    "status": "Successfully processed request."
}
```

## Project Workflow

The system implements the following workflows:

### Data Flow

1. **Data Initialization**:
   - Employee data is loaded from `common/employees.json` into memory when the application starts
   - The `EmployeeAPI` class manages an in-memory map of employee objects

2. **API Request Flow**:
   - Client sends HTTP request to the server
   - Server routes the request to the appropriate handler based on URL pattern
   - Handler invokes `EmployeeAPI` methods to process the request
   - Results are formatted using `ApiFormatter` for consistent JSON response structure
   - Server sends HTTP response back to client

3. **Data Persistence**:
   - When employees are added, updated, or deleted, changes are immediately persisted to `employees.json`
   - This provides basic data durability without requiring a database

### Search Capabilities

The API provides multiple search capabilities:

1. **ID-based Lookup**: Direct lookup by employee ID (O(1) operation)
2. **Name Search**: Case-insensitive substring matching on employee names
3. **Title Search**: Case-insensitive substring matching on job titles
4. **Salary Analysis**: Methods to find highest salary and top 10 earners (sorted by salary in descending order)

## Building and Running the Project

### Prerequisites

- C++17 compiler (g++ or clang++)
- libcurl development package
- pthread library

### Building

A Makefile is provided for easy building. Simply run:

```bash
make
```

This will create these executables:
- `httplib_server`: HTTP server for the employee API
- `employee_client`: HTTP client for interacting with the API
- `employee_cli`: Command-line interface for local employee management
- `api_test`: Unit tests for the API

### Running the Server and Client

To start the server:

```bash
make run-server
```

The server will start on port 8112. You can access the API at `http://localhost:8112/api/employees`.

To run the client in a separate terminal:

```bash
make run-client
```

Alternatively, use the provided shell script to run both server and client with a single command:

```bash
./run.sh
```

## API Usage Examples

### Get All Employees

```bash
curl http://localhost:8112/api/employees
```

### Get Employee by ID

```bash
curl http://localhost:8112/api/employees/id/1
```

### Search Employees by Name

```bash
curl http://localhost:8112/api/employees/search/name/Nixon
```

### Get Highest Salary

```bash
curl http://localhost:8112/api/employees/highestSalary
```

### Get Top Earners

```bash
curl http://localhost:8112/api/employees/topEarners
```

This returns the top 10 employees sorted by salary in descending order with complete employee details for each, not just names. Example response:

```json
{
    "data": [
        {
            "employee_age": "26",
            "employee_email": "yash@yash.com",
            "employee_name": "yash",
            "employee_salary": "1000000000",
            "employee_title": "Senior SDE",
            "id": "10"
        },
        /* More employees... */
    ],
    "status": "Successfully retrieved top earners"
}
```

### Get Employees by Title

```bash
curl http://localhost:8112/api/employees/getEmployeesbyTitle/Engineer
```

### Add New Employee

```bash
curl -X POST http://localhost:8112/api/employees \
  -H "Content-Type: application/json" \
  -d '{
    "employee_name": "John Doe",
    "employee_salary": "75000",
    "employee_age": "35",
    "employee_title": "Software Engineer",
    "employee_email": "jdoe@company.com"
  }'
```

### Update Employee

```bash
curl -X PUT http://localhost:8112/api/employees/update/1 \
  -H "Content-Type: application/json" \
  -d '{
    "employee_name": "Tiger Nixon",
    "employee_salary": "350000"
  }'
```

### Delete Employee

```bash
curl -X DELETE http://localhost:8112/api/employees/id/1
```

## Conclusion

This C++ Employee Management System demonstrates the implementation of a modular, layered architecture for a RESTful API service. It showcases modern C++ features, clean code organization, and best practices for structuring API services.

Key technical achievements include:

1. Separation of concerns through layered architecture
2. RESTful API design with consistent response formatting
3. Advanced search capabilities with case-insensitive matching
4. Data persistence without external database dependencies
5. Multiple interface options (CLI and HTTP)

The implementation is designed to be extensible, allowing for future enhancements such as database integration, authentication, and additional business logic.
