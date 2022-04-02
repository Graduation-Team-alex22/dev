/*----------------------------------------------------------------------------*-

  ttrd2-a07a-t0401a-v001a_instrumented_scheduler.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------

  Instrumented TTC task scheduler for STM32F401.
   
  Uses Timer 2.

  See 'ERES2' Appendix 7 and Chapter 19 for further information.

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

// MoniTTor header 
#include "ttrd2-18a-t0401a-v001a_monittor_i.h"

// PredicTTor header
#include "ttrd2-19a-t0401a-v001b_predicttor_i.h"

// Support timing measurements (instrumented scheduler)
#include "../scheduler/ttrd2-a07a-t0401a-v001a_measure_t4.h"
#include "../scheduler/ttrd2-a07a-t0401a-v001a_measure_t5.h"

// ------ Public Variables ('Processor Global') ------------------------------

// These variables are used to support the instrumented scheduler
// These variables are not duplicated or otherwise checked or protected.
// *** THIS FACILITY IS INTENDED FOR USE DURING DEVELOPMENT ONLY ***

uint32_t Idle_time_max_G = 0;
uint32_t Idle_time_min_G = 10000;

uint32_t Measured_tick_max_G;
uint32_t Measured_tick_min_G;

uint32_t Sum_of_wcets_max_G = 0;

uint32_t Scheduler_tick_us_G;

// ------ Private Duplicated Variables ('File Global') -----------------------

// The array of tasks
// Check array size in scheduler header file
static sTask_t SCH_tasks_g[SCH_MAX_TASKS];
static sTask_t SCH_tasks_ig[SCH_MAX_TASKS];  // Inverted copy

// Tick count
static uint32_t Tick_count_g = 0;
static uint32_t Tick_count_ig = ~0;           // Inverted copy

// ------ Private function prototypes ----------------------------------------

void TIM2_IRQHandler(void);
void SCH_Set_Interrupt_Priority_Levels(void);

/*----------------------------------------------------------------------------*-

  SCH_Init()

  Scheduler initialisation function.  Prepares scheduler data structures and 
   sets up timer interrupts every TICKus microseconds.

  You must call this function before using the scheduler.

  [Required_SystemCoreClock frequency can be found in main.h.]

  PARAMETERS:
     TICKms is the required Tick Interval (milliseconds)

  LONG-TERM DATA:
     SCH_tasks_g (W)

  MCU HARDWARE:
     Timer 2.

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
void SCH_Init_Microseconds(const uint32_t TICKus)
   {
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

   // For use only with the instrumented scheduler
   Scheduler_tick_us_G = TICKus;

   // Clear task array (and inverted copy)
   for (uint32_t Task_id = 0; Task_id < SCH_MAX_TASKS; Task_id++)
      {
      SCH_tasks_g[Task_id].pTask = SCH_NULL_PTR;
      SCH_tasks_ig[Task_id].pTask = (uint32_t (*) (void)) (~((uint32_t) SCH_NULL_PTR));
      SCH_tasks_g[Task_id].Delay = 0;
      SCH_tasks_ig[Task_id].Delay = ~0;
      SCH_tasks_g[Task_id].Period = 0;
      SCH_tasks_ig[Task_id].Period = ~0;
      SCH_tasks_g[Task_id].BCET = 0;
      SCH_tasks_ig[Task_id].BCET = ~0;
      SCH_tasks_g[Task_id].WCET = 0;
      SCH_tasks_ig[Task_id].WCET = ~0;
      }

   // SystemCoreClock gives the system operating freq (Hz)   
   if (SystemCoreClock != REQUIRED_SYSTEM_CORE_CLOCK)
      {
      // Fatal error
      PROCESSOR_Perform_Safe_Shutdown(PFC_SCH_CLOCK_FREQUENCY);
      }

   // Set up Timer 2 as tick source (BEGIN)
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // Enable T2 clock 

#if defined(STM32F401xx)
   // Time base configuration
   // From 84 MHz T2 clock to 1 MHz
   TIM_TimeBaseStructure.TIM_Prescaler = 83 - 1;
#elif defined(STM32F446xx)
   // Time base configuration
   // From 90 MHz T2 clock to 1 MHz
   TIM_TimeBaseStructure.TIM_Prescaler = 90;
#endif
			
   // Set timer overflow value in microseconds
   TIM_TimeBaseStructure.TIM_Period = TICKus - 1;

   TIM_TimeBaseStructure.TIM_ClockDivision = 0;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
      
   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

   // Set up Timer 2 as tick source (END)

   // Set up MoniTTor unit
   MONITTORi_Init();

   // Set up PredicTTor unit
   PREDICTTORi_Init();
            
   // Set up interrupt priorities   
   SCH_Set_Interrupt_Priority_Levels();     

   if (MEASURE_TASK_ET_AND_CPU_LOAD == 1)
      {
      // Prepare for the task execution time measurement
      MEASURE_T4_Init();

      // Prepare to measure scheduler load
      MEASURE_T5_Init();  
      }

   if (MEASURE_TICK_JITTER == 1)
      {
      // Prepare for tick-jitter measurement
      MEASURE_T5_Init();
      }
   }

/*----------------------------------------------------------------------------*-

  SCH_Start()

  Starts the scheduler.

  NOTES: 
  * All tasks must be added before starting scheduler.
  * Any other interrupts MUST be synchronised to this tick.  

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Stored timer register configuration.

  MCU HARDWARE:
     Timer 2.

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
   __disable_irq();  

   // Enable Timer 2
   TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
     
   // Enable interrupt for Timer 2
   TIM_Cmd(TIM2, ENABLE);

   // Store a copy of the TIM2 register configuration
   REG_CONFIG_TIMER_Store(TIM2);

   __enable_irq();  
   }


/*----------------------------------------------------------------------------*-

  TIM2_IRQHandler()

  [Function name determined by CMIS standard.]

  This is the scheduler ISR.  It is called at a rate 
  determined by the timer settings in the SCH_Init() function.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     Timer 2.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void TIM2_IRQHandler(void)
   {
 //  static uint32_t Timing_call_count_s = 0;

   // Instrumented scheduler (begin)
   if (MEASURE_TICK_JITTER == 1)
      {
      uint32_t TimerValue = TIM5->CNT; // Read the current timer value           
      TIM5->CNT = 0;                   // Reset the timer  
      TIM5->CR1 |= TIM_CR1_CEN;        // Start the timer again

      // Update the minimum idle time
      if (TimerValue < Measured_tick_min_G)
         {
         Measured_tick_min_G = TimerValue;
         }

      // Update the maximum idle time
      if (TimerValue > Measured_tick_max_G)
         {
         Measured_tick_max_G = TimerValue;
         }
      }
   // Instrumented scheduler (end)

   // Check the TIM2 register configuration
   REG_CONFIG_TIMER_Check(TIM2);

   // Clear flag
   TIM_ClearITPendingBit(TIM2, TIM_IT_Update);     
   
   // Check pre-conditions (START)

   // Check Tick_count_g
   if ((Tick_count_g != (~Tick_count_ig)))
      {
      // Data have been corrupted: treat as fatal problem
      PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
      }      

   // Check pre-conditions (END)

   // Increment tick count and check against limit
   if (++Tick_count_g > SCH_TICK_COUNT_LIMIT)
      {
      // One or more tasks has taken too long to complete
      // We treat this as a fatal problem
      PROCESSOR_Perform_Safe_Shutdown(PFC_SCH_TICK_COUNT);  
      }

   // Update inverted copy
   Tick_count_ig = ~Tick_count_g;
   } 

/*----------------------------------------------------------------------------*-

  SCH_Dispatch_Tasks()

  This is the 'dispatcher' function.  When a task (function)
  is due to run, SCH_Dispatch_Tasks() will run it.
  This function must be called (repeatedly) from the main loop.

  Triggers move to "idle" mode when all tasks have been released.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     SCH_tasks_g (W)

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
void SCH_Dispatch_Tasks(void) 
   {
   // Used for timing measurements
   static uint32_t Measure_call_count_s = 0;

   uint32_t Status;
   uint32_t TimerValue = 1234;

   __disable_irq();  
   uint32_t Update_required = (Tick_count_g > 0);  // Check Tick_count_g
   __enable_irq();

   // Check pre-conditions (START)

   // We check Tick_count_g integrity in ISR
   // Here we check the scheduler arrays
   for (uint32_t Task_id = 0; Task_id < SCH_MAX_TASKS; Task_id++)
      {
      if ((SCH_tasks_ig[Task_id].pTask  
           != (uint32_t (*) (void)) (~(uint32_t)(SCH_tasks_g[Task_id].pTask))) ||
          (SCH_tasks_ig[Task_id].Delay  != ~(SCH_tasks_g[Task_id].Delay)) ||
          (SCH_tasks_ig[Task_id].Period != ~(SCH_tasks_g[Task_id].Period)) ||
          (SCH_tasks_ig[Task_id].WCET   != ~(SCH_tasks_g[Task_id].WCET)) ||
          (SCH_tasks_ig[Task_id].BCET   != ~(SCH_tasks_g[Task_id].BCET)))
         {  
         // We view a mismatch as a fatal run-time error
         PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
         }
      }
   
   // Reset at start of each tick
   uint32_t Sum_of_wcets_tick = 0;

   // Check pre-conditions (END)
 
   while (Update_required)
      {
      // Used to support scheduler instrumentation
      Measure_call_count_s++;

      // Go through the task array
      for (uint32_t Task_id = 0; Task_id < SCH_MAX_TASKS; Task_id++)
         {
         // Check if there is a task at this location
         if (SCH_tasks_g[Task_id].pTask != SCH_NULL_PTR)
            {
            if (--SCH_tasks_g[Task_id].Delay == 0)
               {
               // First check with PredicTTor
               // (avoid running the wrong task)
               PREDICTTORi_Check_Task_Sequence(Task_id);   
   
               // Start the internal MoniTTor
               MONITTORi_Start(Task_id,
                              SCH_tasks_g[Task_id].WCET,
                              SCH_tasks_g[Task_id].BCET,
                              5);  // 5 usec leeway

               // Instrumented scheduler
               if (MEASURE_TASK_ET_AND_CPU_LOAD == 1)
                  {
                  MEASURE_T4_Start_T4();
                  Status = (*SCH_tasks_g[Task_id].pTask)();  // Run the task
                  Sum_of_wcets_tick += MEASURE_T4_Update_Task_Timing(Task_id, MEASURE_T4_Read_T4());
                  }
               else
                  {
                  Status = (*SCH_tasks_g[Task_id].pTask)();  // Run the task
                  }

               // Stop the MoniTTor: 
               // Triggers a mode change in the event of a BCET fault.
               MONITTORi_Stop();

               if (Status == RETURN_ABNORMAL_STATE)
                  {
                  // Fatal run-time error
                  PROCESSOR_Perform_Safe_Shutdown(PFC_TASK_RETURN);
                  }

               // All tasks are periodic: schedule task to run again
               SCH_tasks_g[Task_id].Delay = SCH_tasks_g[Task_id].Period;
               }

            // We need to update the Delay value only (other values are not changed)
            SCH_tasks_ig[Task_id].Delay = ~(SCH_tasks_g[Task_id].Delay);
            }         
         } 

      __disable_irq();
      Tick_count_g--;                       // Decrement the count
      Update_required = (Tick_count_g > 0); // Check again
      __enable_irq();
      }

   // Update inverted copy of Tick_count_g
   Tick_count_ig = ~Tick_count_g;

   // --------------------------

   // Calculate the 'high water mark' for WCET in a tick
   if (Sum_of_wcets_tick > Sum_of_wcets_max_G)
      {
      Sum_of_wcets_max_G = Sum_of_wcets_tick;
      }

   // --------------------------

   // We report data in 'idle time' (not in a task)
   // This avoids having to alter PredicTTor sequences, etc.
   
   // When reporting, we don't record the idle time 
   // (because the reporting process will interfere with these data)

   if (Measure_call_count_s == MEASURE_REPORTING_INTERVAL)
      {
      if (MEASURE_TASK_ET_AND_CPU_LOAD == 1)
         {
         MEASURE_T4_Write_Data_To_UART_Buffer();
         }

      if (MEASURE_TICK_JITTER == 1)
         {
         MEASURE_T5_Write_Data_To_UART_Buffer();
         }

      Measure_call_count_s = 0;

      // The scheduler enters idle mode at this point 
      __WFI();     
      }
   else
      {
      // Instrumented scheduler
      if (MEASURE_TASK_ET_AND_CPU_LOAD == 1)
         {
         // Measuring WC / BC idle time too
         MEASURE_T4_Start_T4();

         // The scheduler enters idle mode at this point 
         __WFI();     

         TimerValue = MEASURE_T4_Read_T4();

         // Update the minimum idle time
         if (TimerValue < Idle_time_min_G)
            {
            Idle_time_min_G = TimerValue;
            }

         // Update the maximum idle time
         if (TimerValue > Idle_time_max_G)
            {
            Idle_time_max_G = TimerValue;
            }
         }
      else
         {
         // No measurements
         // The scheduler enters idle mode at this point 
         __WFI();     
         }
      }
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
void SCH_Add_Task(uint32_t (* pTask)(),
                  const uint32_t DELAY,
                  const uint32_t PERIOD,
                  const uint32_t WCET,
                  const uint32_t BCET)
   {
   uint32_t Task_id = 0;

   // Check pre-conditions (START)

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
      PROCESSOR_Perform_Safe_Shutdown(PFC_SCH_ADD_TASK);
      }
      
   // Check for sensible BCET / WCET values
   if (BCET > WCET)
      {
      // Doesn't make sense ...
      PROCESSOR_Perform_Safe_Shutdown(PFC_SCH_ADD_TASK);
      }   
      
   // Check for "one shot" tasks
   if (PERIOD == 0)
      {
      // We do not allow "one shot" tasks (all tasks must be periodic)
      PROCESSOR_Perform_Safe_Shutdown(PFC_SCH_ADD_TASK);
      }

   // Check pre-conditions (END)

   // If we're here, there is a space in the task array
   // and the task to be added is periodic
   SCH_tasks_g[Task_id].pTask  = pTask;

   SCH_tasks_g[Task_id].Delay  = DELAY + 1;
   SCH_tasks_g[Task_id].Period = PERIOD;
   SCH_tasks_g[Task_id].WCET   = WCET;
   SCH_tasks_g[Task_id].BCET   = BCET;

   // Store duplicated (inverted) copy of scheduler data
   SCH_tasks_ig[Task_id].pTask
       = (uint32_t (*) (void)) (~(uint32_t)(SCH_tasks_g[Task_id].pTask));
  
   SCH_tasks_ig[Task_id].Delay  = ~(SCH_tasks_g[Task_id].Delay);
   SCH_tasks_ig[Task_id].Period = ~(SCH_tasks_g[Task_id].Period);
   SCH_tasks_ig[Task_id].WCET   = ~(SCH_tasks_g[Task_id].WCET);
   SCH_tasks_ig[Task_id].BCET   = ~(SCH_tasks_g[Task_id].BCET);
   }

/*----------------------------------------------------------------------------*-

  SCH_Set_Interrupt_Priority_Levels()
   
  Set interrupt priorities.

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
void SCH_Set_Interrupt_Priority_Levels(void)
   {
   NVIC_InitTypeDef NVIC_InitStructure;

   // 4 bits for pre-emp priority; 0 bits for sub-priority
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);    
  
   // Timer 2 (used here to drive the scheduler)
   NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;               
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   // SysTick (used for MoniTTor)
   NVIC_SetPriority(SysTick_IRQn, 0);
   }   
    
/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
