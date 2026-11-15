#include "car.h"
#include "engine.h" // Nhìn thấy được vì target Car có include thư mục internal/ dạng PRIVATE
#include <iostream>

Car::Car(const std::string& model) : model_name(model) {}

void Car::start() {
    std::cout << "[Car-PUBLIC API]: Bat dau khoi dong xe " << model_name << "...\n";
    internal::inject_fuel(10);
    internal::ignite_spark_plug();
    std::cout << "[Car-PUBLIC API]: Xe da no may thanh cong!\n";
}

void Car::accelerate(int speed) {
    std::cout << "\n[Car-PUBLIC API]: Nguoi lai dap ga tang toc len " << speed << " km/h!\n";
    internal::inject_fuel(speed / 2);
}
