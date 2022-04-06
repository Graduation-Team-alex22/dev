#include "stdint.h"
#include "../adcs/adcs_sensors/imu_sensor.h"
#include "../processor/ttrd2-19a-t0401a-v001b_processor.h"
#include "app_sensor_imu_task.h"
#include "../main/project.h"

#ifdef DIAGNOSIS_OUTPUT
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#endif

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
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - INIT] IMU Init\n");
      UART2_BUF_O_Send_All_Data();
   #endif
    error_t error_code = IMU_Sensor_Init(I2C1);
   if(error_code != NO_ERROR)
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
    
   return 0;
}
