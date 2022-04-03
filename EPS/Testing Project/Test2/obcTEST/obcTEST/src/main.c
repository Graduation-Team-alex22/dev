/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f4xx.h"

static void RCC_Config(void);
static void GPIO_Config(void);

DMA_InitTypeDef DMA_InitStructure;
USART_InitTypeDef USART_InitStructure;

#include <string.h>
#define TXBUFFERSIZE  (countof(TxBuffer) - 1)
#define countof(a)   (sizeof(a) / sizeof(*(a)))

uint8_t TxBuffer[] = "\r\nabbbccbsdjk\r\n";

int main(void)
{
	 RCC_Config();
	 GPIO_Config();

   USART_InitStructure.USART_BaudRate = 115200;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Tx;
   USART_Init(USART3, &USART_InitStructure);
   USART_Cmd(USART3, ENABLE);

   DMA_DeInit(DMA1_Stream4);
   DMA_InitStructure.DMA_BufferSize = TXBUFFERSIZE;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Channel = DMA_Channel_7 ;
   DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)TxBuffer;
   DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
 // DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
   //DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
   DMA_Init(DMA1_Stream4, &DMA_InitStructure);
   for(;;)
   {
		 USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
		 DMA_Cmd(DMA1_Stream4,ENABLE);

		 while(!DMA_GetFlagStatus(DMA1_Stream4,DMA_FLAG_TCIF4)){}

		 	  DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);
		 	   DMA_Cmd(DMA1_Stream4, DISABLE);

		 	while(DMA_GetCmdStatus(DMA1_Stream4));
   }

	for(;;);
}


static void RCC_Config(void)
 {
   /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
   /* Enable USARTs Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
   /* Enable the DMA periph */
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
 }

static void GPIO_Config(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;

   GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
   GPIO_Init(GPIOD, &GPIO_InitStructure);
  }
