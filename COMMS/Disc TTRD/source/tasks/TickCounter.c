#include "../main/main.h"


static uint32_t currentTick;
#define lastTick  0xFFFF0000

void updateCurrentTick(void)
{
	if(currentTick >= lastTick)
	{
		currentTick = 0;
	}
	
		currentTick = currentTick + 5;
}

uint32_t getCurrentTick(void)
{
	return currentTick;
}
