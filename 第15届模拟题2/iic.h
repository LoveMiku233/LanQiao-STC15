#ifndef __AD__
#define __AD__

#include "def.h"

uint8_t GetAD(uint8_t channel);
void SetAD(uint8_t dat);

sbit scl = P2^0;
sbit sda = P2^1;


#endif
