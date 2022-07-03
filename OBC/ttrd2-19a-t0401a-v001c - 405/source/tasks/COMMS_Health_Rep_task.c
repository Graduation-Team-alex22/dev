#include "COMMS_Health_Rep_task.h"

#include "housekeeping_service.h"
#include "housekeeping.h"

extern uint32_t tick;
extern uint8_t COMMS_Health_Flag;
extern uint8_t COMMS_WOD_Flag;


void COMMS_Health_Rep_task_Init(void)
{

}

uint32_t COMMS_Health_Rep_task_Update(void)
{
	//static uint32_t tick;
	uint32_t now;
	
	if( COMMS_Health_Flag == true )
	{
	  now = getCurrentTick();
	  if(now < tick || now - tick > 5000)//__TX_INTERVAL_MS
		{			
			tick = now;
      COMMS_Health_Req_SCH();
      COMMS_Health_Flag = false;
			COMMS_WOD_Flag = true;
    }			
	}

	return RETURN_NORMAL_STATE;
}


