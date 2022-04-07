#include "../main/project.h"
#include "app_time_keeping_task.h"
#include "../adcs/services_utilities/time.h"


#ifdef DIAGNOSIS_OUTPUT
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#endif

void App_Time_keeping_Init(void)
{
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - INIT] Health Check Init\n");
      UART2_BUF_O_Send_All_Data();
   #endif  

   error_t error_code = time_init();
   if(error_code != NO_ERROR)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
   
   
}
uint32_t App_Time_keeping_Update(void)
{
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - HEALTH]  \n");
      UART2_BUF_O_Send_All_Data();
   #endif
   
   return NO_ERROR;
}

