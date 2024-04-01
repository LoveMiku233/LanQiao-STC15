#include "smg.h"


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


smg_display_t smg;


void display() {
	uint8_t ch = smg.buff[smg.buff_idx];
	uint8_t ch_next = smg.buff[smg.buff_idx+1];
	uint8_t dat = 0xff;
	
	if (ch >= '0' && ch <= '9') dat = Seg_Table[ch-'0'];
	else if (ch >= 'A' && ch <= 'F') dat = Seg_Table[ch-'A'+10];
	else if (ch == '-') dat = 0xbf;
	else if (ch == ' ') dat = 0xff;
	else if (ch == 'H') dat = 0x89;
	else if (ch == 'P') dat = 0x8c;
	// .
	if (ch_next == '.') {
		dat &= 0x7b;
		smg.buff_idx++;
	}
	// display
	HC138(COM, 1<<smg.smg_idx);
	HC138(SMG, dat);
	
	smg.buff_idx++; smg.smg_idx++;
	if (smg.buff[smg.buff_idx] == '\0' || smg.smg_idx == 8) {
		smg.buff_idx = 0; 
		smg.smg_idx = 0;
	}
}


void InitDisplay() {
	sprintf(smg.buff, "12345689.");
	smg.buff_idx = 0;
	smg.smg_idx = 0;
	smg.display = display;
}

smg_display_t* GetSmgStruct() {
	return &smg;
}
