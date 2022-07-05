#include "COMMS_EXT_WOD_Rep_task.h"

#include "housekeeping_service.h"
#include "housekeeping.h"

extern uint32_t tick;
extern uint8_t COMMS_exWOD_Flag;
extern uint8_t EPS_Health_Flag;

void COMMS_EXT_WOD_Rep_task_Init(void)
{
	
}
uint32_t COMMS_EXT_WOD_Rep_task_Update(void)
{
  //static uint32_t tick;
	uint32_t now;
	
	if( COMMS_exWOD_Flag == true )
	{
	  now = getCurrentTick();
	  if(now < tick || now - tick > 10000)//__TX_INTERVAL_MS
		{			
			tick = now;
      COMMS_EXT_WOD_Rep_SCH();
			COMMS_exWOD_Flag = false;
			EPS_Health_Flag = true;
    }			
	}
		
	return RETURN_NORMAL_STATE;
}



