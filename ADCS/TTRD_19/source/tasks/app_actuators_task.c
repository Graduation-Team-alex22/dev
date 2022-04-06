#include "../adcs/adcs_actuators/mgn_torquer_driver.h"
#include "../adcs/adcs_actuators/spin_motor_driver.h"
#include "app_actuators_task.h"
#include "../main/project.h"

#ifdef DIAGNOSIS_OUTPUT
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#endif


void App_Actuator_Init(void)
{
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - INIT] Actuators Init\n");
      UART2_BUF_O_Send_All_Data();
   #endif
}

uint32_t App_Actuator_Update(void)
{
   
   return 0;
}
