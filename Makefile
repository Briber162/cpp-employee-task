CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I. -I./include -I./common -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
LDFLAGS = -pthread -lcurl

# Source file paths
COMMON_DIR = ./common
SERVER_DIR = ./server
CLIENT_DIR = ./client
INCLUDE_DIR = ./include

# Source files
COMMON_SOURCES = $(COMMON_DIR)/employee.cpp $(COMMON_DIR)/employee_api.cpp $(COMMON_DIR)/logger.cpp
CLI_SOURCES = $(CLIENT_DIR)/main.cpp $(COMMON_SOURCES)
HTTPLIB_SERVER_SOURCES = $(SERVER_DIR)/httplib_server.cpp $(COMMON_SOURCES)
CLIENT_SOURCES = $(CLIENT_DIR)/client.cpp $(COMMON_DIR)/logger.cpp
API_TEST_SOURCES = api_test.cpp $(COMMON_SOURCES)

# Object files
CLI_OBJECTS = $(CLI_SOURCES:.cpp=.o)
HTTPLIB_SERVER_OBJECTS = $(HTTPLIB_SERVER_SOURCES:.cpp=.o)
CLIENT_OBJECTS = $(CLIENT_SOURCES:.cpp=.o)
API_TEST_OBJECTS = $(API_TEST_SOURCES:.cpp=.o)

# Executables
CLI_TARGET = employee_cli
HTTPLIB_SERVER_TARGET = httplib_server
CLIENT_TARGET = employee_client
API_TEST_TARGET = api_test

all: $(CLI_TARGET) $(HTTPLIB_SERVER_TARGET) $(CLIENT_TARGET) $(API_TEST_TARGET)

# CLI tool
$(CLI_TARGET): $(CLI_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# HTTPLIB server
$(HTTPLIB_SERVER_TARGET): $(HTTPLIB_SERVER_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# API client using libcurl
$(CLIENT_TARGET): $(CLIENT_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# API test program
$(API_TEST_TARGET): $(API_TEST_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(COMMON_DIR)/*.o $(SERVER_DIR)/*.o $(CLIENT_DIR)/*.o *.o \
	$(CLI_TARGET) $(HTTPLIB_SERVER_TARGET) $(CLIENT_TARGET) $(API_TEST_TARGET)

# Clean only log files (use with: make clean-logs)
clean-logs:
	@echo "Removing log files..."
	rm -f logs/*.log

# Clean everything - both build artifacts and logs (use with: make clean-all)
clean-all: clean clean-logs
	@echo "All build artifacts and logs removed."

# Run the server (use with: make run-server)
run-server: $(HTTPLIB_SERVER_TARGET)
	@echo "Starting server on port 8112..."
	@./$(HTTPLIB_SERVER_TARGET)

# Run the client (use with: make run-client)
run-client: $(CLIENT_TARGET)
	@echo "Starting client..."
	@./$(CLIENT_TARGET)

# Run simple API tests (original tests)
run-tests: $(API_TEST_TARGET)
	@echo "Running simple API tests..."
	@./$(API_TEST_TARGET)

# CMocka test program
CMOCKA_TEST_TARGET = cmocka_test
CMOCKA_TEST_SOURCES = cmocka_test.cpp $(COMMON_SOURCES)

# Build CMocka tests
$(CMOCKA_TEST_TARGET): $(CMOCKA_TEST_SOURCES)
	$(CXX) $(CXXFLAGS) -I/opt/homebrew/opt/cmocka/include -o $@ $^ $(LDFLAGS) -L/opt/homebrew/opt/cmocka/lib -lcmocka

# Run CMocka-based unit tests
run-cmocka-tests: $(CMOCKA_TEST_TARGET)
	@echo "Running CMocka unit tests..."
	@./$(CMOCKA_TEST_TARGET)

.PHONY: all clean run-server run-client run-tests
