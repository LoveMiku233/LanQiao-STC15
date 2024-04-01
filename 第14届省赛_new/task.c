#include "task.h"


uint8_t ui = 50; // 时间 temp = 50
uint8_t ui_bk = 0; // 回显界面-温度
uint8_t tmp_ui = 0; // 界面备份
uint8_t dat_stat = 0; // 保证只采集一次


// 按键状态机
uint8_t cnt_key = 0;
// 时间
uint8_t cnt_rtc = 0;
// 显示更新
uint8_t cnt_dis = 0;
// 温度
uint16_t cnt_temp = 0;
// 频率
uint16_t cnt_freq = 0;
// 3s
uint16_t cnt_3s = 0;
// adc
uint8_t cnt_adc = 0;

uint8_t cnt_led = 0;
uint8_t led_flush_flag = 0;

uint8_t temp_param = 30;
float temp = 0;
float now_temp = 0;
float max_temp = 28;
uint16_t sum_temp = 0;
float avg_temp = 23.2;

uint16_t freq = 0;
uint8_t humi = 0;
uint8_t max_humi = 68;
uint16_t sum_humi = 0;
float avg_humi = 50.4;

uint8_t light = 0;
uint8_t old_light = 0;


uint8_t en_cnt = 0;
uint8_t en_time[2] = {0};


void InitTick() {
	AUXR |= 0x04;
	T2L = 0xCD;
	T2H = 0xD4;
	AUXR |= 0x10;
	IE2 |= 0x04;
	EA = 1;
}


void IsrTaskFunc() interrupt 12 {
	GetSmgStruct()->display();
	
	if (cnt_key < 10) cnt_key++;
	
	if (cnt_dis < 50) cnt_dis++;
	
	if (cnt_rtc < 100) cnt_rtc++;
	
	if (cnt_temp < 500) cnt_temp++;
	
	if (cnt_freq < 1000) cnt_freq++;
	
	if (cnt_3s > 0) cnt_3s--;
	
	if (cnt_adc < 100) cnt_adc++;
	
	if (cnt_led < 100) cnt_led++;
	
}

void InitBsp() {
	HC138(RELAY, 0x00);
	HC138(LED, 0xff);

	InitDisplay();
	InitKey();
	GetTemp();
	InitRtc();
	InitFreq();
	GetADC(1);
}


void TaskDisplay() {
	smg_display_t *smg = GetSmgStruct();
	rtc_t *rtc = GetRtcStruct();
	if (cnt_dis < 50)
		return;
	cnt_dis = 0;
	
	switch(ui) {
		case 0:
			// 时间界面
			sprintf(smg->buff, "%02bu-%02bu-%02bu", rtc->get_time[0], rtc->get_time[1], rtc->get_time[2]);
			break;
		case 1:
			// 回显界面
			if(ui_bk == 0) {
				// 温度
				sprintf(smg->buff, "C %02bu-%02.01f", (uint8_t)max_temp, avg_temp);
			} else if (ui_bk == 1) {
				sprintf(smg->buff, "H %02bu-%02.01f", (uint8_t)max_humi, avg_humi);
			} else if (ui_bk == 2) {
				sprintf(smg->buff, "F%02bu%02bu-%02bu", en_cnt, en_time[0], en_time[1]);
			}
			break;
		case 2:
			// 参数界面
			sprintf(smg->buff, "P     %02bu", temp_param);
			break;
		case 50:
			// 温度采集界面
			if (humi != 128)
				sprintf(smg->buff, "E  %02bu-%02bu", (uint8_t)temp, (uint8_t)humi);
			else 
				sprintf(smg->buff, "E  %02bu-%s", (uint8_t)temp, "AA");
			break;
	}
}

void TaskScanKey() {
	switch(GetKeyStruct()->value){
		case 1: 
			if (ui != 50) {
				ui_bk = 0;
				if (ui != 2) ui++;
				else ui = 0;
			}
			GetKeyStruct()->value = 255;
			break;
		case 2:
			// s8 ++
			if (ui == 2) {
				if(temp_param < 99) temp_param++;
			}
			GetKeyStruct()->value = 255;
			break;
		case 3:
			if(ui != 50 && ui == 1) {
				if(ui_bk != 2) ui_bk++;
				else ui_bk = 0;
			}
			GetKeyStruct()->value = 255;
			break;
		case 4:
			// s9 --
			if (ui == 2) {
				if(temp_param > 0) temp_param--;
			}
			GetKeyStruct()->value = 255;
			break;
	}
}

void TaskCollect() {
	
	if (cnt_key == 10) {
		cnt_key = 0;
		GetKeyStruct()->KeyScan();
	}
	
	if (cnt_rtc == 100) {
		cnt_rtc = 0;
		GetRtcStruct()->GetTime();
	}
	
	if (cnt_temp == 500) {
		cnt_temp = 0;
		temp = GetTemp();
	}
	
	if (cnt_freq == 1000) {
		cnt_freq = 0;
		freq = GetFreq();
	}
	
	if (cnt_adc == 100) {
		cnt_adc = 0;
		light = GetADC(1);
	}
}
void TaskControl() {
	static uint8_t old_temp = 0;
	static uint8_t old_humi = 0;
	static uint8_t led = 0xff;
	
	
	// LED 控制
	
	if (ui == 0) led &= ~(1<<0);
	else led |= (1<<0);
	
	if (ui == 1) led &= ~(1<<1);
	else led |= (1<<1);
	
	if (ui == 50) led &= ~(1<<2);
	else led |= (1<<2);
	// led4 闪烁
	if ((ui == 50) && (temp > temp_param)) {
		led_flush_flag = 1;
	} else if ((ui == 50) && (humi == 128)){
		led_flush_flag = 0;
		led &= ~(1<<3);
	} else {
		led_flush_flag = 0;
		led |= (1<<3);
	}
	
	if (cnt_led == 100) {
		cnt_led = 0;
		if (led_flush_flag) led ^= (1<<3);
	}
	
	HC138(LED, led);
	
	if (light < 30 && old_light > 30) {
		// 采集 cnt_3s == 0 则证明现在可以触发
		if (cnt_3s == 0) {
			dat_stat = 1;
			tmp_ui = ui;
			ui = 50;
			cnt_3s = 3000;
		}
	}
	old_light = light;
	// 返回主界面
	if ((cnt_3s == 0) && (ui == 50)) {
		ui = tmp_ui;
	}
	
	if (dat_stat) {
		dat_stat = 0;
		
		old_temp = now_temp;
		old_humi = humi;
		
		now_temp = temp;
		// 判断频率是否合法
		if ((freq >= 200) && (freq <= 2000)) {
			humi = (unsigned int)((2*freq + 50)/45.0);
			en_cnt++;
		} else {
			humi = 128;
		}
		
		if (humi != 128) {
			// 更新时间
			en_time[0] = GetRtcStruct()->get_time[0];
			en_time[1] = GetRtcStruct()->get_time[1];
			// 更新最大值
			if (temp > max_temp) max_temp = temp;
			sum_temp = (en_cnt-1) * avg_temp + temp;
			avg_temp = sum_temp / en_cnt;
			
			if (humi > max_humi) max_humi = humi;
			sum_humi = (en_cnt-1) * avg_humi + humi;
			avg_humi = sum_humi / en_cnt;
		}
	}
}