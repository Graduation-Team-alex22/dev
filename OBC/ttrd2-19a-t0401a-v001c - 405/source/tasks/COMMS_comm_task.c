#include "COMMS_comm_task.h"


/***************** DMA for UART **************************/
// ------ Private variables -----------------------------
#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))	


uint8_t COMMS_U4_DMA_flag = 0;
struct COMMS_U4_FLAGS_t COMMS_U4_FLAGS;


static char COMMS_U4_Tx_buffer_g [COMMS_U4_TX_BUFFER_SIZE_BYTES];
static char COMMS_U4_Tx_buffer_ig[COMMS_U4_TX_BUFFER_SIZE_BYTES];  // Inverted copy

static char* COMMS_U4_Tx_buffer2_g = (char*)0x2001C000;
static char  COMMS_U4_Tx_buffer2_ig[COMMS_U4_TX_BUFFER_SIZE_BYTES];  // Inverted copy

static char COMMS_U4_Rx_buffer_g[COMMS_U4_RX_BUFFER_SIZE_BYTES];
static char COMMS_U4_Rx_buffer_ig[COMMS_U4_RX_BUFFER_SIZE_BYTES];  // Inverted copy
 
// Index of data in the buffer that has not yet been sent (with inv. copy)
static uint32_t COMMS_U4_Wait_idx_g  = 0;  
static uint32_t COMMS_U4_Wait_idx_ig = ~0; 

// buffer select to switch between the two buffers
static uint8_t COMMS_U4_Tx_buffer_select_g = 1;

// ------ Private function prototypes ----------------------------------------
void COMMS_U4_BUF_O_Check_Data_Integrity(void);

// ---------------------------------------------------------------------------

//Init
void COMMS_U4_init(uint32_t BAUD_RATE)
{
	COMMS_U4_FLAGS.ID_Flag=1;
	COMMS_U4_BUF_O_Init(BAUD_RATE);
}
/*----------------------------------------------------------------------------*-
  COMMS_U4_BUF_O_Init()
  Set up COMMS_U4.
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
void COMMS_U4_BUF_O_Init(uint32_t BAUD_RATE)
{
	// Set up timeout mechanism
  TIMEOUT_T3_USEC_Init();
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef	 hdma_uart4_rx;
	DMA_InitTypeDef	 hdma_uart4_tx;
	// UART4 clock enable 
	RCC_APB1PeriphClockCmd(COMMS_U4_UART_RCC, ENABLE);

	// GPIOA clock enable 
	RCC_AHB1PeriphClockCmd(COMMS_U4_PORT_RCC, ENABLE);
	 
	/* DMA controller clock enable */
	RCC_AHB1PeriphClockCmd(COMMS_U4_DMA_RCC, ENABLE);
	// GPIO config
	GPIO_PinAFConfig(COMMS_U4_PORT, COMMS_U4_RX_PIN_SOURCE, COMMS_U4_AF);	
	GPIO_PinAFConfig(COMMS_U4_PORT, COMMS_U4_TX_PIN_SOURCE, COMMS_U4_AF);
	GPIO_InitStructure.GPIO_Pin   = COMMS_U4_TX_PIN|COMMS_U4_RX_PIN; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(COMMS_U4_PORT, &GPIO_InitStructure); 
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
	USART_Init(COMMS_U4_UARTX, &USART_InitStructure);
	
	//DMA RX init
	DMA_StructInit(&hdma_uart4_rx);
	hdma_uart4_rx.DMA_Channel = COMMS_U4_DMA_Channel;
	hdma_uart4_rx.DMA_PeripheralBaseAddr = (uint32_t) (&(COMMS_U4_UARTX->DR));; 
	hdma_uart4_rx.DMA_Memory0BaseAddr = (uint32_t)COMMS_U4_Rx_buffer_g;  //
	hdma_uart4_rx.DMA_DIR = DMA_DIR_PeripheralToMemory;
	hdma_uart4_rx.DMA_BufferSize = COMMS_U4_RX_BUFFER_SIZE_BYTES;
	hdma_uart4_rx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	hdma_uart4_rx.DMA_MemoryInc = DMA_MemoryInc_Enable;
	hdma_uart4_rx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	hdma_uart4_rx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	hdma_uart4_rx.DMA_Mode = DMA_Mode_Circular;
	hdma_uart4_rx.DMA_Priority = DMA_Priority_VeryHigh;
	hdma_uart4_rx.DMA_FIFOMode = DMA_FIFOMode_Disable;
	hdma_uart4_rx.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	hdma_uart4_rx.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	hdma_uart4_rx.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(COMMS_U4_DMA_RX_STREAM, &hdma_uart4_rx);
	
	//DMA TX init 
//	DMA_StructInit(&hdma_uart4_tx); 
//	hdma_uart4_tx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	hdma_uart4_tx.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	hdma_uart4_tx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	hdma_uart4_tx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	hdma_uart4_tx.DMA_Priority = DMA_Priority_VeryHigh;
//	hdma_uart4_tx.DMA_FIFOMode = DMA_FIFOMode_Disable;
//	hdma_uart4_tx.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;//DMA_FIFOThreshold_1QuarterFull
//	hdma_uart4_tx.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	hdma_uart4_tx.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

//	hdma_uart4_tx.DMA_Channel = COMMS_U4_DMA_Channel;
//	hdma_uart4_tx.DMA_PeripheralBaseAddr = (uint32_t) (&(COMMS_U4_UARTX->DR)); 
//	hdma_uart4_tx.DMA_Memory0BaseAddr = (uint32_t)COMMS_U4_Rx_buffer_g;
//	hdma_uart4_tx.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//	hdma_uart4_tx.DMA_BufferSize = COMMS_U4_TX_BUFFER_SIZE_BYTES+1;
//	hdma_uart4_tx.DMA_Mode = DMA_Mode_Normal;
//	DMA_Init(COMMS_U4_DMA_TX_STREAM, &hdma_uart4_tx);

   // Enable UART4
   USART_Cmd(COMMS_U4_UARTX, ENABLE);
	 
	 
//   Enable USART DMA TX Requsts 
//	 USART_DMACmd(COMMS_U4_UARTX, USART_DMAReq_Tx, ENABLE);
//	 while(!DMA_GetCmdStatus(COMMS_U4_DMA_TX_STREAM))
//	 {
//	 };
	 
	 // Enable USART DMA RX Requsts
   DMA_Cmd (COMMS_U4_DMA_RX_STREAM, ENABLE);
	 USART_DMACmd(COMMS_U4_UARTX, USART_DMAReq_Rx, ENABLE);
	 while(!DMA_GetCmdStatus(COMMS_U4_DMA_RX_STREAM))
	 {
	 };

	// Configure buffer
	for (uint32_t i = 0; i < COMMS_U4_TX_BUFFER_SIZE_BYTES; i++)
		{
		COMMS_U4_Tx_buffer_g[i] = 'X';
		COMMS_U4_Tx_buffer2_g[i] = 'G';
			
		COMMS_U4_Tx_buffer_ig[i] = (char) ~'X'; 
		COMMS_U4_Tx_buffer2_ig[i] =(char) ~'G';
		}
//	for (uint32_t i = 0; i < COMMS_U4_RX_BUFFER_SIZE_BYTES; i++)
//		{
//		COMMS_U4_Rx_buffer_g[i] = 'K';
//			
//		COMMS_U4_Rx_buffer_ig[i] = (char) ~'K'; 
//		}
	// Store the UART register configuration
	REG_CONFIG_CHECKS_UART_Store(UART4);
		
}
// update
uint32_t COMMS_U4_update(void)
{
	import_pkt(COMMS_APP_ID, &obc_data.comms_uart);
	export_pkt(COMMS_APP_ID, &obc_data.comms_uart);

	return RETURN_NORMAL_STATE;
}	
/*----------------------------------------------------------------------------*-
  COMMS_U4_BUF_O_Update()
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
     COMMS_U4_BUF_O_Check_Data_Integrity() is called.
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
uint32_t COMMS_U4_BUF_O_Update(void)
{
	if(COMMS_U4_Wait_idx_g > 0)
	{  
		COMMS_U4_BUF_O_Send_All_Data();
	} 
	return RETURN_NORMAL_STATE;
}
// Setters

/*----------------------------------------------------------------------------*-
  COMMS_U4_BUF_O_Check_Data_Integrity()
  Check data integrity.
  PARAMETER:
     None.
  LONG-TERM DATA:
     COMMS_U4_Tx_buffer_g[]  (R)
     COMMS_U4_Tx_buffer_ig[] (R)   
     COMMS_U4_Wait_idx_g     (R)  
     COMMS_U4_Wait_idx_ig    (R) 
  MCU HARDWARE:
     COMMS_U4.
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
void COMMS_U4_BUF_O_Check_Data_Integrity(void)
{
	// Check the UART register configuration
	REG_CONFIG_CHECKS_UART_Check(UART4);  

	// Check integrity of 'waiting' index
	if ((COMMS_U4_Wait_idx_g != (~COMMS_U4_Wait_idx_ig)))
	{
		// Data have been corrupted: Treat here as Fatal Platform Failure
		PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
	}          

	// Check integrity of data in Tx buffer
	for (uint32_t i = 0; i < COMMS_U4_TX_BUFFER_SIZE_BYTES; i++)  
	{
		if ((COMMS_U4_Tx_buffer_g[i] != ((char) ~COMMS_U4_Tx_buffer_ig[i])))
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
void COMMS_U4_DMA_CHECK(struct uart_data *uart_data)
{
	static uint32_t old_pos=0;
	uint32_t pos=0;
	pos = ARRAY_LEN(COMMS_U4_Rx_buffer_g) - DMA_GetCurrDataCounter(COMMS_U4_DMA_RX_STREAM);
	if (pos>=COMMS_U4_RX_BUFFER_SIZE_BYTES)//COMMS_U4_RX_BUFFER_SIZE_BYTES
		pos=0;
	if (pos != old_pos) /* Check change in received data */
	{
		if (pos > old_pos) 
		{   
		/* Current position is over previous one */
			COMMS_U4_PROCESS_DATA(&COMMS_U4_Rx_buffer_g[old_pos], pos - old_pos);		
		} 
		else 
		{
			COMMS_U4_PROCESS_DATA(&COMMS_U4_Rx_buffer_g[old_pos], ARRAY_LEN(COMMS_U4_Rx_buffer_g) - old_pos);
			if (pos > 0) 
			{
				COMMS_U4_PROCESS_DATA(&COMMS_U4_Rx_buffer_g[0], pos);
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
void COMMS_U4_PROCESS_DATA(void* data, size_t len)
{
	uint8_t* d = data;
	for ( ;len > 0;len--, ++d)
	{
		if(COMMS_U4_FLAGS.SetParam_Flag)
		{
			UART2_BUF_O_Write_Char_To_Buffer(*d);
		}
		if(*d == 0x7E)
		{
			if(COMMS_U4_FLAGS.Error_Flag)
			{
				COMMS_U4_FLAGS.Error_Flag=0;
				UART2_BUF_O_Write_String_To_Buffer("COMMS: msgERRR...\n");
				UART2_BUF_O_Send_All_Data();				
				continue;
			}
			if(COMMS_U4_FLAGS.ID_Flag == 1)
			{
				if((COMMS_U4_FLAGS.End1_Flag == 0) && (COMMS_U4_FLAGS.Start1_Flag ==0))
				{
					COMMS_U4_FLAGS.Start1_Flag=1;	// rises the flag that a packet has started
					COMMS_U4_FLAGS.StartPtr1=d;		// take that pointer of the start of the packet
				}
				else if((COMMS_U4_FLAGS.End1_Flag == 0) && (COMMS_U4_FLAGS.Start1_Flag ==1))
				{
					COMMS_U4_FLAGS.End1_Flag=1;	// rises the flag that a packet has ended
					COMMS_U4_FLAGS.EndPtr1=d;		// take that pointer of the end of the packet
					COMMS_U4_FLAGS.ID_Flag = 2;
					COMMS_U4_DMA_flag =1;
				}
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("COMMS_U4:Frame Error 1\n");
					COMMS_U4_FLAGS.Error_Flag=1;
					
					COMMS_U4_FLAGS.End1_Flag=0;
					COMMS_U4_FLAGS.Start1_Flag=0;
				}
			}
			else if(COMMS_U4_FLAGS.ID_Flag == 2)
			{
				if((COMMS_U4_FLAGS.End2_Flag == 0) && (COMMS_U4_FLAGS.Start2_Flag ==0))
				{
					COMMS_U4_FLAGS.Start2_Flag=1;	// rises the flag that a frame has started
					COMMS_U4_FLAGS.StartPtr2=d;		// take that pointer of the start of the frame
					COMMS_U4_FLAGS.Frame_Flag=1;
				}
				else if((COMMS_U4_FLAGS.End2_Flag == 0) && (COMMS_U4_FLAGS.Start2_Flag ==1))
				{
					COMMS_U4_FLAGS.End2_Flag=1;	// rises the flag that a frame has ended
					COMMS_U4_FLAGS.EndPtr2=d;		// take that pointer of the end of the frame
					COMMS_U4_FLAGS.ID_Flag = 3;
					COMMS_U4_DMA_flag =1;
				}
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("COMMS_U4:Frame Error 2\n");
					COMMS_U4_FLAGS.Error_Flag=1;

					COMMS_U4_FLAGS.End2_Flag=0;
					COMMS_U4_FLAGS.Start2_Flag=0;
				}
			}
			else if(COMMS_U4_FLAGS.ID_Flag == 3)
			{
				if((COMMS_U4_FLAGS.End3_Flag == 0) && (COMMS_U4_FLAGS.Start3_Flag ==0))
				{
					COMMS_U4_FLAGS.Start3_Flag=1;	// rises the flag that a frame has started
					COMMS_U4_FLAGS.StartPtr3=d;		// take that pointer of the start of the frame
					COMMS_U4_FLAGS.Frame_Flag=1;
				}
				else if((COMMS_U4_FLAGS.End3_Flag == 0) && (COMMS_U4_FLAGS.Start3_Flag ==1))
				{
					COMMS_U4_FLAGS.End3_Flag=1;	// rises the flag that a frame has ended
					COMMS_U4_FLAGS.EndPtr3=d;		// take that pointer of the end of the frame
					COMMS_U4_FLAGS.ID_Flag = 1;
					COMMS_U4_DMA_flag =1;
				}
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("COMMS_U4:Frame Error 3\n");
					COMMS_U4_FLAGS.Error_Flag=1;

					COMMS_U4_FLAGS.End3_Flag=0;
					COMMS_U4_FLAGS.Start3_Flag=0;
				}
			}
		}
	}
}

SAT_returnState COMMS_U4_UART_recv(struct uart_data *uart_data) 
{
	if(COMMS_U4_DMA_flag)
	{
		uint8_t* data;
		int32_t length;
		if(COMMS_U4_FLAGS.ID_Flag == 2)// frame recvd from 1
		{
			length = COMMS_U4_FLAGS.EndPtr1 - COMMS_U4_FLAGS.StartPtr1+1;
			data = COMMS_U4_FLAGS.StartPtr1;
					
			// clearing the frame parameters
			COMMS_U4_FLAGS.End1_Flag=0;
			COMMS_U4_FLAGS.EndPtr1=0;
			COMMS_U4_FLAGS.Length1=0;
			COMMS_U4_FLAGS.Start1_Flag=0;
			COMMS_U4_FLAGS.StartPtr1=0;
		}
		else if(COMMS_U4_FLAGS.ID_Flag == 3)// frame recvd from 2
		{
			length = COMMS_U4_FLAGS.EndPtr2 - COMMS_U4_FLAGS.StartPtr2+1;
			data = COMMS_U4_FLAGS.StartPtr2;

			// clearing the frame parameters
			COMMS_U4_FLAGS.Start2_Flag=0;
			COMMS_U4_FLAGS.End2_Flag=0;
			COMMS_U4_FLAGS.StartPtr2=0;
			COMMS_U4_FLAGS.EndPtr2=0;
			COMMS_U4_FLAGS.Length2=0;
		}
		else if (COMMS_U4_FLAGS.ID_Flag == 1)// frame recvd from 3
		{
			length = COMMS_U4_FLAGS.EndPtr3 - COMMS_U4_FLAGS.StartPtr3+1;
			data = COMMS_U4_FLAGS.StartPtr3;
			
			// clearing the frame parameters
			COMMS_U4_FLAGS.Start3_Flag=0;
			COMMS_U4_FLAGS.End3_Flag=0;
			COMMS_U4_FLAGS.StartPtr3=0;
			COMMS_U4_FLAGS.EndPtr3=0;
			COMMS_U4_FLAGS.Length3=0;
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
		
		UART2_BUF_O_Write_String_To_Buffer("COMMS_U4: packet recieved\n");
		uart_data->uart_size = length;
		memcpy(uart_data->uart_unpkt_buf, data, length);
		COMMS_U4_DMA_flag = 0;
		return SATR_EOT;
	}
	return SATR_OK;
}
void COMMS_U4_UART_send( uint8_t *buf, uint16_t size) 
{
	const uint8_t* d = buf;
	for ( ;size > 0;size--, ++d)
	{
		COMMS_U4_BUF_O_Write_Char_To_Buffer(*d);		
	}
	UART2_BUF_O_Write_String_To_Buffer("comms msg sent\n");
}
/*----------------------------------------------------------------------------*-
   
  COMMS_U4_BUF_O_Write_String_To_Buffer()
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
void COMMS_U4_BUF_O_Write_String_To_Buffer(const char* const STR_PTR)
{
 uint32_t i = 0;
 while ((STR_PTR[i] != '\0')&&(STR_PTR[i] != '*'))
	{
		COMMS_U4_BUF_O_Write_Char_To_Buffer(STR_PTR[i]);
		i++;
	}
}
void COMMS_U4_BUF_O_Write_Frame_To_Buffer(const void* data, size_t len)
{
	 const uint8_t* d = data;
   for ( ;len > 0;len--, ++d)
	{
		if(*d==0x7E)
			continue;
		COMMS_U4_BUF_O_Write_Char_To_Buffer(*d);
	}
}
/*----------------------------------------------------------------------------*-
   
  COMMS_U4_BUF_O_Write_Char_To_Buffer()
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
void COMMS_U4_BUF_O_Write_Char_To_Buffer(const char CHARACTER)
{
	// Write to the buffer *only* if there is space
	if (COMMS_U4_Wait_idx_g < COMMS_U4_TX_BUFFER_SIZE_BYTES)
		{
			if( COMMS_U4_Tx_buffer_select_g == 1 )
			{
				COMMS_U4_Tx_buffer_g[COMMS_U4_Wait_idx_g] = CHARACTER;
				COMMS_U4_Tx_buffer_ig[COMMS_U4_Wait_idx_g] = (char) ~CHARACTER;
			}
			else
			{
				COMMS_U4_Tx_buffer2_g[COMMS_U4_Wait_idx_g] = CHARACTER;
				COMMS_U4_Tx_buffer2_ig[COMMS_U4_Wait_idx_g] = (char) ~CHARACTER;
			}
			COMMS_U4_Wait_idx_g++;     
		}
	else
		{
		// Write buffer is full
		// No error handling / reporting here (characters may be lost)
		// Adapt as required to meet the needs of your application
			UART2_BUF_O_Write_String_To_Buffer("Error inside COMMS_U4 Write buffer is full\n");
		}

	// Update the copy
	COMMS_U4_Wait_idx_ig = ~COMMS_U4_Wait_idx_g;
}
/*----------------------------------------------------------------------------*-
  COMMS_U4_BUF_O_Send_All_Data()
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
     COMMS_U4_BUF_O_Check_Data_Integrity() is called.
  POST-CONDITION CHECKS:
     None.
  ERROR DETECTION / ERROR HANDLING:
     None.
  RETURN VALUE:
     None.
-*----------------------------------------------------------------------------*/
void COMMS_U4_BUF_O_Send_All_Data(void)
{
	uint32_t T3;
	// wait until the previous transfer is done
	if(COMMS_U4_DMA_TX_STREAM->NDTR == 0 || COMMS_U4_DMA_TX_STREAM->NDTR == COMMS_U4_TX_BUFFER_SIZE_BYTES+1)
	{
		// Check data integrity
		COMMS_U4_BUF_O_Check_Data_Integrity();
		// set the dma transfer data length
		COMMS_U4_DMA_TX_STREAM->NDTR = COMMS_U4_Wait_idx_g;
		// set memory base address to one of two buffers
		if( COMMS_U4_Tx_buffer_select_g == 1 )
		{
			COMMS_U4_DMA_TX_STREAM->M0AR = (uint32_t)COMMS_U4_Tx_buffer_g ;
			COMMS_U4_Tx_buffer_select_g = 2;
		}
		else
		{
			COMMS_U4_DMA_TX_STREAM->M0AR = (uint32_t)COMMS_U4_Tx_buffer2_g ;
			COMMS_U4_Tx_buffer_select_g = 1;
		}
		
		// Clear DMA Stream Flags// needs to be changed for every Stream
		DMA2->HIFCR |= (0x3D << 22);
		
		// Clear USART Transfer Complete Flags
		USART_ClearFlag(COMMS_U4_UARTX,USART_FLAG_TC);  
		
		// Enable DMA USART TX Stream 
		DMA_Cmd(COMMS_U4_DMA_TX_STREAM,ENABLE);
		
		// wait until it's enabled
		// while( DMA_GetCmdStatus(DMA1_Stream6) == DISABLE );
		
		// Reset the indices
		COMMS_U4_Wait_idx_g = 0;
		COMMS_U4_Wait_idx_ig = ~0;
	}
}

