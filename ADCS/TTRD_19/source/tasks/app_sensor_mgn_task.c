#include "stdint.h"
#include "../adcs/adcs_sensors/mgn_sensor.h"
#include "../processor/ttrd2-19a-t0401a-v001b_processor.h"
#include "app_sensor_mgn_task.h" 
#include "../main/project.h"

#ifdef DIAGNOSIS_OUTPUT
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#endif

/*----------------------------------------------------------------------------*-

  App_Sensor_Mgn_Init()

  Set up magnomter sensor.
 
  The magnometer sensor acts as a digital compass providing magnetic
  feild reading to the controller.

  Datasheet values: 
  Power on reset time (Maximum): 350 uS
  I2C ready time (Maximum) : 200 uS
  Measurement ready time (Maximum : 50 ms
  
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
void App_Sensor_Mgn_Init(void)
{
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG] MGN Init\n");
      UART2_BUF_O_Send_All_Data();
   #endif
   mgn_init_t mgn_init;
   mgn_init.over_sampling = MGN_OVER_SAMPLING_512;
   mgn_init.op_mode = MGN_OP_MODE_CONT;
   mgn_init.output_rate = MGN_OUTPUT_RATE_10;
   mgn_init.range = MGN_RANGE_2;

   error_t error_code = MGN_Sensor_Init(I2C1, &mgn_init);
   if(error_code != NO_ERROR)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
}


/*----------------------------------------------------------------------------*-

  App_Sensor_Mgn_Update()

  Update magnomter sensor readings.
 
  The magnometer sensor acts as a digital compass providing magnetic
  feild reading to the controller.

  Datasheet values: 
  Power on reset time (Maximum): 350 uS

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
uint32_t App_Sensor_Mgn_Update(void)
{
   
	error_t error_code = MGN_Sensor_Update();
	if(error_code != NO_ERROR)
	{
		PROCESSOR_Perform_Safe_Shutdown(error_code);
	}
   
	return 0;
}
