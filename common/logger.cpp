#include "logger.h"

// Define the static member that was declared in the header
std::shared_ptr<spdlog::logger> Logger::logger = nullptr;
