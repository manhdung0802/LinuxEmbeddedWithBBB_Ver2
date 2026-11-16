#pragma once
#include <cstdint>

// ============================================================================
// [MỤC #13: HEADER NỘI BỘ (PRIVATE)]
// Chỉ có sensor.cpp được phép dùng file này để tương tác phần cứng I2C.
// App bên ngoài tuyệt đối KHÔNG ĐƯỢC PHÉP nhìn thấy file này!
// ============================================================================
namespace Internal {
    void raw_i2c_write(uint8_t reg, uint8_t val);
    uint16_t raw_i2c_read(uint8_t reg);
}
