#include "gps_sensor.h"
//#include "stdlib.h"							// strtof function
#include "../services_utilities/common.h"


// ------ Private constants --------------------------------------------------

// NMEA buffer 2d array size
#define NMEA_SENTENCE_BUFFER_SIZE		20
#define NMEA_SENTENCE_MAX_CHAR			96

#define MAX_ROOT_CHILDS		6

#define ERROR_CODE_FIELD_LENGTH	190
#define ERROR_CODE_BAD_NODE_RV	191

// ------ Private variables --------------------------------------------------
static char Rx_buffer_g[GPS_RX_BUFFER_SIZE];
static DMA_Stream_TypeDef * dma_streamx_g;
static gps_sensor_t gps_sensor_data = {.DOP[0] = 0, .DOP[1] = 0, .DOP[2] = 0};
// NMEA sentences buffer
static char nmea_sentences_g[NMEA_SENTENCE_BUFFER_SIZE][NMEA_SENTENCE_MAX_CHAR] = {0};
static uint8_t nmea_pointer_g = 0;


// ------ Private Structs --------------------------------------------------

//************************************************************************
typedef struct _node_t{
	char* ID_str;
	uint8_t feilds_num;               // number of fields in NMEA format
	void* pData;                      // arbitrary data member (each node may/can use it differently)
	char* pStr;                       // NMEA string to proccess
	uint8_t (*fun_ptr)(void*);				// the node proccessing function pointer
	struct _node_t* child1;           // pointer to next node
} node_t;

typedef struct {
	char* pStr;
	node_t* childs[MAX_ROOT_CHILDS];
} root_node_t;

// ---------------------------------------------
// GPGGA nodes
node_t tr_altitude = {.feilds_num = 1, .pData = &gps_sensor_data.p_gps_lla[2], .child1 = 0};
node_t tr_satnum = {.feilds_num = 2, .pData = &gps_sensor_data.num_sat, .child1 = &tr_altitude};
node_t tr_fix = {.feilds_num = 1, .pData = &gps_sensor_data.d3fix, .child1 = &tr_satnum};
node_t tr_longitude = {.feilds_num = 2, .pData = &gps_sensor_data.p_gps_lla[1], .child1 = &tr_fix};
node_t tr_latitude = {.feilds_num = 2, .pData = &gps_sensor_data.p_gps_lla[0], .child1 = &tr_longitude};
node_t tr_time = {.feilds_num = 1, .pData = &gps_sensor_data.utc_time, .child1 = &tr_latitude};
node_t tr_gpgga = {.ID_str="GPGGA", .feilds_num = 1, .pData = 0, .child1 = &tr_time};
// GPGSA nodes
node_t tr_vdop = {.feilds_num = 1, .pData = (void*)(&gps_sensor_data.DOP[2]), .child1 = 0};
node_t tr_hdop = {.feilds_num = 1, .pData = (void*)(&gps_sensor_data.DOP[1]), .child1 = &tr_vdop};
node_t tr_pdop = {.feilds_num = 1, .pData = (void*)(&gps_sensor_data.DOP[0]), .child1 = &tr_hdop};
node_t tr_gpgsa = {.ID_str="GPGSA", .feilds_num = 15, .pData = 0, .child1 = &tr_pdop};
// ROOT $
root_node_t tr_root = {.childs[0] = &tr_gpgsa, .childs[1] = &tr_gpgga};
// ---------------------------------------------


// ---------------------------------------------
uint8_t xnode_proccess(node_t*);

// Root node function
uint8_t root_f(void* arg){
	// compare the string to ID "$". if not equal, return
	if(tr_root.pStr[0] != '$') return 1;
	
	// loop over childs and execute them
	uint8_t rv = 0;
	for(uint8_t i = 0; i < MAX_ROOT_CHILDS; i++)
	{
		// check if a child exists at this location
		if(tr_root.childs[i] != 0)
		{
			tr_root.childs[i]->pStr = tr_root.pStr + 1;
			rv += xnode_proccess(tr_root.childs[i]) - 2;
		}
	}
	
	return rv + 2;
}

uint8_t satnum_f(void* self_node)
{
	node_t* this_node = (node_t*)self_node;
	
	uint8_t value = 0;
	
	value = (this_node->pStr[0] - '0') * 10
				+ (this_node->pStr[1] - '0') ;
	
	*(uint8_t*)(this_node)->pData = value;
	
	return 0;
}

uint8_t fix_f(void* self_node)
{
	node_t* this_node = (node_t*)self_node;
	
	*(uint8_t*)(this_node)->pData = (this_node->pStr[0] - '0');
	
	return 0;
}

uint8_t lla_f(void* self_node){
	double lla_value;
	node_t* this_node = (node_t*)self_node;
	
	uint8_t str_len = 0;
	
	// extract float from pStr
	lla_value = str_to_float_nl(this_node->pStr, &str_len);
	
	// check for direction N/S
	if(this_node->pStr[str_len+2] == 'S' || this_node->pStr[str_len+2] == 'W') lla_value = -lla_value;
	
	// store it
	*(double*)(this_node)->pData = lla_value;
	
	return 0;
	
}

uint8_t time_f(void* self_node)
{
	float time;
	node_t* this_node = (node_t*)self_node;
	
	// extract float from pStr
	time = str_to_float(this_node->pStr, 9 );

	// store it
	*(float*)(this_node)->pData = time;
	
	return 0;
}

// pdop, hdop, vdop nodes
uint8_t xdop_f(void* self_node)
{
	float xdop_value = 1.5;
	
	node_t* this_node = (node_t*)self_node;
	
	// extract float vdop from pStr
	xdop_value = str_to_float(this_node->pStr, 4);
	
	// store it
	*(float*)(this_node)->pData = xdop_value;
	
	return 0;
}


uint8_t xnode_proccess(node_t* this_node)
{
	// check on node ID if exists
	if(this_node->ID_str != 0)
	{
		for(uint8_t i = 0; i < 5; i++)
		{
			if(this_node->pStr[i] != this_node->ID_str[i]) return 2;
		}
	}
	
	// execute the node proccessing function if required
	if(this_node->fun_ptr != 0)
	{
		uint8_t node_rv = (this_node->fun_ptr)(this_node);
		if(node_rv) return ERROR_CODE_BAD_NODE_RV;
	}
	
	// if a child node doesn't exist, return 0
	if(this_node->child1 == 0) return 0;
	
	// passs the string to the child node after skipping
	// the string part related to the current node
	uint8_t comma_count = 0;
	for(uint8_t i=0; i<NMEA_SENTENCE_MAX_CHAR; i++)
	{
		if(',' == this_node->pStr[i]) comma_count++;
		if(comma_count == this_node->feilds_num)
		{
			this_node->child1->pStr = this_node->pStr + i + 1;
			// execute the child node
			return xnode_proccess(this_node->child1);
		}
	}
	
	// we shouldn't be here
	// if we are here, there is a problem in node feild lentgh
	return ERROR_CODE_FIELD_LENGTH;
}


// ---------------------------------------------


void update_node_func(void)
{
	tr_pdop.fun_ptr = xdop_f;
	tr_hdop.fun_ptr = xdop_f;
	tr_vdop.fun_ptr = xdop_f;
	tr_time.fun_ptr = time_f;
	tr_altitude.fun_ptr = lla_f;
	tr_latitude.fun_ptr = lla_f;
	tr_longitude.fun_ptr = lla_f;
	tr_fix.fun_ptr = fix_f;
	tr_satnum.fun_ptr = satnum_f;
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