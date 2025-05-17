#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <string>
#include "logger.h"

class Timer {
private:
    std::string operationName;
    std::chrono::high_resolution_clock::time_point startTime;
    
public:
    Timer(const std::string& operation, LogComponent component = LogComponent::API) 
        : operationName(operation) {
        // component parameter is kept for API compatibility but not used
        (void)component;
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    ~Timer() {
        // Auto-log when the timer goes out of scope
        stop();
    }
    
    // Explicitly stop and log the timer
    void stop() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        // Log the operation duration
        Logger::info("PERF: '{}' operation completed in {} ms", operationName, duration);
    }
};

// Macro for timing a scope
#define TIME_OPERATION(name, component) Timer timer##__LINE__(name, component)

#endif // TIMER_H
