/*----------------------------------------------------------------------------*-

  ttrd2-19a-t0401a-v001b_monittor_i.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------

  Internal MoniTTor mechanism for TTC scheduler.

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

   CorrelaTTor, DuplicaTTor, MoniTTor, PredicTTor, ReliabiliTTy, SafeTTy, 
   SafeTTy Systems and WarranTTor are registered trademarks or trademarks 
   of SafeTTy Systems Ltd in the UK and other countries.

-*----------------------------------------------------------------------------*/

// Module header
#include "ttrd2-18a-t0401a-v001a_monittor_i.h"

// ------ Private Duplicated Variables ('File Global') -----------------------

// BCET of task that is currently being monitored (microseconds)
static uint32_t Task_BCET_g;
static uint32_t Task_BCET_ig;

// Allow timing variation for task that is currently being monitored (microseconds)
static uint32_t Task_t_variation_g;
static uint32_t Task_t_variation_ig;

/*----------------------------------------------------------------------------*-

  MONITTORi_Init()

  Monitor initialisation function.

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
void MONITTORi_Init(void)
   {
   // Nothing to do in this version
   }

/*----------------------------------------------------------------------------*-

  MONITTORi_Start()

  Starts the MoniTTor mechanism.

  PARAMETERS:
     TASK_ID      - The task position in the task array.
     TASK_WCET_us - Task WCET in microseconds.
     TASK_BCET_us - Task BCET in microseconds.
     LEEWAY_us    - Gives some flexibility in WCET and BCET checks.

  LONG-TERM DATA:
     Task_BCET_g, Task_BCET_ig;
     Task_t_variation_g, Task_t_variation_ig;
       
  MCU HARDWARE:
      SysTick.
      DWT.

  PRE-CONDITION CHECKS:
     Sanity checks on function parameters.

     If any PreC check fails, we force a shutdown
     [other behaviour may be more appropriate in your system.]

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     See PreCs.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void MONITTORi_Start(const uint32_t TASK_ID,
                     const uint32_t TASK_WCET_us,
                     const uint32_t TASK_BCET_us,
                     const uint32_t LEEWAY_us)
   {
   // Check pre-conditions (START)

   // Failure of any PreC will force a shutdown in this design
   // Other behaviour may be more appropriate in your system

   // Sanity checks on function parameters
   if ((TASK_ID > SCH_MAX_TASKS) ||
       (TASK_BCET_us > TASK_WCET_us)) 
      {
      PROCESSOR_Perform_Safe_Shutdown(PFC_MONITTOR_PARAMETERS);
      }

   // Check pre-conditions (END)

   // We perform the monitoring activities if we met the pre-conditions
   // (otherwise we won't get this far)

   Task_BCET_g = TASK_BCET_us;
   Task_BCET_ig = ~TASK_BCET_us;

   Task_t_variation_g = LEEWAY_us;
   Task_t_variation_ig = ~LEEWAY_us;

   // Set up SysTick timer
   SysTick_Config((SystemCoreClock / 1000000) * (TASK_WCET_us + LEEWAY_us));

   // Stop timer (started by SysTick_Config()) & disable interrupt
   SysTick->CTRL &= 0xFFFFFFFC; 

   //---

   // Using DWT to measure elapsed time

   // Enable TRC
   CoreDebug->DEMCR &= ~0x01000000;
   CoreDebug->DEMCR |=  0x01000000;

   // Reset counter
   DWT->CYCCNT = 0;

   // Enable counter
   DWT->CTRL &= ~0x00000001;
   DWT->CTRL |=  0x00000001;

   // Start SysTick timer & enable interrupt
   SysTick->CTRL |= 0x00000003;
   }


/*----------------------------------------------------------------------------*-

  MONITTORi_Stop()

  Stop monitoring a task (call just after task completes).

  Task has not overrun - need to check for possible underrun.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Task_BCET_g, Task_BCET_ig;
     Task_t_variation_g, Task_t_variation_ig;

  MCU HARDWARE:
      SysTick.
      DWT.

  PRE-CONDITION CHECKS:
     Check Duplicated Variables.

     If any PreC check fails, we force a shutdown
     [other behaviour may be more appropriate in your system.]

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     See PreCs.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void MONITTORi_Stop(void)
   {
   SysTick->CTRL &= 0xFFFFFFFC;  // Stop timer + disable interrupt

   // Execution time in microseconds from DWT counter (runs at 84 MHz)
   uint32_t Execution_time_us = DWT->CYCCNT / 84;

   // After time-sensitive operations, check the pre-conditions (START)

   // Failure of any PreC will force a shutdown in this design
   // Other behaviour may be more appropriate in your system

   // Check data integrity (DVs)
   if ((Task_BCET_g != ~Task_BCET_ig) ||
       (Task_t_variation_g != ~Task_t_variation_ig))
      {
      PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
      }

   // Check pre-conditions (END)

   if ((Execution_time_us + Task_t_variation_g) < Task_BCET_g)
      {
      // Task has completed more quickly than would normally be expected

      // Check the current mode
      if (PROCESSOR_Get_MoSt() == STARTUP_06b_MONITTOR_URUN_CHECK_M)
         {
         // This is expected behaviour in this mode (run-time test)
         // We now go on to test the PredicTTor
         PROCESSOR_Store_Reqd_MoSt(STARTUP_07_PREDICTTOR_CHECK_M);  
         PROCESSOR_Store_PFC(PFC_NO_FAULT);
         PROCESSOR_Change_MoSt();
         }
      else
         {
         // This is *not* expected behaviour
         PROCESSOR_Perform_Safe_Shutdown(PFC_MONITTOR_UNDERRUN);
         }
      }
   else
      {
      // No underrun detected: again, we check the mode
      if (PROCESSOR_Get_MoSt() == STARTUP_06b_MONITTOR_URUN_CHECK_M)
         {
         // Should have detected underrun
         PROCESSOR_Perform_Safe_Shutdown(PFC_MONITTOR_UNDERRUN_TEST);
         }
      }
   }

/*----------------------------------------------------------------------------*-

  MONITTORi_Disable()

  Disable the MoniTTor unit by disabling the SysTick timer.
  Typically used in the event of an unrelated fault, prior to mode
  change, to avoid MoniTTor ISR being triggered erroneously.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.
       
  MCU HARDWARE:
     SysTick.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void MONITTORi_Disable(void)
   {
   SysTick->CTRL &= 0xFFFFFFFC;  // Stop timer + disable interrupt
   }

/*----------------------------------------------------------------------------*-

  SysTick_Handler()

  This is the task overrun ISR.

  It will be triggered only if the monitored task exceeds its allowed WCET 
  (including any allowed leeway)

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.
       
  MCU HARDWARE:
     SysTick.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void SysTick_Handler(void)
   {
   // Check the current mode
   if (PROCESSOR_Get_MoSt() == STARTUP_06a_MONITTOR_ORUN_CHECK_M)
      {
      // This is expected behaviour in this mode (run-time test)
      PROCESSOR_Store_Reqd_MoSt(STARTUP_06b_MONITTOR_URUN_CHECK_M);
      PROCESSOR_Store_PFC(PFC_NO_FAULT);
      PROCESSOR_Change_MoSt();
      }
   else
      {
      // This is *not* expected behaviour
      PROCESSOR_Perform_Safe_Shutdown(PFC_MONITTOR_OVERRUN);
      }
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
