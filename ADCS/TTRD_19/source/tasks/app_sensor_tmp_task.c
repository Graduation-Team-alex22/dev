#include "stdint.h"
#include "../adcs/adcs_sensors/tmp_sensor.h"
#include "../processor/ttrd2-19a-t0401a-v001b_processor.h"
#include "app_sensor_tmp_task.h" 
#include "../main/project.h"

#ifdef DIAGNOSIS_OUTPUT
#include "../adcs/services_utilities/printf.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#endif

/*----------------------------------------------------------------------------*-

  App_Sensor_Tmp_Init()

  Sets up Temperature sensor.
 
  The temperature sensor provides ambient temperature
  readings to the controller.

  Datasheet values: 
     None.

  PARAMETERS:
     None. 
   
  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     I2C1 peripheral.
     Port B pin 8 (SCL).
     Port B pin 9 (SDA).

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void App_Sensor_Tmp_Init(void)
{
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - TMP] Init\n");
      UART2_BUF_O_Send_All_Data();
   #endif
	error_t error_code = TMP_Sensor_Init(I2C1);
   if(error_code != NO_ERROR)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
}



/*----------------------------------------------------------------------------*-

  App_Sensor_Tmp_Update()

  Updates temperature sensor readings.
 
  The temperature sensor provides ambient temperature
  readings to the controller.

  Datasheet values:
     None.

  PARAMETERS:
     None. 
   
  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     I2C1 peripheral.
     Port B pin 8 (SCL).
     Port B pin 9 (SDA).

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
uint32_t App_Sensor_Tmp_Update(void)
{
	error_t error_code = TMP_Sensor_update();
   if(error_code != NO_ERROR)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
   
   #ifdef DIAGNOSIS_OUTPUT
      char buf[200] = {0};
      tmp_sensor_t tg = TMP_Sensor_GetData();
      sprintf(buf, "[DIAG - TMP] Temperature: %3.4f\n", tg.temprature );
      UART2_BUF_O_Write_String_To_Buffer(buf);
      UART2_BUF_O_Send_All_Data();
   #endif
   
	return NO_ERROR;
}
