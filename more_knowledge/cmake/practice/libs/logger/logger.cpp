#include "logger.h"
#include <iostream>

namespace logger {
void info(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}
}

