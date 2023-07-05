#pragma once

#include <cmath>
#include <vector>
#include <functional>
#include <tuple>
#include <iostream>
#include <mutex>

struct HapticFunctionCall {
    std::function<double(double, const std::tuple<double, double, double>&)> function;
    std::tuple<double, double, double> args;
};

extern std::vector<HapticFunctionCall> functionCalls;
extern std::mutex mtx; // 互斥锁，保护functionCalls一边写入一边读取

double calculateExponential(double base, double exponent, double t);
double calculateSin(double amplitude, double frequency, double phaseShift, double t);
void addFunctionCall(const std::function<double(double, const std::tuple<double, double, double>&)>& function, double a, double b, double c);
float basicCollision(float L, float B, float freq, float t);