#include "stdint.h"
#include "../port/port.h"
#include "../adcs/adcs_sensors/imu_sensor.h"

#include "app_sensor_imu_task.h"


/*----------------------------------------------------------------------------*-

  App_Sensor_Imu_Init()

  Set up IMU sensor.
 
  The IMU sensor provides acceleration, rotation and magnetic feild
  readings to the controller.

  Datasheet values: 
  I2C ready time (Typical):  11 mS
  I2C ready time (Maximum): 100 mS

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
void App_Sensor_Imu_Init(void)
{
   /******* initializing IMU Sensor *******/
   uint8_t error_code = IMU_Sensor_Init(I2C1);
   if(error_code)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
}

/*----------------------------------------------------------------------------*-

  App_Sensor_Imu_Update()

  Update IMU sensor readings.
 
  The IMU sensor provides acceleration, rotation and magnetic feild
  readings to the controller.

  Datasheet values: 
  I2C ready time (Typical):  11 mS
  I2C ready time (Maximum): 100 mS

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
uint32_t App_Sensor_Imu_Update(void)
{
   uint8_t error_code = IMU_Sensor_Update(I2C1);
   if(error_code)   
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
    
   return RETURN_NORMAL_STATE;
}
