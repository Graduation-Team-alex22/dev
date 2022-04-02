/*----------------------------------------------------------------------------*-

  ttrd2-a07a-t0401a-v001a_measure_t5.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
  Supports measurement of jitter in the scheduler tick.

  Assumes availability of UART-based reporting facility on target
  (adapt code if such a facility is not available).
  
  PLEASE NOTE:
  This module is expected to be used (with care) during system development:
  it is *not* generally expected that this module will be included in the
  finished system.

  Targets STM32F401RE.
  
  Uses T5 (16-bit timer).  
  
  See 'ERES2' Appendix 7 for further information.

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

// Support for buffered UART reporting
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"

// ------ Public Variables ('Processor Global') ------------------------------

// These variables are used to support the instrumented scheduler
// These variables are not duplicated or otherwise checked or protected.
// *** THIS FACILITY IS INTENDED FOR USE DURING DEVELOPMENT ONLY ***

// Max and min interval between ticks (as measured)
extern uint32_t Measured_tick_max_G;
extern uint32_t Measured_tick_min_G;

// Tick interval (microseconds)
extern uint32_t Scheduler_tick_us_G;

/*----------------------------------------------------------------------------*-
   
 MEASURE_T5_Init()

 Prepare to measure task execution times. 

 Initialises the 2-D array with min and max values.

 Sets up Timer 5.

-*----------------------------------------------------------------------------*/

void MEASURE_T5_Init(void)
   {
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

   Measured_tick_max_G = 0;
   Measured_tick_min_G = 9999;
      
   // Enable T5 clock 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); 

   TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

   // Time base configuration
   // No prescalar (increment timer at 84 MHz)
   TIM_TimeBaseStructure.TIM_Prescaler = 0;

   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
   
   TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
         
   TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
   }


/*----------------------------------------------------------------------------*-
   
  MEASURE_T5_Write_Data_To_UART_Buffer()

  Simple data reporting.

-*----------------------------------------------------------------------------*/
void MEASURE_T5_Write_Data_To_UART_Buffer(void)
   {
   static uint32_t First_call_s = 1;
   uint32_t Jitter_t5_count = Measured_tick_max_G - Measured_tick_min_G;

   // First data are nonsense (this is a crude but effective solution) ...
   if (First_call_s == 0)
      {
      // Assumes 84 MHz T5 timer settings
      uint32_t Jitter_ns = (Jitter_t5_count * Scheduler_tick_us_G) / 84;
                   
      UART2_BUF_O_Write_String_To_Buffer("\n");

      UART2_BUF_O_Write_String_To_Buffer("Max tick interval (T5 counter) : ");
      UART2_BUF_O_Write_Number10_To_Buffer(Measured_tick_max_G);
      UART2_BUF_O_Write_String_To_Buffer("\n");

      UART2_BUF_O_Write_String_To_Buffer("Min tick interval (T5 counter) : ");
      UART2_BUF_O_Write_Number10_To_Buffer(Measured_tick_min_G);
      UART2_BUF_O_Write_String_To_Buffer("\n");

      UART2_BUF_O_Write_String_To_Buffer("Raw jitter value (T5 counter)  : ");
      UART2_BUF_O_Write_Number10_To_Buffer(Jitter_t5_count);
      UART2_BUF_O_Write_String_To_Buffer("\n");

      UART2_BUF_O_Write_String_To_Buffer("Jitter (nanoseconds)           : ");
      UART2_BUF_O_Write_Number10_To_Buffer(Jitter_ns);
      UART2_BUF_O_Write_String_To_Buffer("\n");
      }
   else
      {
      UART2_BUF_O_Write_String_To_Buffer("Please wait ... \n");
      }

   // First data are nonsense (always restart count first time)
   // After this, decision about restart is determined by 'RESTART' flag
   if ((RESTART_DATA_MEASUREMENT_AFTER_REPORTING == 1) || (First_call_s == 1))
      {
      // Clear 'first call' flag
      First_call_s = 0;

      // Start the measurements from scratch
      Measured_tick_max_G = 0;
      Measured_tick_min_G = 999999;
      }
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
