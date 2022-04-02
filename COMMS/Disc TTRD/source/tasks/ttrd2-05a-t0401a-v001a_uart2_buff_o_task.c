/*----------------------------------------------------------------------------*-

  ttrd2-05a-t0401a-v001a_uart2_buff_o_task.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------

  Simple UART2 'buffered output' library for STM32F4 (Nucleo-F401RE board).
  
  Also offers unbuffered ('flush whole buffer') option: use this with care!
  
  Allows use of USB port on board as UART2 interface.
    
  See 'ERES2' (Chapter 3 and Chapter 5) for further information.

-*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*-

  This code is copyright (c) 2014-2017 SafeTTy Systems Ltd.

  This code forms part of a Time-Triggered Reference Design (TTRD) that is 
  documented in the following book: 

   Pont, M.J. (2016) 
   "The Engineering of Reliable Embedded Systems (Second Edition)", 
   Published by SafeTTy Systems Ltd. ISBN: 978-0-9930355-3-1.

  Both the TTRDs and the above book ("ERES2") describe patented 
  technology and are subject to copyright and other restrictions.

  This code may be used without charge: [i] by universities and colleges on 
  courses for which a degree up to and including 'MSc' level (or equivalent) 
  is awarded; [ii] for non-commercial projects carried out by individuals 
  and hobbyists.

  Any and all other use of this code and / or the patented technology 
  described in ERES2 requires purchase of a ReliabiliTTy Technology Licence:
  http://www.safetty.net/technology/reliabilitty-technology-licences

  Please contact SafeTTy Systems Ltd if you require clarification of these 
  licensing arrangements: http://www.safetty.net/contact

  CorrelaTTor, DecomposiTTor, DuplicaTTor, MoniTTor, PredicTTor, ReliabiliTTy,  
  SafeTTy, SafeTTy Systems, TriplicaTTor and WarranTTor are registered 
  trademarks or trademarks of SafeTTy Systems Ltd in the UK & other countries.

-*----------------------------------------------------------------------------*/

// Module header
#include "ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"


// Supports checks of register integrity
#include "../hsi_reg_config_checks/ttrd2-05a-t0401a-v001a_reg_conf_chk_uart.h"
#include "ttrd2-05a-t0401a-v001a_switch_task.h"

// ------ Private constants --------------------------------------------------
#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))
// The transmit buffer length
#define UART2_TX_BUFFER_SIZE_BYTES 4000
#define UART2_RX_BUFFER_SIZE_BYTES 4000
char * data="Hello TTRD\n";
uint8_t UART2_Flag=0;
// variablees belong to RX ---------------------------------------------------
uint8_t CC_RX_SP_Flag;
char CC_RX_SP_Command[10];
uint8_t CC_RX_Sent_Flag=0;
// variablees belong to TX ---------------------------------------------------
uint8_t CC_TX_SP_Flag;
char CC_TX_SP_Command[10];
uint8_t CC_TX_Sent_Flag=0;
// ------ Private variables --------------------------------------------------

static char  UART2_Tx_buffer_g[UART2_TX_BUFFER_SIZE_BYTES];
static char  UART2_Tx_buffer_ig[UART2_TX_BUFFER_SIZE_BYTES];  // Inverted copy
static char* UART2_Tx_buffer2_g = (char*)0x2001C000;

static char UART2_Rx_buffer_g[UART2_RX_BUFFER_SIZE_BYTES];
static char UART2_Rx_buffer_ig[UART2_RX_BUFFER_SIZE_BYTES];  // Inverted copy

// Index of data in the buffer that has not yet been sent (with inv. copy)
static uint32_t Wait_idx_g  = 0;  
static uint32_t Wait_idx_ig = ~0; 



// buffer select to switch between the two buffers
static uint8_t UART2_Tx_buffer_select_g = 1;
// ------ Private function prototypes ----------------------------------------

void UART2_BUF_O_Check_Data_Integrity(void);
/*----------------------------------------------------------------------------*-

  UART2PFC_BUF_O_Init()

  Set up UART2.

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
void UART2PFC_BUF_O_Init(uint32_t BAUD_RATE)
   {
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;

   // USART2 clock enable 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

   // GPIOA clock enable 
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

   // GPIO config
   GPIO_InitStructure.GPIO_Pin   = UART2_TX_PIN; 
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
   GPIO_Init(UART2_PORT, &GPIO_InitStructure);

   GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

   // USART2 configuration
   // - BaudRate as specified in function parameter
   // - Word Length = 8 Bits
   // - One Stop Bit
   // - No parity
   // - Hardware flow control disabled (RTS and CTS signals)
   // - Tx (only) enabled
   USART_InitStructure.USART_BaudRate = BAUD_RATE;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Tx;
   USART_Init(USART2, &USART_InitStructure);

   // Enable UART2
   USART_Cmd(USART2, ENABLE);
   
   // Set up timeout mechanism
   TIMEOUT_T3_USEC_Init();



   // Store the UART register configuration
   REG_CONFIG_CHECKS_UART_Store(USART2);
   }

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Init()

  Set up UART2.

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
void UART2_BUF_O_Init(uint32_t BAUD_RATE)
   {
		// Set up timeout mechanism
   TIMEOUT_T3_USEC_Init();
		 
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
	 DMA_InitTypeDef	 hdma_usart2_rx;
	 DMA_InitTypeDef	 hdma_usart2_tx;
   // USART2 clock enable 
   RCC_APB1PeriphClockCmd(UART2_UART_RCC, ENABLE);

   // GPIOA clock enable 
   RCC_AHB1PeriphClockCmd(UART2_PORT_RCC, ENABLE);

	/* DMA controller clock enable */
	 RCC_AHB1PeriphClockCmd(UART2_DMA_RCC, ENABLE);
   // GPIO config
	 GPIO_PinAFConfig(UART2_PORT, UART2_TX_PIN_SOURCE, GPIO_AF_USART2);	
	 GPIO_PinAFConfig(UART2_PORT, UART2_RX_PIN_SOURCE, GPIO_AF_USART2);
   GPIO_InitStructure.GPIO_Pin   = UART2_TX_PIN|UART2_RX_PIN; 
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
   GPIO_Init(UART2_PORT, &GPIO_InitStructure);


   // USART2 configuration
   // - BaudRate as specified in function parameter
   // - Word Length = 8 Bits
   // - One Stop Bit
   // - No parity
   // - Hardware flow control disabled (RTS and CTS signals)
   // - Tx (only) enabled
   USART_InitStructure.USART_BaudRate = BAUD_RATE;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
   USART_Init(UART2_UARTX, &USART_InitStructure);
	
	//DMA RX init
		
	 DMA_StructInit(&hdma_usart2_rx);
	 hdma_usart2_rx.DMA_Channel = DMA_Channel_4;
	 hdma_usart2_rx.DMA_PeripheralBaseAddr = (uint32_t) (&(USART2->DR)); 
	 hdma_usart2_rx.DMA_Memory0BaseAddr = (uint32_t)UART2_Rx_buffer_g;  //
	 hdma_usart2_rx.DMA_DIR = DMA_DIR_PeripheralToMemory;
	 hdma_usart2_rx.DMA_BufferSize = UART2_RX_BUFFER_SIZE_BYTES+1;
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
	 DMA_Init(UART2_DMA_RX_STREAM, &hdma_usart2_rx);
	 
	 
	 //DMA TX init 
	 DMA_StructInit(&hdma_usart2_tx); 
	 hdma_usart2_tx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	 hdma_usart2_tx.DMA_MemoryInc = DMA_MemoryInc_Enable;
	 hdma_usart2_tx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	 hdma_usart2_tx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	 hdma_usart2_tx.DMA_Priority = DMA_Priority_High;
	 hdma_usart2_tx.DMA_FIFOMode = DMA_FIFOMode_Disable;
	 hdma_usart2_tx.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;//DMA_FIFOThreshold_1QuarterFull
	 hdma_usart2_tx.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	 hdma_usart2_tx.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	 
	 hdma_usart2_tx.DMA_Channel = UART2_DMA_Channel;
	 hdma_usart2_tx.DMA_PeripheralBaseAddr = (uint32_t) (&(UART2_UARTX->DR)); 
	 hdma_usart2_tx.DMA_Memory0BaseAddr = (uint32_t)UART2_Rx_buffer_g;
	 hdma_usart2_tx.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	 hdma_usart2_tx.DMA_BufferSize = UART2_TX_BUFFER_SIZE_BYTES+1;
	 hdma_usart2_tx.DMA_Mode = DMA_Mode_Normal;
	 DMA_Init(UART2_DMA_TX_STREAM, &hdma_usart2_tx);
	
   // Enable UART2
   USART_Cmd(UART2_UARTX, ENABLE);
	 
	 
//   Enable USART DMA TX Requsts 
//	 DMA_Cmd (UART2_DMA_TX_STREAM, ENABLE);
	 USART_DMACmd(UART2_UARTX, USART_DMAReq_Tx, ENABLE);
//	 while(!DMA_GetCmdStatus(UART2_DMA_TX_STREAM))
//	 {
//	 };
	 
	 // Enable USART DMA RX Requsts
   DMA_Cmd (UART2_DMA_RX_STREAM, ENABLE);
	 USART_DMACmd(UART2_UARTX, USART_DMAReq_Rx, ENABLE);
	 while(!DMA_GetCmdStatus(UART2_DMA_RX_STREAM))
	 {
	 };




   // Configure buffer
   for (uint32_t i = 0; i < UART2_TX_BUFFER_SIZE_BYTES; i++)
      {
      UART2_Tx_buffer_g[i] = 'X';
			UART2_Tx_buffer2_g[i] = 'G';
      UART2_Tx_buffer_ig[i] = (char) ~'X'; 
      }
		Wait_idx_g = 0;
		Wait_idx_ig = ~0;
   // Store the UART register configuration
   REG_CONFIG_CHECKS_UART_Store(USART2);
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Update()

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
     UART2_BUF_O_Check_Data_Integrity() is called.

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
uint32_t UART2_BUF_O_Update(void)
{
	uint32_t Return_value = RETURN_NORMAL_STATE;
	UART2_DMA_CHECK();
	if(Wait_idx_g > 0)
	{  
		UART2_BUF_O_Send_All_Data();
	}  
	
	return Return_value;
}

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Send_All_Data()

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
     UART2_BUF_O_Check_Data_Integrity() is called.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Send_All_Data(void)
{
	uint32_t T3;
	// wait until the previous transfer is done
	if(DMA1_Stream6->NDTR == 0 || DMA1_Stream6->NDTR == UART2_TX_BUFFER_SIZE_BYTES+1)
	{
		// Check data integrity
		UART2_BUF_O_Check_Data_Integrity();
		// set the dma transfer data length
		DMA1_Stream6->NDTR = Wait_idx_g;
		// set memory base address to one of two buffers
		if( UART2_Tx_buffer_select_g == 1 )
		{
			DMA1_Stream6->M0AR = (uint32_t)UART2_Tx_buffer_g ;
			UART2_Tx_buffer_select_g = 2;
		}
		else
		{
			DMA1_Stream6->M0AR = (uint32_t)UART2_Tx_buffer2_g ;
			UART2_Tx_buffer_select_g = 1;
		}
		
		// Clear DMA Stream Flags
		DMA1->HIFCR |= (0x3D << 16);
		
		// Clear USART Transfer Complete Flags
		USART_ClearFlag(USART2,USART_FLAG_TC);  
		
		// Enable DMA USART TX Stream 
		DMA_Cmd(DMA1_Stream6,ENABLE);
		
		// wait until it's enabled
		// while( DMA_GetCmdStatus(DMA1_Stream6) == DISABLE );
		
		// Reset the indices
		Wait_idx_g = 0;
		Wait_idx_ig = ~0;
	}
}


/*----------------------------------------------------------------------------*-
   
  UART2_BUF_O_Write_String_To_Buffer()

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
void UART2_BUF_O_Write_String_To_Buffer(const char* const STR_PTR)
{
 uint32_t i = 0;
 while (STR_PTR[i] != '\0')
	{
	UART2_BUF_O_Write_Char_To_Buffer(STR_PTR[i]);
	i++;
	}
}

/*----------------------------------------------------------------------------*-
   
  UART2_BUF_O_Write_Char_To_Buffer()

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
void UART2_BUF_O_Write_Char_To_Buffer(const char CHARACTER)
{
	// Write to the buffer *only* if there is space
	if (Wait_idx_g < UART2_TX_BUFFER_SIZE_BYTES)
		{
			if( UART2_Tx_buffer_select_g == 1 )
			{
				UART2_Tx_buffer_g[Wait_idx_g] = CHARACTER;
				UART2_Tx_buffer_ig[Wait_idx_g] = (char) ~CHARACTER;
			}
			else
			{
				UART2_Tx_buffer2_g[Wait_idx_g] = CHARACTER;
			}
			Wait_idx_g++;     
		}
	else
		{
		// Write buffer is full
		// No error handling / reporting here (characters may be lost)
		// Adapt as required to meet the needs of your application
		}

	// Update the copy
	Wait_idx_ig = ~Wait_idx_g;
}
	 


/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Write_Number10_To_Buffer()

  Writes 10-digit (decimal, unsigned) number to Tx buffer as a string.
   
  Supported values: 0 - 9,999,999,999. 
   
  (Can be used with 32-bit unsigned integer values.) 

  PARAMETERS:
     DATA : The number to be stored.

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
void UART2_BUF_O_Write_Number10_To_Buffer(const uint32_t DATA)
   {
   char Digit[11];

   __disable_irq();
   Digit[10] = '\0';  // Null terminator
   Digit[9]  = 48 + (DATA % 10);
   Digit[8]  = 48 + ((DATA/10) % 10);
   Digit[7]  = 48 + ((DATA/100) % 10);
   Digit[6]  = 48 + ((DATA/1000) % 10);
   Digit[5]  = 48 + ((DATA/10000) % 10);
   Digit[4]  = 48 + ((DATA/100000) % 10);
   Digit[3]  = 48 + ((DATA/1000000) % 10);
   Digit[2]  = 48 + ((DATA/10000000) % 10);
   Digit[1]  = 48 + ((DATA/100000000) % 10);
   Digit[0]  = 48 + ((DATA/1000000000) % 10);
   __enable_irq();

   UART2_BUF_O_Write_String_To_Buffer(Digit);
   }

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Write_Number04_To_Buffer()

  Writes 4-digit (decimal, unsigned) number to Tx buffer as a string.

  Supported values: 0 - 9999.   

  PARAMETERS:
     DATA : The number to be stored.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     A simple range check is performed.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     The function simply returns if the data are out of range.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Write_Number04_To_Buffer(const uint32_t DATA)
   {
   char Digit[5];
     
   if (DATA <= 9999)
      {    
      __disable_irq();
      Digit[4] = '\0';  // Null terminator
      Digit[3] = 48 + (DATA % 10);
      Digit[2] = 48 + ((DATA/10) % 10);
      Digit[1] = 48 + ((DATA/100) % 10);
      Digit[0] = 48 + ((DATA/1000) % 10);
      __enable_irq();

      UART2_BUF_O_Write_String_To_Buffer(Digit);
      }
   }

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Write_Number03_To_Buffer()

  Writes 3-digit (decimal, unsigned) number to Tx buffer as a string.

  Supported values: 0 - 999.   

  PARAMETERS:
     DATA : The number to be stored.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     A simple range check is performed.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     The function simply returns if the data are out of range.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Write_Number03_To_Buffer(const uint32_t DATA)
   {
   char Digit[4];
     
   if (DATA <= 999)
      {    
      __disable_irq();
      Digit[3] = '\0';  // Null terminator
      Digit[2] = 48 + (DATA % 10);
      Digit[1] = 48 + ((DATA/10) % 10);
      Digit[0] = 48 + ((DATA/100) % 10);
      __enable_irq();

      UART2_BUF_O_Write_String_To_Buffer(Digit);
      }
   }

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Write_Number02_To_Buffer()

  Writes 2-digit (decimal, unsigned) number to Tx buffer as a string.

  Supported values: 0 - 99.   

  PARAMETERS:
     DATA : The number to be stored.
   
  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     A simple range check is performed.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     The function simply returns if the data are out of range.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_BUF_O_Write_Number02_To_Buffer(const uint32_t DATA)
   {
   char Digit[3];

   if (DATA <= 99)
      {      
      __disable_irq();
      Digit[2] = '\0';  // Null terminator
      Digit[1] = 48 + (DATA % 10);
      Digit[0] = 48 + ((DATA/10) % 10);
      __enable_irq();
      }  

   UART2_BUF_O_Write_String_To_Buffer(Digit);
   }

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Check_Data_Integrity()

  Check data integrity.

  PARAMETER:
     None.

  LONG-TERM DATA:
     Tx_buffer_g[]  (R)
     Tx_buffer_ig[] (R)    
     Wait_idx_g     (R)  
     Wait_idx_ig    (R) 

  MCU HARDWARE:
     UART2.

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
void UART2_BUF_O_Check_Data_Integrity(void)
   {
   // Check the UART register configuration
   REG_CONFIG_CHECKS_UART_Check(USART2);   

   // Check integrity of 'waiting' index
   if ((Wait_idx_g != (~Wait_idx_ig)))
      {
      // Data have been corrupted: Treat here as Fatal Platform Failure
      PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
      }            

   // Check integrity of data in Tx buffer
   for (uint32_t i = 0; i < UART2_TX_BUFFER_SIZE_BYTES; i++)  
      {
      if ((UART2_Tx_buffer_g[i] != ((char) ~UART2_Tx_buffer_ig[i])))
         {
         // Data have been corrupted: Treat here as Fatal Platform Failure
         PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
         }            
      }
   }

	 	 /*----------------------------------------------------------------------------*-

  UART2_BUF_Check_toTransmit()

  Check if the button is Pressed it transmit a hello TTRD string
  it uses the Switch task

  PARAMETERS:
	 DATA : 
				None.


		RETURN VALUE:
			 None.

-*----------------------------------------------------------------------------*/
	 
	 uint32_t		 UART2_Check_toTransmit(void)
	 {
		 uint32_t Return_value = RETURN_NORMAL_STATE;
		 if ((SWITCH_BUTTON1_Get_State() == BUTTON1_PRESSED)||UART2_Flag)
			 {
				 UART2_Flag++;
				 if(UART2_Flag>20)	//200 ms
				 {
				 UART2_BUF_O_Write_String_To_Buffer(data);
				 UART2_Flag=0;
				 }
			 }
		 
		 return Return_value;
	 }

uint32_t UART2_DMA_CHECK(void)
{
	uint32_t Return_value = RETURN_NORMAL_STATE;
	static uint32_t old_pos;
	uint32_t pos;
	//__HAL_DMA_GET_COUNTER(Sucess) IS_DMA_BUFFER_SIZE  IS_DMA_PERIPHERAL_DATA_SIZE  IS_DMA_MEMORY_DATA_SIZE
	pos = ARRAY_LEN(UART2_Rx_buffer_g)+1 - DMA_GetCurrDataCounter(DMA1_Stream5);
	if (pos==0xFFFFFFFF)//UART2_RX_BUFFER_SIZE_BYTES
		pos=0;
	if (pos != old_pos) 
	 {                       /* Check change in received data */
		if (pos > old_pos) 
			{                    /* Current position is over previous one */
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
				UART2_PROCESS_DATA(&UART2_Rx_buffer_g[old_pos], pos - old_pos);
			} 
		else 
			{
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
				UART2_PROCESS_DATA(&UART2_Rx_buffer_g[old_pos], ARRAY_LEN(UART2_Rx_buffer_g) - old_pos);
				if (pos > 0) 
					{
						UART2_PROCESS_DATA(&UART2_Rx_buffer_g[0], pos);
					}
			}
		old_pos = pos;                          /* Save current position as old for next transfers */
	}
 return Return_value;
}	 

/**
 * \brief           Process received data over UART
 * \note            Either process them directly or copy to other bigger buffer
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
void UART2_PROCESS_DATA(const void* data, size_t len)
{// the almost code got an error
	const uint8_t* d = data;
	if((*d == 'R')&&(*(d+1) == 'X')&&(*(d+2)=='_')&&(*(d+3)=='A')&&(*(d+4)=='T'))// checking for the sequence of the input data
	{
		CC_RX_SP_Flag = 1;
		CC_RX_Sent_Flag=0;
		UART2_BUF_O_Write_String_To_Buffer("\nRX:");
		for (int CC_RX_counter=0; CC_RX_counter < 10;CC_RX_counter++)
		{
			CC_RX_SP_Command[CC_RX_counter] = *(d+3+CC_RX_counter);
			if(CC_RX_SP_Command[CC_RX_counter]=='*')
				break;
			UART2_BUF_O_Write_Char_To_Buffer(CC_RX_SP_Command[CC_RX_counter]);
		}
		UART2_BUF_O_Write_Char_To_Buffer('\n');	
	}
	else if((*d == 'T')&&(*(d+1) == 'X')&&(*(d+2)=='_')&&(*(d+3)=='A')&&(*(d+4)=='T'))// checking for the sequence of the input data
	{
		CC_TX_SP_Flag = 1;
		CC_TX_Sent_Flag=0;
		UART2_BUF_O_Write_String_To_Buffer("\nTX:");
		for (int CC_TX_counter=0; CC_TX_counter < 10;CC_TX_counter++)
		{
			CC_TX_SP_Command[CC_TX_counter] = *(d+3+CC_TX_counter);
			if(CC_TX_SP_Command[CC_TX_counter]=='*')
				break;
			UART2_BUF_O_Write_Char_To_Buffer(CC_TX_SP_Command[CC_TX_counter]);
		}
		UART2_BUF_O_Write_Char_To_Buffer('\n');	
	}
	else
	{	
		UART2_BUF_O_Write_String_To_Buffer("Wrong Input: Echo Mode\n");
		for ( ;len > 0;len--, ++d)
		{
		UART2_BUF_O_Write_Char_To_Buffer(*d);
		}
		UART2_BUF_O_Write_Char_To_Buffer('\n');
	}
}
	 /*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
