#include "sensor.h"
#include "driver.h" // Nhìn thấy driver.h vì nội bộ thư viện sensor được include PRIVATE
#include <iostream>
#include <cmath>    // [MỤC #8]: Yêu cầu liên kết với thư viện toán học libm (find_library)

Sensor::Sensor(const std::string& sensor_name) : name(sensor_name) {}

void Sensor::init() {
    std::cout << "  [Sensor-PUBLIC] Khoi tao cam bien: " << name << "...\n";
    Internal::raw_i2c_write(0x00, 0x01); // Lệnh reset
    Internal::raw_i2c_write(0x0F, 0x20); // Cấu hình chế độ đo liên tục
}

std::tuple<float, float> Sensor::read_data() {
    uint16_t raw_temp = Internal::raw_i2c_read(0x01);
    uint16_t raw_press = Internal::raw_i2c_read(0x02);

    // Tính toán có sử dụng std::sqrt và std::pow từ libm (-lm)
    float temp_c = static_cast<float>(raw_temp) / 50.0f;
    float pressure_bar = std::sqrt(std::pow(static_cast<float>(raw_press) / 10.0f, 2.0f)) / 100.0f;

    return {temp_c, pressure_bar};
}
