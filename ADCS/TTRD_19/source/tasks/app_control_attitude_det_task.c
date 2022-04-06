#include "../main/main.h"
#include "../adcs/adcs_controller/controller.h"
#include "app_Control_attitude_det_task.h"

uint32_t App_Control_Attitude_Det_Update(void)
{
   CTRL_Control_Determination_Update(); 
   return RETURN_NORMAL_STATE;
}
