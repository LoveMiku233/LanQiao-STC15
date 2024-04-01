#ifndef __DEF__
#define __DEF__


#include "STC15F2K60S2.h"
#include <stdio.h>
#include <string.h>
#include <intrins.h>

#define HC138(dev, dat) {P0 = (dat);P2|=(dev<<5);P2&=0x1b;}
#define LED 4
#define COM 6
#define SMG 7
#define RELAY 5

#define uint8_t unsigned char
#define uint16_t unsigned int

#include "key.h"
#include "ds1302.h"
#include "iic.h"
#include "task.h"


#endif