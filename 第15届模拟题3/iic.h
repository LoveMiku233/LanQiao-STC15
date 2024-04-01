#ifndef __IIC__
#define __IIC__

#include "def.h"

sbit sda = P2^1;
sbit scl = P2^0;

uint8_t GetAd();

#endif