#ifndef __smg__
#define __smg__

#include "def.h"

typedef struct {
	uint8_t buff[10];
	uint8_t buff_idx;
	uint8_t smg_idx; 
	void (code *display)(void);
}smg_display_t;

void InitDisplay();
smg_display_t* GetSmgStruct();


#endif