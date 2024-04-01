#ifndef __DS1302__
#define __DS1302__

#include "def.h"

void SetDs1302(uint8_t *time);
void GetDs1302(uint8_t *time);

sbit SCK = P1^7;
sbit SDA = P2^3;
sbit RST = P1^3;

#endif