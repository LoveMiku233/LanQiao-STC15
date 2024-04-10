#include "freq.h"

// P34 Timer1
void InitFreq() {
	AUXR &= 0x7F;
	TMOD |= 0x04;
	TL0 = 0;
	TH0 = 0;
	TR0 = 1;
	TF0 = 0;
}

uint16_t GetFreq() {
	uint16_t freq = 0;
	TR0 = 0;
	freq = ((TH0<<8) | TL0);
	TH0 = 0;
	TL0 = 0;
	TR0 = 1;
	return freq;
}
