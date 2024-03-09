#include <STC15F2K60S2.h>
#include <intrins.h>
#include <stdio.h>
#include "ds1302.h"

#define uint8_t unsigned char
#define uint16_t unsigned int
#define func(x) do{x}while(0)
#define HC138(x, dat) func(P2&=0x1f;P2|=(x<<5);P0=dat;)



uint8_t update_signal = 0;
code unsigned char Seg_Table[] =  
{ 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e}; 
uint8_t str[10] = "ABCDEF-9.";
uint8_t time[3] = {13, 3, 5};
uint8_t mode = 1;
uint8_t callback_mode = 1;
uint16_t freq = 0;
float temp = 0, humi = 0, avg_temp = 0, avg_humi = 0;
uint8_t max_temp = 0, max_humi = 0;
uint8_t cnt = 0;
uint8_t last_time[2] = {0, 0};
uint8_t tmp_temp = 30;

void display();
void Timer0Init(void);
void read_freq();
void Timer1Init(void);
void display_time();
void display_freq();
void display_callback();
void display_parma();
void display_temphumi();
void Delay10ms();
uint8_t get_key();
void key_proc();

void init() {
	HC138(5, 0x00);
	HC138(4, 0xff);
	set_time(time);
	Timer0Init();
	Timer1Init();
	EA = 1;
}

void main() {
	init();
	while(1){
		key_proc();
		switch(mode) {
		case 1: display_time(); break;
		case 2: display_callback(); break;
		case 3: display_parma(); break;
		case 9: display_temphumi(); break;
		}
	}
}


void T1_ISR() interrupt 3 {
	static uint16_t t_ms = 0, f_ms = 0;
	// 数码管刷新
	display();
	if(++t_ms == 500) {
		t_ms = 0;
		update_signal ^= 1;
	}
	// 1S 测量频率
	if(++f_ms == 1000) {
		f_ms = 0;
		read_freq();
	}
	
}

void Timer0Init(void) {
	TMOD |= 0x04;
	TH0 = 0;
	TL0 = 0;
	TR0 = 1;
}

void Timer1Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x18;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1;
}


void read_freq() {
	TR0 = 0;
	freq = (TH0<<8) | TL0;
	humi = freq * 0.045;
	TH0 = 0;
	TL0 = 0;
	TR0 = 1;
}

void display_time() {
	if(update_signal) {
		get_time(time);
		sprintf(str, "%02bu-%02bu-%02bu", time[0], time[1], time[2]);
	}
}

void display_freq() {
	if(update_signal) {
		sprintf(str, "%8u", freq);
	}
}

void display_callback() {
	if(update_signal) {
		if(callback_mode == 1) {
			sprintf(str, "C %02bu-%02.1f", max_temp, avg_temp);
		}else if(callback_mode == 2){
			sprintf(str, "H %02bu-%02.1f", max_humi, avg_humi);
		}else if(callback_mode == 3){
			sprintf(str, "F%02bu%02bu-%02bu", last_time[0], last_time[1]);
		}
	}
}
void display_parma() {
	if(update_signal) {
		sprintf(str, "P     %02bu", tmp_temp);
	}
}
void display_temphumi() {
	if(update_signal) {
		sprintf(str, "E  %02.0f-%02.0f", temp, humi);
	}
}

void key_proc() {
	uint8_t key = get_key();
	if(key == 4){
		if(mode == 3) mode = 1;
		else if(mode != 9) mode++;	
	}else if(key == 5){
		if(mode == 2) {
			if(callback_mode == 3) callback_mode = 1;
			else callback_mode++;
		}
	}else if(key == 8) {
		if(mode == 3) {
			if(tmp_temp != 99) tmp_temp++;
		}
	}else if(key == 9) {
		if(mode == 3) {
			if(tmp_temp != 0) tmp_temp--;
		}
	}
}

uint8_t get_key() {
	uint8_t k1 = 0, k2 = 0;
	P3 &= 0xf0; P42 = 1; P44 = 1;
	if(!P44 || !P42) {
		Delay10ms();
		if(!P44 || !P42) {
			if(!P44) k1 = 7;
			else if(!P42) k1 = 11;
			
			P3 |= 0x0f; P42 = 0; P44 = 0;
			if((P3 & 0x01) == 0) k2 = 0;
			else if((P3 & 0x02) == 0) k2 = 1;
			else if((P3 & 0x04) == 0) k2 = 2;
			else if((P3 & 0x08) == 0) k2 = 3;
			while((P3 & 0x0f) != 0x0f);
			return k1-k2;
		}
	} 
	return 0;
}

void display() {
	static uint8_t str_idx = 0, smg_idx = 0;
	uint8_t cn = str[str_idx];
	uint8_t dat = 0xff;
	if(cn >= '0' && cn <= '9') dat = Seg_Table[cn - '0'];
	else if(cn >= 'A' && cn <= 'F') dat = Seg_Table[cn - 'A' + 10];
	else if(cn == ' ') dat = 0xff;
	else if(cn == '-') dat = 0xbf;
	else if(cn == 'H') dat = 0x89;
	else if(cn == 'P') dat = 0x8c;
	if(str[str_idx + 1] == '.') {
		str_idx++;
		dat &= 0x7f;
	}
	HC138(6, 0x01<<smg_idx);
	HC138(7, dat);
	
	str_idx++; smg_idx++;
	if(smg_idx == 8 || str[str_idx] == '\0') {
		str_idx = 0; smg_idx = 0;
	}
}
void Delay10ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 117;
	j = 184;
	do
	{
		while (--j);
	} while (--i);
}