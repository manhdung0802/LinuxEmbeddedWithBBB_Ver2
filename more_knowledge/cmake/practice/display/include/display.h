#pragma once
#include <string>

// ============================================================================
// [MỤC #5: OPTIONAL LIBRARY]
// Thư viện này chỉ được biên dịch khi người dùng bật:
// option(USE_DISPLAY "Enable OLED Display" ON)
// ============================================================================
namespace Display {
    void show_message(const std::string& msg);
    void render_dashboard(float temp, float pressure);
}
