#ifndef __DEF__
#define __DEF__

#include "STC15F2K60S2.h"
#include <stdio.h>
#include <string.h>
#include <intrins.h>

#define uint8_t unsigned char
#define uint16_t unsigned int

#define HC138(dev, dat) {P0=(dat);P2|=(dev<<5);P2&=0x1f;}
#define COM 6
#define SMG 7

#include "freq.h"
#include "csb.h"
#include "task.h"


#endif