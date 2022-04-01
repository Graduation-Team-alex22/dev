#include "stdint.h"
#include "../port/port.h"
#include "../adcs/adcs_sensors/imu_sensor.h"

#include "app_sensor_imu_task.h"

#include "ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"

void App_Sensor_Imu_Init(void)
{
   /******* initializing I2C1 peripheral: SCL @ PB8, SDA @ PB9 *******/
   // GPIO Init
   GPIO_InitTypeDef GPIO_InitStruct;

   // GPIOB clock enable 
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

   // GPIO config
   GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
   GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
   GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

   GPIO_Init(GPIOB, &GPIO_InitStruct);

   GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

   REG_CONFIG_CHECKS_GPIO_Store();

   // I2C1 Init
   I2C_InitTypeDef I2C_InitStruct;

   // I2C1 clock enable 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

   //I2C1 configuration
   I2C_InitStruct.I2C_ClockSpeed = IMU_I2C_CLOCK;
   I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
   I2C_InitStruct.I2C_OwnAddress1 = 0;
   I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
   I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
   I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

   I2C_Init(I2C1, &I2C_InitStruct);

   //I2C_StretchClockCmd(I2C1, ENABLE);

   // Enable I2C1
   I2C_Cmd(I2C1, ENABLE);

   /******* initializing IMU Sensor *******/
   uint8_t error_code = IMU_Init(I2C1);
   if(error_code)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
   UART2_BUF_O_Write_String_To_Buffer("IMU Init Done\n");
   UART2_BUF_O_Send_All_Data();
}

uint32_t App_Sensor_Imu_Update(void)
{
   uint8_t error_code = IMU_Update(I2C1);
   if(error_code)   
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }

   /*imu_sensor_t t = IMU_Get_Data();

   sprintf(buf, "A: %+.3f %+.3f %+.3f  G: %+.3f %+.3f %+.3f  M: %+.3f %+.3f %+.3f\n",
                        t.Ax, t.Ay, t.Az, t.Gx, t.Gy, t.Gz, t.Mx, t.My, t.Mz);
   UART2_BUF_O_Write_String_To_Buffer(buf);*/
    
   return RETURN_NORMAL_STATE;
}
