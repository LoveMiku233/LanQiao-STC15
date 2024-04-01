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


uint8_t mode = 0;

uint8_t display_buff[10] = "12345678.";
uint8_t cnt_key = 0;
uint8_t cnt_dis = 0;
uint8_t cnt_time = 0;
uint8_t cnt_led = 0;
uint16_t input_dat = 0;
uint16_t old_input_dat = 0;

uint8_t time[3] = {23, 9, 59};
uint8_t rom_dat[4] = {0};
uint8_t save_flag = 0;
uint8_t read_flag = 0;

void InitTick() {
	AUXR |= 0x04;		
	T2L = 0xCD;	
	T2H = 0xD4;	
	AUXR |= 0x10;
	IE2 |= 0x04;
}

void InitBsp() {
	HC138(LED, 0xff);
	HC138(RELAY, 0x00);
	InitKey();
	SetDs1302(time);
	read_flag = 1;
	EA = 1;
}

void IsrTask() interrupt 12 {
	_display();
	// 按键
	if(cnt_key < 10) cnt_key++;
	if(cnt_dis < 50) cnt_dis++;
	if(cnt_time < 200) cnt_time++;
	if(cnt_led < 150) cnt_led++;
	
}


void TaskCollect() {
	if(cnt_key == 10) {
		cnt_key = 0;
		GetKeyStruct()->key_scan();
	}
	
	if(cnt_time == 200) {
		cnt_time = 0;
		GetDs1302(time);
	}
	
}


void TaskKey() {
	switch(GetKeyStruct()->key_value) {
		case 1:
			// s4
			if(mode != 2) mode++;
			else mode = 0;
			GetKeyStruct()->key_value = 255;
			break;
		case 5:
			// s5
			if(mode == 1) {
				input_dat = 0;
			}
			GetKeyStruct()->key_value = 255;
			
			break;
		case 2:case 3:case 4:case 6:case 7:case 8:case 9:case 10: case 11: case 12:
		if(mode == 1 && input_dat <= 999) {
			uint8_t tmp = 0;
			uint8_t key = GetKeyStruct()->key_value;
			if (key >= 9) {
				tmp = key - 9;
			} else if (key >= 5) {
				tmp = key - 2;
			} else {
				tmp = key + 5;
			}
			// 缺少检测溢出
			input_dat *= 10;
			input_dat += tmp;
		}
		GetKeyStruct()->key_value = 255;
		break;
	}
}

void TaskDisplay() {
	if(cnt_dis < 50) return;
	cnt_dis = 0;
	
	switch(mode){
		case 0: 
			sprintf(display_buff, "%02bu-%02bu-%02bu", time[0], time[1], time[2]);
			break;
		case 1:
			if (input_dat == 0)
				sprintf(display_buff, "C       ");
			else
				sprintf(display_buff, "C   %4d", input_dat);
			
			break;
		case 2:
			sprintf(display_buff, "E  %02bu-%02bu", rom_dat[0], rom_dat[1]);
			break;
	}
	
}

void TaskControl() {
	static uint8_t led = 0xff;
	
	if(mode == 0) led &= ~(0x01);
	else led |= 0x01;
	
	if(mode == 1) led &= ~(0x02);
	else led |= 0x02;
	
	if(mode == 2) led &= ~(0x04);
	else led |= 0x04;
	
	if(mode == 2 && input_dat) {
		save_flag = 1;
		if(old_input_dat < input_dat) {
			led &= ~(0x08);
		}else {
			led |= 0x08;
		}
		old_input_dat = input_dat;
		input_dat = 0;
	}
	
	if(save_flag) {
		save_flag = 0;
		// 保存
		rom_dat[0] = time[0];
		rom_dat[1] = time[1];
		rom_dat[2] = (uint8_t)(old_input_dat >> 8);
		rom_dat[3] = (uint8_t)old_input_dat;
		SetRom(rom_dat);
	}
	
	if(read_flag) {
		read_flag = 0;
		// 读取
		GetRom(rom_dat);
		old_input_dat = (uint16_t)(rom_dat[2]<<8) | (rom_dat[3]);
	}
	
	// 更新LED
	if(cnt_led == 150) {
		cnt_led = 0;
		HC138(LED, led);
	}

}


void _display() {
	static uint8_t buff_idx = 0;
	static uint8_t smg_idx = 0;
	
	uint8_t ch = display_buff[buff_idx];
	uint8_t dat = 0xff;
	
	if (ch >= '0' && ch <= '9') 	dat = Seg_Table[ch - '0'];
	else if (ch >= 'A' && ch <= 'F')dat = Seg_Table[ch - 'A' + 10];
	else if (ch == '-') 			dat = 0xbf;
	if (display_buff[buff_idx + 1] == '.') {
		dat &= 0x7f;
		buff_idx++;
	}
	
	HC138(COM, 1<<smg_idx);
	HC138(SMG, dat);
	
	buff_idx++;smg_idx++;
	if (display_buff[buff_idx] == '\0' || smg_idx == 8) {
		buff_idx = 0;
		smg_idx = 0;
	}
}
