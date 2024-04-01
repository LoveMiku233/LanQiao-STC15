#ifndef __KEY__
#define __KEY__

#include "def.h"

typedef struct {
	uint8_t value;
	uint8_t status;
	void (code *KeyScan)(void);
}Key_t;

void InitKey();
Key_t* GetKeyStruct();


#endif

