#include "raa.h"
#include "ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#include "ttrd2-05a-t0401a-v001a_switch_task.h"


uint32_t Rofa_Task_Update(void)
{ 
uint32_t r=RETURN_NORMAL_STATE;
 if (SWITCH_BUTTON1_Get_State() == BUTTON1_NOT_PRESSED)
 {UART2_BUF_O_Write_String_To_Buffer("Hello World\n");
  UART2_BUF_O_Send_All_Data();}

	return r;
}
