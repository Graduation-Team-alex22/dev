#include "ADCS_exHealth_Rep_task.h"

#include "housekeeping_service.h"
#include "housekeeping.h"


void     ADCS_exHealth_Rep_task_Init(void)
{
}

uint32_t ADCS_exHealth_Rep_task_Update(void)
{
  ADCS_exHealth_Req_SCH();
	
	return RETURN_NORMAL_STATE;
}