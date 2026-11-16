#include "car.h"       // Nhìn thấy được vì car/include/ được cấu hình là PUBLIC
#include "fast_math.h" // Nhìn thấy được vì fast_math/include/ là INTERFACE
#include <iostream>

// =========================================================================
// [BÀI TEST TÍNH ĐÓNG GÓI - PRIVATE]:
// Sau khi cấu hình CMake xong, bạn hãy thử bỏ comment dòng dưới đây:
// #include "engine.h"
//
// 👉 NẾU BẠN CẤU HÌNH ĐÚNG (internal/ là PRIVATE của Car):
// Trình biên dịch sẽ báo lỗi ngay lập tức:
// "fatal error: engine.h: No such file or directory"
// chứng minh App hoàn toàn không thể chạm tới header nội bộ của Car!
// =========================================================================

int main() {
	std::cout << "=========================================================\n";
	std::cout << "     DEMO THỰC HÀNH: PUBLIC / PRIVATE / INTERFACE        \n";
	std::cout << "=========================================================\n\n";

	// 1. Thao tác với thư viện Car (Qua giao diện PUBLIC)
	Car my_car("VinFast Lux A2.0");
	my_car.start();
	my_car.accelerate(90);

	// 2. Thao tác với thư viện FastMath (Được link dạng INTERFACE)
	double distance_km = 320.0;
	double fuel_used_liters = 22.5;
	double efficiency = FastMath::calculate_fuel_efficiency(distance_km, fuel_used_liters);

	std::cout << "\n[App]: Tinh toan tieu thu nhien lieu: "
		  << efficiency << " km/lit\n";

	std::cout << "\n=========================================================\n";
	return 0;
}
