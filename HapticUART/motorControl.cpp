#include"serial.h"

unsigned char motorPushByteValue[40] = { 0 };
int motorCurrentValue[20] = { 0 };

int uchar_to_int(unsigned char high, unsigned char low)
{
    int number = high;
    number = (number << 8) | low;
    return number;
}

void pushValue2Current() {
    motorCurrentValue[0] = uchar_to_int(motorPushByteValue[0], motorPushByteValue[1]);
}

