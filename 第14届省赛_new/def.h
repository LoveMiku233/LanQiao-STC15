#ifndef __def__
#define __def__


#include "STC15F2K60S2.h"
#include <intrins.h>
#include <stdio.h>
#include <string.h>

#define uint8_t unsigned char
#define uint16_t unsigned int
#define HC138(drv, dat) {P0=(dat);P2|=(drv<<5);P2&=0x1f;}
#define LED 4
#define SMG 7
#define COM 6
#define RELAY 5

#include "smg.h"
#include "key.h"
#include "onewire.h"
#include "ds1302.h"
#include "freq.h"
#include "iic.h"
#include "task.h"


#endif