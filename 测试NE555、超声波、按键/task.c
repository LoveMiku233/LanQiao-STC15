#include "task.h"

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


uint8_t display_buff[10] = "12345678.";
// tick
uint8_t cnt_dis = 0;
uint16_t cnt_freq = 0;
uint16_t cnt_csb = 0;
uint8_t cnt_key = 0;
uint16_t cnt_key_long = 0;

uint8_t mode = 1;

uint8_t key_value = 0;
uint16_t freq = 0;
uint8_t csb = 0;

void InitTick() {
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x20;		//设置定时初值
	T2H = 0xD1;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
	IE2 |= 0x04;
}

void InitBsp() {
	HC138(4, 0xff);
	HC138(5, 0x00);
	InitFreq();
	InitCsb();
	EA = 1;
}


void IsrTask() interrupt 12 {
	_display();
	if (cnt_dis < 50) cnt_dis++;
	if (cnt_freq < 1000) cnt_freq++;
	if (cnt_csb < 500) cnt_csb++;
	if (cnt_key < 10) cnt_key++;
}

void TaskCollect() {
	if (cnt_freq == 1000) {
		cnt_freq = 0;
		freq = GetFreq();
	}
	
	if (cnt_csb == 500) {
		cnt_csb = 0;
		csb = GetCsb();
	}
	
	if (cnt_key == 10) {
		cnt_key = 0;
		_keyscan();
	}
}


void TaskKey() {
	switch(key_value) {
		case 4:
			mode = 0;
			key_value = 0;
			break;
		case 5:
			mode = 1;
			key_value = 0;
			break;
		case 7:
			mode = 5;
			key_value = 0;
			break;
		case 8:
			mode = 6;
			key_value = 0;
			break;
		case 55:
			mode = 55;
			key_value = 0;
			break;
		case 24:
			mode = 66;
			key_value = 0;
			break;
		case 34:
			mode = 77;
			key_value = 0;
			break;
		default:
			key_value = 0;
			break;
	}
}

void TaskControl() {
	
}
 
void TaskDisplay() {
	if (cnt_dis < 50) return;
	cnt_dis = 0;
	
	switch(mode){
		case 0:
			sprintf(display_buff, "A  %5d", freq);
			break;
		case 1:
			sprintf(display_buff, "B    %3bu", csb);
			break;
		case 5:
			sprintf(display_buff, "C    AAA");
			break;
		case 6:
			sprintf(display_buff, "D    BBB");
			break;
		case 55:
			sprintf(display_buff, "11111111");
			break;
		case 66:
			sprintf(display_buff, "22222222");
			break;
		case 77:
			sprintf(display_buff, "33333333");
			break;
	}
	
}

void _display() {
	static uint8_t smg_idx = 0;
	static uint8_t dis_idx = 0;
	uint8_t ch = display_buff[dis_idx];
	uint8_t dat = 0xff;
	
	if (ch >= '0' && ch <= '9') dat = Seg_Table[ch - '0'];
	else if (ch >= 'A' && ch <= 'F') dat = Seg_Table[ch - 'A' + 10];
	else if (ch == '-') dat = 0xbf;
	
	if (display_buff[dis_idx + 1] == '.') {
		dis_idx++;
		dat &= 0x7f;
	}
	
	HC138(COM, 1<<smg_idx);
	HC138(SMG, dat);
	smg_idx++; dis_idx++;
	if (smg_idx == 8 || display_buff[dis_idx] == '\0') {
		smg_idx = 0; dis_idx = 0;
	}
}
 

uint8_t GetKey16() {
	uint16_t key_temp;
	uint8_t key = 0;
	
	P44=0;P42=1;P35=1;P34=1;P3|=0x0f;
	key_temp = P3;
	P44=1;P42=0;P35=1;P34=1;P3|=0x0f;
	key_temp = (key_temp<<4)| (P3 & 0x0f);
	P44=1;P42=1;P35=0;P34=1;P3|=0x0f;
	key_temp = (key_temp<<4)| (P3 & 0x0f);
	P44=1;P42=1;P35=1;P34=0;P3|=0x0f;
	key_temp = (key_temp<<4)| (P3 & 0x0f);
	
	switch(~key_temp){
		case 0x8000: key = 4;break;
		case 0x4000: key = 5;break;
		case 0x2000: key = 6;break;
		case 0x1000: key = 7;break;
		case 0x0800: key = 8;break;
		case 0x0400: key = 9;break;
		case 0x0200: key = 10;break;
		case 0x0100: key = 11;break;
		case 0x0080: key = 12;break;
		case 0x0040: key = 13;break;
		case 0x0020: key = 14;break;
		case 0x0010: key = 15;break;
		case 0x0008: key = 16;break;
		case 0x0004: key = 17;break;
		case 0x0002: key = 18;break;
		case 0x0001: key = 19;break;
		case 0x8800: key = 55;break;
	}
	return key;
}


void _keyscan() {
	static uint8_t key_old = 0;
	// 长按
	static uint16_t key_down_time = 0;
	static uint16_t key_up_time = 0;
	
	// 双击
	static uint16_t key_double_first_time = 0;
	static uint16_t key_double_second_time = 0;
	static uint8_t key_first = 0;
	static uint8_t key_second = 0;
	uint8_t key_down, key_up, key_val;
	
	// tick
	cnt_key_long++;
	
	key_val = GetKey16();
	key_down = key_val & (key_old ^ key_val);
	key_up = ~key_val & (key_old ^ key_val);
	key_old = key_val;
	
	if (key_down){
		key_down_time = cnt_key_long;
		if ((key_first == 0) && (key_second == 0)) {  //第一次按下
			key_double_first_time = cnt_key_long;
			key_first = key_down;
		} else if ((key_first != 0) && (key_second == 0)){ //第二次按下
			key_double_second_time = cnt_key_long;
			key_second = key_down;
		}
	}
	
	if (key_up) {
		// 长按
		key_up_time = cnt_key_long;
		if (key_up_time - key_down_time > 100) {
			key_value = key_up + 20;
		}
	}
	
	if (key_first != 0 && (cnt_key_long - key_double_first_time > 30)) {
		if (key_second == 0) {
			key_value = key_first;
			key_first = 0; key_second = 0;
		} else if (key_second == key_first) {
			key_value = key_second + 30;
			key_first = 0; key_second = 0;
		} else if ((key_second != 0) && (key_second != key_first)) {
			key_value = 0;
			key_first = 0; key_second = 0;
		}
	}
}

