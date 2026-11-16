#pragma once
#include <string>
#include <tuple>

// ============================================================================
// [MỤC #2 & #13: GIAO DIỆN CÔNG KHAI (PUBLIC)]
// App bên ngoài sẽ include file này để sử dụng các hàm đọc cảm biến.
// ============================================================================
class Sensor {
public:
    Sensor(const std::string& sensor_name);
    void init();

    // Đọc cảm biến: trả về tuple (nhiệt độ *C, áp suất Bar)
    // Phục vụ kiểm tra tính năng C++17 Structured Binding trong App
    std::tuple<float, float> read_data();

private:
    std::string name;
};
