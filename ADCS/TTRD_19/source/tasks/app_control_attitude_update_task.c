#include "../main/main.h"
#include "../adcs/adcs_controller/controller.h"
#include "app_Control_attitude_update_task.h"

#ifdef DIAGNOSIS_OUTPUT
#include "stdio.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#endif

uint32_t App_Control_Attitude_Update(void)
{
   CTRL_Control_Attitude_Update(); 
   
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - Update] Attitude Control Update\n");
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - Data]: \n");
      UART2_BUF_O_Send_All_Data();
   #endif
   
   return RETURN_NORMAL_STATE;
}
