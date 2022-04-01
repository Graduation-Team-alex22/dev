#include "../main/main.h"


static uint32_t currentTick = 0;
const uint32_t lastTick = 0xFFFFFFFF;

void updateCurrentTick()
{
	if(currentTick >= lastTick)
	{
		currentTick = 0;
	}
	
		currentTick ++;
}

uint32_t getCurrentTick()
{
	return currentTick;
}
