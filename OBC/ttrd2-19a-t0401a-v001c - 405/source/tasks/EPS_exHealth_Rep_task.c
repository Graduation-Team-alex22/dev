#include "EPS_exHealth_Rep_task.h"

#include "housekeeping_service.h"
#include "housekeeping.h"

extern uint32_t tick;
extern uint8_t EPS_exHealth_Flag;
extern uint8_t ADCS_exHealth_Flag;


void     EPS_exHealth_Rep_task_Init(void)
{
}

uint32_t EPS_exHealth_Rep_task_Update(void)
{
  //static uint32_t tick;
	uint32_t now;
	
	if( EPS_exHealth_Flag == true )
	{
	  now = getCurrentTick();
	  if(now < tick || now - tick > 5000)//__TX_INTERVAL_MS
		{			
			tick = now;
      EPS_exHealth_Req_SCH();
			EPS_exHealth_Flag = false;
			ADCS_exHealth_Flag = true;
    }	
	}
		
	return RETURN_NORMAL_STATE;
}


