#include "stdint.h"
#include "../port/port.h"
#include "../adcs/adcs_sensors/gps_sensor.h"

#include "app_sensor_gps_task.h" 

/*----------------------------------------------------------------------------*-

  App_Sensor_Gps_Init()

  Sets up GPS sensor.
 
  The GPS sensor provides GPS info to the controller.

  Datasheet values: 
     None.

  PARAMETERS:
     None. 
   
  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     UART2 peripheral.
     DMA1 Stream 2.
     Port A pin 1 (RX).

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void App_Sensor_Gps_Init(void)
{
   GPS_Sensor_Init( DMA1_Stream2 );
}

/*----------------------------------------------------------------------------*-

  App_Sensor_Gps_Update()

  Updates GPS sensor readings.
 
  The GPS sensor provides GPS info to the controller.

  Datasheet values: 
     None.

  PARAMETERS:
     None. 
   
  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     UART2 peripheral.
     DMA1 Stream 2.
     Port A pin 1 (RX).

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
uint32_t App_Sensor_Gps_Update(void)
{
   GPS_Sensor_Update();
   return RETURN_NORMAL_STATE;
}
