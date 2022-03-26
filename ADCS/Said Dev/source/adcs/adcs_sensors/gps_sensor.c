#include "gps_sensor.h"


// ------ Private constants --------------------------------------------------
#define NMEA_SENTENCE_MAX_CHAR	96

#define ERROR_CODE_FIELD_LENGTH	190

// ------ Private variables --------------------------------------------------
static char Rx_buffer_g[GPS_RX_BUFFER_SIZE];
static DMA_Stream_TypeDef * dma_streamx_g;
static gps_sensor_t gps_sensor_data = {.DOP[0] = 0, .DOP[1] = 0, .DOP[2] = 0};

// space for NMEA sentences
static char nmea_sentences_g[10][NMEA_SENTENCE_MAX_CHAR] = {0};
static uint8_t nmea_pointer_g = 0;

// ------ Private Structs --------------------------------------------------

//************************************************************************
typedef struct _tree_node_t{
	uint8_t feilds_num;
	void* pData;
	char* pStr;
	uint8_t (*fun_ptr)(void*);
	struct _tree_node_t* child1;
	struct _tree_node_t* child2;
} tree_node_t;

// initializing parsing tree
tree_node_t tr_vdop = {.feilds_num = 1, .pData = (void*)(&gps_sensor_data.DOP[2]), .child1 = 0, .child2 = 0};
tree_node_t tr_hdop = {.feilds_num = 1, .pData = (void*)(&gps_sensor_data.DOP[1]), .child1 = &tr_vdop, .child2 = 0};
tree_node_t tr_pdop = {.feilds_num = 1, .pData = (void*)(&gps_sensor_data.DOP[0]), .child1 = &tr_hdop, .child2 = 0};
tree_node_t tr_gpgsa = {.feilds_num = 15, .pData = 0, .child1 = &tr_pdop, .child2 = 0};
tree_node_t tr_root = {.feilds_num = 1, .pData = 0, .child1 = &tr_gpgsa, .child2 = 0};

uint8_t xdop_f(void* self_node)
{
	float xdop_value = 1.5;
	
	tree_node_t* this_node = (tree_node_t*)self_node;
	
	// extract float vdop from pStr
	xdop_value = (this_node->pStr[0] - '0') 
				      + (this_node->pStr[2] - '0') / 10.0
							+ (this_node->pStr[3] - '0') / 100.0;	
	
	// store it
	*(float*)(this_node)->pData = xdop_value;
	
	// if a child node exists, run its function
	// otherwise, return 0
	if (this_node->child1 == 0)	return 0;
	
	// set pStr of next node
	this_node->child1->pStr = this_node->pStr + 5;
	
	return (this_node->child1->fun_ptr)( this_node->child1 );
}

uint8_t gpgsa_f(void* arg){
	// compare the string to ID "GPGSA"
	char *s = "GPGSA";
	for(uint8_t i = 0; i < 5; i++)
	{
		if(tr_gpgsa.pStr[i] != s[i]) return 2;
	}
	
	uint8_t comma_count = 0;
	for(uint8_t i=5; i<NMEA_SENTENCE_MAX_CHAR; i++)
	{
		if(',' == tr_gpgsa.pStr[i]) comma_count++;
		if(comma_count == tr_gpgsa.feilds_num)
		{
			tr_pdop.pStr = tr_gpgsa.pStr + i + 1;
			return (tr_pdop.fun_ptr)(&tr_pdop);
		}
	}
	
	return ERROR_CODE_FIELD_LENGTH;
	
}

uint8_t root_f(void* arg){
	// compare the string to ID "$". if not equal, return 0
	if(tr_root.pStr[0] != '$') return 1;
	
	tr_gpgsa.pStr = tr_root.pStr+1;
	
	// if equal, run the function of next node
	return (tr_gpgsa.fun_ptr)(0);
}

void update_node_func(void)
{
	tr_vdop.fun_ptr = xdop_f;
	tr_hdop.fun_ptr = xdop_f;
	tr_pdop.fun_ptr = xdop_f;
	tr_gpgsa.fun_ptr = gpgsa_f;
	tr_root.fun_ptr = root_f;
}

uint8_t sentence_parsing(char* nmea_sen)
{
	update_node_func();
	tr_root.pStr = nmea_sen;
	return root_f(0);
}

//************************************************************************



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
	uint16_t buffer_end_index = 0;
	uint16_t sentence_char_index = 0;
	
	// get new sentences from the buffer [Start]
	
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
	
	// get new sentences from the buffer [End]
	
	// process new sentences in the buffer [Start]
	
	
	
	// process new sentences in the buffer [End]
		
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
		return sentence_parsing(nmea_sentences_g[nmea_pointer_g]);	
		//return 0;	
  }
	
	// we reach here if there is not sentences
	buf[0] = 0;
	return 0;
}

gps_sensor_t get_struct(void)
{
	return gps_sensor_data;
}