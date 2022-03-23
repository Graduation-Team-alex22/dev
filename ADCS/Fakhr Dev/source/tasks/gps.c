#include "gps.h"
#include "../port/port.h" 
#include <string.h>
#include "ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#include "..\main\main.h"
#include <stdio.h>
// ------ Private constants --------------------------------------------------

// The reciever buffer length
#define RX_BUFFER_SIZE_BYTES ((uint16_t)512)


// ------ Private variables --------------------------------------------------

static char Rx_buffer_g[RX_BUFFER_SIZE_BYTES];
char buff[20];
static USART_TypeDef *uartx_g;

// space for NMEA sentences
static char nmea_sentences_g[10][96] = {0};
static uint8_t nmea_pointer_g = 0;

uint8_t GPS_Sensor_Init(USART_TypeDef * uartx)
{
	 uartx_g = uartx;
	
	 GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
	 DMA_InitTypeDef  DMA_InitStructure;

   // USART3 clock enable 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

   // GPIOB clock enable 
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	  /* Enable the DMA clock */
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

   // GPIO config
   GPIO_InitStructure.GPIO_Pin   = UART3_RX_PIN; 
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
   GPIO_Init(UART3_PORT, &GPIO_InitStructure);

	 GPIO_PinAFConfig(GPIOB, UART3_RX_PINSOURCE, GPIO_AF_USART3);

   // USART3 configuration
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
   USART_Init(USART3, &USART_InitStructure);


	/* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_BufferSize = RX_BUFFER_SIZE_BYTES ;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(uartx_g->DR)) ;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;

	/* Configure RX DMA */
  DMA_InitStructure.DMA_Channel = DMA_Channel_4 ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)Rx_buffer_g ; 
  DMA_Init(DMA1_Stream1,&DMA_InitStructure);

   // Enable UART3
   USART_Cmd(USART3, ENABLE);
	 
	 /* Enable DMA USART RX Stream */
  DMA_Cmd(DMA1_Stream1,ENABLE);
	 
	 /* Enable USART DMA RX Requsts */
  USART_DMACmd(uartx_g, USART_DMAReq_Rx, ENABLE);
	
	
	 //Init timeout
	 TIMEOUT_T3_USEC_Init();
}


uint32_t GPS_Sensor_Update(void)
{
	static uint8_t stage_index = 1;
	static uint16_t nmea_sen_start =0;
	static uint16_t buffer_pointer_old =0;
	uint16_t buffer_pointer_curr;
	
	// If there is no sentences to procces, get new sentences from the buffer  
	if(nmea_pointer_g == 0)
	{
		UART2_BUF_O_Write_String_To_Buffer("First Stage\n");
		// check if new data has been received
		buffer_pointer_curr = RX_BUFFER_SIZE_BYTES - DMA_GetCurrDataCounter(DMA1_Stream1);
		
		char c[20]; sprintf(c, "old: %d\n", buffer_pointer_old);
		UART2_BUF_O_Write_String_To_Buffer(c);
		sprintf(c, "new: %d\n", buffer_pointer_curr);
		UART2_BUF_O_Write_String_To_Buffer(c);
		
		if(buffer_pointer_curr > buffer_pointer_old)
		{
			// find sentences
			nmea_sen_start = buffer_pointer_old;
			for( uint16_t i = buffer_pointer_old; i < buffer_pointer_curr; i++ )
			{
				if(Rx_buffer_g[i] == '$')
				{
					nmea_sen_start = i;
					//UART2_BUF_O_Write_String_To_Buffer("Found $\n");
				}
				
				nmea_sentences_g[nmea_pointer_g][i - nmea_sen_start] = Rx_buffer_g[i];
				
				if(Rx_buffer_g[i] == '\n')
				{
					nmea_sentences_g[nmea_pointer_g][i - nmea_sen_start + 1 ] = 0;
					nmea_sen_start = i+1;
					buffer_pointer_old = nmea_sen_start;
					nmea_pointer_g++;
					//UART2_BUF_O_Write_String_To_Buffer("Found N\n");
				}
			}
		}
	}
	else
	{
		UART2_BUF_O_Write_String_To_Buffer("Second Stage\nSEN_NUM:\n");
		char b[20]; sprintf(b, "%d\n", nmea_pointer_g);
		UART2_BUF_O_Write_String_To_Buffer(b);
		
		UART2_BUF_O_Write_String_To_Buffer(nmea_sentences_g[--nmea_pointer_g]);	
	}
	
	
	UART2_BUF_O_Send_All_Data();
		
	return RETURN_NORMAL_STATE;
}

uint8_t GPS_Sensor_GetData(void)
{
		
	
}
