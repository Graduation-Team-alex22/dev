#include "EPS_Health_Rep_task.h"

#include "housekeeping_service.h"
#include "housekeeping.h"

extern uint32_t tick;
extern uint8_t EPS_Health_Flag;
extern uint8_t COMMS_Health_Flag;

void EPS_Health_Rep_task_Init(void)
{
   hk_INIT();	
}
uint32_t EPS_Health_Rep_task_Update(void)
{
	//static uint32_t tick;
	uint32_t now;
	
	if( EPS_Health_Flag == true )
	{
	  now = getCurrentTick();
		
	  if(now < tick || now - tick > 10000)//__TX_INTERVAL_MS
		{			
			tick = now;
      EPS_Health_Req_SCH();
			EPS_Health_Flag = false;
      COMMS_Health_Flag = true;
    }		
	}
	
	return RETURN_NORMAL_STATE;
}

