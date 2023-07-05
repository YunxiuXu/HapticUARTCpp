#include <iostream>
#include <cmath>
#include "signalGenerator.h"


std::vector<HapticFunctionCall> functionCalls;//所有的震动事件写在这里

double calculateExponential(double base, double exponent, double t) {
    double result = std::pow(base, exponent * t);
    return result;
}

double calculateSin(double amplitude, double frequency, double phaseShift, double t) {
    double radians = 2.0 * 3.1415926 * frequency * t + phaseShift;
    return amplitude * std::sin(radians);
}

float basicCollision(float L, float B, float freq, float t) {
    float result = L * std::exp(-B * t) * std::sin(2 * 3.14159 * freq * t);
    return result;
}

void addFunctionCall(const std::function<double(double, const std::tuple<double, double, double>&)>& function, double a, double b, double c) {
    functionCalls.push_back(HapticFunctionCall{ function, std::make_tuple(a, b, c) });
}

