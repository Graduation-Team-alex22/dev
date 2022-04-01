/*----------------------------------------------------------------------------*-

  ttrd2-05a-t0401a-v001a_timeout_t3.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
  Timeout code library.

  Use as basis of timeout mechanisms, dummy tasks and Sandwich Delays.

  STM32F401RE.
  
  Uses T3 (16-bit timer).  

  Update every microsecond => max 65 ms timeout.

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

// Processor Header
#include "../main/main.h"

/*----------------------------------------------------------------------------*-

  TIMEOUT_T3_USEC_Init()

  Prepare T3 for use as delay mechanism.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.
       
  MCU HARDWARE:
     Timer 3.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     Stores the Timer 3 register configuration (to allow subsequent checks).

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void TIMEOUT_T3_USEC_Init(void)
   {
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
      
   // Enable T3 clock 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

   TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

   // Time base configuration
   // From 84 MHz T3 clock to 1 MHz
   TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;

   // Set maximum timer overflow value in microseconds: 65000 = 65 ms.  
   TIM_TimeBaseStructure.TIM_Period = 65000 - 1;

   TIM_TimeBaseStructure.TIM_ClockDivision = 0;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
         
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
   }

/*----------------------------------------------------------------------------*-

  TIMEOUT_T3_USEC_Start()

  Starts Timer 3.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.
       
  MCU HARDWARE:
     Timer 3.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void TIMEOUT_T3_USEC_Start(void)
   {
   // Reset the timer
   TIM_SetCounter(TIM3, 0);
   
   // Start the timer
   TIM_Cmd(TIM3, ENABLE);    

   // Store a copy of the TIM3 register configuration
   REG_CONFIG_TIMER_Store(TIM3);
   }

/*----------------------------------------------------------------------------*-

  TIMEOUT_T3_USEC_Get_Timer_State()

  Checks the T3 counter value against a specified threshold.

  PARAMETERS:
     COUNT: The required timeout value (in microseconds). 

  LONG-TERM DATA:
     None.
       
  MCU HARDWARE:
     Timer 3.

  PRE-CONDITION CHECKS:
     Checks the Timer 3 register configuration.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     See PreCs.

  RETURN VALUE:
     TIMED_OUT if the time limit has been exceeded;
     COUNTING if the time limit has not been exceeded.

-*----------------------------------------------------------------------------*/

uint32_t TIMEOUT_T3_USEC_Get_Timer_State(const uint32_t COUNT)
   {
   // Check the TIM3 register configuration
   REG_CONFIG_TIMER_Check(TIM3);

   if (TIM_GetCounter(TIM3) >= COUNT)
      {
      return TIMED_OUT;
      }
   else
      {
      return COUNTING;
      }
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
