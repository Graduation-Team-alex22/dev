#include "../main/project.h"
#include "app_sensor_health_task.h"

#ifdef DIAGNOSIS_OUTPUT
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#endif


void App_Sensor_Health_Init(void)
{
  #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - INIT] Health Check Init\n");
      UART2_BUF_O_Send_All_Data();
   #endif
  
}

uint32_t App_Sensor_Health_Update(void)
{
   
   return NO_ERROR;
}
