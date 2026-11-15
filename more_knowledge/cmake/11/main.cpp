#include <iostream>

int main() {
    std::cout << "=========================================================\n";
    std::cout << "          DEMO: CROSS-COMPILATION & TOOLCHAIN            \n";
    std::cout << "=========================================================\n\n";

    // 1. Kiểm tra Kiến trúc CPU đích được biên dịch
#if defined(__arm__)
    std::cout << "[KIEN TRUC]: ARM 32-bit (Phu hop cho BeagleBone Black - Cortex-A8)\n";
    #if defined(__ARM_NEON)
        std::cout << "-> Da kich hoat ho tro phan cung NEON SIMD!\n";
    #endif
#elif defined(__aarch64__)
    std::cout << "[KIEN TRUC]: ARM 64-bit (AArch64)\n";
#elif defined(__x86_64__)
    std::cout << "[KIEN TRUC]: x86_64 (May tinh ca nhan - Host PC)\n";
#elif defined(__i386__)
    std::cout << "[KIEN TRUC]: x86 32-bit\n";
#else
    std::cout << "[KIEN TRUC]: Chua xac dinh\n";
#endif

    // 2. Kích thước con trỏ (32-bit vs 64-bit)
    std::cout << "[BO NHO]: Kich thuoc con tro (Pointer Size) = " 
              << sizeof(void*) * 8 << " bit\n";

    // 3. Thông tin phiên bản trình biên dịch đã dùng để build file này
    std::cout << "[TRINH BIEN DICH]: " << __VERSION__ << "\n";

    std::cout << "\n=========================================================\n";
    return 0;
}
