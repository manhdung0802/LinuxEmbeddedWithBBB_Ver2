#pragma once
#include <string>

class Car {
public:
    Car(const std::string& model);
    void start();
    void accelerate(int speed);

private:
    std::string model_name;
};
