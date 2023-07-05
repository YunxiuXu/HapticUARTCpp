#include <iostream>
#include <cmath>
#include <mutex>
#include "signalGenerator.h"


std::vector<HapticFunctionCall> functionCalls;//所有的震动事件写在这里
std::mutex mtx;

double calculateExponential(double base, double exponent, double t) {
    double result = std::pow(base, exponent * t);
    return result;
}

double calculateSin(double amplitude, double frequency, double phaseShift, double t) {
    double radians = 2.0 * 3.1415926 * frequency * t + phaseShift;
    return amplitude * std::sin(radians);
}

float basicCollision(float t0, float L, float B, float freq, float t) {
    t = t - t0;
    float result = L * std::exp(-B * t) * std::sin(2 * 3.14159 * freq * t);
    return result;
}


void addFunctionCall(const std::function<double(double, const std::tuple<float, double, double, double>&)>& function, int t0, double a, double b, double c) {
    std::lock_guard<std::mutex> lock(mtx);  // 在这个作用域中锁定互斥量
    functionCalls.push_back(HapticFunctionCall{ function, std::make_tuple(t0, a, b, c) });
}  // 退出作用域时，析构函数自动解锁互斥量

