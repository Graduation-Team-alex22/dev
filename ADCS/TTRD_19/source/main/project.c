#include "../hsi_library/stm32f4xx_gpio.h"
#include "../hsi_library/stm32f4xx_i2c.h"
#include "../hsi_library/stm32f4xx_usart.h"
#include "../hsi_library/stm32f4xx_dma.h"
#include "../scheduler/ttrd2-19a-t0401a-v001c_scheduler.h"

#include "project.h"

#include "../adcs/adcs_sensors/gps_sensor.h"
#include "../adcs/services_utilities/obc_comm.h"

//-- Private funtions -----------------------
void I2C1_Init(void);
void UART4_DMA_RX_Init(void);
void Activation_Pins_Init(void);
void OBC_COMM_USART_DMA_Init(uint16_t BAUD_RATE);

// initializes the peripherals needed in the project
void Project_MSP_Init(void)
{
   I2C1_Init();
   UART4_DMA_RX_Init();
   OBC_COMM_USART_DMA_Init(9600);
}

void I2C1_Init()
{
   /******* initializing I2C1 peripheral: SCL @ PB8, SDA @ PB9 *******/

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
}

// GPS UART
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


void Activation_Pins_Init(void)
{
   // GPIO Init
   GPIO_InitTypeDef GPIO_InitStruct;

   // clock enable 
   if( SENSOR_ACTIVATION_PORT == GPIOA )
   {
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
   }
   else
   {
      // in case another developer used another port for sensor activation
      // but forgot to add a case for it above
      assert_failed((uint8_t *)__FILE__, __LINE__);
   }
   
   // GPIO config
   GPIO_InitStruct.GPIO_Pin   = GPS_ACTIVATION_PIN | IMU_ACTIVATION_PIN | MGN_ACTIVATION_PIN | TMP_ACTIVATION_PIN;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed; 
   GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;

   GPIO_Init(SENSOR_ACTIVATION_PORT, &GPIO_InitStruct);
   
   GPIO_ResetBits(SENSOR_ACTIVATION_PORT, GPS_ACTIVATION_PIN);
   GPIO_ResetBits(SENSOR_ACTIVATION_PORT, IMU_ACTIVATION_PIN);
   GPIO_ResetBits(SENSOR_ACTIVATION_PORT, MGN_ACTIVATION_PIN);
   GPIO_ResetBits(SENSOR_ACTIVATION_PORT, TMP_ACTIVATION_PIN);
}


void OBC_COMM_USART_DMA_Init(uint16_t BAUD_RATE)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
   DMA_InitTypeDef  DMA_InitStructure;
   
   // USART clock enable 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

   // GPIO clock enable 
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		 
   /* Enable the DMA clock */
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
   
   // GPIO pin config TX - PB10
   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10; 
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
   GPIO_Init(GPIOB, &GPIO_InitStructure);

   GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
   
   // USART configuration
   // - BaudRate as specified in function parameter
   // - Word Length = 8 Bits
   // - One Stop Bit
   // - No parity
   // - Hardware flow control disabled (RTS and CTS signals)
   // - Tx (only) enabled
   USART_InitStructure.USART_BaudRate = BAUD_RATE;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Tx;
   USART_Init(USART3, &USART_InitStructure);
   
   /* Configure DMA Initialization Structure */
   DMA_InitStructure.DMA_BufferSize = 1;  // doesn't really matter
   DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
   DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
   DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART3->DR)) ;
   DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
   
   /* Configure TX DMA stream - stream 3 channel 4 */
   DMA_InitStructure.DMA_Channel = DMA_Channel_4 ;
   DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
   DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)OBC_Comm_GetpBuffer_TX();
   DMA_Init(DMA1_Stream3,&DMA_InitStructure);
   
   // Enable UART3
   USART_Cmd(USART3, ENABLE);

   // Enable USART DMA TX Requsts 
   USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
   
   
   // zero the NDTR register initially
   DMA1_Stream3->NDTR = 0;
}


void HardFault_Handler(void)
{
   // print task id
   static uint32_t tid = 0;
   tid = SCH_Get_LastRunTaskID();
   while(1);
}
