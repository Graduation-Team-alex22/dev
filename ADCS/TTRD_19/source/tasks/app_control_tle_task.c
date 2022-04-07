#include "../main/project.h"
#include "../adcs/adcs_controller/tle.h"
#include "app_Control_tle_task.h"
#include "../processor/ttrd2-19a-t0401a-v001b_processor.h"

#ifdef DIAGNOSIS_OUTPUT
#include <stdio.h>
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#endif

void App_Control_Tle_Init(void)
{
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - INIT] TLE Init\n");
      UART2_BUF_O_Send_All_Data();
   #endif
   error_t error_code = CTRL_TLE_Init();
   if(error_code)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
}

uint32_t App_Control_Tle_Update(void)
{
   error_t error_code = CTRL_TLE_Update();
   if(error_code)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
   
   #ifdef DIAGNOSIS_OUTPUT
      char buf[200];
      tle_data_t t = CTRL_TLE_GetData();
      sprintf(buf, "Status: %d  ARGP: %f EP_YEAR: %d\n",
                    t.data_status, t.orbit_data.argp, t.orbit_data.ep_year); 
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - Update] TLE Update\n");
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - Data]: \n");
      UART2_BUF_O_Write_String_To_Buffer(buf);
      UART2_BUF_O_Send_All_Data();
   #endif
   
   return RETURN_NORMAL_STATE;
}
