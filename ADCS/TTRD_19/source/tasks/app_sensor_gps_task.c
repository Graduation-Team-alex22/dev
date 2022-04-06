#include "stdint.h"
#include "../adcs/adcs_sensors/gps_sensor.h"
#include "../main/project.h"
#include "app_sensor_gps_task.h" 

#ifdef DIAGNOSIS_OUTPUT
#include "stdio.h"
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
   error_t error_code = GPS_Sensor_Update();
   if(error_code != NO_ERROR)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }

   #ifdef DIAGNOSIS_OUTPUT
      char buff[200];
      gps_sensor_t gps_data = GPS_Sensor_GetData();
      sprintf(buff, "fix: %d \t lat: %f \t long: %f \t alt: %f\n utc: %f\tsatnum: %d, DOP: %f  %f  %f",
         gps_data.d3fix, gps_data.p_gps_lla[0], gps_data.p_gps_lla[1], gps_data.p_gps_lla[2],
         gps_data.utc_time, gps_data.num_sat, gps_data.DOP[0], gps_data.DOP[1], gps_data.DOP[2]);
   
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - Update] GPS Update\n");
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - Data]: \n");
      UART2_BUF_O_Write_String_To_Buffer(buff);
      UART2_BUF_O_Send_All_Data();
   #endif
   
   return NO_ERROR;
}
