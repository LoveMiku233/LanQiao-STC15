#include "key.h"

Key_t key;

void cls_c() {
	P44 = 0;
	P42 = 0;
	P32 = 1;
	P33 = 1;
}

void cls_r() {
	P44 = 1;
	P42 = 1;
	P32 = 0;
	P33 = 0;
}


void KeyScan() {
	static uint8_t key_value = 0;
	static uint16_t key_time = 0;
	static uint8_t mid_value = 0;
	
	switch(key.status) {
		case 0:
			cls_c();
			if (!P32 || !P33) {
				key.status = 1;
			}
			break;
		case 1:
			cls_c();
			if (!P32) {
				key_value = 3;
				key.status = 2;
			}else if (!P33) {
				key_value = 1;
				key.status = 2;
			}
			break;
		case 2:
			cls_r();
			if (!P44) {
				mid_value = key_value;
				key.status = 3;
			}else if (!P42){
				mid_value = key_value + 1;
				key.status = 3;
			}
			break;
		case 3:
			cls_r();
			if(P44 && P42) {
				if(key_time < 200) {
					key.value = mid_value;
				}else {
					key.value = (mid_value == 1) ? ('L') : (mid_value);
				}
				key_time = 0;
				key.status = 0;
			} else {
				if(key_time < 200) {
					key_time++;
				}
			}
			break;
	}
}


void InitKey() {
	key.value = 255;
	key.status = 0;
	key.KeyScan = KeyScan;
}



Key_t* GetKeyStruct() {
	return &key;
}
