#include "ADCS_comm_task.h"


/***************** DMA for UART **************************/
// ------ Private variables -----------------------------
#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))	
#define ADCS_U6_SettingDelay 60
extern uint8_t ADCS_U6_Sent_Flag;
static uint8_t	ADCS_U6_Ready=1;
uint8_t ADCS_U6_SET_Counter=0;
uint8_t ADCS_U6_DMA_flag = 0;
struct ADCS_U6_FLAGS_t ADCS_U6_FLAGS;


static char ADCS_U6_Tx_buffer_g [ADCS_U6_TX_BUFFER_SIZE_BYTES];
static char ADCS_U6_Tx_buffer_ig[ADCS_U6_TX_BUFFER_SIZE_BYTES];  // Inverted copy

static char* ADCS_U6_Tx_buffer2_g = (char*)0x2001C000;
static char  ADCS_U6_Tx_buffer2_ig[ADCS_U6_TX_BUFFER_SIZE_BYTES];  // Inverted copy

static char ADCS_U6_Rx_buffer_g[ADCS_U6_RX_BUFFER_SIZE_BYTES];
static char ADCS_U6_Rx_buffer_ig[ADCS_U6_RX_BUFFER_SIZE_BYTES];  // Inverted copy
 
// Index of data in the buffer that has not yet been sent (with inv. copy)
static uint32_t ADCS_U6_Wait_idx_g  = 0;  
static uint32_t ADCS_U6_Wait_idx_ig = ~0; 

// buffer select to switch between the two buffers
static uint8_t ADCS_U6_Tx_buffer_select_g = 1;

// ------ Private function prototypes ----------------------------------------
void ADCS_U6_BUF_O_Check_Data_Integrity(void);

// ---------------------------------------------------------------------------

//Init
void ADCS_U6_init(uint32_t BAUD_RATE)
{
	ADCS_U6_FLAGS.ID_Flag=1;
	ADCS_U6_BUF_O_Init(BAUD_RATE);
}
/*----------------------------------------------------------------------------*-
  ADCS_U6_BUF_O_Init()
  Set up ADCS_U6.
  PARAMETER:
     Required baud rate.
  LONG-TERM DATA:
     Tx_buffer_g[] (W)
     Tx_buffer_ig[] (W)  
     Timeout_us_g (W)
     Timeout_us_ig (W)
  MCU HARDWARE:
     UART2.
  PRE-CONDITION CHECKS:
     None.
  POST-CONDITION CHECKS:
     None.
  ERROR DETECTION / ERROR HANDLING:
     None.
  RETURN VALUE:
     None.
-*----------------------------------------------------------------------------*/
void ADCS_U6_BUF_O_Init(uint32_t BAUD_RATE)
{
	// Set up timeout mechanism
  TIMEOUT_T3_USEC_Init();
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef	 hdma_usart6_rx;
	DMA_InitTypeDef	 hdma_usart6_tx;
	// USART6 clock enable 
	RCC_APB2PeriphClockCmd(ADCS_U6_UART_RCC, ENABLE);

	// GPIOC clock enable 
	RCC_AHB1PeriphClockCmd(ADCS_U6_PORT_RCC, ENABLE);
	 
	/* DMA controller clock enable */
	RCC_AHB1PeriphClockCmd(ADCS_U6_DMA_RCC, ENABLE);
	// GPIO config
	GPIO_PinAFConfig(ADCS_U6_PORT, ADCS_U6_RX_PIN_SOURCE, ADCS_U6_AF);	
	GPIO_PinAFConfig(ADCS_U6_PORT, ADCS_U6_TX_PIN_SOURCE, ADCS_U6_AF);
	GPIO_InitStructure.GPIO_Pin   = ADCS_U6_TX_PIN|ADCS_U6_RX_PIN; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(ADCS_U6_PORT, &GPIO_InitStructure); 
  // Set pin init 


	// USART3 configuration
	// - BaudRate as specified in function parameter
	// - Word Length = 8 Bits
	// - One Stop Bit
	// - No parity
	// - Hardware flow control disabled (RTS and CTS signals)
	// - Tx & Rx enabled

	USART_InitStructure.USART_BaudRate = BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(ADCS_U6_UARTX, &USART_InitStructure);
	
	//DMA RX init
	DMA_StructInit(&hdma_usart6_rx);
	hdma_usart6_rx.DMA_Channel = ADCS_U6_DMA_Channel;
	hdma_usart6_rx.DMA_PeripheralBaseAddr = (uint32_t) (&(ADCS_U6_UARTX->DR));; 
	hdma_usart6_rx.DMA_Memory0BaseAddr = (uint32_t)ADCS_U6_Rx_buffer_g;  
	hdma_usart6_rx.DMA_DIR = DMA_DIR_PeripheralToMemory;
	hdma_usart6_rx.DMA_BufferSize = ADCS_U6_RX_BUFFER_SIZE_BYTES;
	hdma_usart6_rx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	hdma_usart6_rx.DMA_MemoryInc = DMA_MemoryInc_Enable;
	hdma_usart6_rx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	hdma_usart6_rx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	hdma_usart6_rx.DMA_Mode = DMA_Mode_Circular;
	hdma_usart6_rx.DMA_Priority = DMA_Priority_VeryHigh;
	hdma_usart6_rx.DMA_FIFOMode = DMA_FIFOMode_Disable;
	hdma_usart6_rx.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	hdma_usart6_rx.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	hdma_usart6_rx.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(ADCS_U6_DMA_RX_STREAM, &hdma_usart6_rx);
	
	//DMA TX init 
//	DMA_StructInit(&hdma_usart6_tx); 
//	hdma_usart6_tx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	hdma_usart6_tx.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	hdma_usart6_tx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	hdma_usart6_tx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	hdma_usart6_tx.DMA_Priority = DMA_Priority_VeryHigh;
//	hdma_usart6_tx.DMA_FIFOMode = DMA_FIFOMode_Disable;
//	hdma_usart6_tx.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;//DMA_FIFOThreshold_1QuarterFull
//	hdma_usart6_tx.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	hdma_usart6_tx.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

//	hdma_usart6_tx.DMA_Channel = ADCS_U6_DMA_Channel;
//	hdma_usart6_tx.DMA_PeripheralBaseAddr = (uint32_t) (&(ADCS_U6_UARTX->DR)); 
//	hdma_usart6_tx.DMA_Memory0BaseAddr = (uint32_t)ADCS_U6_Rx_buffer_g;
//	hdma_usart6_tx.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//	hdma_usart6_tx.DMA_BufferSize = ADCS_U6_TX_BUFFER_SIZE_BYTES+1;
//	hdma_usart6_tx.DMA_Mode = DMA_Mode_Normal;
//	DMA_Init(ADCS_U6_DMA_TX_STREAM, &hdma_usart6_tx);

   // Enable UART6
   USART_Cmd(ADCS_U6_UARTX, ENABLE);
	 
	 
//   Enable USART DMA TX Requsts 
//	 USART_DMACmd(ADCS_U6_UARTX, USART_DMAReq_Tx, ENABLE);
//	 while(!DMA_GetCmdStatus(ADCS_U6_DMA_TX_STREAM))
//	 {
//	 };
	 
	 // Enable USART DMA RX Requsts
   DMA_Cmd (ADCS_U6_DMA_RX_STREAM, ENABLE);
	 USART_DMACmd(ADCS_U6_UARTX, USART_DMAReq_Rx, ENABLE);
	 while(!DMA_GetCmdStatus(ADCS_U6_DMA_RX_STREAM))
	 {
	 };

	// Configure buffer
	for (uint32_t i = 0; i < ADCS_U6_TX_BUFFER_SIZE_BYTES; i++)
		{
		ADCS_U6_Tx_buffer_g[i] = 'X';
		ADCS_U6_Tx_buffer2_g[i] = 'G';
			
		ADCS_U6_Tx_buffer_ig[i] = (char) ~'X'; 
		ADCS_U6_Tx_buffer2_ig[i] =(char) ~'G';
		}
//	for (uint32_t i = 0; i < ADCS_U6_RX_BUFFER_SIZE_BYTES; i++)
//		{
//		ADCS_U6_Rx_buffer_g[i] = 'K';
//			
//		ADCS_U6_Rx_buffer_ig[i] = (char) ~'K'; 
//		}
	// Store the UART register configuration
	REG_CONFIG_CHECKS_UART_Store(USART6);
		
}
// update
uint32_t ADCS_U6_update(void)
{
	import_pkt(ADCS_APP_ID, &obc_data.adcs_uart);
	export_pkt(ADCS_APP_ID, &obc_data.adcs_uart);

	return RETURN_NORMAL_STATE;
}	
/*----------------------------------------------------------------------------*-
  ADCS_U6_BUF_O_Update()
  Sends next character from the software transmit buffer
  NOTE: Output-only library (Cannot receive chars)
  Uses on-chip UART hardware.
  PARAMETERS:
     None.
     Wait_idx_g  (W)  
     Wait_idx_ig (W) 
  MCU HARDWARE:
     UART6.
  PRE-CONDITION CHECKS:
     ADCS_U6_BUF_O_Check_Data_Integrity() is called.
  POST-CONDITION CHECKS:
     None.
  ERROR DETECTION / ERROR HANDLING:
     None.
  WCET:
     Not yet determined.
  BCET:
     Not yet determined.
  RETURN VALUE:
     None.
-*----------------------------------------------------------------------------*/
uint32_t ADCS_U6_BUF_O_Update(void)
{
	if(ADCS_U6_Wait_idx_g > 0)
	{  
		ADCS_U6_BUF_O_Send_All_Data();
	} 
	return RETURN_NORMAL_STATE;
}
// Setters

/*----------------------------------------------------------------------------*-
  ADCS_U6_BUF_O_Check_Data_Integrity()
  Check data integrity.
  PARAMETER:
     None.
  LONG-TERM DATA:
     ADCS_U6_Tx_buffer_g[]  (R)
     ADCS_U6_Tx_buffer_ig[] (R)   
     ADCS_U6_Wait_idx_g     (R)  
     ADCS_U6_Wait_idx_ig    (R) 
  MCU HARDWARE:
     ADCS_U6.
  PRE-CONDITION CHECKS:
     None.
  POST-CONDITION CHECKS:
     None.
  ERROR DETECTION / ERROR HANDLING:
     Data integrity and register checks.
     Forces shutdown in the event of failure.
  RETURN VALUE:
     None.
-*----------------------------------------------------------------------------*/
void ADCS_U6_BUF_O_Check_Data_Integrity(void)
{
	// Check the UART register configuration
	REG_CONFIG_CHECKS_UART_Check(USART6);  

	// Check integrity of 'waiting' index
	if ((ADCS_U6_Wait_idx_g != (~ADCS_U6_Wait_idx_ig)))
	{
		// Data have been corrupted: Treat here as Fatal Platform Failure
		PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
	}          

	// Check integrity of data in Tx buffer
	for (uint32_t i = 0; i < ADCS_U6_TX_BUFFER_SIZE_BYTES; i++)  
	{
		if ((ADCS_U6_Tx_buffer_g[i] != ((char) ~ADCS_U6_Tx_buffer_ig[i])))
		{
			 // Data have been corrupted: Treat here as Fatal Platform Failure
			 PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
		 }            
	}
}
/**----------------------------------------------------------------------------*-
 * \brief           Check for new data received with DMA
 *
 * Not doing reads fast enough may cause DMA to overflow unread received bytes,
 * hence application will lost useful data.
 *
 * Solutions to this are:
 * - Improve architecture design to achieve faster reads
 * - Increase raw buffer size and allow DMA to write more data before this function is called
 */
void ADCS_U6_DMA_CHECK(struct uart_data *uart_data)
{
	static uint32_t old_pos=0;
	uint32_t pos=0;
	pos = ARRAY_LEN(ADCS_U6_Rx_buffer_g) - DMA_GetCurrDataCounter(ADCS_U6_DMA_RX_STREAM);
	if (pos>=ADCS_U6_RX_BUFFER_SIZE_BYTES)//ADCS_U6_RX_BUFFER_SIZE_BYTES
		pos=0;
	if (pos != old_pos) /* Check change in received data */
	{
		if (pos > old_pos) 
		{   
		/* Current position is over previous one */
			ADCS_U6_PROCESS_DATA(&ADCS_U6_Rx_buffer_g[old_pos], pos - old_pos);		
		} 
		else 
		{
			ADCS_U6_PROCESS_DATA(&ADCS_U6_Rx_buffer_g[old_pos], ARRAY_LEN(ADCS_U6_Rx_buffer_g) - old_pos);
			if (pos > 0) 
			{
				ADCS_U6_PROCESS_DATA(&ADCS_U6_Rx_buffer_g[0], pos);
			}
		}
		old_pos = pos;                          /* Save current position as old for next transfers */
	}  
}

/**
 * \brief           Process received data over UART
 * \note            Either process them directly or copy to other bigger buffer
 * \param[in]       data: Data to process input data
 * \param[in]       len: Length in units of bytes
 * \param[in]       uart_data: output data
 * \param[in]       old_pos: last position 
 * \param[in]       pos: the position of the last byte
 */
/**
 * \brief           Process received data over UART
 * \note            Either process them directly or copy to other bigger buffer
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
void ADCS_U6_PROCESS_DATA(void* data, size_t len)
{
	uint8_t* d = data;
	for ( ;len > 0;len--, ++d)
	{
		if(ADCS_U6_FLAGS.SetParam_Flag)
		{
			UART2_BUF_O_Write_Char_To_Buffer(*d);
		}
		if(*d == 0x7E)
		{
			if(ADCS_U6_FLAGS.Error_Flag)
			{
				ADCS_U6_FLAGS.Error_Flag=0;
				UART2_BUF_O_Write_String_To_Buffer("ADCS: msgERRR...\n");
				UART2_BUF_O_Send_All_Data();				
				continue;
			}
			if(ADCS_U6_FLAGS.ID_Flag == 1)
			{
				UART2_BUF_O_Send_All_Data();
				if((ADCS_U6_FLAGS.End1_Flag == 0) && (ADCS_U6_FLAGS.Start1_Flag ==0))
				{
					ADCS_U6_FLAGS.Start1_Flag=1;	// rises the flag that a packet has started
					ADCS_U6_FLAGS.StartPtr1=d;		// take that pointer of the start of the packet
				}
				else if((ADCS_U6_FLAGS.End1_Flag == 0) && (ADCS_U6_FLAGS.Start1_Flag ==1))
				{
					ADCS_U6_FLAGS.End1_Flag=1;	// rises the flag that a packet has ended
					ADCS_U6_FLAGS.EndPtr1=d;		// take that pointer of the end of the packet
					ADCS_U6_FLAGS.ID_Flag = 2;
					ADCS_U6_DMA_flag =1;
				}
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("ADCS_U6:Frame Error 1\n");
					ADCS_U6_FLAGS.Error_Flag=1;
					
					ADCS_U6_FLAGS.End1_Flag=0;
					ADCS_U6_FLAGS.Start1_Flag=0;
				}
			}
			else if(ADCS_U6_FLAGS.ID_Flag == 2)
			{
				if((ADCS_U6_FLAGS.End2_Flag == 0) && (ADCS_U6_FLAGS.Start2_Flag ==0))
				{
					ADCS_U6_FLAGS.Start2_Flag=1;	// rises the flag that a frame has started
					ADCS_U6_FLAGS.StartPtr2=d;		// take that pointer of the start of the frame
					ADCS_U6_FLAGS.Frame_Flag=1;
				}
				else if((ADCS_U6_FLAGS.End2_Flag == 0) && (ADCS_U6_FLAGS.Start2_Flag ==1))
				{
					ADCS_U6_FLAGS.End2_Flag=1;	// rises the flag that a frame has ended
					ADCS_U6_FLAGS.EndPtr2=d;		// take that pointer of the end of the frame
					ADCS_U6_FLAGS.ID_Flag = 3;
					ADCS_U6_DMA_flag =1;
				}
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("ADCS_U6:Frame Error 2\n");
					ADCS_U6_FLAGS.Error_Flag=1;

					ADCS_U6_FLAGS.End2_Flag=0;
					ADCS_U6_FLAGS.Start2_Flag=0;
				}
			}
			else if(ADCS_U6_FLAGS.ID_Flag == 3)
			{
				if((ADCS_U6_FLAGS.End3_Flag == 0) && (ADCS_U6_FLAGS.Start3_Flag ==0))
				{
					ADCS_U6_FLAGS.Start3_Flag=1;	// rises the flag that a frame has started
					ADCS_U6_FLAGS.StartPtr3=d;		// take that pointer of the start of the frame
					ADCS_U6_FLAGS.Frame_Flag=1;
				}
				else if((ADCS_U6_FLAGS.End3_Flag == 0) && (ADCS_U6_FLAGS.Start3_Flag ==1))
				{
					ADCS_U6_FLAGS.End3_Flag=1;	// rises the flag that a frame has ended
					ADCS_U6_FLAGS.EndPtr3=d;		// take that pointer of the end of the frame
					ADCS_U6_FLAGS.ID_Flag = 1;
					ADCS_U6_DMA_flag =1;
				}
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("ADCS_U6:Frame Error 3\n");
					ADCS_U6_FLAGS.Error_Flag=1;

					ADCS_U6_FLAGS.End3_Flag=0;
					ADCS_U6_FLAGS.Start3_Flag=0;
				}
			}
		}
	}
}

SAT_returnState ADCS_U6_UART_recv(struct uart_data *uart_data) 
{
	if(ADCS_U6_DMA_flag)
	{
		uint8_t* data;
		int32_t length;
		if(ADCS_U6_FLAGS.ID_Flag == 2)// frame recvd from 1
		{
			length = ADCS_U6_FLAGS.EndPtr1 - ADCS_U6_FLAGS.StartPtr1+1;
			data = ADCS_U6_FLAGS.StartPtr1;
						
			// clearing the frame parameters
			ADCS_U6_FLAGS.End1_Flag=0;
			ADCS_U6_FLAGS.EndPtr1=0;
			ADCS_U6_FLAGS.Length1=0;
			ADCS_U6_FLAGS.Start1_Flag=0;
			ADCS_U6_FLAGS.StartPtr1=0;
		}
		else if(ADCS_U6_FLAGS.ID_Flag == 3)// frame recvd from 2
		{
			length = ADCS_U6_FLAGS.EndPtr2 - ADCS_U6_FLAGS.StartPtr2+1;
			data = ADCS_U6_FLAGS.StartPtr2;

			// clearing the frame parameters
			ADCS_U6_FLAGS.Start2_Flag=0;
			ADCS_U6_FLAGS.End2_Flag=0;
			ADCS_U6_FLAGS.StartPtr2=0;
			ADCS_U6_FLAGS.EndPtr2=0;
			ADCS_U6_FLAGS.Length2=0;
		}
		else if (ADCS_U6_FLAGS.ID_Flag == 1)// frame recvd from 3
		{
			length = ADCS_U6_FLAGS.EndPtr3 - ADCS_U6_FLAGS.StartPtr3+1;
			data = ADCS_U6_FLAGS.StartPtr3;
			
			// clearing the frame parameters
			ADCS_U6_FLAGS.Start3_Flag=0;
			ADCS_U6_FLAGS.End3_Flag=0;
			ADCS_U6_FLAGS.StartPtr3=0;
			ADCS_U6_FLAGS.EndPtr3=0;
			ADCS_U6_FLAGS.Length3=0;
		}
		UART2_BUF_O_Write_String_To_Buffer("\n--> Size:");
		UART2_BUF_O_Write_Number04_To_Buffer(length);
		UART2_BUF_O_Write_String_To_Buffer("\n");
		if(length > TC_MAX_PKT_SIZE) // here need to make a request for the corrupted packet
		{
			UART2_BUF_O_Write_String_To_Buffer("\nSize Error:");
			UART2_BUF_O_Write_String_To_Buffer("\n--> Size:");
			UART2_BUF_O_Write_Number10_To_Buffer(length);
			UART2_BUF_O_Write_String_To_Buffer("\n");
			length=TC_MAX_PKT_SIZE;
		}
		
		UART2_BUF_O_Write_String_To_Buffer("ADCS_U6: packet recieved\n");
		uart_data->uart_size = length;
		memcpy(uart_data->uart_unpkt_buf, data, length);
		ADCS_U6_DMA_flag = 0;
		return SATR_EOT;
	}
	return SATR_OK;
}
void ADCS_U6_UART_send( uint8_t *buf, uint16_t size) 
{
	const uint8_t* d = buf;
	for ( ;size > 0;size--, ++d)
	{
		ADCS_U6_BUF_O_Write_Char_To_Buffer(*d);		
	}
	UART2_BUF_O_Write_String_To_Buffer("ADCS msg sent\n");
}
/*----------------------------------------------------------------------------*-
   
  ADCS_U6_BUF_O_Write_String_To_Buffer()
  Copies a (null terminated) string to the module Tx buffer.  
  PARAMETERS:
     None.
  LONG-TERM DATA:
     None.
  MCU HARDWARE:
     None.
  PRE-CONDITION CHECKS:
     None.
  POST-CONDITION CHECKS:
     None.
  ERROR DETECTION / ERROR HANDLING:
     None.
  RETURN VALUE:
     None.
-*----------------------------------------------------------------------------*/
void ADCS_U6_BUF_O_Write_String_To_Buffer(const char* const STR_PTR)
{
 uint32_t i = 0;
 while ((STR_PTR[i] != '\0')&&(STR_PTR[i] != '*'))
	{
		ADCS_U6_BUF_O_Write_Char_To_Buffer(STR_PTR[i]);
		i++;
	}
}
void ADCS_U6_BUF_O_Write_Frame_To_Buffer(const void* data, size_t len)
{
	 const uint8_t* d = data;
   for ( ;len > 0;len--, ++d)
	{
		if(*d==0x7E)
			continue;
		ADCS_U6_BUF_O_Write_Char_To_Buffer(*d);
	}
}
/*----------------------------------------------------------------------------*-
   
  ADCS_U6_BUF_O_Write_Char_To_Buffer()
  Stores a single character in the Tx buffer.
  PARAMETERS:
     CHARACTER is character to be stored.
  LONG-TERM DATA:
     Tx_buffer_g[] (W)
     Tx_buffer_ig[] (W)  
     Wait_idx_g  (W)  
     Wait_idx_ig (W) 
  MCU HARDWARE:
     None.
  PRE-CONDITION CHECKS:
     None.
  POST-CONDITION CHECKS:
     None.
  ERROR DETECTION / ERROR HANDLING:
     None.
  RETURN VALUE:
     None.
-*----------------------------------------------------------------------------*/
void ADCS_U6_BUF_O_Write_Char_To_Buffer(const char CHARACTER)
{
	// Write to the buffer *only* if there is space
	if (ADCS_U6_Wait_idx_g < ADCS_U6_TX_BUFFER_SIZE_BYTES)
		{
			if( ADCS_U6_Tx_buffer_select_g == 1 )
			{
				ADCS_U6_Tx_buffer_g[ADCS_U6_Wait_idx_g] = CHARACTER;
				ADCS_U6_Tx_buffer_ig[ADCS_U6_Wait_idx_g] = (char) ~CHARACTER;
			}
			else
			{
				ADCS_U6_Tx_buffer2_g[ADCS_U6_Wait_idx_g] = CHARACTER;
				ADCS_U6_Tx_buffer2_ig[ADCS_U6_Wait_idx_g] = (char) ~CHARACTER;
			}
			ADCS_U6_Wait_idx_g++;     
		}
	else
		{
		// Write buffer is full
		// No error handling / reporting here (characters may be lost)
		// Adapt as required to meet the needs of your application
			UART2_BUF_O_Write_String_To_Buffer("Error inside ADCS_U6 Write buffer is full\n");
		}

	// Update the copy
	ADCS_U6_Wait_idx_ig = ~ADCS_U6_Wait_idx_g;
}
/*----------------------------------------------------------------------------*-
  ADCS_U6_BUF_O_Send_All_Data()
  Sends all data from the software transmit buffer.
  NOTES: 
  * May have very long execution time!  
  * Intended for use when the scheduler is NOT running.
  PARAMETERS:
     None.
  LONG-TERM DATA:
     Wait_idx_g  (W)  
     Wait_idx_ig (W) 
  MCU HARDWARE:
     UART2.
  PRE-CONDITION CHECKS:
     ADCS_U6_BUF_O_Check_Data_Integrity() is called.
  POST-CONDITION CHECKS:
     None.
  ERROR DETECTION / ERROR HANDLING:
     None.
  RETURN VALUE:
     None.
-*----------------------------------------------------------------------------*/
void ADCS_U6_BUF_O_Send_All_Data(void)
{
	uint32_t T3;
	// wait until the previous transfer is done
	if(ADCS_U6_DMA_TX_STREAM->NDTR == 0 || ADCS_U6_DMA_TX_STREAM->NDTR == ADCS_U6_TX_BUFFER_SIZE_BYTES+1)
	{
		// Check data integrity
		ADCS_U6_BUF_O_Check_Data_Integrity();
		// set the dma transfer data length
		ADCS_U6_DMA_TX_STREAM->NDTR = ADCS_U6_Wait_idx_g;
		// set memory base address to one of two buffers
		if( ADCS_U6_Tx_buffer_select_g == 1 )
		{
			ADCS_U6_DMA_TX_STREAM->M0AR = (uint32_t)ADCS_U6_Tx_buffer_g ;
			ADCS_U6_Tx_buffer_select_g = 2;
		}
		else
		{
			ADCS_U6_DMA_TX_STREAM->M0AR = (uint32_t)ADCS_U6_Tx_buffer2_g ;
			ADCS_U6_Tx_buffer_select_g = 1;
		}
		
		// Clear DMA Stream Flags// needs to be changed for every Stream
		DMA2->HIFCR |= (0x3D << 22);
		
		// Clear USART Transfer Complete Flags
		USART_ClearFlag(ADCS_U6_UARTX,USART_FLAG_TC);  
		
		// Enable DMA USART TX Stream 
		DMA_Cmd(ADCS_U6_DMA_TX_STREAM,ENABLE);
		
		// wait until it's enabled
		// while( DMA_GetCmdStatus(DMA1_Stream6) == DISABLE );
		
		// Reset the indices
		ADCS_U6_Wait_idx_g = 0;
		ADCS_U6_Wait_idx_ig = ~0;
	}
}

