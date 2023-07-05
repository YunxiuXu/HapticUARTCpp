#pragma once

#include <cmath>
#include <vector>
#include <functional>
#include <tuple>
#include <iostream>
#include <mutex>

struct HapticFunctionCall {
    std::function<double(double, const std::tuple<float, double, double, double>&)> function;
    std::tuple<float, double, double, double> args;
};

extern std::vector<HapticFunctionCall> functionCalls;
extern std::mutex mtx; // ������������functionCallsһ��д��һ�߶�ȡ

double calculateExponential(double base, double exponent, double t);
double calculateSin(double amplitude, double frequency, double phaseShift, double t);
void addFunctionCall(const std::function<double(double, const std::tuple<float, double, double, double>&)>& function, int t0, double a, double b, double c);
float basicCollision(float t0, float L, float B, float freq, float t);