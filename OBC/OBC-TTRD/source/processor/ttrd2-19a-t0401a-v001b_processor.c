/*----------------------------------------------------------------------------*-

   ttrd2-19a-t0401a-v001b_processor.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------

   Configures Processor in one of the following Modes / States after a reset:

      FAIL_SAFE_S
      STARTUP_01_PROCESSOR_MEMORY_CHECKS_S
      STARTUP_02_SOFTWARE_CONFIGURATION_CHECKS_S
      STARTUP_03_ENVIRONMENT_CHECKS_S
      STARTUP_04_WDT_CHECK_S
      STARTUP_05a_SCHEDULER_OSC_CHECK_M
      STARTUP_05b_SCHEDULER_OSC_CHECK_M
      STARTUP_06a_MONITTOR_ORUN_CHECK_M
      STARTUP_06b_MONITTOR_URUN_CHECK_M
      STARTUP_07_PREDICTTOR_CHECK_M
      NORMAL_M

   Provides support for Mode / State changes.

   Provides support for storage, retrival and reporting of PFCs.

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

// Support for run-time tests
#include "ttrd2-19a-t0401a-v001b_run_time_tests.h"

// Support for reporting PFCs
#include "ttrd2-04a-t0401a-v001a_pfc_reporting.h"

// Tasks
#include "../tasks/ttrd2-05a-t0401a-v001a_adc_task.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_heartbeat_sw_task.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_iwdt_task.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_switch_task.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#include "../tasks/ttrd2-19a-t0401a-v001b_processor_task.h"
#include "../tasks/HELLOWORLD_TRANSMIT_task.h"


// MoniTTor header 
// This module has access to the MoniTTor: use with care!
#include "../scheduler/ttrd2-18a-t0401a-v001a_monittor_i.h"

// ------ Private Duplicated Variables ('File Global') -----------------------

// The current system Mode / State
static eProc_MoSt Processor_MoSt_g;
static uint32_t   Processor_MoSt_ig;

// ------ Private function prototypes (declarations) -------------------------

static void       PROCESSOR_Identify_Reqd_MoSt(void);
static void       PROCESSOR_Configure_Reqd_MoSt(void);

static void       PROCESSOR_Set_MoSt(const eProc_MoSt MOST);

static eProc_MoSt PROCESSOR_Retrieve_Reqd_MoSt(void);

static uint32_t   PROCESSOR_Retrieve_PFC(void);

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

  Identify the cause of the system reset and set the system mode accordingly.

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
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void PROCESSOR_Identify_Reqd_MoSt(void)
   {
   uint32_t Fault_code;

   if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST))
      {
      // WDT reset.  Could be the result of WDT test.
      // => we need to check the mode and fault codes
      Fault_code = PROCESSOR_Retrieve_PFC();

      if (Fault_code == PFC_NO_FAULT)
         {
         // WDT triggered during testing => retrieve required mode 
         PROCESSOR_Set_MoSt(PROCESSOR_Retrieve_Reqd_MoSt());      
         }
      else
         {
         // Not a test => Fail Safe
         PROCESSOR_Set_MoSt(FAIL_SAFE_S);
         }
      }
   else if (RCC_GetFlagStatus(RCC_FLAG_SFTRST))
      {
      // Software reset (incl. debugger)
      // Under normal operation this will be used for planned mode changes
      // => we need to check the required mode 
      PROCESSOR_Set_MoSt(PROCESSOR_Retrieve_Reqd_MoSt());
      }
   else if (RCC_GetFlagStatus(RCC_FLAG_PORRST))
      {
      // Power-On Reset
      // Start by performing self tests
      PROCESSOR_Set_MoSt(STARTUP_01_PROCESSOR_MEMORY_CHECKS_S);
      }
   else if (RCC_GetFlagStatus(RCC_FLAG_PINRST))
      {
      // 'Reset button' pressed on Nucleo board
      // Here we treat this is the same way as a POR
      PROCESSOR_Set_MoSt(STARTUP_01_PROCESSOR_MEMORY_CHECKS_S);
      }
   else
      {
      // Other possible reset causes
      // Low-power management reset / Windowed WDT / Brown-out
      // In all cases we enter fail-safe state
      PROCESSOR_Set_MoSt(FAIL_SAFE_S);
      }   
  
   // Clear all reset flags
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
   int32_t Temp_c;  // CPU temperature

   // Prepare UART-USB interface
   UART2_BUF_O_Init(230400);
   UART2_BUF_O_Write_String_To_Buffer("\nStarting up\n");

   // Check data integrity (Processor_MoSt_g)
   if (Processor_MoSt_ig != ~((uint32_t) Processor_MoSt_g))
      {
      // Data-integrity problem
      Processor_MoSt_g = FAIL_SAFE_S;
      Processor_MoSt_ig = ~((uint32_t) FAIL_SAFE_S);
      }

   switch (Processor_MoSt_g)
      {
      default:          // Default to "FAIL_SAFE_S"
      case FAIL_SAFE_S:
         {
         UART2_BUF_O_Write_String_To_Buffer("\nFAIL_SAFE_S\n");
         UART2_BUF_O_Send_All_Data();

         // Trigger "fail safe" behaviour
         PROCESSOR_Perform_Safe_Shutdown(PROCESSOR_Retrieve_PFC());

         break;
         }

      case STARTUP_01_PROCESSOR_MEMORY_CHECKS_S:
         {
         // Report Mode / State (for demo purposes only)
         UART2_BUF_O_Write_String_To_Buffer("STARTUP_01_PROCESSOR_MEMORY_CHECKS_S\n");
         UART2_BUF_O_Send_All_Data();

         // PLACEHOLDER: 

         // Here we will add calls to third-party library for checks of
         // 1. CPU registers
         // 2. RAM

         // See ERES2, Chapter 13

         // Here we assume that the tests are passed

         // Now move on to next startup test
         PROCESSOR_Store_Reqd_MoSt(STARTUP_02_SOFTWARE_CONFIGURATION_CHECKS_S);
         PROCESSOR_Store_PFC(PFC_NO_FAULT);
         PROCESSOR_Change_MoSt();

         break;
         }

      case STARTUP_02_SOFTWARE_CONFIGURATION_CHECKS_S:
         {
         // Report Mode / State (for demo purposes only)
         UART2_BUF_O_Write_String_To_Buffer("STARTUP_02_SOFTWARE_CONFIGURATION_CHECKS_S\n");
         UART2_BUF_O_Send_All_Data();

         // PLACEHOLDER: 

         // Here we will check the software configuration (matched to 'Golden Signature')

         // See ERES2, Chapter 14

         // Here we assume that the tests are passed

         PROCESSOR_Store_Reqd_MoSt(STARTUP_03_ENVIRONMENT_CHECKS_S);
         PROCESSOR_Store_PFC(PFC_NO_FAULT);
         PROCESSOR_Change_MoSt();

         break;
         }

      case STARTUP_03_ENVIRONMENT_CHECKS_S:
         {
         // Report Mode / State (for demo purposes only)
         UART2_BUF_O_Write_String_To_Buffer("STARTUP_03_ENVIRONMENT_CHECKS_S\n");
         UART2_BUF_O_Send_All_Data();

         // Here we check only the CPU temperature 
         // via the sensor on the STM32F401, which is linked to ADC1
         ADC1_Init();

         // Take ADC reading (direct Task call)
         if (RETURN_NORMAL_STATE != ADC1_Update())
            {
            // Treated as Fatal Platform Failure 
            PROCESSOR_Perform_Safe_Shutdown(PFC_PROCESSOR_TEMPERATURE_STARTUP);
            }

         // Get the CPU temperature (available after above ADC task call)
         Temp_c = ADC1_Get_Temperature_Celsius();   

         // MCU operating temperature range is –40 to +105 °C
         // We ensure that we are well within this range
         if ((Temp_c < (-20)) || (Temp_c > 85))
            {
            // Temperature out of range: cannot operate safely
            PROCESSOR_Perform_Safe_Shutdown(PFC_PROCESSOR_TEMPERATURE_STARTUP);
            }

         // You may also wish to check supply voltages, other relevant inputs, etc.

         // See ERES2, Chapter 13
         // See DuplicaTTor Evaluation Board examples for more comprehensive examples

         // Move on (if we get this far) ...
         PROCESSOR_Store_Reqd_MoSt(STARTUP_04_WDT_CHECK_S);
         PROCESSOR_Store_PFC(PFC_NO_FAULT);
         PROCESSOR_Change_MoSt();

         break;
         }

      case STARTUP_04_WDT_CHECK_S:
         {
         // Report Mode / State (for demo purposes only)
         UART2_BUF_O_Write_String_To_Buffer("STARTUP_04_WDT_CHECK_S\n");
         UART2_BUF_O_Send_All_Data();

         // See ERES2, Chapter 16

         // NOTE:
         // We are using the iWDT unit.
         // Prepare for next mode
         // (we enter this mode *IF* we pass the test).

         PROCESSOR_Store_Reqd_MoSt(STARTUP_05a_SCHEDULER_OSC_CHECK_M);
         PROCESSOR_Store_PFC(PFC_NO_FAULT);

         WATCHDOG_Init(8);  // 8 x 125 µs => 1 ms 
         WATCHDOG_Update();

         // WDT should trigger during this function call
         RTT_Startup_04_WDT_Check();

         // We shouldn't get this far ...
         WATCHDOG_Update();
         PROCESSOR_Perform_Safe_Shutdown(PFC_WDT_TEST);
         break;
         }

      case STARTUP_05a_SCHEDULER_OSC_CHECK_M:
         {
         // Report Mode / State (for demo purposes only)
         UART2_BUF_O_Write_String_To_Buffer("STARTUP_05a_SCHEDULER_OSC_CHECK_M\n");
         UART2_BUF_O_Send_All_Data();

         // Having confirmed that the iWDT is operational,
         // we now use this component to test the scheduler operation
         // (and the MCU oscillator frequency, using the iWDT osc as a benchmark)

         // The test is in two stages (STARTUP_05a ..., STARTUP_05b ...)

         // In this test, we should refresh the iWDT correctly: 
         // if not, we will try to enter a Fail-Safe state

         // iWDT timeout    = 2 ms
         // WDT task period = 1 ms

         // Note that the accuracy of the iWDT clock source is limited and we can't 
         // perform oscillator checks much more precisely than this.

         // See ERES2, Chapter 16
         
         PROCESSOR_Store_Reqd_MoSt(FAIL_SAFE_S);
         PROCESSOR_Store_PFC(PFC_SCH_STARTUP_TEST_5A);
         
         WATCHDOG_Init(16);  // 16 x 125 µs => 2 ms 
         WATCHDOG_Update();

         // Set up the scheduler (1 ms Ticks)
         SCH_Init_Microseconds(1000);

         // Add task to refresh WDT every 1 ms
         SCH_Add_Task(RTT_05a_Scheduler_Osc_Test_Update, 0, 1, 100, 0);

         // Scheduler is started in main()

         break;
         }

      case STARTUP_05b_SCHEDULER_OSC_CHECK_M:
         {
         UART2_BUF_O_Write_String_To_Buffer("STARTUP_05b_SCHEDULER_OSC_CHECK_M\n");
         UART2_BUF_O_Send_All_Data();

         // This is the second part of our scheduler / oscillator check

         // In this test, we should *not* refresh the iWDT correctly: 
         // if not, we will try to enter a Fail-Safe state

         // iWDT timeout    = 2 ms
         // WDT task period = 4 ms

         // See ERES2, Chapter 16

         PROCESSOR_Store_Reqd_MoSt(STARTUP_06a_MONITTOR_ORUN_CHECK_M);
         PROCESSOR_Store_PFC(PFC_NO_FAULT);
         
         WATCHDOG_Init(16);  // Param x 125 µs => 2 ms 
         WATCHDOG_Update();

         // Set up the scheduler
         SCH_Init_Microseconds(1000);

         // Add task to refresh WDT every 4 ms
         SCH_Add_Task(RTT_05b_Scheduler_Osc_Test_Update, 0, 4, 100, 0);

         // Scheduler is started in main()

         break;
         }

      case STARTUP_06a_MONITTOR_ORUN_CHECK_M:
         {
         UART2_BUF_O_Write_String_To_Buffer("STARTUP_06a_MONITTOR_ORUN_CHECK_M\n");
         UART2_BUF_O_Send_All_Data();

         // See ERES2, Chapter 18
         
         // Set up the scheduler
         SCH_Init_Microseconds(1000);

         // Set WCET for test task to ~0 usec
         SCH_Add_Task(RTT_06a_MoniTTor_Overrun_Test_Update, 0, 1, 5, 0);

         // Task should trigger MoniTTor ISR during first call
         // => test has been passed and we move to next mode
         // If MoniTTor is not triggered, task will run again
         // => test failed and we try to fail safely.

         break;
         }

      case STARTUP_06b_MONITTOR_URUN_CHECK_M:
         {
         UART2_BUF_O_Write_String_To_Buffer("STARTUP_06b_MONITTOR_URUN_CHECK_M\n");
         UART2_BUF_O_Send_All_Data();

         // See ERES2, Chapter 18
         
         // Set up the scheduler 
         SCH_Init_Microseconds(1000); 

         // Set BCET for test task to 4000 usec
         // This should be detected by the MoniTTor unit
         // Note: BCET cannot be > WCET!
         SCH_Add_Task(RTT_06b_MoniTTor_Underrun_Test_Update, 0, 1, 4500, 4000);

         break;
         }

      case STARTUP_07_PREDICTTOR_CHECK_M:
         {
         UART2_BUF_O_Write_String_To_Buffer("STARTUP_07_PREDICTTOR_CHECK_M\n");
         UART2_BUF_O_Send_All_Data();

         // See ERES2, Chapter 19
         
         // Set up the scheduler (short TI) 
         SCH_Init_Microseconds(400); 

         // Execute task set with 'incorrect' sequence
         // PredicTTor should detect this: if not, this task will shut the system down.
         SCH_Add_Task(RTT_07_PredicTTor_Test_Update, 0, 1, 1000, 0);

         break;
         }

      case NORMAL_M:
         {
         // Store default state and fault code
         PROCESSOR_Store_Reqd_MoSt(FAIL_SAFE_S);
         PROCESSOR_Store_PFC(PFC_UNSPECIFIED_FAULT);

         // Report Mode / State (for demo purposes only)
         UART2_BUF_O_Write_String_To_Buffer("NORMAL_M\n");

         // Set up scheduler for 5 ms ticks
         SCH_Init_Microseconds(5000);

         // Set up WDT 
         // Timeout is parameter * 125 µs: param 80 => ~10 ms
         // NOTE: WDT driven by RC oscillator - timing varies with temperature            
         WATCHDOG_Init(80);

         // Prepare for heartbeat task
         HEARTBEAT_SW_Init();

         // Prepare for the switch-reading task
         SWITCH_BUTTON1_Init(); 
             
         // Prepare for ADC task
         // Includes ADC start-up checks 
         ADC1_Init();
         
         // Prepare for the Processor Task
         // Controls Mode / State changes
         // Checks CPU temperature
         PROCESSOR_TASK_Init();
				 
				 HELLOWORLD_TRANSMIT_Init();

         // Store the GPIO register configuration
         // Check in the Heartbeat task 
         // - and any tasks that perform safety-related IO
         REG_CONFIG_CHECKS_GPIO_Store();

         // Add tasks to schedule.
         // Parameters are:
         // A. Task name
         // B. Initial delay / offset (in Ticks)
         // C. Task period (in Ticks): Must be > 0
         // D. WCET (microseconds)
         // E. BCET (microseconds)
         //
         // Note: WCET / BCET data obtained using TTRD2-19b
         //
         //           A                      B   C    D     E
         SCH_Add_Task(WATCHDOG_Update,       0,  1,   1,  0);    // iWDT
         SCH_Add_Task(HEARTBEAT_SW_Update,   0,  200, 7,  6);    // Heartbeat
         SCH_Add_Task(SWITCH_BUTTON1_Update, 10, 2,   8,  7);    // Switch
         SCH_Add_Task(ADC1_Update,           0,  100, 36, 35);   // ADC1
         SCH_Add_Task(PROCESSOR_TASK_Update, 0,  200, 17, 2);    // Proc task   
         SCH_Add_Task(UART2_BUF_O_Update,    0,  1,   212, 104); // UART2
         SCH_Add_Task(HELLOWORLD_TRANSMIT_Update,    10,  50,   5000, 2); // UART2_HelloWorld
				 
				 
         // Feed the watchdog
         WATCHDOG_Update();

         break;
         }
      }
   }

/*----------------------------------------------------------------------------*-

  PROCESSOR_Set_MoSt()

  Set the value of 'Processor_MoSt_g' (Duplicated Variable). 
   
  PARAMETERS:
     MOST : Value to be assigned.

  LONG-TERM DATA:
     Processor_MoSt_g (W)
     Processor_MoSt_ig (W)
   
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
static void PROCESSOR_Set_MoSt(const eProc_MoSt MOST)
   {
   Processor_MoSt_g = MOST;
   Processor_MoSt_ig = ~((uint32_t)MOST);
   }

/*----------------------------------------------------------------------------*-

  PROCESSOR_Get_MoSt()

  Returns the value of 'Processor_MoSt_g' (Duplicated Variable). 
   
  PARAMETERS:
     None.

  LONG-TERM DATA:
     Processor_MoSt_g (R)
     Processor_MoSt_ig (R)
   
  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     Checks for data corruption (=> shutdown)

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     The value of 'Processor_MoSt_g' (Duplicated Variable). 

-*----------------------------------------------------------------------------*/
eProc_MoSt PROCESSOR_Get_MoSt(void)
   {
   // Check data integrity (file-global variable)
   // Shut down if corruption is detected.
   // Other design solutions may be more appropriate in your system.
   if (Processor_MoSt_ig != ~((uint32_t) Processor_MoSt_g))
      {
      PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
      }
 
   return Processor_MoSt_g;
   }

/*----------------------------------------------------------------------------*-

  PROCESSOR_Change_MoSt()

  Force software reset, which will (at minimum) cause a 'Same-Mode Reset'.

  Note.  Before calling this function, PROCESSOR_Store_Reqd_MoSt() will 
  usually be called (to indicate the required new Mode / State). 

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.
       
  MCU HARDWARE:
      NVIC.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void PROCESSOR_Change_MoSt(void)
   {
   // Trigger software reset
   NVIC_SystemReset();       
   }

/*----------------------------------------------------------------------------*-

  PROCESSOR_Store_Reqd_MoSt()

  Store the required (next) Mode / State in a backup register.

  Stored as a Duplicated Variable.

  PARAMETERS:
     The required Mode / State.

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
void PROCESSOR_Store_Reqd_MoSt(const eProc_MoSt REQD_MoSt)
   {
   // About to change mode
   // Disable the MoniTTor (or the mode change may trigger a task overrun)
   // Stopping the MoniTTor must always be handled with care
   MONITTORi_Disable();  

   RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
   PWR_BackupAccessCmd(ENABLE);

   RTC->BKP0R =  (uint32_t)REQD_MoSt;
   RTC->BKP1R = ~(uint32_t)REQD_MoSt;
  
   PWR_BackupAccessCmd(DISABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, DISABLE);
   }

/*----------------------------------------------------------------------------*-

  PROCESSOR_Retrieve_Reqd_MoSt()

  Retrieve the required (next) Mode / State from the backup registers.

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
     If the copies don't match, FAIL_SAFE_S is returned.

  RETURN VALUE:
     The stored Mode / State (or FAIL_SAFE_S - see above).

-*----------------------------------------------------------------------------*/
eProc_MoSt PROCESSOR_Retrieve_Reqd_MoSt(void)
   {
   eProc_MoSt return_value;
  
   if (RTC->BKP0R != ~(RTC->BKP1R))
      {
      // Backup register content has been corrupted
      return_value = FAIL_SAFE_S;
      }
  else
     {
    // Backup register content seems to be correct
    return_value = (eProc_MoSt) RTC->BKP0R;
    }    
   
   return return_value;
   }

/*----------------------------------------------------------------------------*-

  PROCESSOR_Store_PFC()

  Store a Processor Fault Code in backup registers.

  Stored as a Duplicated Variable.

  PARAMETERS:
     The PFC.

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
void PROCESSOR_Store_PFC(const uint32_t FAULT_CODE)
   {
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
   PWR_BackupAccessCmd(ENABLE);

   RTC->BKP2R =  FAULT_CODE;
   RTC->BKP3R = ~FAULT_CODE;
  
   PWR_BackupAccessCmd(DISABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, DISABLE);
   }

/*----------------------------------------------------------------------------*-

  PROCESSOR_Retrieve_Reqd_MoSt()

  Retrieve the Processor Fault Code from the backup registers.

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
     If the copies don't match, PFC_UNKNOWN_FAULT is returned.

  RETURN VALUE:
     The stored PFC (or PFC_UNKNOWN_FAULT - see above).

-*----------------------------------------------------------------------------*/
uint32_t PROCESSOR_Retrieve_PFC(void)
   {
   uint32_t return_value;
   
   if (RTC->BKP2R != ~(RTC->BKP3R))
      {
      // Backup register content has been corrupted
      return_value = PFC_UNKNOWN_FAULT;  
      }
   else
      {
      // Backup register content seems to be correct
      return_value = RTC->BKP2R;
      }
  
   return return_value;
   }

/*----------------------------------------------------------------------------*-

  PROCESSOR_Perform_Safe_Shutdown()

  Attempt to move the system into a safe ('silent') state.

  If WDT is running, we aim to reset the processor and call this function again
  (without the WDT running), in order to keep this function very simple,
  and reduce the risks that we will end up in a repeated sequence of resets.

  PARAMETERS:
     The Processor Fault Code.

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
void PROCESSOR_Perform_Safe_Shutdown(const uint32_t PFC)
   {
   uint32_t WDT_state = WATCHDOG_Get_State();

   // Check if the iWDT is running
   if (WDT_state == WDT_RUNNING)
      {
      // Feed the WDT
      WATCHDOG_Update();

      // Reset (with the aim of disabling the iWDT)
      PROCESSOR_Store_Reqd_MoSt(FAIL_SAFE_S);
      PROCESSOR_Store_PFC(PFC);
      PROCESSOR_Change_MoSt();
      }

   if (WDT_state == WDT_UNKNOWN_STATE)
      {
      // Reset (with the aim of disabling the iWDT, just in case)
      PROCESSOR_Store_Reqd_MoSt(FAIL_SAFE_S);
      PROCESSOR_Store_PFC(PFC);
      PROCESSOR_Change_MoSt();
      }

   // If we've got this far, we assume that the iWDT is *not* running

   // Set up 'Heartbeat' LED pin to report the PFC
   REPORT_PFC_Init();

   while(1)
      {
      // 0.5-second delay (max timeout is ~65 ms => we use 50 ms x 10)
      for (uint32_t i = 0; i <= 10; i++)
         {
         TIMEOUT_T3_USEC_Start();
         while (COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(50000));
         }       

      // Report PFC
      REPORT_PFC_Update(PFC);
      }
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
