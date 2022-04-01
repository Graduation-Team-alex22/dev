#include "stdint.h"
#include "../port/port.h"
#include "../adcs/adcs_sensors/tmp_sensor.h"

#include "app_sensor_tmp_task.h" 


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
	uint8_t error_code = TMP_Sensor_Init(I2C1);
   if(error_code)
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
	TMP_Sensor_update();
	return RETURN_NORMAL_STATE;;
}
