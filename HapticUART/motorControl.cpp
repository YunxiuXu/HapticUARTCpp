#include"serial.h"

unsigned char val[40] = { 0 }; 
int motorCurrentValue[20] = { 0 };
int motorBaseCurrentValue[20] = { 0 };
int motorQ[20] = { 0 };
int DiffuseQ = 50;
int MaxQ = 200000;

int uchar_to_int(unsigned char high, unsigned char low)
{
    int number = high;
    number = (number << 8) | low;
    return number;
}

void pushValue2Current() {
    motorCurrentValue[0] = uchar_to_int(val[0], val[1]);
}

void clearMotorCurrentValue() {
    std::memset(motorCurrentValue, 0, sizeof(motorCurrentValue));
}

void clearMotorBaseCurrentValue() {
    std::memset(motorBaseCurrentValue, 0, sizeof(motorBaseCurrentValue));
}