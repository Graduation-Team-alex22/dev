#include "../main/project.h"
#include "app_sensor_health_task.h"
#include "app_sensor_gps_task.h"
#include "app_sensor_imu_task.h"
#include "app_sensor_mgn_task.h"
#include "app_sensor_tmp_task.h"

#include "../adcs/adcs_sensors/gps_sensor.h"
#include "../adcs/adcs_sensors/imu_sensor.h"
#include "../adcs/adcs_sensors/mgn_sensor.h"
#include "../adcs/adcs_sensors/tmp_sensor.h"

#ifdef DIAGNOSIS_OUTPUT
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#endif


void App_Sensor_Health_Init(void)
{
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - Health Check] Init\n");
      UART2_BUF_O_Send_All_Data();
   #endif
  
}

uint32_t App_Sensor_Health_Update(void)
{   
   // check on GPS sensor status
   switch(GPS_Sensor_GetData().status)
   {
      case DEVICE_BROKEN:
         #ifdef DIAGNOSIS_OUTPUT
            UART2_BUF_O_Write_String_To_Buffer("[DIAG - HEALTH] GPS Sensor Broken \n");
            UART2_BUF_O_Send_All_Data();
         #endif
         // switch to the other instance
         GPS_Change_Activated_Module();
      
         // update the status
         GPS_Sensor_SetStatus(DEVICE_REINIT);
      break;
      
      case DEVICE_REINIT:
         #ifdef DIAGNOSIS_OUTPUT
            UART2_BUF_O_Write_String_To_Buffer("[DIAG - HEALTH] GPS Sensor Reinit \n");
            UART2_BUF_O_Send_All_Data();
         #endif
         // reinit the device
         App_Sensor_Gps_Init();
      break;
      
      case READING_ERROR:
         
      break;
      
      case DEVICE_OK:
         
      break;
   }
   
   // check on IMU sensor status
   switch( IMU_Sensor_GetData().status )
   {
      case DEVICE_BROKEN:
         #ifdef DIAGNOSIS_OUTPUT
            UART2_BUF_O_Write_String_To_Buffer("[DIAG - HEALTH] IMU Sensor Broken \n");
            UART2_BUF_O_Send_All_Data();
         #endif
         // switch to the other instance
         IMU_Change_Activated_Module();
      
         // update the status
         IMU_Sensor_SetStatus(DEVICE_REINIT);
         
      break;
      
      case DEVICE_REINIT:
         #ifdef DIAGNOSIS_OUTPUT
            UART2_BUF_O_Write_String_To_Buffer("[DIAG - HEALTH] IMU Sensor Reinit \n");
            UART2_BUF_O_Send_All_Data();
         #endif
         // reinit the device
         App_Sensor_Imu_Init();
      break;
      
      case READING_ERROR:
         
      break;
      
      case DEVICE_OK:
         
      break;
   }
   
   // check on MGN sensor status
   switch( MGN_Sensor_GetData().status )
   {
      case DEVICE_BROKEN:
         #ifdef DIAGNOSIS_OUTPUT
            UART2_BUF_O_Write_String_To_Buffer("[DIAG - HEALTH] MGN Sensor Broken \n");
            UART2_BUF_O_Send_All_Data();
         #endif
         // switch to the other instance
         MGN_Change_Activated_Module();
      
         // update the status
         MGN_Sensor_SetStatus(DEVICE_REINIT);
         
      break;
      
      case DEVICE_REINIT:
         #ifdef DIAGNOSIS_OUTPUT
            UART2_BUF_O_Write_String_To_Buffer("[DIAG - HEALTH] MGN Sensor Reinit \n");
            UART2_BUF_O_Send_All_Data();
         #endif
         // reinit the device
         App_Sensor_Mgn_Init();
      break;
      
      case READING_ERROR:
         
      break;
      
      case DEVICE_OK:
         
      break;
   }
   
   // check on TMP sensor status
   switch( TMP_Sensor_GetData().status )
   {
      case DEVICE_BROKEN:
         #ifdef DIAGNOSIS_OUTPUT
            UART2_BUF_O_Write_String_To_Buffer("[DIAG - HEALTH] TMP Sensor Broken \n");
            UART2_BUF_O_Send_All_Data();
         #endif
         // switch to the other instance
         TMP_Change_Activated_Module();
         
         // update the status
         TMP_Sensor_SetStatus(DEVICE_REINIT);
         
      break;
      
      case DEVICE_REINIT:
         #ifdef DIAGNOSIS_OUTPUT
            UART2_BUF_O_Write_String_To_Buffer("[DIAG - HEALTH] TMP Sensor Reinit \n");
            UART2_BUF_O_Send_All_Data();
         #endif
         // reinit the device
         App_Sensor_Tmp_Init();
      break;
      
      case READING_ERROR:
         
      break;
      
      case DEVICE_OK:
         
      break;
   }
   
   return NO_ERROR;
}
