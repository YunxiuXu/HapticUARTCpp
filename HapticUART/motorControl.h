#pragma once



extern unsigned char val[40]; //save motor's pushing value(for interpolate). 40 for 20 motors, u8type ult output to UART
extern int motorCurrentValue[20]; //up to 20 motors, int type
extern int motorBaseCurrentValue[20]; //0x01's value will in there, can only changed by command, and no + -

void pushValue2Current();
int uchar_to_int(unsigned char high, unsigned char low);
void clearMotorCurrentValue();
<<<<<<< HEAD
void clearMotorBaseCurrentValue();
=======
void clearMotorBaseCurrentValue()
>>>>>>> a5a45c828dc6eae964681e4e13041b9fcb6f547b


