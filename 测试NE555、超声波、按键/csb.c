#include "csb.h"

void InitCsb() {
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xF4;		//设置定时初值
	TH1 = 0xFF;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
}




uint8_t GetCsb() {
	uint8_t csb = 0, cnt = 10;
	TL1 = 0xF4;		//设置定时初值
	TH1 = 0xFF;		//设置定时初值
	TR1 = 1;
	// 发送
	while(cnt--) {
		while(!TF1);
		TX ^= 1;
		TF1 = 0;
	}
	// GET
	TR1 = 0;
	TL1 = 0x00;		//设置定时初值
	TH1 = 0x00;		//设置定时初值
	TR1 = 1;
	while(!TF1 && RX);
	TR1 = 0;
	if (TF1) {
		TF1 = 0;
		csb = 255;
	} else {
		TF1 = 0;
		csb = ((TH1<<8) | TL1) * 0.0172;
	}
	return csb;
}