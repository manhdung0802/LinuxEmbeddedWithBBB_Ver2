#include "driver.h"
#include <iostream>

namespace Internal {
    void raw_i2c_write(uint8_t reg, uint8_t val) {
        std::cout << "    [Driver-PRIVATE] Ghi thanh ghi I2C 0x" 
                  << std::hex << (int)reg 
                  << " -> gia tri: 0x" << (int)val << std::dec << "\n";
    }

    uint16_t raw_i2c_read(uint8_t reg) {
        // Giả lập đọc giá trị raw từ cảm biến
        return 0x05A0 + reg * 10;
    }
}
