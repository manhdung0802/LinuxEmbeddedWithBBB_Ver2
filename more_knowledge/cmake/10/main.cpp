#include <iostream>
#include <vector>
#include <chrono>

// ============================================================================
// 1. PHẦN KIỂM TRA: -Wall, -Wextra, -Werror
// ============================================================================

// Hàm này có tham số 'unused_param' không được dùng đến:
// - -Wall: KHÔNG bắt lỗi này (bỏ qua).
// - -Wextra: BẮT ĐƯỢC cảnh báo "unused parameter 'unused_param'".
// - -Werror: Sẽ biến cảnh báo trên thành LỖI (build fail ngay lập tức).
void check_warnings(int active_val, int unused_param) {
    std::cout << "Gia tri hop le: " << active_val << "\n";
}

// ============================================================================
// 2. PHẦN KIỂM TRA HIỆU NĂNG: -O0 vs -O1 vs -O2 vs -O3 vs -Os
// ============================================================================
void run_optimization_benchmark() {
    std::cout << "\n>>> Dang chay benchmark tinh toan tren 10 trieu phan tu...\n";

    const int N = 10'000'000;
    std::vector<float> a(N, 1.5f), b(N, 2.5f), c(N, 0.0f);

    auto start = std::chrono::high_resolution_clock::now();

    // 10 vòng lặp tính toán đại số mảng để thấy rõ sức mạnh của tối ưu hóa
    for (int rep = 0; rep < 10; ++rep) {
        for (int i = 0; i < N; ++i) {
            c[i] = a[i] * b[i] + 0.1f;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << ">>> Ket qua c[0]: " << c[0] << "\n";
    std::cout << ">>> THOI GIAN THUC THI: " << elapsed.count() << " ms\n";
}

int main() {
    std::cout << "=========================================================\n";
    std::cout << "    DEMO: COMPILER WARNING FLAGS & OPTIMIZATION LEVELS   \n";
    std::cout << "=========================================================\n\n";

    // --- A. Kiểm tra mức độ tối ưu hóa qua Macro nội tại của GCC/Clang ---
#if defined(__OPTIMIZE_SIZE__)
    std::cout << "[CHE DO TOI UU]: Dang dung -Os (Toi uu dung luong nho nhat cho Embedded)\n";
#elif defined(__OPTIMIZE__)
    std::cout << "[CHE DO TOI UU]: Dang dung -O1, -O2 hoac -O3 (Co toi uu hoa)\n";
#else
    std::cout << "[CHE DO TOI UU]: Dang dung -O0 (Khong toi uu - Mac dinh cua Debug)\n";
#endif

#ifdef NDEBUG
    std::cout << "[BUILD TYPE]: RELEASE MODE (-DNDEBUG)\n";
#else
    std::cout << "[BUILD TYPE]: DEBUG MODE\n";
#endif

    std::cout << "\n---------------------------------------------------------\n";

    // --- B. Kích hoạt các tình huống để thử nghiệm cờ cảnh báo ---
    
    // Tình huống 1: Biến cục bộ không dùng
    // -> -Wall sẽ cảnh báo: "unused variable 'unused_local_var'"
    // -> Nếu có -Werror: Compilation FAILED!
    int unused_local_var = 100;

    // Tình huống 2: Gọi hàm có tham số không dùng
    // -> Chỉ -Wextra mới phát hiện và cảnh báo
    check_warnings(42, 99);

    std::cout << "---------------------------------------------------------\n";

    // --- C. Chạy benchmark đo thời gian ---
    run_optimization_benchmark();

    std::cout << "\n=========================================================\n";
    return 0;
}
