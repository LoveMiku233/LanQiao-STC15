#ifndef __DS18B20__
#define __DS18B20__

#include "def.h"

sbit DQ = P1^4;

float GetTemp();


#endif