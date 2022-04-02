/*----------------------------------------------------------------------------*-

  ttrd2-a08a-t0401a-v001a_dry_scheduler.c (Release Release 2017-03-06a)

  ----------------------------------------------------------------------------

  Time-Triggered Co-operative (TTC) task scheduler for STM32F401.

  *** DRY SCHEDULER *** Instrumented to generate Tick Lists ***

  See 'ERES2' (Appendix 8) for further information about this code example.

-*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*-

  This code is copyright (c) 2014-2016 SafeTTy Systems Ltd.

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

// Support for reporting Tick List
#include "../support_functions/ttrd2-a08a-t0401a-v001a_uart2_unbuff_o.h"

// ------ Constants and variables used to control generation of Tick List ----

// DS => 'Dry Scheduler'
// These variables / constants are not generally required in a scheduler.

// Calculated hyperperiod (in Ticks)
static uint32_t DS_hyperperiod_ticks_g = 1000;

// Calculated hyperperiod (in task releases)
static uint32_t DS_hyperperiod_task_releases_g;

// Keeps track of the position in the tick list
static int32_t DS_Tick_ID_g = -1;

// Set when DS should start reporting tick information (after TSIP)
static uint32_t DS_start_reporting_g = 0;

// This the largest offset (delay) in the task set (in Ticks)
static uint32_t DS_max_offset_g = 0;

// The length of the TSIP (in task calls)
static uint32_t DS_tsip_task_releases_g = 0;

// ------ Private variable definitions ---------------------------------------

// The array of tasks
// Check array size in scheduler header file
static sTask_t SCH_tasks_g[SCH_MAX_TASKS];

// Tick count
static uint32_t Tick_count_g = 0;

// ------ Private function prototypes ----------------------------------------

void SysTick_Handler(void);

uint32_t GCD_2_Numbers(const uint32_t N1, 
                       const uint32_t N2);

uint32_t LCM_2_Numbers(const uint32_t N1, 
                       const uint32_t N2);

/*----------------------------------------------------------------------------*-

  SCH_Init()

  Scheduler initialisation function.  

  Sets up LONG Tick Interval (to allowing scheduler to report task sequence).

  You must call this function before using the scheduler.

  [Required_SystemCoreClock frequency can be found in main.h.]

  PARAMETERS:
     TICKms is the required Tick Interval (milliseconds)

  LONG-TERM DATA:
     SCH_tasks_g (W)

  MCU HARDWARE:
     SysTick timer.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     PROCESSOR_Perform_Safe_Shutdown() is called if the requested timing 
     requirements cannot be met.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void SCH_Init(void)
   {
   for (uint32_t Task_id = 0; Task_id < SCH_MAX_TASKS; Task_id++)
      {
      // Set pTask to "null pointer"
      SCH_tasks_g[Task_id].pTask = SCH_NULL_PTR;
      }

   // Using CMSIS
      
   // SystemCoreClock gives the system operating frequency (in Hz)
   if (SystemCoreClock != REQUIRED_PROCESSOR_CORE_CLOCK)
      {
      // Fatal error
      PROCESSOR_Perform_Safe_Shutdown();
      }

   // Now to set up SysTick timer for "ticks" at every second
   if (SysTick_Config(1000 * SystemCoreClock / 1000))
      {
      // Fatal error
      PROCESSOR_Perform_Safe_Shutdown();
      }

   // Timer is started by SysTick_Config():
   // we need to disable SysTick timer and SysTick interrupt until
   // all tasks have been added to the schedule.
   SysTick->CTRL &= 0xFFFFFFFC;
   }

/*----------------------------------------------------------------------------*-

  SCH_Start()

  Starts the scheduler, by enabling SysTick interrupt.

  NOTES: 
  * All tasks must be added before starting scheduler.
  * Any other interrupts MUST be synchronised to this tick.  

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     SysTick timer.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void SCH_Start(void) 
   {
   // Enable SysTick timer
   SysTick->CTRL |= 0x01;

   // Enable SysTick interrupt
   SysTick->CTRL |= 0x02;
   }


/*----------------------------------------------------------------------------*-

  SysTick_Handler()

  [Function name determined by CMIS standard.]

  This is the scheduler ISR.  It is called at a rate 
  determined by the timer settings in the SCH_Init() function.

  In this Dry Scheduler it also supports data reporting.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Tick_count_g (W)

  MCU HARDWARE:
     SysTick timer.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     Checks Tick_count_g value => 'Fail Safe'
     [See ERES2, Ch2 for details.]

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void SysTick_Handler(void)
   {
   static uint32_t TSIP_ticks_s = 0;

   // Increment standard scheduler 'tick count' (no limit checks here)
   Tick_count_g++;
   
   // Increment count for dry scheduler
   DS_Tick_ID_g++;

   if (++TSIP_ticks_s > DS_max_offset_g)
      {
      DS_start_reporting_g = 1;
      TSIP_ticks_s = DS_max_offset_g;
      }

   if (DS_Tick_ID_g == (DS_hyperperiod_ticks_g + DS_max_offset_g))
      {
      // Reached required data limit: clear flag and stop the scheduler
      DS_start_reporting_g = 0;
      SysTick->CTRL &= 0xFFFFFFFC; 

      // Report length of hyperperiod
      UART2_NO_BUF_O_Write_String("\n\nLength of hyperperiod (Ticks)               : ");
      UART2_NO_BUF_O_Write_Number10(DS_hyperperiod_ticks_g);
      UART2_NO_BUF_O_Write_String("\n");

      UART2_NO_BUF_O_Write_String("Length of hyperperiod (Task releases)       : ");
      UART2_NO_BUF_O_Write_Number10(DS_hyperperiod_task_releases_g);
      UART2_NO_BUF_O_Write_String("\n");

      UART2_NO_BUF_O_Write_String("TSIP (Ticks)                                : ");
      UART2_NO_BUF_O_Write_Number10(DS_max_offset_g);
      UART2_NO_BUF_O_Write_String("\n");

      UART2_NO_BUF_O_Write_String("TSIP (Task releases)                        : ");
      UART2_NO_BUF_O_Write_Number10(DS_tsip_task_releases_g);
      UART2_NO_BUF_O_Write_String("\n");
      }
   } 

/*----------------------------------------------------------------------------*-

  SCH_Dispatch_Tasks()

  This is the 'dispatcher' function.  When a task (function)
  is due to run, SCH_Dispatch_Tasks() will run it.
  This function must be called (repeatedly) from the main loop.

  Triggers move to "idle" mode when all tasks have been released.

  In this Dry Scheduler, it also supports data reporting.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     SCH_tasks_g (W)
     Tick_count_g (W)

  MCU HARDWARE:
     Triggers move to idle mode.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void SCH_Dispatch_Tasks(void) 
   {
   uint32_t Spaces = 0;

   __disable_irq();  
   uint32_t Update_required = (Tick_count_g > 0);  // Check tick count
   __enable_irq();

   while (Update_required)
      {
      // Go through the task array
      for (uint32_t Task_id = 0; Task_id < SCH_MAX_TASKS; Task_id++)
         {
         // Check if there is a task at this location
         if (SCH_tasks_g[Task_id].pTask != SCH_NULL_PTR)
            {
            if (--SCH_tasks_g[Task_id].Delay == 0)
               {
               // (*SCH_tasks_g[Task_id].pTask)();  // Run the task
               // No tasks are released (Dry Scheduler)
               // Assumes < 1000 tasks [adapt if required ...]
               if (DS_start_reporting_g == 1)
                  {
                  // Report current Task ID
                  if (Task_id <= 9)
                     {
                     UART2_NO_BUF_O_Write_Number01(Task_id);
                     Spaces += 2;
                     }

                  if ((Task_id > 9) && (Task_id <= 99))
                     {
                     UART2_NO_BUF_O_Write_Number02(Task_id);
                     Spaces += 3;
                     }

                  if (Task_id > 99)
                     {
                     UART2_NO_BUF_O_Write_Number03(Task_id);
                     Spaces += 4;
                     }

                  UART2_NO_BUF_O_Write_String(",");

                  // Increment count of task releases
                  DS_hyperperiod_task_releases_g++;
                  } 
               else
                  {
                  // Not reporting
                  // Still need to count up the length of the TSIP
                  DS_tsip_task_releases_g++;
                  } 

               // All tasks are periodic: schedule task to run again
               SCH_tasks_g[Task_id].Delay = SCH_tasks_g[Task_id].Period;
               }
            }         
         } 

      __disable_irq();
      Tick_count_g--;                       // Decrement the count
      Update_required = (Tick_count_g > 0); // Check again
      __enable_irq();
      }

   if (DS_start_reporting_g == 1)
      {
      // Rather crude way of tidying up the reporting of Tick ID
      if (Spaces <= 40)
         {
         for (uint32_t i = 0; i < (40 - Spaces); i++)
             {
             UART2_NO_BUF_O_Write_String(" ");
             }
         }

      UART2_NO_BUF_O_Write_String("// Tick : ");
      UART2_NO_BUF_O_Write_Number10(DS_Tick_ID_g);
      UART2_NO_BUF_O_Write_String("\n");
      }

   // The scheduler enters idle mode at this point 
   __WFI();     
   }

/*----------------------------------------------------------------------------*-

  SCH_Add_Task()

  Adds a task (function) to the schedule.
  The task will be released periodically by the scheduler.

  PARAMETERS:
     pTask  : The name of the task (function) to be scheduled.
              NOTE: All scheduled functions must be 'void, void' -
              that is, they must take no parameters, and have 
              a void return type (in this design).
                   
     DELAY  : The interval (ticks) before the task is first executed.

     PERIOD : Task period (in ticks).  Must be > 0.

  LONG-TERM DATA:
     SCH_tasks_g (W)
  
  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     1. There is space in the task array.
     2. The task is periodic ('one-shot' tasks are not supported. 

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     PROCESSOR_Perform_Safe_Shutdown() is called:
     - if the task cannot be added to the schedule (array too small)
     - if an attempt is made to schedule a "one shot" task

  RETURN VALUE:  
     None.
 
-*----------------------------------------------------------------------------*/
void SCH_Add_Task(void (* pTask)(),
                  const uint32_t DELAY,
                  const uint32_t PERIOD)
   {
   uint32_t Task_id = 0;
   
   // First find a gap in the array (if there is one)
   while ((SCH_tasks_g[Task_id].pTask != SCH_NULL_PTR) 
          && (Task_id < SCH_MAX_TASKS))
      {
      Task_id++;
      } 
   
   // Have we reached the end of the list?   
   if (Task_id == SCH_MAX_TASKS)
      {
      // Task list is full - fatal error
      PROCESSOR_Perform_Safe_Shutdown();
      }
      
   // Check for "one shot" tasks
   if (PERIOD == 0)
      {
      // We do not allow "one shot" tasks (all tasks must be periodic)
      PROCESSOR_Perform_Safe_Shutdown();
      }

   // If we're here, there is a space in the task array
   // and the task to be added is periodic
   SCH_tasks_g[Task_id].pTask  = pTask;

   SCH_tasks_g[Task_id].Delay  = DELAY + 1;
   SCH_tasks_g[Task_id].Period = PERIOD;
   }

/*----------------------------------------------------------------------------*-

  SCH_Init_Dry_Scheduler()

  Sets up the Dry Scheduler (call after tasks have been added to the array).

-*----------------------------------------------------------------------------*/
void SCH_Init_Dry_Scheduler(void)
   {
   // Calculate the length of the hyperperiod (in ticks)
   // Assume at least 2 tasks ...
   DS_hyperperiod_ticks_g = LCM_2_Numbers(SCH_tasks_g[0].Period, 
                                          SCH_tasks_g[1].Period);

   for (uint32_t i = 2; i < SCH_MAX_TASKS; i++)
      {
      if (SCH_tasks_g[i].pTask != SCH_NULL_PTR)
         {
         DS_hyperperiod_ticks_g = LCM_2_Numbers(DS_hyperperiod_ticks_g, 
                                                SCH_tasks_g[i].Period);
         }
      }

   // Determine the largest task offset
   for (uint32_t i = 0; i < SCH_MAX_TASKS; i++)
      {
      if (SCH_tasks_g[i].pTask != SCH_NULL_PTR)
         {
         if ((SCH_tasks_g[i].Delay - 1) > DS_max_offset_g)
            {
            DS_max_offset_g = SCH_tasks_g[i].Delay - 1; 
            }
         }
      }
   }

/*----------------------------------------------------------------------------*-

  GCD_2_Numbers()

  Calculate the 'Greatest Common Divisor' of two numbers.

  Can be used to determine the length of the hyperperiod (and tick interval).

  See 'ERES2', Chapter 11.

-*----------------------------------------------------------------------------*/
uint32_t GCD_2_Numbers(const uint32_t N1, const uint32_t N2)
   {
   uint32_t Tmp;

   uint32_t n1 = N1;
   uint32_t n2 = N2;
   
   do {
      if (n1 < n2)
         {
         // Swap the numbers
         Tmp = n1;
         n1 = n2;
         n2 = Tmp;
         } 

      n1 = n1 % n2;
      } while (n1);

   // Now n2 is the GCD of the two numbers
   return n2;
   }

/*----------------------------------------------------------------------------*-

  LCM_2_Numbers()

  Calculate the 'Lowest Common Multiple' of two numbers.

  Can be used to determine the length of the hyperperiod.

  See 'ERES2', Chapter 11.

-*----------------------------------------------------------------------------*/
uint32_t LCM_2_Numbers(const uint32_t N1, const uint32_t N2)
   {
   uint32_t Product = N1 * N2;
   uint32_t Gcd = GCD_2_Numbers(N1, N2);

   return Product / Gcd;
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
