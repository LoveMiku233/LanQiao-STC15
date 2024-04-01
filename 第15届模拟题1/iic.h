#ifndef __IIC__
#define __IIC__

#include "def.h"

void GetRom(uint8_t* dat);
void SetRom(uint8_t* dat);

sbit scl = P2^0;
sbit sda = P2^1;

#endif