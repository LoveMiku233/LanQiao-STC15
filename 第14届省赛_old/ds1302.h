#ifndef __DS1302__
#define __DS1302__

#include <STC15F2K60S2.h>
#include <intrins.h>

void set_time(unsigned char *time);
void get_time(unsigned char *time);

#endif