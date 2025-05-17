#ifndef LOGGER_H
#define LOGGER_H

#include "../include/spdlog/spdlog.h"
#include "../include/spdlog/sinks/stdout_color_sinks.h"
#include "../include/spdlog/sinks/rotating_file_sink.h"
#include "../include/spdlog/sinks/daily_file_sink.h"
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <sys/stat.h>

// Logger component types
enum class LogComponent {
    API,     // Core API operations
    SERVER,  // HTTP server
    CLIENT,  // Client operations
    TEST     // Unit tests
};

class Logger {
private:
    static std::shared_ptr<spdlog::logger> logger;
    static bool ensureLogDirExists(const std::string& logDir) {
        // Create log directory if it doesn't exist
        std::filesystem::path dir(logDir);
        if (!std::filesystem::exists(dir)) {
            try {
                std::filesystem::create_directories(dir);
                return true;
            } catch (const std::exception& e) {
                std::cerr << "Error creating log directory: " << e.what() << std::endl;
                return false;
            }
        }
        return true;
    }
    
public:
    static void init(LogComponent component = LogComponent::API) {
        // Ensure logs directory exists
        std::string logDir = "logs";
        ensureLogDirExists(logDir);
        
        // Determine log file name based on component
        std::string loggerName;
        std::string logFile;
        
        switch (component) {
            case LogComponent::SERVER:
                loggerName = "server";
                logFile = logDir + "/server.log";
                break;
            case LogComponent::CLIENT:
                loggerName = "client";
                logFile = logDir + "/client.log";
                break;
            case LogComponent::TEST:
                loggerName = "test";
                logFile = logDir + "/test.log";
                break;
            case LogComponent::API:
            default:
                loggerName = "api";
                logFile = logDir + "/api.log";
                break;
        }
        
        // Create console sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::info);
        
        // Create file sink - 5MB size, 3 rotated files
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            logFile, 5 * 1024 * 1024, 3);
        file_sink->set_level(spdlog::level::trace);

        // Create logger with both sinks
        std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
        logger = std::make_shared<spdlog::logger>(loggerName, sinks.begin(), sinks.end());
        
        // Set log pattern: [time] [level] [component] [message]
        logger->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] [%l] [" + loggerName + "] %v%$");
        logger->set_level(spdlog::level::trace);
        
        // Set flush policy - flush after every log message to ensure logs are written
        logger->flush_on(spdlog::level::trace);
        
        // Set automatic flushing every 3 seconds for buffered logs
        spdlog::flush_every(std::chrono::seconds(3));
        
        // Register the logger with spdlog
        spdlog::register_logger(logger);
        
        // Log initialization complete
        logger->info("Logger initialized for component: {}", loggerName);
    }
    
    static std::shared_ptr<spdlog::logger>& get() {
        if (!logger) {
            init();
        }
        return logger;
    }
    
    // Initialize for a specific component
    static void initForComponent(LogComponent component) {
        init(component);
    }
    
    // Log a message with the specified component
    template<typename... Args>
    static void log(spdlog::level::level_enum level, const char* fmt, const Args&... args) {
        get()->log(level, fmt, args...);
    }
    
    // Convenience methods for different log levels
    template<typename... Args>
    static void trace(const char* fmt, const Args&... args) {
        get()->trace(fmt, args...);
    }
    
    template<typename... Args>
    static void debug(const char* fmt, const Args&... args) {
        get()->debug(fmt, args...);
    }
    
    template<typename... Args>
    static void info(const char* fmt, const Args&... args) {
        get()->info(fmt, args...);
    }
    
    template<typename... Args>
    static void warn(const char* fmt, const Args&... args) {
        get()->warn(fmt, args...);
    }
    
    template<typename... Args>
    static void error(const char* fmt, const Args&... args) {
        get()->error(fmt, args...);
    }
    
    template<typename... Args>
    static void critical(const char* fmt, const Args&... args) {
        get()->critical(fmt, args...);
    }
    
    // Cleanup
    static void shutdown() {
        if (logger) {
            logger->flush();
        }
        spdlog::shutdown();
    }
};

// Static member is defined in logger.cpp

#endif // LOGGER_H
