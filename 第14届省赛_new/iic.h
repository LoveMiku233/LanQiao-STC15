#ifndef __IIC__
#define __IIC__

#include "def.h"

sbit sda = P2^1;
sbit scl = P2^0;

unsigned char GetADC(unsigned char channel);
#endif