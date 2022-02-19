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

// ------ Private constants --------------------------------------------------

// The transmit buffer length
#define TX_BUFFER_SIZE_BYTES 500

// ------ Private variables --------------------------------------------------

static char Tx_buffer_g[TX_BUFFER_SIZE_BYTES];
static char Tx_buffer_ig[TX_BUFFER_SIZE_BYTES];  // Inverted copy

// Index of data in the buffer that has been sent (with copy)
static uint32_t Sent_idx_g  = 0;  
static uint32_t Sent_idx_ig = ~0;  

// Index of data in the buffer that has not yet been sent (with inv. copy)
static uint32_t Wait_idx_g  = 0;  
static uint32_t Wait_idx_ig = ~0; 

// Timeout values (used when characters are transmitted)
static uint32_t Timeout_us_g;
static uint32_t Timeout_us_ig;

// ------ Private function prototypes ----------------------------------------

void UART2_BUF_O_Send_Char(const char);
void UART2_BUF_O_Check_Data_Integrity(void);

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

   // Time taken to send one character at given baud rate (microseconds)
   // We add 10 usec leeway
   Timeout_us_g = (10000000 / BAUD_RATE) + 10;
   Timeout_us_ig = ~Timeout_us_g;

   // Configure buffer
   for (uint32_t i = 0; i < TX_BUFFER_SIZE_BYTES; i++)
      {
      Tx_buffer_g[i] = 'X';
      Tx_buffer_ig[i] = (char) ~'X'; 
      }

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

  LONG-TERM DATA:
     Sent_idx_g (W)  
     Sent_idx_ig (W)  
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

   // Check data integrity
   UART2_BUF_O_Check_Data_Integrity();

   // Are there any data ready to send?
   if (Sent_idx_g < Wait_idx_g)
      {
      UART2_BUF_O_Send_Char(Tx_buffer_g[Sent_idx_g]);     

      Sent_idx_g++;
      }
   else
      {
      // No data to send - just reset the buffer index
      Wait_idx_g = 0;
      Sent_idx_g = 0;
      }

   // Update the copies
   Wait_idx_ig = ~Wait_idx_g;
   Sent_idx_ig = ~Sent_idx_g;

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
     Sent_idx_g  (W)  
     Sent_idx_ig (W)  
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
   // Check data integrity
   UART2_BUF_O_Check_Data_Integrity();

   while (Sent_idx_g < Wait_idx_g)
      {
      UART2_BUF_O_Send_Char(Tx_buffer_g[Sent_idx_g]);

      Sent_idx_g++;
      Sent_idx_ig = ~Sent_idx_g;
      }

   // Reset the indices
   Sent_idx_g = 0;
   Sent_idx_ig = ~0;
   Wait_idx_g = 0;
   Wait_idx_ig = ~0;
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
   if (Wait_idx_g < TX_BUFFER_SIZE_BYTES)
      {
      Tx_buffer_g[Wait_idx_g] = CHARACTER;
      Tx_buffer_ig[Wait_idx_g] = (char) ~CHARACTER;
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
   
  UART2_BUF_O_Send_Char()

  Uses on-chip UART0 hardware to send a single character.

  PARAMETERS:
     CHARACTER : The data to be sent.

  LONG-TERM DATA:
     Timeout_us_g (R)

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
void UART2_BUF_O_Send_Char(const char CHARACTER)
   {
   // Check data integrity
   UART2_BUF_O_Check_Data_Integrity();

   // We use a baudrate-dependent timeout (based on T3)
   TIMEOUT_T3_USEC_Start();
   while((USART_GetFlagStatus(USART2, USART_FLAG_TXE) == 0) &&
         (COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(Timeout_us_g)));

   USART_SendData(USART2, CHARACTER);
   }

/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Check_Data_Integrity()

  Check data integrity.

  PARAMETER:
     None.

  LONG-TERM DATA:
     Tx_buffer_g[]  (R)
     Tx_buffer_ig[] (R)  
     Timeout_us_g   (R)
     Timeout_us_ig  (R)
     Sent_idx_g     (R)  
     Sent_idx_ig    (R)  
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

   // Check integrity of 'sent' index
   if ((Sent_idx_g != (~Sent_idx_ig)))
      {
      // Data have been corrupted: Treat here as Fatal Platform Failure
      PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);  
      }      

   // Check integrity of 'waiting' index
   if ((Wait_idx_g != (~Wait_idx_ig)))
      {
      // Data have been corrupted: Treat here as Fatal Platform Failure
      PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
      }      

   // Check integrity of timeout value
   if ((Timeout_us_g != (~Timeout_us_ig)))
      {
      // Data have been corrupted: Treat here as Fatal Platform Failure
      PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
      }      

   // Check integrity of data in Tx buffer
   for (uint32_t i = 0; i < TX_BUFFER_SIZE_BYTES; i++)  
      {
      if ((Tx_buffer_g[i] != ((char) ~Tx_buffer_ig[i])))
         {
         // Data have been corrupted: Treat here as Fatal Platform Failure
         PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
         }            
      }
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
