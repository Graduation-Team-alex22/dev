/*----------------------------------------------------------------------------*-

  ttrd2-a08a-t0401a-v001a_uart2_unbuff_o.c (Release Release 2017-03-06a)

  ----------------------------------------------------------------------------

  Simple UART2 'buffered output' library for STM32F4 (Nucleo-F401RE board).
  
  Arranged as 'support functions' (not as a task).  

  Designed as a 'developer toolkit':  
  * Functions have long and variable execution time!
  * No timeout behaviour!

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
#include "ttrd2-a08a-t0401a-v001a_uart2_unbuff_o.h"

/*----------------------------------------------------------------------------*-

  UART2_NO_BUF_O_Init()

  Set up UART1.

  PARAMETER:
     Required baud rate.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     UART1.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_NO_BUF_O_Init(uint32_t BAUD_RATE)
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
   }

/*----------------------------------------------------------------------------*-
   
  UART2_NO_BUF_O_Write_String()

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
void UART2_NO_BUF_O_Write_String(const char* const STR_PTR)
   {
   uint32_t i = 0;

   while (STR_PTR[i] != '\0')
      {
      // Wait for USART to be free
      // Requires a 'peripheral timeout' mechanism (see ERES2, Chapter 5) 
      while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == 0);

      USART_SendData(USART2, STR_PTR[i]);      
   
      i++;
      }
   }

/*----------------------------------------------------------------------------*-

  UART2_NO_BUF_O_Write_Number10()

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
void UART2_NO_BUF_O_Write_Number10(const uint32_t DATA)
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

   UART2_NO_BUF_O_Write_String(Digit);
   }

/*----------------------------------------------------------------------------*-

  UART2_NO_BUF_O_Write_Number03()

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
void UART2_NO_BUF_O_Write_Number03(const uint32_t DATA)
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

      UART2_NO_BUF_O_Write_String(Digit);
      }
   }

/*----------------------------------------------------------------------------*-

  UART2_NO_BUF_O_Write_Number02()

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
void UART2_NO_BUF_O_Write_Number02(const uint32_t DATA)
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

   UART2_NO_BUF_O_Write_String(Digit);
   }

/*----------------------------------------------------------------------------*-

  UART2_NO_BUF_O_Write_Number02()

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
void UART2_NO_BUF_O_Write_Number01(const uint32_t DATA)
   {
   char Digit[2];

   if (DATA <= 9)
      {      
      __disable_irq();
      Digit[1] = '\0';  // Null terminator
      Digit[0] = 48 + (DATA % 10);
      __enable_irq();
      }  

   UART2_NO_BUF_O_Write_String(Digit);
   }

/*----------------------------------------------------------------------------*-
   
  UART2_NO_BUF_O_Send_Char()

  Uses on-chip UART0 hardware to send a single character.

  PARAMETERS:
     CHARACTER : The data to be sent.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     UART1.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void UART2_NO_BUF_O_Send_Char(const char CHARACTER)
   {
   // Wait for USART to be free
   // No timeout provided (adapt if required).
   while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == 0);

   USART_SendData(USART2, CHARACTER);
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
