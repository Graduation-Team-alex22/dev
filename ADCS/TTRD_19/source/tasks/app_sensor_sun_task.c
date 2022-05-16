#include "../main/project.h"
#include "../adcs/adcs_sensors/sun_sensor.h"
#include "app_sensor_sun_task.h"
#include "../main/main.h"

#ifdef DIAGNOSIS_OUTPUT
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#endif

/*----------------------------------------------------------------------------*-

  App_Sensor_Sun_Init()

  Set up sun sensor.
 
  The sun sensor provides a unit vector pointing to the sun in the local body frame.

  Datasheet values: 
     None.

  PARAMETERS:
     None. 
   
  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void App_Sensor_Sun_Init(void)
{
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - Sun] Init\n");
      UART2_BUF_O_Send_All_Data();
   #endif
   error_t error_code = SUN_Sensor_Init();
   if(error_code)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
}

uint32_t App_Sensor_Sun_Update(void)
{
   error_t error_code = SUN_Sensor_Update();
   if(error_code)   
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
    
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - Sun]: \n");
      UART2_BUF_O_Send_All_Data();
   #endif
   
   return RETURN_NORMAL_STATE;
}
