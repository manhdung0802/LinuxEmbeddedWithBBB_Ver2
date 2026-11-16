#pragma once
#include <chrono>
#include <iostream>
#include <string_view>

// ============================================================================
// [MỤC #13: THƯ VIỆN HEADER-ONLY (INTERFACE)]
// Thư viện này không có file .cpp nào để biên dịch.
// Bản thân nó không cần compile, người khác link INTERFACE tới nó sẽ tự nhận include.
// ============================================================================
namespace Utils {
    inline void print_timestamp(std::string_view tag) {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        std::cout << "[" << tag << " @ " << ms << " ms] ";
    }
}
