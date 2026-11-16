#include <iostream>
#include <thread>      // [MỤC #9: find_package(Threads REQUIRED)]
#include <string_view>

#include "config.h"    // [MỤC #4: configure_file()]
#include "sensor.h"    // [MỤC #2 & #13: PUBLIC Header từ thư viện Sensor]
#include "timer.h"     // [MỤC #13: INTERFACE Header-only từ Utils]

// [MỤC #5: OPTIONAL LIBRARY]
// Chỉ include khi cờ USE_DISPLAY được bật trong CMake option()
#ifdef USE_DISPLAY
#include "display.h"
#endif

// ============================================================================
// [THỬ NGHIỆM TÍNH ĐÓNG GÓI - MỤC #13: PRIVATE]
// Sau khi viết xong CMake, hãy thử bỏ comment dòng dưới đây:
// #include "driver.h"
//
// 👉 NẾU BẠN CẤU HÌNH ĐÚNG (internal/ là PRIVATE của Sensor):
// Trình biên dịch sẽ báo lỗi: "driver.h: No such file or directory"
// ============================================================================

int main() {
    Utils::print_timestamp("INIT");
    std::cout << "\n=========================================================\n";
    std::cout << "      HE THONG IOT EMBEDDED SENSOR GATEWAY (CMAKE PRO)   \n";
    std::cout << "=========================================================\n";

    // 1. Kiểm tra Version được sinh ra từ CMake (Mục #4)
    std::cout << "[PHAT HANH] Phien ban: " << PROJECT_VERSION_STRING 
              << " (Major: " << PROJECT_VERSION_MAJOR 
              << ", Minor: " << PROJECT_VERSION_MINOR 
              << ", Patch: " << PROJECT_VERSION_PATCH << ")\n";

    // 2. Kiểm tra Kiến trúc biên dịch (Mục #11: Toolchain)
#if defined(__arm__)
    std::cout << "[KIEN TRUC]: ARM 32-bit (Target BeagleBone Black Cortex-A8)\n";
#elif defined(__x86_64__)
    std::cout << "[KIEN TRUC]: x86_64 (Host PC / Laptop)\n";
#else
    std::cout << "[KIEN TRUC]: Khac\n";
#endif

    // 3. Kiểm tra Build Type (Mục #10: Cờ tối ưu)
#ifdef NDEBUG
    std::cout << "[BUILD TYPE]: RELEASE MODE (-O2/-O3) -> Toi uu toi da\n";
#else
    std::cout << "[BUILD TYPE]: DEBUG MODE (-O0 -g) -> Phuc vu go loi\n";
#endif

    // 4. Kiểm tra Macro tùy biến (Mục #10: target_compile_definitions)
#ifdef SENSOR_CALIBRATED
    std::cout << "[CALIBRATION]: Cam bien da duoc hieu chuan san!\n";
#else
    std::cout << "[CALIBRATION]: Che do mac dinh (Chua hieu chuan)\n";
#endif

    std::cout << "---------------------------------------------------------\n";

    // 5. Khởi tạo cảm biến (Mục #2 & #13: Thư viện tĩnh/động)
    Sensor bme_sensor("BME280_I2C");
    bme_sensor.init();

    float temp_result = 0.0f;
    float press_result = 0.0f;

    // 6. Chạy Thread đọc dữ liệu song song (Mục #9: Threads)
    // Đòi hỏi phải tìm gói Threads và link Threads::Threads
    Utils::print_timestamp("WORKER");
    std::cout << "Khoi tao luong doc du lieu song song...\n";
    
    std::thread reader_thread([&]() {
        // [MỤC #10: C++17 Structured Binding]
        // Yêu cầu CMAKE_CXX_STANDARD 17
        auto [t, p] = bme_sensor.read_data();
        temp_result = t;
        press_result = p;
    });

    reader_thread.join(); // Đợi luồng đọc xong

    // 7. Hiển thị kết quả ra màn hình (Mục #5: Optional Lib)
#ifdef USE_DISPLAY
    std::cout << "\n[FEATURE]: Thu vien OLED Display dang BAT:\n";
    Display::render_dashboard(temp_result, press_result);
#else
    std::cout << "\n[FEATURE]: Thu vien OLED Display dang TAT (Tiet kiem ROM/RAM):\n";
    std::cout << "  -> Nhiet do do duoc: " << temp_result << " *C\n";
    std::cout << "  -> Ap suat do duoc : " << press_result << " Bar\n";
#endif

    Utils::print_timestamp("FINISH");
    std::cout << "He thong hoat dong thanh cong hoan hao!\n";
    std::cout << "=========================================================\n";
    return 0;
}
