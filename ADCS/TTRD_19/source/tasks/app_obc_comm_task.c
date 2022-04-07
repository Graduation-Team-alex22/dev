#include "app_obc_comm_task.h"
#include "../main/project.h"
#include "../adcs/services_utilities/obc_comm.h"

#ifdef DIAGNOSIS_OUTPUT
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#endif


void App_Obc_Comm_Init(void)
{
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - INIT] OBC Comm Init\n");
      UART2_BUF_O_Send_All_Data();
   #endif
   
   error_t error_code = OBC_Comm_Init();
   if(error_code)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
}

uint32_t App_Obc_Comm_Update(void)
{
   error_t error_code = OBC_Comm_Update();
   if(error_code)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
   
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - Update] OBC Comm Update\n");
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - Data]: \n");
      UART2_BUF_O_Send_All_Data();
   #endif
   
   return RETURN_NORMAL_STATE;
}
