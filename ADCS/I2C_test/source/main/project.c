#include "../hsi_library/stm32f4xx_gpio.h"
#include "../hsi_library/stm32f4xx_i2c.h"
#include "../hsi_library/stm32f4xx_usart.h"
#include "../hsi_library/stm32f4xx_rcc.h"
#include "../hsi_library/stm32f4xx_dma.h"

#include "project.h"

#include "../adcs/adcs_sensors/gps_sensor.h"

//-- Private funtions -----------------------
void I2C1_Init(void);
void UART4_DMA_RX_Init(void);
void Activation_Pins_Init(void);

// initializes the peripherals needed in the project
void Project_MSP_Init(void)
{
   I2C1_Init();
   UART4_DMA_RX_Init();
   
}

void I2C1_Init()
{
   /******* initializing I2C1 peripheral: SCL @ PB8, SDA @ PB9 *******/
   // I2C1 Init
   I2C_InitTypeDef I2C_InitStruct;
   // I2C1 clock enable 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
   //I2C1 configuration
   I2C_InitStruct.I2C_ClockSpeed    = PROJECT_I2C1_CLOCK;
   I2C_InitStruct.I2C_Mode          = I2C_Mode_I2C;
   I2C_InitStruct.I2C_OwnAddress1   = 0;
   I2C_InitStruct.I2C_Ack           = I2C_Ack_Disable;
   I2C_InitStruct.I2C_DutyCycle     = I2C_DutyCycle_2;
   I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
   I2C_Init(I2C1, &I2C_InitStruct);
   // Enable I2C1
   I2C_Cmd(I2C1, ENABLE);
   
   
   // GPIO Init
   GPIO_InitTypeDef GPIO_InitStruct;
   // GPIOB clock enable 
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
   // GPIO config
   GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
   GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
   GPIO_Init(GPIOB, &GPIO_InitStruct);
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
   
}

// GPS UART
void UART4_DMA_RX_Init(void)
{
   
   
}




void HardFault_Handler(void)
{
   while(1);
}
