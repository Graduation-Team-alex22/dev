
#include "SCHEDULE_SERVICES_task.h"
#include "scheduling_service.h"

void     SCHEDULE_SERVICES_Init(void)
{
	 scheduling_service_init();
}

uint32_t SCHEDULE_SERVICES_Update(void)
{
	 cross_schedules();
	
	return RETURN_NORMAL_STATE;	
}

	