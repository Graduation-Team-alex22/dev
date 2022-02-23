/*----------------------------------------------------------------------------*-

  ttrd2-19a-t0401a-v001b_processor_task.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
  Processor Task.

  Perform basic checks on MCU operating temperature.

  Optionally, the system will perform periodic resets.

  Please see the following #DEFINEs (below):

  #define DEMO_PERIODIC_PROCESSOR_RESETS (1)
  #define DEMO_RESET_PERIOD (120)

  If periodic resets are performed, the system stores / restores a simple
  representation of the overall system state (reflected in a counter value) 
  between resets.

  Please refer to 'ERES2' (Chapter 19) for further information.

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

#include "../main/main.h"

// ADC library is used to measure and report CPU temperature
#include "../tasks/ttrd2-05a-t0401a-v001a_adc_task.h"

// Support for UART / USB reporting on Nucleo board
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"

// ------ Private constants --------------------------------------------------

// Used to demostrate the potential for periodic processor resets
// with associated POSTs.  May be employed in very 'hostile' environments
// (e.g. high EMI, high radiation).
// Set to 0 to remove this option
#define DEMO_PERIODIC_PROCESSOR_RESETS (1)

// Sets the interval between periodic resets (if used)
// Assumes processor task is released once per second
#define DEMO_RESET_PERIOD (120)

// ------ Private Duplicated Variables ('File Global') -----------------------

static uint32_t Call_count_g = 0;
static uint32_t Call_count_ig = ~0;

// ------ Private function prototypes (declarations) -------------------------

static void     PROCESSOR_TASK_Store_State(const uint32_t STATE);
static uint32_t PROCESSOR_TASK_Retrieve_State(void);

/*----------------------------------------------------------------------------*-

  PROCESSOR_TASK_Init()

  Prepare for Processor Task - see below.
  
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
void PROCESSOR_TASK_Init(void)
   {
   // Get the stored state (if available)
   Call_count_g  = PROCESSOR_TASK_Retrieve_State();

   // Set up inverted copy
   Call_count_ig = ~Call_count_g;
   }

/*----------------------------------------------------------------------------*-

  PROCESSOR_TASK_Update()

  Must schedule every second.
   
  PARAMETERS:
     None.

  LONG-TERM DATA:
     Call_count_g (W)
     Call_count_ig (W)

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     Data-integrity check on Call_count_g.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     The Processor temperature is monitored.
     If the temperature is close to the allowed upper or lower limit
     the Processor is moved into a Fail-Safe state.

  WCET:
     Not yet determined.

  BCET:
     Not yet determined.

  RETURN VALUE:
     RETURN_NORMAL_STATE or RETURN_ABNORMAL_STATE.
 
-*----------------------------------------------------------------------------*/
uint32_t PROCESSOR_TASK_Update(void)
   {
   uint32_t Return_value = RETURN_NORMAL_STATE;
   int32_t Temp_c;

   // Check pre-conditions (START)

   // Check data integrity 
   if (Call_count_g != ~Call_count_ig)
      {
      Return_value = RETURN_ABNORMAL_STATE;
      }

   // Check pre-conditions (END)

   // We perform the core task activities if we meet the pre-conditions
   if (Return_value == RETURN_NORMAL_STATE)
      {
      // Increment call count
      Call_count_g++;
      Call_count_ig = ~Call_count_g;

      // Get the CPU temperature 
      Temp_c = ADC1_Get_Temperature_Celsius();   

      // MCU operating temperature range is –40 to +105 °C
      // We ensure that we are well within this range
      if ((Temp_c < (-20)) || (Temp_c > 85))
         {
         // Temperature out of range: cannot operate safely
         // We treat this as a Fatal Platform Failure
         PROCESSOR_Perform_Safe_Shutdown(PFC_PROCESSOR_TEMPERATURE);
         }

      if ((Call_count_g % 5) == 0)
         {
         // Simple temperature reporting (for demo purposes)
         // Temperature assumed to be > 0 on the testbench ...
         UART2_BUF_O_Write_String_To_Buffer("CPU temp: ");
         UART2_BUF_O_Write_Number03_To_Buffer(Temp_c);
         UART2_BUF_O_Write_String_To_Buffer("\n");
         }

      if (DEMO_PERIODIC_PROCESSOR_RESETS == 1)
         {
         // Report call count periodically
         if ((Call_count_g % 5) == 0)
            {
            UART2_BUF_O_Write_Number03_To_Buffer(Call_count_g % 1000);
            UART2_BUF_O_Write_String_To_Buffer("\n");
            } 
         
         // Trigger reset at required interval
         if ((Call_count_g % DEMO_RESET_PERIOD) == 0)
            {
            // Reset processor and perform appropriate POSTs
            // (assume lower-level tests are continuous)
            PROCESSOR_Store_Reqd_MoSt(STARTUP_03_ENVIRONMENT_CHECKS_S);

            // Store 'Processor State' 
            PROCESSOR_TASK_Store_State(Call_count_g);

            // No fault (planned mode change)
            PROCESSOR_Store_PFC(PFC_NO_FAULT);

            // Change mode
            PROCESSOR_Change_MoSt();   
            }
         }
      }

   return Return_value;
   }

/*----------------------------------------------------------------------------*-

  PROCESSOR_TASK_Store_State()

  Store the current state of the Processor Task in a backup register.

  Stored as a Duplicated Variable.

  PARAMETERS:
     The current state.

  LONG-TERM DATA:
     The data in the backup registers.
       
  MCU HARDWARE:
     Two 32-bit backup registers.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
static void PROCESSOR_TASK_Store_State(const uint32_t STATE)
   {
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
   PWR_BackupAccessCmd(ENABLE);

   RTC->BKP4R =  (uint32_t)STATE;
   RTC->BKP5R = ~(uint32_t)STATE;
  
   PWR_BackupAccessCmd(DISABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, DISABLE);
   }

/*----------------------------------------------------------------------------*-

  PROCESSOR_TASK_Retrieve_State()

  Retrieve the stored state of the Processor Task from the backup registers.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     The data in the backup registers.
       
  MCU HARDWARE:
     Two 32-bit backup registers.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     The data are stored in the form of a Duplicated Variable.
     If the copies don't match, 0 is returned.

  RETURN VALUE:
     The stored Mode / State (or 0).

-*----------------------------------------------------------------------------*/
static uint32_t PROCESSOR_TASK_Retrieve_State(void)
   {
   uint32_t return_value;

      if (RTC->BKP4R != ~(RTC->BKP5R))
         {
         // Backup register content has been corrupted, 
         // or no state has been stored previously.
         // In this demo, we assume that this is the first run.
         return_value = 0;         
         }
      else
         {
         // Backup register content seems to be correct
         return_value = (uint32_t) RTC->BKP4R;
         }    

   return return_value;
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
