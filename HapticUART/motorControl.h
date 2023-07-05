#pragma once



extern unsigned char motorPushByteValue[40]; //save motor's pushing value(for interpolate). 40 for 20 motors, u8type
extern int motorCurrentValue[20]; //up to 10 motors, int type

void pushValue2Current();
int uchar_to_int(unsigned char high, unsigned char low);




