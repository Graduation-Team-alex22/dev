/*----------------------------------------------------------------------------*-

   ttrd2-19a-t0401a-v001b_run_time_tests.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
   Functions to support run-time testing (POSTs, BISTs).

   STM32F401RE.

   See 'ERES2' (Chapter 19) for details.

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

// Processor Header
#include "../main/main.h"

// Access to iWDT
#include "../tasks/ttrd2-05a-t0401a-v001a_iwdt_task.h"

/*----------------------------------------------------------------------------*-

  RTT_Startup_04_WDT_Check()

  Test internal watchdog timer (based on simple loop delay).

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
void RTT_Startup_04_WDT_Check(void)
   {
   static volatile uint32_t i, x, y, z, p;

   // We use a loop delay here (timer may use same clock source as WDT).

   // Total delay of *approx* 8 ms in this Keil project
   // Timing behaviour will vary if a different compiler is used ...
   for (i = 0; i < 400; i++)
      {
      for (x = 0; x < 10; x++)
         {
         for (y = 0; y < 10; y++)
            {
            z = p;
            p = z*3;
            }
         }
      }
   }

/*----------------------------------------------------------------------------*-

  RTT_05a_Scheduler_Osc_Test_Update()

  Attempts to refresh the WDT for ~10 ms (assumes one call per millisecond).

  If this is successful, perform next startup test.

  If the test is not passed, WDT reset will trigger a change to FAIL_SAFE_S mode

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Call_count_s (not duplicated).
       
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
uint32_t RTT_05a_Scheduler_Osc_Test_Update(void)
   {
   static uint32_t Call_count_s = 0;

   WATCHDOG_Update();

   if (++Call_count_s == 10)
      {
      // If we are here, the test was passed
      PROCESSOR_Store_Reqd_MoSt(STARTUP_05b_SCHEDULER_OSC_CHECK_M);
      PROCESSOR_Store_PFC(PFC_NO_FAULT);
      PROCESSOR_Change_MoSt();
      }

   return RETURN_NORMAL_STATE;
   }

/*----------------------------------------------------------------------------*-

  RTT_05b_Scheduler_Osc_Test_Update()

  Attempt to refresh the WDT for a few milliseconds.
  
  WDT refreshes should *not* be frequent enough.

  If the test is not passed, we trigger a change to FAIL_SAFE_S mode

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Call_count_s (not duplicated).
       
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
uint32_t RTT_05b_Scheduler_Osc_Test_Update(void)
   {
   static uint32_t Call_count_s = 0;

   WATCHDOG_Update();

   if (++Call_count_s == 10)
      {
      // WDT should have triggered a reset by now ...
      // If we are here, the test was *not* passed
      PROCESSOR_Perform_Safe_Shutdown(PFC_SCH_STARTUP_TEST_5B);
      }

   return RETURN_NORMAL_STATE;
   }

/*----------------------------------------------------------------------------*-

  RTT_06a_MoniTTor_Overrun_Test_Update()

  This test task will be scheduled for periodic release but it should only 
  run once (during which the overrun should be detected).

  If it runs again, the overrun was not detected correctly and we force
  a shutdown.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Call_count_s (not duplicated).
       
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
uint32_t RTT_06a_MoniTTor_Overrun_Test_Update(void)
   {
   uint32_t i,j,k,l;
   static uint32_t Call_count_s = 0;
   
   // If task runs twice, the overrun (below) was not detected correctly ...
   if (++Call_count_s > 1)
      {
      PROCESSOR_Perform_Safe_Shutdown(PFC_MONITTOR_OVERRUN_TEST);
      }
   
   // This long delay should be detected ...
   for (i = 0; i < 100000; i++)
      {
      for (j = 0; j < 100000; j++)
         {
         k = l++;
         l = k*3;
         }
      }
        
   return RETURN_NORMAL_STATE;
   }
   
/*----------------------------------------------------------------------------*-

  RTT_06b_MoniTTor_Underrun_Test_Update()

  This very short test task will be scheduled for periodic release
  but it should only run once (during which the underrun should be detected).

  If it runs again, the first underrun was not detected correctly and
  we force a shutdown.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Call_count_s (not duplicated).
       
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
uint32_t RTT_06b_MoniTTor_Underrun_Test_Update(void)
   {
   static uint32_t Call_count_s = 0;

   Call_count_s++;

   uint32_t x,y,z,p;

   // Delay of around 35 usec
   for (x = 0; x < 10; x++)
      {
      for (y=0; y<10; y++)
         {
         z = p;
         p = z*3;
         }
      }

   // Check for multiple calls ...
   if (Call_count_s > 1)
      {
      PROCESSOR_Perform_Safe_Shutdown(PFC_MONITTOR_UNDERRUN_TEST);
      }
   
   return RETURN_NORMAL_STATE;
   }   

/*----------------------------------------------------------------------------*-

  RTT_07_PredicTTor_Test_Update()

  This task should be part of an 'incorrect' task sequence.

  The PredicTTor should detect this within a few ticks.

  If not, this task will force a shutdown.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Call_count_s (not duplicated).
       
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
uint32_t RTT_07_PredicTTor_Test_Update(void)
   {
   static uint32_t Call_count_s = 0;

   // Call count must be > TSIP for the task set used for testing
   if (++Call_count_s == 30)
      {
      // If we are here, the test was not passed
      PROCESSOR_Perform_Safe_Shutdown(PFC_PREDICTTOR_TASK_SEQ_FAULT_TEST);
      }

   return RETURN_NORMAL_STATE;
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
