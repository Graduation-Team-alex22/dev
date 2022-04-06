/*----------------------------------------------------------------------------*-

  ttrd2-a08a-t0401a-v001a_processor.c (Release Release 2017-03-06a)

  Configures a Dry Scheduler to generate the Tick List for TRRD2-19a.

  See 'ERES2' (Appendix 8 and Chapter 19) for details.

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

// Tasks
#include "../tasks/ttrd2-a08a-t0401a-v001a_dummy_task.h"
#include "../tasks/app_sensor_mgn_task.h"
#include "../tasks/app_sensor_gps_task.h"
#include "../tasks/app_sensor_imu_task.h"
#include "../tasks/app_sensor_tmp_task.h"

// Support for reporting Tick List
#include "../support_functions/ttrd2-a08a-t0401a-v001a_uart2_unbuff_o.h"


// ------ Private variable definitions ---------------------------------------

// The current Processor Mode / State
static eProc_MoSt Processor_MoSt_g;

// ------ Private function declarations --------------------------------------

void PROCESSOR_Identify_Reqd_MoSt(void);
void PROCESSOR_Configure_Reqd_MoSt(void);

/*----------------------------------------------------------------------------*-

  PROCESSOR_Init()

  Wrapper for system startup functions.
  
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
void PROCESSOR_Init(void)
   {
   PROCESSOR_Identify_Reqd_MoSt();
   PROCESSOR_Configure_Reqd_MoSt();
   }

/*----------------------------------------------------------------------------*-

  PROCESSOR_Identify_Reqd_MoSt()

  Identify the cause of the system reset.
  Set the system mode / state accordingly.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Processor_MoSt_g (W)

  MCU HARDWARE:
     Registers indicating cause of reset (R).

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     Check for reset caused by iWDT => 'fail safe'

  RETURN VALUE:
     None.
   
-*----------------------------------------------------------------------------*/
void PROCESSOR_Identify_Reqd_MoSt(void)
   {
   // Check cause of reset
   if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)
      {
      // Reset was caused by WDT => "Fail Safe" state
      Processor_MoSt_g = FAIL_SAFE_S;
      }
   else
      {
      // Here we treat all other forms of reset in the same way
      // => "Normal" mode
      Processor_MoSt_g = NORMAL_M;
      }
      
   // Clear cause-of-reset flags
   RCC_ClearFlag();      
   }

/*----------------------------------------------------------------------------*-

  PROCESSOR_Configure_Reqd_MoSt()

  Configure the system in the required mode.  

  PARAMETERS:
     None.

  LONG-TERM DATA:
     System_node_G (R)

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
void PROCESSOR_Configure_Reqd_MoSt(void)
   {
   // Used for reporting by Dry Scheduler
   UART2_NO_BUF_O_Init(230400);
   UART2_NO_BUF_O_Write_String("Reporting task sequence after TSIP:\n");

   switch (Processor_MoSt_g)
      {
      // Default to "Fail Safe" state 
      default:          
      case FAIL_SAFE_S:
         {
         // Reset caused by iWDT
         // Trigger "fail safe" behaviour
         PROCESSOR_Perform_Safe_Shutdown();

         break;
         }

      case NORMAL_M:
         {
         // Set up the scheduler
         // No Tick Interval is specified here  
         SCH_Init();

         // Task init (for completeness)
         DUMMY_TASK_Init();


         // Add DUMMY tasks to schedule.
         // ALL TASK RELEASES REPRESENTED BY THE SAME DUMMY TASK

         // This task set matches TTRD2-19a 

         // Parameters are:
         // A. Task name
         // B. Initial delay / offset (in Ticks)
         // C. Task period (in Ticks): Must be > 0
         //           A                  B  C
         SCH_Add_Task(DUMMY_TASK_Update, 0,  1);    // iWDT
         SCH_Add_Task(DUMMY_TASK_Update, 0,  100);  // Heartbeat
         SCH_Add_Task(DUMMY_TASK_Update, 0,  50);  // ADC1    
         SCH_Add_Task(DUMMY_TASK_Update, 0,  100);  // Proc task     
         SCH_Add_Task(DUMMY_TASK_Update, 0,  1);    // UART2         
         SCH_Add_Task(DUMMY_TASK_Update, 1,  20);    // IMU      
         SCH_Add_Task(DUMMY_TASK_Update, 2,  20);    // mgn      
         SCH_Add_Task(DUMMY_TASK_Update, 3,  20);    // tmp 
         SCH_Add_Task(DUMMY_TASK_Update, 4,  20);    // GPS
         SCH_Add_Task(DUMMY_TASK_Update, 5,  20);    // sun sensor
         SCH_Add_Task(DUMMY_TASK_Update, 6,  20);    // health check
         SCH_Add_Task(DUMMY_TASK_Update, 7,  20);    // tle
         SCH_Add_Task(DUMMY_TASK_Update, 8,  20);    // sgp4
         SCH_Add_Task(DUMMY_TASK_Update, 9,  20);    // ref verctors
         SCH_Add_Task(DUMMY_TASK_Update, 10,  20);    // attitude determination
         SCH_Add_Task(DUMMY_TASK_Update, 11,  20);    // control update
         SCH_Add_Task(DUMMY_TASK_Update, 12,  20);    // Actuators
         SCH_Add_Task(DUMMY_TASK_Update, 13,  100);    // OBC Comm
         

         // Now configure the Dry Scheduler elements
         SCH_Init_Dry_Scheduler();

         break;
         }
      }
   }

/*----------------------------------------------------------------------------*-

  PROCESSOR_Perform_Safe_Shutdown()

  Attempt to move the system into a safe ('silent') state.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     Heartbeat LED (GPIO pin).

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.
   
-*----------------------------------------------------------------------------*/
void PROCESSOR_Perform_Safe_Shutdown(void)
   {
   // Here we simply "fail silent"
   while(1);
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/

