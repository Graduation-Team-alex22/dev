#include "obc_comm.h"


/***************** DMA for UART **************************/
// ------ Private variables -----------------------------
#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))	
#define OBC_C_SettingDelay 60
extern uint8_t OBC_C_Sent_Flag;
static uint8_t	OBC_C_Ready=1;
uint8_t OBC_C_SET_Counter=0;
uint8_t OBC_C_DMA_flag = 0;
struct OBC_C_FLAGS_t OBC_C_FLAGS;


static char OBC_C_Tx_buffer_g [OBC_C_TX_BUFFER_SIZE_BYTES];
static char OBC_C_Tx_buffer_ig[OBC_C_TX_BUFFER_SIZE_BYTES];  // Inverted copy

static char* OBC_C_Tx_buffer2_g = (char*)0x2001C000;
static char  OBC_C_Tx_buffer2_ig[OBC_C_TX_BUFFER_SIZE_BYTES];  // Inverted copy

static char OBC_C_Rx_buffer_g[OBC_C_RX_BUFFER_SIZE_BYTES];
static char OBC_C_Rx_buffer_ig[OBC_C_RX_BUFFER_SIZE_BYTES];  // Inverted copy

static uint16_t counter1 =0;
uint8_t Frame1[OBC_C_RX_BUFFER_SIZE_BYTES];
static uint16_t counter2 =0;
uint8_t Frame2[OBC_C_RX_BUFFER_SIZE_BYTES];
 
// Index of data in the buffer that has not yet been sent (with inv. copy)
static uint32_t OBC_C_Wait_idx_g  = 0;  
static uint32_t OBC_C_Wait_idx_ig = ~0; 

// buffer select to switch between the two buffers
static uint8_t OBC_C_Tx_buffer_select_g = 1;

// ------ Private function prototypes ----------------------------------------
void OBC_C_BUF_O_Check_Data_Integrity(void);

// ---------------------------------------------------------------------------

//Init
void OBC_C_init(uint32_t BAUD_RATE)
{
	OBC_C_FLAGS.ID_Flag=1;
	OBC_C_BUF_O_Init(BAUD_RATE);
}
/*----------------------------------------------------------------------------*-

  OBC_C_BUF_O_Init()

  Set up OBC_C.

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
void OBC_C_BUF_O_Init(uint32_t BAUD_RATE)
{
	// Set up timeout mechanism
  TIMEOUT_T3_USEC_Init();
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef	 hdma_usart6_rx;
	DMA_InitTypeDef	 hdma_usart6_tx;
	// USART3 clock enable 
	RCC_APB2PeriphClockCmd(OBC_C_UART_RCC, ENABLE);

	// GPIOB clock enable 
	RCC_AHB1PeriphClockCmd(OBC_C_PORT_RCC, ENABLE);
	 
	/* DMA controller clock enable */
	RCC_AHB1PeriphClockCmd(OBC_C_DMA_RCC, ENABLE);
	// GPIO config
	GPIO_PinAFConfig(OBC_C_PORT, OBC_C_RX_PIN_SOURCE, OBC_C_AF);	
	GPIO_PinAFConfig(OBC_C_PORT, OBC_C_TX_PIN_SOURCE, OBC_C_AF);
	GPIO_InitStructure.GPIO_Pin   = OBC_C_TX_PIN|OBC_C_RX_PIN; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(OBC_C_PORT, &GPIO_InitStructure); 
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
	USART_Init(OBC_C_UARTX, &USART_InitStructure);
	
	//DMA RX init
	DMA_StructInit(&hdma_usart6_rx);
	hdma_usart6_rx.DMA_Channel = OBC_C_DMA_Channel;
	hdma_usart6_rx.DMA_PeripheralBaseAddr = (uint32_t) (&(OBC_C_UARTX->DR));; 
	hdma_usart6_rx.DMA_Memory0BaseAddr = (uint32_t)OBC_C_Rx_buffer_g;  //
	hdma_usart6_rx.DMA_DIR = DMA_DIR_PeripheralToMemory;
	hdma_usart6_rx.DMA_BufferSize = OBC_C_RX_BUFFER_SIZE_BYTES;
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
	DMA_Init(OBC_C_DMA_RX_STREAM, &hdma_usart6_rx);
	
	//DMA TX init 
	DMA_StructInit(&hdma_usart6_tx); 
	hdma_usart6_tx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	hdma_usart6_tx.DMA_MemoryInc = DMA_MemoryInc_Enable;
	hdma_usart6_tx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	hdma_usart6_tx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	hdma_usart6_tx.DMA_Priority = DMA_Priority_VeryHigh;
	hdma_usart6_tx.DMA_FIFOMode = DMA_FIFOMode_Disable;
	hdma_usart6_tx.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;//DMA_FIFOThreshold_1QuarterFull
	hdma_usart6_tx.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	hdma_usart6_tx.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	hdma_usart6_tx.DMA_Channel = OBC_C_DMA_Channel;
	hdma_usart6_tx.DMA_PeripheralBaseAddr = (uint32_t) (&(OBC_C_UARTX->DR)); 
	hdma_usart6_tx.DMA_Memory0BaseAddr = (uint32_t)OBC_C_Tx_buffer_g;
	hdma_usart6_tx.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	hdma_usart6_tx.DMA_BufferSize = OBC_C_TX_BUFFER_SIZE_BYTES+1;
	hdma_usart6_tx.DMA_Mode = DMA_Mode_Normal;
	DMA_Init(OBC_C_DMA_TX_STREAM, &hdma_usart6_tx);

   // Enable UART6
   USART_Cmd(OBC_C_UARTX, ENABLE);
	 
	 
//   Enable USART DMA TX Requsts 
	 USART_DMACmd(OBC_C_UARTX, USART_DMAReq_Tx, ENABLE);
	 
	 // Enable USART DMA RX Requsts
   DMA_Cmd (OBC_C_DMA_RX_STREAM, ENABLE);
	 USART_DMACmd(OBC_C_UARTX, USART_DMAReq_Rx, ENABLE);
	 while(!DMA_GetCmdStatus(OBC_C_DMA_RX_STREAM))
	 {
	 };

	// Configure buffer
	for (uint32_t i = 0; i < OBC_C_TX_BUFFER_SIZE_BYTES; i++)
		{
		OBC_C_Tx_buffer_g[i] = 'X';
		OBC_C_Tx_buffer2_g[i] = 'G';
			
		OBC_C_Tx_buffer_ig[i] = (char) ~'X'; 
		OBC_C_Tx_buffer2_ig[i] =(char) ~'G';
		}

	// Store the UART register configuration
	REG_CONFIG_CHECKS_UART_Store(USART6);	
}
// update
uint32_t OBC_C_update(void)
{
	import_pkt (OBC_APP_ID, &comms_data.obc_uart);
	export_pkt (OBC_APP_ID, &comms_data.obc_uart);

	return RETURN_NORMAL_STATE;
}	
/*----------------------------------------------------------------------------*-

  OBC_C_BUF_O_Update()

  Sends next character from the software transmit buffer

  NOTE: Output-only library (Cannot receive chars)

  Uses on-chip UART hardware.

  PARAMETERS:
     None.
     Wait_idx_g  (W)  
     Wait_idx_ig (W) 

  MCU HARDWARE:
     UART2.

  PRE-CONDITION CHECKS:
     OBC_C_BUF_O_Check_Data_Integrity() is called.

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
uint32_t OBC_C_BUF_O_Update(void)
{
	if(OBC_C_Wait_idx_g > 0)
	{  
		OBC_C_BUF_O_Send_All_Data();
	} 
	return RETURN_NORMAL_STATE;
}
// Setters
int32_t  OBC_C_data_packet(const uint8_t *data, size_t size, uint8_t *rec_data)
{
	const uint8_t* d = data;
	for ( ;size > 0;size--, ++d)
	{
		OBC_C_BUF_O_Write_Char_To_Buffer(*d);
	}
	return COMMS_STATUS_OK;
}


/*----------------------------------------------------------------------------*-

  OBC_C_BUF_O_Check_Data_Integrity()

  Check data integrity.

  PARAMETER:
     None.

  LONG-TERM DATA:
     OBC_C_Tx_buffer_g[]  (R)
     OBC_C_Tx_buffer_ig[] (R)   
     OBC_C_Wait_idx_g     (R)  
     OBC_C_Wait_idx_ig    (R) 

  MCU HARDWARE:
     OBC_C.

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
void OBC_C_BUF_O_Check_Data_Integrity(void)
{
	// Check the UART register configuration
	REG_CONFIG_CHECKS_UART_Check(USART6);  

	// Check integrity of 'waiting' index
	if ((OBC_C_Wait_idx_g != (~OBC_C_Wait_idx_ig)))
	{
		// Data have been corrupted: Treat here as Fatal Platform Failure
		PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
	}          

	// Check integrity of data in Tx buffer
	for (uint32_t i = 0; i < OBC_C_TX_BUFFER_SIZE_BYTES; i++)  
	{
		if ((OBC_C_Tx_buffer_g[i] != ((char) ~OBC_C_Tx_buffer_ig[i])))
		{
			 // Data have been corrupted: Treat here as Fatal Platform Failure
			 PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
		 }            
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
void OBC_C_PROCESS_DATA(void* data, size_t len)
{
	uint8_t* d = data;

	for ( ;len > 0;len--, ++d)
	{
		if(OBC_C_FLAGS.SetParam_Flag)
		{
			UART2_BUF_O_Write_Char_To_Buffer(*d);
		}
		if(*d == 0x7E)
		{
			if(OBC_C_FLAGS.Error_Flag)
			{
				OBC_C_FLAGS.Error_Flag=0;
				continue;
			}
			UART2_BUF_O_Write_String_To_Buffer("\nOBC_RX:rcvd 7E\n");
			if(OBC_C_FLAGS.ID_Flag == 1)
			{
				if((OBC_C_FLAGS.End1_Flag == 0) && (OBC_C_FLAGS.Start1_Flag ==0))
				{
					counter1 =0;
					Frame1[counter1++]=0x7E;
					OBC_C_FLAGS.Start1_Flag=1;	// rises the flag that a frame has started
					OBC_C_FLAGS.StartPtr1=d;		// take that pointer of the start of the frame
				}
				else if((OBC_C_FLAGS.End1_Flag == 0) && (OBC_C_FLAGS.Start1_Flag ==1))
				{
					OBC_C_FLAGS.End1_Flag=1;	// rises the flag that a frame has ended
					OBC_C_FLAGS.EndPtr1=d;		// take that pointer of the end of the frame
					OBC_C_FLAGS.ID_Flag = 2;
					OBC_C_DMA_flag =1;
					Frame1[counter1++]=0x7E;
					Frame1[counter1]='\0';
					UART2_BUF_O_Write_String_To_Buffer("msg rcvd1\n");
				}
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("CC_RX:Frame Error 1\n");
					OBC_C_FLAGS.Error_Flag=1;
					counter1=0;
					OBC_C_FLAGS.End1_Flag=0;
					OBC_C_FLAGS.Start1_Flag=0;
				}
			}
			else if(OBC_C_FLAGS.ID_Flag == 2)
			{
				if((OBC_C_FLAGS.End2_Flag == 0) && (OBC_C_FLAGS.Start2_Flag ==0))
				{
					counter2 =0;
					Frame2[counter2++]=0x7E;
					OBC_C_FLAGS.Start2_Flag=1;	// rises the flag that a frame has started
					OBC_C_FLAGS.StartPtr2=d;		// take that pointer of the start of the frame
				}
				else if((OBC_C_FLAGS.End2_Flag == 0) && (OBC_C_FLAGS.Start2_Flag ==1))
				{
					OBC_C_FLAGS.End2_Flag=1;	// rises the flag that a frame has ended
					OBC_C_FLAGS.EndPtr2=d;		// take that pointer of the end of the frame
					OBC_C_FLAGS.ID_Flag = 1;
					OBC_C_DMA_flag =1;
					Frame2[counter2++]=0x7E;
					Frame2[counter2]='\0';
					UART2_BUF_O_Write_String_To_Buffer("msg rcvd2\n");
				}
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("CC_RX:Frame Error 2\n");
					OBC_C_FLAGS.Error_Flag=1;
					counter2=0;
					OBC_C_FLAGS.End2_Flag=0;
					OBC_C_FLAGS.Start2_Flag=0;
				}
			}

		}
		else
		{
			if((OBC_C_FLAGS.ID_Flag == 1)&&(OBC_C_FLAGS.Start1_Flag==1)&&(OBC_C_FLAGS.End1_Flag==0))
			{
					Frame1[counter1++] =*d;
			}
			if((OBC_C_FLAGS.ID_Flag == 2)&&(OBC_C_FLAGS.Start2_Flag==1)&&(OBC_C_FLAGS.End2_Flag==0))
			{
					Frame2[counter2++] =*d;
			}
		}
	}
}
void OBC_C_UART_send( uint8_t *buf, uint16_t size) 
{
	const uint8_t* d = buf;
	for ( ;size > 0;size--, ++d)
	{
		OBC_C_BUF_O_Write_Char_To_Buffer(*d);
	}
}
/*----------------------------------------------------------------------------*-
   
  OBC_C_BUF_O_Write_String_To_Buffer()

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
void OBC_C_BUF_O_Write_String_To_Buffer(const char* const STR_PTR)
{
 uint32_t i = 0;
 while ((STR_PTR[i] != '\0')&&(STR_PTR[i] != '*'))
	{
		OBC_C_BUF_O_Write_Char_To_Buffer(STR_PTR[i]);
		i++;
	}
}
void OBC_C_BUF_O_Write_Frame_To_Buffer(const void* data, size_t len)
{
	 const uint8_t* d = data;
   for ( ;len > 0;len--, ++d)
	{
		if(*d==0x7E)
			continue;
		OBC_C_BUF_O_Write_Char_To_Buffer(*d);
	}
}
/*----------------------------------------------------------------------------*-
   
  OBC_C_BUF_O_Write_Char_To_Buffer()

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
void OBC_C_BUF_O_Write_Char_To_Buffer(const char CHARACTER)
{
	// Write to the buffer *only* if there is space
	if (OBC_C_Wait_idx_g < OBC_C_TX_BUFFER_SIZE_BYTES)
		{
			if( OBC_C_Tx_buffer_select_g == 1 )
			{
				OBC_C_Tx_buffer_g[OBC_C_Wait_idx_g] = CHARACTER;
				OBC_C_Tx_buffer_ig[OBC_C_Wait_idx_g] = (char) ~CHARACTER;
			}
			else
			{
				OBC_C_Tx_buffer2_g[OBC_C_Wait_idx_g] = CHARACTER;
				OBC_C_Tx_buffer2_ig[OBC_C_Wait_idx_g] = (char) ~CHARACTER;
			}
			OBC_C_Wait_idx_g++;     
		}
	else
		{
		// Write buffer is full
		// No error handling / reporting here (characters may be lost)
		// Adapt as required to meet the needs of your application
			UART2_BUF_O_Write_String_To_Buffer("Error inside OBC_C Write buffer is full\n");
		}

	// Update the copy
	OBC_C_Wait_idx_ig = ~OBC_C_Wait_idx_g;
}
/*----------------------------------------------------------------------------*-

  OBC_C_BUF_O_Send_All_Data()

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
     OBC_C_BUF_O_Check_Data_Integrity() is called.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void OBC_C_BUF_O_Send_All_Data(void)
{
	uint32_t T3;
	// wait until the previous transfer is done
	if(OBC_C_DMA_TX_STREAM->NDTR == 0 || OBC_C_DMA_TX_STREAM->NDTR == OBC_C_TX_BUFFER_SIZE_BYTES+1)
	{
		// Check data integrity
		OBC_C_BUF_O_Check_Data_Integrity();
		// set the dma transfer data length
		OBC_C_DMA_TX_STREAM->NDTR = OBC_C_Wait_idx_g;
		// set memory base address to one of two buffers
		if( OBC_C_Tx_buffer_select_g == 1 )
		{
			OBC_C_DMA_TX_STREAM->M0AR = (uint32_t)OBC_C_Tx_buffer_g ;
			OBC_C_Tx_buffer_select_g = 2;
		}
		else
		{
			OBC_C_DMA_TX_STREAM->M0AR = (uint32_t)OBC_C_Tx_buffer2_g ;
			OBC_C_Tx_buffer_select_g = 1;
		}
		
		// Clear DMA Stream Flags// needs to be changed for every Stream
		DMA2->HIFCR |= (0x3D << 22);
		
		// Clear USART Transfer Complete Flags
		USART_ClearFlag(OBC_C_UARTX,USART_FLAG_TC);  
		
		// Enable DMA USART TX Stream 
		DMA_Cmd(OBC_C_DMA_TX_STREAM,ENABLE);
		
		// wait until it's enabled
		// while( DMA_GetCmdStatus(DMA1_Stream6) == DISABLE );
		
		// Reset the indices
		OBC_C_Wait_idx_g = 0;
		OBC_C_Wait_idx_ig = ~0;
	}
}

// Getters
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
void OBC_C_DMA_CHECK(struct _uart_data *uart_data)
{
	static uint32_t old_pos=0;
	uint32_t pos=0;
	pos = ARRAY_LEN(OBC_C_Rx_buffer_g) - DMA_GetCurrDataCounter(OBC_C_DMA_RX_STREAM);
	if (pos >= OBC_C_RX_BUFFER_SIZE_BYTES)
		pos-=OBC_C_RX_BUFFER_SIZE_BYTES;
	if (pos != old_pos) /* Check change in received data */
	{
//		UART2_BUF_O_Write_String_To_Buffer("Pos: ");
//		UART2_BUF_O_Write_Number10_To_Buffer(pos);
//		UART2_BUF_O_Write_String_To_Buffer("\n\n");
		if (pos > old_pos) 
		{                    /* Current position is over previous one */
			OBC_C_PROCESS_DATA(&OBC_C_Rx_buffer_g[old_pos], pos - old_pos);		
		} 
		else 
		{
			OBC_C_PROCESS_DATA(&OBC_C_Rx_buffer_g[old_pos], ARRAY_LEN(OBC_C_Rx_buffer_g) - old_pos);
			if (pos > 0) 
			{
				OBC_C_PROCESS_DATA(&OBC_C_Rx_buffer_g[0], pos);
			}
		}
		old_pos = pos;                          /* Save current position as old for next transfers */

	}
}

SAT_returnState OBC_C_UART_recv(struct _uart_data *uart_data) 
{
	if(OBC_C_DMA_flag)
	{
		int32_t length;
		if(OBC_C_FLAGS.ID_Flag == 2)// frame recvd from 1
		{
			UART2_BUF_O_Write_String_To_Buffer("\nOBC_C_UART_recv1");
			length = counter1;
			// clearing the frame parameters
			OBC_C_FLAGS.End1_Flag=0;
			OBC_C_FLAGS.EndPtr1=0;
			OBC_C_FLAGS.Length1=0;
			OBC_C_FLAGS.Start1_Flag=0;
			OBC_C_FLAGS.StartPtr1=0;
		}
		else if(OBC_C_FLAGS.ID_Flag == 1)// frame recvd from 2
		{
			UART2_BUF_O_Write_String_To_Buffer("\nOBC_C_UART_recv2");
			length = counter2;
			// clearing the frame parameters
			OBC_C_FLAGS.Start2_Flag=0;
			OBC_C_FLAGS.End2_Flag=0;
			OBC_C_FLAGS.StartPtr2=0;
			OBC_C_FLAGS.EndPtr2=0;
			OBC_C_FLAGS.Length2=0;
		}
		if(length > OBC_C_RX_BUFFER_SIZE_BYTES) // here need to make a request for the corrupted packet
		{
			UART2_BUF_O_Write_String_To_Buffer("\nSize Error:");
			UART2_BUF_O_Write_String_To_Buffer("\n--> Size:");
			UART2_BUF_O_Write_Signed_Number_To_Buffer(length);
			UART2_BUF_O_Write_String_To_Buffer("\n");
			length=OBC_C_RX_BUFFER_SIZE_BYTES;
		}

		uart_data->uart_size = length;
		if(OBC_C_FLAGS.ID_Flag == 2)// frame recvd from 1
			memcpy(uart_data->uart_unpkt_buf, Frame1, length);
		else if(OBC_C_FLAGS.ID_Flag == 1)// frame recvd from 2
			memcpy(uart_data->uart_unpkt_buf, Frame2, length);
		
		OBC_C_DMA_flag = 0;
		return SATR_EOT;
	}
	return SATR_OK;
}








