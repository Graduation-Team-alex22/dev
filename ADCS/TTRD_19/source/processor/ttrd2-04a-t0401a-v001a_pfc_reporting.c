/*----------------------------------------------------------------------------*-

   ttrd2-04a-t0401a-v001a_pfc_reporting.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
   Simple 'PFC reporting' module for STM32F401RE.

   Targets Nucleo F401RE board.
  
   Reports PFC via UART2 and on 'Heartbeat' LED.
  
   [May not be required in this form after prototyping.]
  
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
#include "ttrd2-04a-t0401a-v001a_pfc_reporting.h"

// Support for UART / USB reporting on Nucleo board
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"

/*----------------------------------------------------------------------------*-

  REPORT_PFC_Init()

  Prepare for REPORT_PFC_Update() function.
  
  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     Heartbeat LED pin.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void REPORT_PFC_Init(void)
   {
   // Set up LED pin (BEGIN)
   GPIO_InitTypeDef GPIO_InitStruct;
     
   // Enable GPIOA clock 
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
     
   // Configure port pin for the LED
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed; 
   GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStruct.GPIO_Pin   = HEARTBEAT_LED_PIN;
   
   GPIO_Init(HEARTBEAT_LED_PORT, &GPIO_InitStruct); 
   // Set up LED pin (END)

   // Set up UART / USB link (BEGIN)
   UART2_BUF_O_Init(230400);
   // Set up UART / USB link (END)   

   // Set up timer for 'TT00' architecture (BEGIN)
   TIMEOUT_T3_USEC_Init();
   // Set up timer for 'TT00' architecture (END)
   }

/*----------------------------------------------------------------------------*-

  REPORT_PFC_Update()

  Call this function periodically (roughly once per second).
  
  [Not usually called by a full scheduler - usually in a 'TT00' architecture.]
   
  PARAMETERS:
     None.

  LONG-TERM DATA:
     PFC_led_state_s (W)

  MCU HARDWARE:
     Heartbeat LED pin.

  PRE-CONDITION CHECKS:
     None.

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
void REPORT_PFC_Update(uint32_t PFC_CODE)
   {
   // No copies (nothing we can do if data are corrupted)   
   static uint32_t PFC_led_state_s = 0;
   static uint32_t PFC_counter_s = 0;

   // No PreCs (make no sense in this context)

   // Report PFC via UART
   UART2_BUF_O_Write_String_To_Buffer("PFC: ");
   UART2_BUF_O_Write_Number03_To_Buffer(PFC_CODE);
   UART2_BUF_O_Write_String_To_Buffer("\n");
   UART2_BUF_O_Send_All_Data();

   // Increment counter used for LED reporting
   PFC_counter_s++;

   // Short 'blank' period before code is counted out
   if (PFC_counter_s < 6)
      {
      PFC_led_state_s = 0;
      GPIO_ResetBits(HEARTBEAT_LED_PORT, HEARTBEAT_LED_PIN);
      }
   else
      {
      // Flash out the error code
      if (PFC_counter_s < ((PFC_CODE*2) + 6))
         {
         // Change the LED from OFF to ON (or vice versa)
         if (PFC_led_state_s == 1)
            {
            PFC_led_state_s = 0;
            GPIO_ResetBits(HEARTBEAT_LED_PORT, HEARTBEAT_LED_PIN);
            }
         else
            {
            PFC_led_state_s = 1;
            GPIO_SetBits(HEARTBEAT_LED_PORT, HEARTBEAT_LED_PIN);
            }
         }
      else
         {
         PFC_counter_s = 0;
         }
      }
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
