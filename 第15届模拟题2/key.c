#include "key.h"

key_t key;


void Cls() {
	P32 = 1;
	P33 = 1;
	P44 = 0;
}


void KeyScan() {
	static uint8_t mid_key = 0;
	switch(key.key_status) {
		case 0:
			Cls();
			if (!P32 || !P33) key.key_status = 1;
			break;
		case 1:
			Cls();
			if (!P32 || !P33) {
				if (!P32) mid_key = 1;
				else if (!P33) mid_key = 2;
				key.key_status = 2;
			}
			break;
		case 2:
			Cls();
			if (P32 && P33) {
				key.key_value = mid_key;
				key.key_status = 0;
				mid_key = 0;
			}
			break;
	}
}

void InitKey() {
	key.key_value = 255;
	key.key_status = 0;
	key.KeyScan = KeyScan;
}

key_t *GetKeyStruct() {
	return &key;
}