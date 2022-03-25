#include "gps_sensor.h"


// ------ Private constants --------------------------------------------------


// ------ Private variables --------------------------------------------------
static char Rx_buffer_g[GPS_RX_BUFFER_SIZE];
static DMA_Stream_TypeDef * dma_streamx_g;


// space for NMEA sentences
static char nmea_sentences_g[10][96] = {0};
static uint8_t nmea_pointer_g = 0;


/*
GPS_Sensor_Init
  Initializes the USARTx peipheral and sets up the GPS sensor.
  This module Uses DMA1_Stream1

@note:
  Application should take care of setting the needed GPIO pins


*/
uint8_t GPS_Sensor_Init(USART_TypeDef * uartx, DMA_Stream_TypeDef * DMA_streamx, uint32_t DMA_channelx)
{	
  assert_param(IS_USART_ALL_PERIPH(uartx));
  assert_param(IS_DMA_ALL_PERIPH(DMA_streamx));
  assert_param(IS_DMA_CHANNEL(DMA_channelx));
  
	dma_streamx_g = DMA_streamx;
	
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
  USART_Init(uartx, &USART_InitStructure);

  /* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_BufferSize = GPS_RX_BUFFER_SIZE ;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(uartx->DR)) ;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;

  // Configure RX DMA
  DMA_InitStructure.DMA_Channel = DMA_channelx ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)Rx_buffer_g ; 
  DMA_Init(DMA_streamx, &DMA_InitStructure);

  // Enable UART3
  USART_Cmd(uartx, ENABLE);

  // Enable DMA USART RX Stream 
  DMA_Cmd(DMA_streamx,ENABLE);

  // Enable USART DMA RX Requsts 
  USART_DMACmd(uartx, USART_DMAReq_Rx, ENABLE);
	
	return 0;
}


uint16_t GPS_Sensor_Update(void)
{
	// an index to mark where to start proccessing
	static uint16_t buffer_start_index =0;
	// an index to mark the
	uint16_t buffer_end_index = 0;
	uint16_t sentence_char_index = 0;
	
	//get new sentences from the buffer  
	// check if new data has been received
	buffer_end_index = GPS_RX_BUFFER_SIZE - DMA_GetCurrDataCounter(dma_streamx_g);
	
	if(buffer_end_index > buffer_start_index)
	{
		// find sentences
		for( uint16_t i = buffer_start_index; i < buffer_end_index; i++ )
		{
			if(Rx_buffer_g[i] == '$')
			{
				// write to the beginning of the sentence
				sentence_char_index = 0;
				// mark this location as the start of NMEA sentence
				buffer_start_index = i;
			}
			
			nmea_sentences_g[nmea_pointer_g][sentence_char_index++] = Rx_buffer_g[i];
			
			if(Rx_buffer_g[i] == '\n')
			{
				nmea_sentences_g[nmea_pointer_g][sentence_char_index] = 0;
				// mark this location as the end of NMEA sentence
				buffer_start_index = i;
				nmea_pointer_g++;
				sentence_char_index = 0;
			}
		}
	}
	// check if overflow has happened in circular buffer
	else if(buffer_end_index < buffer_start_index)
	{
		// find sentences
		for( uint16_t i = buffer_start_index; i < GPS_RX_BUFFER_SIZE; i++ )
		{
			if(Rx_buffer_g[i] == '$')
			{
				// write to the beginning of the sentence
				sentence_char_index = 0;
				// mark this location as the start of NMEA sentence
				buffer_start_index = i;
			}
			
			nmea_sentences_g[nmea_pointer_g][sentence_char_index++] = Rx_buffer_g[i];
			
			if(Rx_buffer_g[i] == '\n')
			{
				nmea_sentences_g[nmea_pointer_g][sentence_char_index] = 0;
				// mark this location as the end of NMEA sentence
				buffer_start_index = i;
				nmea_pointer_g++;
				sentence_char_index = 0;
			}
		}
		for( uint16_t i = 0; i < buffer_end_index; i++ )
		{
			if(Rx_buffer_g[i] == '$')
			{
				// write to the beginning of the sentence
				sentence_char_index = 0;
				// mark this location as the start of NMEA sentence
				buffer_start_index = i;
			}
			
			nmea_sentences_g[nmea_pointer_g][sentence_char_index++] = Rx_buffer_g[i];
			
			if(Rx_buffer_g[i] == '\n')
			{
				nmea_sentences_g[nmea_pointer_g][sentence_char_index] = 0;
				// mark this location as the end of NMEA sentence
				buffer_start_index = i;
				nmea_pointer_g++;
				sentence_char_index = 0;
			}
		}
	}
	
		
	return nmea_pointer_g;
}

uint8_t GPS_Sensor_GetData(char * buf)
{
  if(nmea_pointer_g)
  {
		nmea_pointer_g--;
		for(int i=0; i < 96; i++)
		{
			buf[i] = nmea_sentences_g[nmea_pointer_g][i];
			if(buf[i] == 0) break;
		}
  }
	else
  {
		buf[0] = 0;
	}
	
	return 0;
}
