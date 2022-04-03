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
#include "stm32f4xx_rcc.h"
#include "stdio.h"
#include "string.h"

#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))
uint8_t tx_buf[512] = "\r\nIAM GONNA PUT YOU IN THE WORST MODE AH, P1 CLEANER THAN YOUR CHURCHE, MILI POINT TWO JUST TO HURT YOU AH, ALL RED LAMPS JUST TO TEASE YOU AH, NONE OF THESE TOYS ON LEASE TOO AH, MADE YOUR WHOLE YEAR IN A WEEK TOO AH.\r\n";	
	
void USART2_Config(void);
void MYDMA_Init(DMA_Stream_TypeDef* DMA_Streamx, uint32_t chx, uint32_t padd, uint32_t madd, uint16_t ndtr);
void MYDMA_Enable(DMA_Stream_TypeDef* DMA_Streamx, uint16_t ndtr);

void Delay_ms(uint32_t delay);

int main(void)
{
	USART2_Config();
    	
	
	
	
	while(1)
	{
		MYDMA_Init(DMA1_Stream6, DMA_Channel_4, (uint32_t)&(USART2->DR), (uint32_t)tx_buf, ARRAY_LEN(tx_buf));
		DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5);
		DMA_Cmd (DMA1_Stream5, DISABLE);
    MYDMA_Enable(DMA1_Stream6, ARRAY_LEN(tx_buf));
		//while(!DMA_GetFlagStatus(DMA1_Stream5,DMA_FLAG_TCIF5));
		Delay_ms(1000);


	}
	
	
}








//DMA initialization
void MYDMA_Init(DMA_Stream_TypeDef* DMA_Streamx, uint32_t chx, uint32_t padd, uint32_t madd, uint16_t ndtr)
{

	DMA_InitTypeDef DMA_InitStructure;



	// Judge whether it is DMA1 perhaps DMA2
	if ((u32)DMA_Streamx > (u32)DMA2)
	{

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	}
	else
	{

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	}

	// Uninitialize
	DMA_DeInit(DMA_Streamx);

	// wait for DMA You can configure the
	while(DMA_GetCmdStatus(DMA_Streamx) != DISABLE)
	{

	}


	// To configure DMA Stream


	// Channel selection
	DMA_InitStructure.DMA_Channel = chx;
	//DMA Peripheral address
	DMA_InitStructure.DMA_PeripheralBaseAddr = padd;
	//DMA Memory 0 Address
	DMA_InitStructure.DMA_Memory0BaseAddr = madd;
	// Memory to peripheral mode
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	// Data transmission volume
	DMA_InitStructure.DMA_BufferSize = ndtr;
	// Peripheral non incremental mode
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// Memory incremental mode
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	// Peripheral data length 8 position
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	// Memory data length 8 position
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	// Use normal mode
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	// medium priority
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	// close FIFO Pattern
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	// choice FIFO threshold   Has not started FIFO  Whatever you choose
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	// Memory burst single transmission
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	// Peripheral burst single transmission
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	// initialization
	DMA_Init(DMA_Streamx, &DMA_InitStructure);
}

//DMA Can make
void MYDMA_Enable(DMA_Stream_TypeDef* DMA_Streamx, uint16_t ndtr)
{

	// close DMA transmission
	DMA_Cmd(DMA_Streamx, DISABLE);

	// Guarantee DMA Can be set
	while(DMA_GetCmdStatus(DMA_Streamx) != DISABLE)
	{

	}

	// Set the amount of data transfer
	DMA_SetCurrDataCounter(DMA_Streamx, ndtr);

	// Turn on DMA transmission
	DMA_Cmd(DMA_Streamx, ENABLE);
}

void USART2_Config(void)
   {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Enable USARTs Clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/*
	     * USART2 GPIO Configuration
	     *
	     * PA2   ------> USART2_TX
	     * PA3  ------> USART2_RX
	*/

	  // GPIO config
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
	  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

	  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
	  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
	  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);


   // USART2 configuration
   // - BaudRate as specified in function parameter
   // - Word Length = 8 Bits
   // - One Stop Bit
   // - No parity
   // - Hardware flow control disabled (RTS and CTS signals)
   // - Tx & Rx enabled

   USART_InitStructure.USART_BaudRate = 115200;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART2, &USART_InitStructure);
   USART_Cmd(USART2, ENABLE);
   USART_DMACmd(USART2, USART_DMAReq_Rx|USART_DMAReq_Tx, ENABLE);

   }

void Delay_ms(uint32_t delay)
	 {
		 delay *= 45000;
		 while(delay--);
	 }

