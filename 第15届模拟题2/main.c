#include "def.h"



void main() {
	InitTick();
	InitBsp();
	while(1) {
		TaskKey();
		TaskDisplay();
		TaskCollect();
		TaskControl();
	}
}