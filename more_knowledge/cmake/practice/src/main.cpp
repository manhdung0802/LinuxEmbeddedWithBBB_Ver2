#include <iostream>
#include "config.h"
#include "math_utils.h"

#ifdef USE_LOGGER
#include "logger.h"
#endif

#ifdef USE_FMT
#include <fmt/core.h>
#endif

int main() {
    std::cout << "Practice App v"
              << APP_VERSION_MAJOR << "."
              << APP_VERSION_MINOR << "."
              << APP_VERSION_PATCH << std::endl;

#ifdef USE_LOGGER
    logger::info("Logger library is ENABLED");
#else
    std::cout << "Logger library is DISABLED" << std::endl;
#endif

    int sum = math_utils::add(10, 5);
    int product = math_utils::multiply(10, 5);

    std::cout << "10 + 5 = " << sum << std::endl;
    std::cout << "10 * 5 = " << product << std::endl;

#ifdef USE_FMT
    fmt::print("Formatted with fmt submodule: sum={}, product={}\n", sum, product);
#endif

    return 0;
}

