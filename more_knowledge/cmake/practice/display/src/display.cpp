#include "display.h"
#include <iostream>

namespace Display {
    void show_message(const std::string& msg) {
        std::cout << "  [Display OLED] >>> " << msg << " <<<\n";
    }

    void render_dashboard(float temp, float pressure) {
        std::cout << "  +--------------------------------+\n";
        std::cout << "  |       OLED SENSOR DISPLAY      |\n";
        std::cout << "  | Nhiet do: " << temp << " *C             |\n";
        std::cout << "  | Ap suat : " << pressure << " Bar            |\n";
        std::cout << "  +--------------------------------+\n";
    }
}
