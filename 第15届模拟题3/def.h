#ifndef __DEF__
#define __DEF__

#include "STC15F2K60S2.h"
#include <stdio.h>
#include <intrins.h>
#include <string.h>

#define HC138(dev, dat) {P0 = (dat); P2 |= dev<<5; P2 &= 0x1f;}
#define LED 4
#define SMG 7
#define COM 6
#define RELAY 5


#define uint8_t unsigned char
#define uint16_t unsigned int

#include "iic.h"
#include "sonic.h"
#include "task.h"


#endif