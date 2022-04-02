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
#include "stdio.h"
#include "string.h"

#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))

uint8_t usart_rx_dma_buffer[64];

static void RCC_Config(void);
void USART2_Config(void);

void usart_rx_check(void);
void usart_process_data(const void* data, size_t len);
void usart_send_string(const char* str);


int main(void)
{
	
	RCC_Config();
	USART2_Config();

	usart_send_string("USART DMA example: DMA HT & TC + USART IDLE LINE interrupts\r\n");
	usart_send_string("Start sending data to STM32\r\n");

	DMA_Cmd(DMA1_Stream5, ENABLE);







	while(1)
	{
			usart_rx_check();


		//while(!DMA_GetFlagStatus(DMA1_Stream5,DMA_FLAG_TCIF5));
		//usart_rx_check();
		//DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5);
		//DMA_Cmd (DMA1_Stream5, DISABLE);


	}
}


static void RCC_Config(void)
 {

   /* Enable Syscfg */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
 }




void USART2_Config(void)
   {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef     hdma_usart2_rx;


	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Enable USARTs Clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Enable the DMA periph */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

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


	hdma_usart2_rx.DMA_Channel = DMA_Channel_4;
	hdma_usart2_rx.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
	hdma_usart2_rx.DMA_Memory0BaseAddr = (uint32_t)usart_rx_dma_buffer;
	hdma_usart2_rx.DMA_DIR = DMA_DIR_PeripheralToMemory;
	hdma_usart2_rx.DMA_BufferSize = ARRAY_LEN(usart_rx_dma_buffer);
	hdma_usart2_rx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	hdma_usart2_rx.DMA_MemoryInc = DMA_MemoryInc_Enable;
	hdma_usart2_rx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	hdma_usart2_rx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	hdma_usart2_rx.DMA_Mode = DMA_Mode_Circular;
	hdma_usart2_rx.DMA_Priority = DMA_Priority_High;
	hdma_usart2_rx.DMA_FIFOMode = DMA_FIFOMode_Disable;
	hdma_usart2_rx.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	hdma_usart2_rx.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	hdma_usart2_rx.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream5, &hdma_usart2_rx);


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



/**
 * \brief           Check for new data received with DMA
 *
 * User must select context to call this function from:
 * - Only interrupts (DMA HT, DMA TC, UART IDLE) with same preemption priority level
 * - Only thread context (outside interrupts)
 *
 * If called from both context-es, exclusive access protection must be implemented
 * This mode is not advised as it usually means architecture design problems
 *
 * When IDLE interrupt is not present, application must rely only on thread context,
 * by manually calling function as quickly as possible, to make sure
 * data are read from raw buffer and processed.
 *
 * Not doing reads fast enough may cause DMA to overflow unread received bytes,
 * hence application will lost useful data.
 *
 * Solutions to this are:
 * - Improve architecture design to achieve faster reads
 * - Increase raw buffer size and allow DMA to write more data before this function is called
 */
void
usart_rx_check(void) {
    /*
     * Set old position variable as static.
     *
     * Linker should (with default C configuration) set this variable to `0`.
     * It is used to keep latest read start position,
     * transforming this function to not being reentrant or thread-safe
     */
    static size_t old_pos;
    size_t pos;

    /* Calculate current position in buffer and check for new data available */
    pos = ARRAY_LEN(usart_rx_dma_buffer) - DMA_GetCurrDataCounter(DMA1_Stream5);
    if (pos != old_pos) {                       /* Check change in received data */
        if (pos > old_pos) {                    /* Current position is over previous one */
            /*
             * Processing is done in "linear" mode.
             *
             * Application processing is fast with single data block,
             * length is simply calculated by subtracting pointers
             *
             * [   0   ]
             * [   1   ] <- old_pos |------------------------------------|
             * [   2   ]            |                                    |
             * [   3   ]            | Single block (len = pos - old_pos) |
             * [   4   ]            |                                    |
             * [   5   ]            |------------------------------------|
             * [   6   ] <- pos
             * [   7   ]
             * [ N - 1 ]
             */
            usart_process_data(&usart_rx_dma_buffer[old_pos], pos - old_pos);
        } else {
            /*
             * Processing is done in "overflow" mode..
             *
             * Application must process data twice,
             * since there are 2 linear memory blocks to handle
             *
             * [   0   ]            |---------------------------------|
             * [   1   ]            | Second block (len = pos)        |
             * [   2   ]            |---------------------------------|
             * [   3   ] <- pos
             * [   4   ] <- old_pos |---------------------------------|
             * [   5   ]            |                                 |
             * [   6   ]            | First block (len = N - old_pos) |
             * [   7   ]            |                                 |
             * [ N - 1 ]            |---------------------------------|
             */
            usart_process_data(&usart_rx_dma_buffer[old_pos], ARRAY_LEN(usart_rx_dma_buffer) - old_pos);
            if (pos > 0) {
                usart_process_data(&usart_rx_dma_buffer[0], pos);
            }
        }
        old_pos = pos;                          /* Save current position as old for next transfers */
    }
}






/**
 * \brief           Process received data over UART
 * \note            Either process them directly or copy to other bigger buffer
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
void usart_process_data(const void* data, size_t len)
{
    const uint8_t* d = data;

    /*
     * This function is called on DMA TC or HT events, and on UART IDLE (if enabled) event.
     *
     * For the sake of this example, function does a loop-back data over UART in polling mode.
     * Check ringbuff RX-based example for implementation with TX & RX DMA transfer.
     */

    for (; len > 0; --len, ++d) {
        USART_SendData(USART2, *d);
        while (!USART_GetFlagStatus(USART2 , USART_FLAG_TXE)) {}
    }
    while (!USART_GetFlagStatus(USART2 , USART_FLAG_TXE)) {}
}


/**
 * \brief           Send string to USART
 * \param[in]       str: String to send
 */

void usart_send_string(const char* str)
{
    usart_process_data(str, strlen(str));
}


