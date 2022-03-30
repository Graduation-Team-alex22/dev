#include "stdint.h"
#include "../port/port.h"
#include "../adcs/adcs_sensors/tmp_sensor.h"

#include "app_sensor_tmp_task.h" 

void App_Sensor_Tmp_Init(void)
{
	uint8_t error_code = TMP_Sensor_Init(I2C1);
   if(error_code)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
}

uint32_t App_Sensor_Tmp_Update(void)
{
	TMP_Sensor_update();
	return RETURN_NORMAL_STATE;;
}
