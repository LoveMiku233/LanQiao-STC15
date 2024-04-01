#include "task.h"

uint8_t display_buff[10] = "";
code unsigned char Seg_Table[] =  
{ 
0xc0,  
0xf9,  
0xa4,  
0xb0,  
0x99,  
0x92,  
0x82,  
0xf8,  
0x80,  
0x90,  
0x88,  
0x83,  
0xc6,  
0xa1,  
0x86,  
0x8e  
};

uint8_t cnt_key = 0;
uint8_t cnt_dis = 0;
uint8_t cnt_temp = 0;
uint8_t cnt_light = 0;
uint8_t cnt_out = 0;
uint8_t cnt_led = 0;

uint8_t mode_bk = 0;
uint8_t mode = 0;
float temp = 0;
uint16_t temp_bk = 0;
uint8_t light = 0;
uint8_t ad = 0;

void InitTick() {
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x20;		//设置定时初值
	T2H = 0xD1;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
	IE2 |= 0x04;
	
}

void Delay500ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 23;
	j = 205;
	k = 120;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void InitBsp() {
	HC138(4, 0xff);
	HC138(5, 0x00);
	InitKey();
	GetTemp();
	Delay500ms();
	EA = 1;
}

void IsrTask() interrupt 12 { 
	_display();
	if (cnt_key < 8) cnt_key++;
	if (cnt_dis < 50) cnt_dis++;
	if (cnt_temp < 200) cnt_temp++;
	if (cnt_light < 200) cnt_light++;
	if (cnt_out < 250) cnt_out++;
	if (cnt_led < 250) cnt_led++;
} 

void TaskKey() {
	switch(GetKeyStruct()->key_value) {
		case 2:
			if (mode != 2) {
				if(mode != 1) mode++; else mode = 0;
				mode_bk = mode;
			}
			GetKeyStruct()->key_value = 255;
			break;
		case 1:
			if (mode == 2) mode = mode_bk;
			else mode = 2;
			GetKeyStruct()->key_value = 255;
			break;
	}
}

void TaskDisplay() {
	if(cnt_dis < 50) return;
	cnt_dis = 0;
	
	switch(mode) {
		case 0:
			sprintf(display_buff, "1    %2.1f", temp);
			break;
		case 1:
			sprintf(display_buff, "2    %3bu", light);
			break;
		case 2:
			sprintf(display_buff, "U     %.1f", ad * 0.019);
			break;
	}
}

void TaskCollect() {
	if (cnt_key == 8) {
		cnt_key = 0;
		GetKeyStruct()->KeyScan();
	}
	if (cnt_light == 200) {
		cnt_light = 0;
		light = GetAD(1);
	}
	if (cnt_temp == 200) {
		cnt_temp = 0;
		temp = GetTemp();
	}
	
	
}

void TaskControl() {
	if (mode_bk == 0){
		if (temp >= 10 && temp <= 40) {
			ad = (uint8_t)((2 * temp - 5) / 15 / 0.019);
		}else {
			ad = 52;
		}
	}else if (mode_bk == 1) {
		if (light >= 10 && light <= 240) {
			ad = (uint8_t)((2 * light + 95) / 115 / 0.019);
		} else {
			ad = 52;
		}
	} 
	
	if (cnt_out == 250) {
		cnt_out = 0;
		SetAD(ad);
	}
	
	if (cnt_led == 250) {
		cnt_led = 0;
		if(mode_bk == 0) {
			HC138(LED, ~(0x01));
		}else if(mode_bk == 1) {
			HC138(LED, ~(0x02));
		}
	}
	
}



void _display() {
	static uint8_t buff_idx = 0;
	static uint8_t smg_idx  = 0;
	
	uint8_t ch = display_buff[buff_idx];
	uint8_t dat = 0xff;
	if (ch >= '0' && ch <= '9') dat = Seg_Table[ch - '0'];
	else if (ch >= 'A' && ch <= 'F') dat = Seg_Table[ch - 'A' + 10];
	else if (ch == '-') dat = 0xbf;
	else if (ch == 'U') dat = 0xc1;
	
	if (display_buff[buff_idx + 1] == '.') {
		dat &= 0x7f;
		buff_idx++;
	}
	HC138(COM, 1<<smg_idx);
	HC138(SMG, dat);
	
	buff_idx++; smg_idx++;
	if (display_buff[buff_idx] == '\0' || smg_idx == 8) {
		buff_idx = 0;
		smg_idx = 0;
	}
	
}