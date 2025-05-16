#include"serial.h"

unsigned char val[40] = { 0 }; 
int motorCurrentValue[20] = { 0 };
int motorBaseCurrentValue[20] = { 0 };
float linearAmplitute[20] = { 0 }, rotationalAmplitute[20] = { 0 }; //save value for square waveform realtime
int motorQ[20] = { 0 };
int DiffuseQ = 80;
int MaxQ = 1200000;
bool isCooling[20] = { 0 };
int last_motorBaseCurrentValue[20] = { 0 };
float tilt_motorBaseCurrentValue[20] = { 0 };

bool motorBaseSign[20] = { false }; // 记录每路基准电流的正负符号


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
    std::memset(rotationalAmplitute, 0, sizeof(rotationalAmplitute));
    std::memset(linearAmplitute, 0, sizeof(linearAmplitute));
}