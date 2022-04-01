
#include "COMMS_WOD_Rep_task.h"

#include "housekeeping_service.h"
#include "housekeeping.h"


void COMMS_WOD_Rep_task_Init(void)
{
	
}
uint32_t COMMS_WOD_Rep_task_Update(void)
{
   COMMS_WOD_Rep_SCH();
	
	return RETURN_NORMAL_STATE;
}

