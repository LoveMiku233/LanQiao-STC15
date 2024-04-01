#include "key.h"

key_t key;

void ClsR() {
	P44 = 0;
	P42 = 0;
	P35 = 0;
	P34 = 0;
	P3 |= 0x0f;
}

void ClsC() {
	P44 = 1;
	P42 = 1;
	P35 = 1;
	P34 = 1;
	P3 &= 0xf0;
}

void GetKeyValue() {
	static uint8_t mid_value = 0;

	switch(key.key_status) {
		case 1:
			ClsR();
			if ((P3 & 0x0f) != 0x0f) {
				key.key_status = 2;
			}
			break;
		case 2:
			ClsR();
			if ((P3 & 0x0f) != 0x0f) {
				key.key_status = 3;
				if (!P31)      mid_value = 9; 
				else if (!P32) mid_value = 5;
				else if (!P33) mid_value = 1;
			}
			break;
		case 3:
			ClsC();
			if (P44 == 0 || P42 == 0 || P35 == 0 || P34 == 0) {
				key.key_status = 4;
				if (!P44)      mid_value += 0; 
				else if (!P42) mid_value += 1;
				else if (!P35) mid_value += 2;
				else if (!P34) mid_value += 3;
			}
			break;
		case 4:
			ClsC();
			if (P44 && P42 && P35 && P34) {
				key.key_status = 1;
				key.key_value = mid_value;
				mid_value = 0;
			}
			break;
	}
}


void InitKey() {
	key.key_value = 255;
	key.key_status = 1;
	key.key_scan = GetKeyValue;
}


key_t* GetKeyStruct() {
	return &key;
}