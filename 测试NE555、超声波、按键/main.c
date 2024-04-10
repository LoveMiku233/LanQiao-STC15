#include "def.h"


void main() {
	InitTick();
	InitBsp();
	while(1) {
		TaskCollect();
		TaskKey();
		TaskControl();
		TaskDisplay();
	}
}