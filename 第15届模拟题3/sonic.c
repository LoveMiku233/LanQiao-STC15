#include "sonic.h"


// 初始化定时器0

void Timer0_Init(void)		//12微秒@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0xF4;				//设置定时初始值
	TH0 = 0xFF;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
}

uint8_t juli() {
	uint8_t unm=10, dist;
	TX = 0;
	TL0 = 0xF4;
	TH0 = 0xFF;
	TR0 = 1;
	// 12us 发送一次
	while(unm--){
		while(!TF0);
		TX^=1;
		TF0 = 0;
	}
	// 接收
	TR0 = 0;
	TL0 = 0;
	TH0 = 0;
	TR0 = 1;
	while(!TF0&&RX);
	TR0 = 0;
	if(TF0){
		TF0 = 0;
		dist = 255;
	}else{
		dist = ((TH0<<8) + TL0) * 0.017;
	}
	return dist;
}