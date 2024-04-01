#ifndef __DS1302__
#define __DS1302__

#include "def.h"

typedef struct {
	uint8_t set_time[3];
	uint8_t get_time[3];
	void (code *GetTime)(void);
}rtc_t;

void InitRtc();
rtc_t* GetRtcStruct();

sbit SCK = P1^7;
sbit RST = P1^3;
sbit SDA = P2^3;

#endif