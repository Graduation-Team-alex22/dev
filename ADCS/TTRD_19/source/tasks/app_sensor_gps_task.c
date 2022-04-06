#include "stdint.h"
#include "../adcs/adcs_sensors/gps_sensor.h"
#include "../main/project.h"
#include "app_sensor_gps_task.h" 

#ifdef DIAGNOSIS_OUTPUT
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#endif

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
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - INIT] GPS Init\n");
      UART2_BUF_O_Send_All_Data();
   #endif
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
   return NO_ERROR;
}
