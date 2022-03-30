/*----------------------------------------------------------------------------*-

  ttrd2-a07a-t0401a-v001a_measure_t4.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
  Task (execution time) profiler.
  
  Measures and reports WCET and BCET for all tasks in the system.

  Also reports max scheduler load (as percentage of CPU tick).

  Assumes availability of UART-based reporting facility on target
  (adapt code if such a facility is not available).
  
  PLEASE NOTE:
  This module is expected to be used (with care) during system development:
  it is not generally expected that this module will be included in the
  finished system.

  STM32F401RE.
  
  Uses T4 (16-bit timer).  
  
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

extern uint32_t Idle_time_max_G;
extern uint32_t Idle_time_min_G;

extern uint32_t Sum_of_wcets_max_G;

extern uint32_t Scheduler_tick_us_G;

// --- Private variable Definitions ('File Global') -------------------------

// This array is used to support the instrumented scheduler
// The array contents are not duplicated or otherwise checked or protected.
// *** THIS FACILITY IS INTENDED FOR USE DURING DEVELOPMENT ONLY ***

// The two dimensional array stores the best-case and worse-case execution
// times for all tasks in TTC.  Also records max idle time.
static uint32_t SCH_timing_data_g[SCH_MAX_TASKS][2];

/*----------------------------------------------------------------------------*-
   
 MEASURE_T4_Init()

 Prepare to measure task execution times. 

 Initialises the 2-D array with min and max values.

 Sets up Timer 0.

-*----------------------------------------------------------------------------*/

void MEASURE_T4_Init(void)
   {
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

   for (int i = 0; i < SCH_MAX_TASKS; i++)
      {
      SCH_timing_data_g[i][0] = 9999;
      SCH_timing_data_g[i][1] = 0;
      }

   Idle_time_max_G = 0;
   Idle_time_min_G = 9999;
      
   // Enable T4 clock 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 

   TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

   // Time base configuration
   // From 84 MHz T4 clock to 1 MHz
   //TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;
   TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;

   // Set maximum timer overflow value in microseconds: 65000 = 65 ms.  
   TIM_TimeBaseStructure.TIM_Period = 65000 - 1;

   //TIM_TimeBaseStructure.TIM_ClockDivision = 0;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
   TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
         
   TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
   }

/*----------------------------------------------------------------------------*-

 MEASURE_T4_Start_T4()

 This function starts Timer 0.

-*----------------------------------------------------------------------------*/
void MEASURE_T4_Start_T4(void)
   {
   // Stop the timer
   TIM_Cmd(TIM4, DISABLE);    

   // Reset the timer
   TIM_SetCounter(TIM4, 0);
   
   // Start the timer
   TIM_Cmd(TIM4, ENABLE);    
   }

/*----------------------------------------------------------------------------*-

 MEASURE_T4_Read_Timer0()

 Returns the current value of Timer 0.

-*----------------------------------------------------------------------------*/
uint32_t MEASURE_T4_Read_T4(void)
   {
   // Returns elapsed time in microseconds
   return TIM_GetCounter(TIM4);
   }

/*----------------------------------------------------------------------------*-

 MEASURE_T4_Update_Task_Timing()

 Updates the stored max and min timing values.

-*----------------------------------------------------------------------------*/
uint32_t MEASURE_T4_Update_Task_Timing(uint32_t Index, uint32_t Timer_value)
   {
   // Check the best-case execution task time
   if (Timer_value < SCH_timing_data_g[Index][0])
      {
      SCH_timing_data_g[Index][0] = Timer_value;
      }

   // Check the worse-case execution task time
   if (Timer_value > SCH_timing_data_g[Index][1])
      {
      SCH_timing_data_g[Index][1] = Timer_value;
      }

   // Return the available WCET value for this task
   return SCH_timing_data_g[Index][1];
   }

/*----------------------------------------------------------------------------*-
   
  MEASURE_T4_Write_Data_To_UART_Buffer()

  Converts the binary values of best-case and worse-case execution times
  of all the tasks in TTC into their corresponding ASCII characters which
   are copied to the character buffer.
  (The contents of the buffer are then passed over the serial link)

-*----------------------------------------------------------------------------*/
void MEASURE_T4_Write_Data_To_UART_Buffer(void)
   {
   uint32_t Total_task_wcet = 0;

   UART2_BUF_O_Write_String_To_Buffer(
   "\n\nTask ID    WCET (us)   BCET (us)\n");
   
   for (uint32_t Task_id = 0; Task_id <= SCH_MAX_TASKS; Task_id++)
      {
      // Deal with tasks
      if (Task_id < SCH_MAX_TASKS)
         {
         // Don't display tasks with WCET = 0 (assume no task information)
         if (SCH_timing_data_g[Task_id][1] != 0)
            {
            // *Assumes* up to 999 tasks ...
            UART2_BUF_O_Write_Number03_To_Buffer(Task_id);
            UART2_BUF_O_Write_String_To_Buffer("        ");

            UART2_BUF_O_Write_Number04_To_Buffer(SCH_timing_data_g[Task_id][1]);
            UART2_BUF_O_Write_String_To_Buffer("        ");
            UART2_BUF_O_Write_Number04_To_Buffer(SCH_timing_data_g[Task_id][0]);
            UART2_BUF_O_Write_String_To_Buffer("\n");

            Total_task_wcet += SCH_timing_data_g[Task_id][1];
            }
         }

      // Now deal with idle time and sch load
      if (Task_id == SCH_MAX_TASKS)
         {
         // We assume all tasks have ET < tick interval (otherwise this will be detected)
         UART2_BUF_O_Write_String_To_Buffer("\n");

         // Calculate maximum CPU load
         uint32_t Max_cpu_load = ((Scheduler_tick_us_G - Idle_time_min_G) * 100) / Scheduler_tick_us_G;         

         UART2_BUF_O_Write_String_To_Buffer("Scheduler_tick_us_G : ");
         UART2_BUF_O_Write_Number10_To_Buffer(Scheduler_tick_us_G);
         UART2_BUF_O_Write_String_To_Buffer("\n");

         UART2_BUF_O_Write_String_To_Buffer("Idle_time_min_G     : ");
         UART2_BUF_O_Write_Number10_To_Buffer(Idle_time_min_G);
         UART2_BUF_O_Write_String_To_Buffer("\n");

         UART2_BUF_O_Write_String_To_Buffer("Total task WCET     : ");
         UART2_BUF_O_Write_Number10_To_Buffer(Sum_of_wcets_max_G);
         UART2_BUF_O_Write_String_To_Buffer("\n\n");

         UART2_BUF_O_Write_String_To_Buffer("Max task load (Percent of tick) : ");
         uint32_t Max_task_load = (Sum_of_wcets_max_G * 100) / Scheduler_tick_us_G;       
         UART2_BUF_O_Write_Number04_To_Buffer(Max_task_load);
         UART2_BUF_O_Write_String_To_Buffer("\n");

         UART2_BUF_O_Write_String_To_Buffer("Max sch load  (Percent of tick) : ");
         UART2_BUF_O_Write_Number04_To_Buffer((Max_cpu_load - Max_task_load));
         UART2_BUF_O_Write_String_To_Buffer("\n\n");

         UART2_BUF_O_Write_String_To_Buffer("Max CPU load  (Percent of tick) : ");
         UART2_BUF_O_Write_Number04_To_Buffer(Max_cpu_load);
         UART2_BUF_O_Write_String_To_Buffer("\n\n---\n");


         if (RESTART_DATA_MEASUREMENT_AFTER_REPORTING == 1)
            {
            // Start the measurements from scratch
            for (int i = 0; i < SCH_MAX_TASKS; i++)
               {
               SCH_timing_data_g[i][0] = 9999;
               SCH_timing_data_g[i][1] = 0;
               }

            Idle_time_max_G = 0;
            Idle_time_min_G = 999999;
            }
         }
      }
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
