#ifndef __KEY__
#define __KEY__

#include "def.h"

typedef struct {
	uint8_t key_value;
	uint8_t key_status;
	void (code *key_scan)(void);
}key_t;


void InitKey();
key_t* GetKeyStruct();


#endif
