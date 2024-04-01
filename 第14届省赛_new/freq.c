#include "freq.h"

void InitFreq() {
	TMOD |= 0x04;
	TH0 = 0;
	TL0 = 0;
	TR0 = 1;
}


uint16_t GetFreq() {
	uint16_t freq = 0;
	TR0 = 0;
	freq = (TH0 << 8) | TL0;
	TH0 = 0;
	TL0 = 0;
	TR0 = 1;
	return freq;
}