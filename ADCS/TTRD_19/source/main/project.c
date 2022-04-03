#include "../hsi_library/stm32f4xx_gpio.h"
#include "../hsi_library/stm32f4xx_i2c.h"
#include "../hsi_library/stm32f4xx_usart.h"

#include "project.h"

#include "../adcs/adcs_sensors/gps_sensor.h"

//-- Private funtions -----------------------
void I2C1_Init(void);
void UART4_DMA_RX_Init(void);

// initializes the peripherals needed in the project
void Project_MSP_Init(void)
{
   I2C1_Init();
   UART4_DMA_RX_Init();
}

void I2C1_Init()
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

   // I2C1 Init
   I2C_InitTypeDef I2C_InitStruct;

   // I2C1 clock enable 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

   //I2C1 configuration
   I2C_InitStruct.I2C_ClockSpeed = PROJECT_I2C1_CLOCK;
   I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
   I2C_InitStruct.I2C_OwnAddress1 = 0;
   I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
   I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
   I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

   I2C_Init(I2C1, &I2C_InitStruct);

   // Enable I2C1
   I2C_Cmd(I2C1, ENABLE);
}

void UART4_DMA_RX_Init(void)
{
   //----------set up GPIO pin A1 as usart4 RX pin-----------
   GPIO_InitTypeDef GPIO_InitStructure;
   // GPIOB clock enable 
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
   // GPIO config
   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1; 
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);

   USART_InitTypeDef USART_InitStructure;
   DMA_InitTypeDef  DMA_InitStructure;

  // USART4 clock enable 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
  /* Enable the DMA clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

  // USART configuration
  // - BaudRate as specified in function parameter
  // - Word Length = 8 Bits
  // - One Stop Bit
  // - No parity
  // - Hardware flow control disabled (RTS and CTS signals)
  // - Tx and Rx enabled
  USART_InitStructure.USART_BaudRate = GPS_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx;
  USART_Init(UART4, &USART_InitStructure);

  /* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_BufferSize = GPS_RX_BUFFER_SIZE ;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(UART4->DR)) ;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;

  // Configure RX DMA
  DMA_InitStructure.DMA_Channel = DMA_Channel_4 ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)GPS_Sensor_get_pBuffer() ; 
  DMA_Init(DMA1_Stream2, &DMA_InitStructure);

  // Enable UART4
  USART_Cmd(UART4, ENABLE);

  // Enable DMA USART RX Stream 
  DMA_Cmd(DMA1_Stream2,ENABLE);

  // Enable USART DMA RX Requsts 
  USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
   
}

