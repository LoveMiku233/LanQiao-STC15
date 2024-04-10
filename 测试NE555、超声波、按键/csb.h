#ifndef _CSB_
#define _CSB_

#include "def.h"

sbit TX = P1^0;
sbit RX = P1^1;


void InitCsb();
uint8_t GetCsb();


#endif