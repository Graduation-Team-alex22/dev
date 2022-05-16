#include "../main/project.h"
#include "../adcs/adcs_controller/sgp4.h"
#include "app_Control_sgp4_task.h"
#include "../processor/ttrd2-19a-t0401a-v001b_processor.h"
#include "../main/main.h"

#ifdef DIAGNOSIS_OUTPUT
#include "../adcs/services_utilities/printf.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#endif


void App_Control_Sgp4_Init(void)
{
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - SGP4] Init\n");
      UART2_BUF_O_Send_All_Data();
   #endif
   error_t error_code = CTRL_SGP4_Init();
   if(error_code)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
}

uint32_t App_Control_Sgp4_Update(void)
{
   error_t error_code = CTRL_SGP4_Update();
   if(error_code)   
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
   
   #ifdef DIAGNOSIS_OUTPUT
      char buf[200] = {0};
      xyz_t p = CTRL_SGP4_GetPECI(), v = CTRL_SGP4_GetVECI();
      sprintf(buf, "[DIAG - SGP4] P_eci: %f %f %f\n[DIAG - SGP4] V_eci: %f %f %f\n",
                    p.x, p.y, p.z, v.x, v.y, v.z);
      UART2_BUF_O_Write_String_To_Buffer(buf);
      UART2_BUF_O_Send_All_Data();
   #endif
   
   return RETURN_NORMAL_STATE;
}
