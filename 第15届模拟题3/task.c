#include "task.h"

// 数码管
uint8_t display_buff[10] = "12345678.";


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


// 按键
uint8_t key_value = 255;
uint8_t key_status = 0;


// 刷新
uint8_t cnt_key = 0;
uint8_t cnt_dis = 0;
uint8_t cnt_ad = 0;
uint16_t cnt_dist = 0;
uint8_t cnt_led = 0;

// mode
uint8_t mode = 0;
uint8_t parma = 1;

// 参数限制
uint8_t parma_max = 60;
uint8_t parma_min = 10;
uint8_t set_max_flag = 0;
uint8_t set_min_flag = 0;

// 数据
uint16_t sonic_dat = 123;
uint16_t ad = 0;
uint8_t arm_cnt = 0;
uint8_t dist = 10;
uint8_t old_dist = 10;
uint8_t now_alarm_flag = 0;
uint8_t led8_flag = 0;

void InitTick() {
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x20;		//设置定时初值
	T2H = 0xD1;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
	IE2 |= 0x04;

}



void InitBsp() {
	HC138(LED, 0xff);
	HC138(RELAY, 0x00);
	Timer0_Init();
	juli();
	EA = 1;
}

void TaskCollect() {
	if (cnt_key == 10) {
		cnt_key = 0;
		KeyScan();
	}
	
	if (cnt_ad == 200) {
		cnt_ad = 0;
		ad = (uint16_t)(GetAd() * 500.0 / 255);
	}
	
	if (cnt_dist == 400) {
		cnt_dist = 0;
		old_dist = dist;
		dist = juli();
			// 报警次数
		if (old_dist >= parma_min && old_dist <= parma_max) {
			if (dist < parma_min || dist > parma_max) {
				arm_cnt++;
			}
		}
	}
}

void TaskDisplay() {
	if (cnt_dis < 50) return;
	cnt_dis = 0;
	
	switch(mode) {
		case 0:
			sprintf(display_buff, "A    %3bu", dist);
			break;
		case 1:
			sprintf(display_buff, "P%bu %02bu-%02bu", parma, parma_min, parma_max);
			break;
		case 2:
			if (arm_cnt < 9) {
				sprintf(display_buff, "E      %bu", arm_cnt);
			} else {
				sprintf(display_buff, "E      -");
			}
			
			break;
	}
	
}


void TaskKey() {
	switch(key_value) {
		case 1: // s4
			if (mode != 2) mode++;
			else mode = 0;
			set_max_flag = 0;
			set_min_flag = 0;
			parma = 1;
			key_value = 255;
			break;
		
		case 2: // s8
			key_value = 255;
			if (mode == 1) {
				if (parma == 1) {
					if (parma_min != 40) {
						parma_min += 10;
					} else {
						parma_min = 0;
					}
				}else if (parma == 2) {
					set_max_flag = 0;
					set_min_flag = 1;
				}
			}
			
			break;
		
		case 3: // s5
			if (mode == 1) {
				parma = parma == 1 ? 2 : 1;
				set_max_flag = 0;
				set_min_flag = 0;
			}
			else if(mode == 2) arm_cnt = 0;
			key_value = 255;
			break;
		
		case 4: // s9
			key_value = 255;
			if (mode == 1) {
				if (parma == 1) {
					if (parma_max != 90) {
						parma_max += 10;
					} else {
						parma_max = 50;
					}
				}else if(parma == 2) {
					set_max_flag = 1;
					set_min_flag = 0;
				}
			}
			break;
	}
}

void TaskControl() {
	static uint8_t led = 0xff;
	
	if (set_max_flag) {
		if (ad >= 0 && ad < 100) {
			parma_max = 50;
		} else if(ad >= 100 && ad < 200) {
			parma_max = 60;
		} else if(ad >= 200 && ad < 300) {
			parma_max = 70;
		} else if(ad >= 300 && ad < 400) {
			parma_max = 80;
		} else if(ad >= 400 && ad < 500){
			parma_max = 90;
		}
	}
	
	if (set_min_flag) {
		if (ad >= 0 && ad < 100) {
			parma_min = 0;
		} else if(ad >= 100 && ad < 200) {
			parma_min = 10;
		} else if(ad >= 200 && ad < 300) {
			parma_min = 20;
		} else if(ad >= 300 && ad < 400) {
			parma_min = 30;
		} else if(ad >= 400 && ad < 500){
			parma_min = 40;
		}
	}
	

	
	
	if (mode == 0) {
		led |= 0x07;
		led &= ~(0x01);
	}else if(mode == 1) {
		led |= 0x07;
		led &= ~(0x02);
	}else if(mode == 2) {
		led |= 0x07;
		led &= ~(0x04);
	}
	
	if (dist >= parma_min && dist <= parma_max) {
		// L8 flag;
		led8_flag = 0;
		led &= ~(0x80);
	} else {
		led8_flag = 1;
	}
	
	if (cnt_led == 100) {
		cnt_led = 0;
		if (led8_flag) {
			led ^= 0x80;
		}
		HC138(LED, led);
	}
}

void KeyScan() {
	static uint8_t mid_value = 0;
	
	switch(key_status) {
		case 0:
			P32 = 0; P33 = 0; P44 = 1; P42 = 1;
			if (!P44 || !P42) key_status = 1;
			break;
		case 1:
			P32 = 0; P33 = 0; P44 = 1; P42 = 1;
			if (!P44 || !P42) {
				if (!P44) mid_value = 1;
				if (!P42) mid_value = 2;
				key_status = 2;
			}
			break;
		case 2:
			P32 = 1; P33 = 1; P44 = 0; P42 = 0;
			if (!P33 || !P32) {
				if (!P33) mid_value += 0;
				if (!P32) mid_value += 2;
				key_status = 3;
			}
			break;
		case 3:
			P32 = 1; P33 = 1; P44 = 0; P42 = 0;
			if (P33 && P32) {
				key_value = mid_value;
				mid_value = 0;
				key_status = 0;
			}
			break;
			
	}

}

void Display() {
	static uint8_t buff_idx = 0;
	static uint8_t smg_idx = 0;
	
	uint8_t ch = display_buff[buff_idx];
	uint8_t dat = 0xff;
	
	if (ch >= '0' && ch <= '9') dat = Seg_Table[ch - '0'];
	else if (ch >= 'A' && ch <= 'F') dat = Seg_Table[ch - 'A' + 10];
	else if (ch == '-') dat = 0xbf;
	else if (ch == 'P') dat = 0x8c;
	
	if (display_buff[buff_idx + 1] == '.') {
		buff_idx++;
		dat &= 0x7f;
	}
	
	HC138(COM, 1<<smg_idx);
	HC138(SMG, dat);
	
	buff_idx++;smg_idx++;
	if (display_buff[buff_idx] == '\0' || smg_idx == 8) {
		buff_idx = 0; smg_idx = 0;
	}
}



void IsrFunc() interrupt 12 {
	Display();
	if (cnt_key < 10) cnt_key++;
	if (cnt_dis < 50) cnt_dis++;
	if (cnt_ad < 200) cnt_ad++;
	if (cnt_dist < 400) cnt_dist++;
	if (cnt_led < 100) cnt_led++;
}

