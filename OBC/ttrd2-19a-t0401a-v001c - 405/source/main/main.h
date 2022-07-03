/*----------------------------------------------------------------------------*-

   main.h (Release 2017-08-17a)

  ----------------------------------------------------------------------------
   
   This is the Processor Header file.

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

#ifndef _MAIN_H
#define _MAIN_H 1

// Required system operating frequency (in Hz)
// Will be checked in the scheduler initialisation file
#define REQUIRED_SYSTEM_CORE_CLOCK (168000000)

// Links to HSI libraries
#include "../hsi_library/misc.h"
#include "../hsi_library/stm32f4xx_gpio.h"
#include "../hsi_library/stm32f4xx_pwr.h"
#include "../hsi_library/stm32f4xx_rcc.h"
#include "../hsi_library/stm32f4xx_tim.h"
#include "../hsi_library/stm32f4xx_usart.h"
#include "../hsi_library/stm32f4xx_adc.h"
#include "../hsi_library/stm32f4xx_iwdg.h"
#include "../hsi_library/stm32f4xx_rtc.h"

// HSI register configuration checks
#include "../hsi_reg_config_checks/ttrd2-05a-t0401a-v001a_reg_conf_chk_adc1.h"
#include "../hsi_reg_config_checks/ttrd2-05a-t0401a-v001a_reg_conf_chk_gpio.h"
#include "../hsi_reg_config_checks/ttrd2-05a-t0401a-v001a_reg_conf_chk_timer.h"
#include "../hsi_reg_config_checks/ttrd2-05a-t0401a-v001a_reg_conf_chk_uart.h"

// Port header
#include "../port/port.h"

// Processor module
#include "../processor/ttrd2-19a-t0401a-v001b_processor.h"

// Scheduler module
#include "../scheduler/ttrd2-19a-t0401a-v001c_scheduler.h"

// Support functions
#include "../support_functions/ttrd2-05a-t0401a-v001a_timeout_t3.h"

// Services
#include "obc.h"
#include "service_utilities.h"
#include "time_management_service.h"
#include "scheduling_service.h"
#include "obc_hsi.h"

// Tasks
//#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"

/*----------------------------------------------------------------------------*-
   Processor constants
-*----------------------------------------------------------------------------*/

// Generic constants
#define TRUE                  (0x12345678U)
#define FALSE                 (0xEDCBA987U)

// Used for task return values
#define RETURN_NORMAL_STATE   (0x5555AAAAU)
#define RETURN_ABNORMAL_STATE (0xAAAA5555U)

// Used to report results of data-integrity checks on Duplicated Variables
#define DATA_OK               (0x12AB34CDU)
#define DATA_CORRUPTED        (0xED54CB32U)

// Used in checks of stored register configuration
#define REGISTERS_OK          (0x5A5A5A5AU)
#define REGISTERS_CHANGED     (0xA5A5A5A5U)

// Used to indicate if iWDT is running
#define WDT_RUNNING           (0xAAAAAAAAU)
#define WDT_NOT_RUNNING       (0x55555555U)
#define WDT_UNKNOWN_STATE     (0x55AA55AAU)

/*----------------------------------------------------------------------------*-
   Processor Fault Codes (PFCs)
-*----------------------------------------------------------------------------*/

// Problem detected in HSI library
#define PFC_HSI_LIBRARY                                         ((uint8_t) 5)

// Processor temperature outside spec (startup)
#define PFC_PROCESSOR_TEMPERATURE_STARTUP                      ((uint8_t) 10)

// Processor temperature outside spec (normal operation)
#define PFC_PROCESSOR_TEMPERATURE                              ((uint8_t) 11)

// Incorrect function parameter (e.g. out of range)
#define PFC_FUNCTION_PARAMETER_INCORRECT                       ((uint8_t) 15)

// Corruption detected in long-term data
#define PFC_LONG_TERM_DATA_CORRUPTION                          ((uint8_t) 20)

// PFC is unknown - corrupted data in the backup register
#define PFC_UNKNOWN_FAULT                                      ((uint8_t) 30)

// Problem detected when attempting to add task to schedule
#define PFC_SCH_ADD_TASK                                       ((uint8_t) 35)

// Problem detected during scheduler startup checks
#define PFC_SCH_STARTUP_OPERATION                              ((uint8_t) 36)

// Problem detected during scheduler startup checks
#define PFC_SCH_STARTUP_TEST_5A                                ((uint8_t) 37)

// Problem detected during scheduler startup checks
#define PFC_SCH_STARTUP_TEST_5B                                ((uint8_t) 38)

// Osc problem detected when setting up scheduler 
#define PFC_SCH_CLOCK_FREQUENCY                                ((uint8_t) 39)

// Tick_count_g exceeds limit (in Dispatcher)
#define PFC_SCH_TICK_COUNT                                     ((uint8_t) 40)

// Task has reported an abnormal state
#define PFC_TASK_RETURN                                        ((uint8_t) 45)

// Problem during iWDT testing
#define PFC_WDT_TEST                                           ((uint8_t) 50)

// Problem when MoniTTor is tested 
#define PFC_MONITTOR_OVERRUN_TEST                              ((uint8_t) 55)
#define PFC_MONITTOR_UNDERRUN_TEST                             ((uint8_t) 56)

// Problem during normal MoniTTor use
#define PFC_MONITTOR_PARAMETERS                                ((uint8_t) 60)
#define PFC_MONITTOR_OVERRUN                                   ((uint8_t) 61)
#define PFC_MONITTOR_UNDERRUN                                  ((uint8_t) 62)

// Problem when PredicTTor is tested 
#define PFC_PREDICTTOR_TASK_SEQ_FAULT_TEST                     ((uint8_t) 65)

// Problem during normal PredicTTor use
#define PFC_PREDICTTOR_TASK_SEQ_FAULT                          ((uint8_t) 66)

// Invalid UART selected
#define PFC_INVALID_UART                                       ((uint8_t) 67)

// ADC reference value is out of range
#define PFC_ADC_RANGE                                          ((uint8_t) 70)

// Temperature value from ADC is changing too quickly
#define PFC_ADC_TEMP_CHANGE                                    ((uint8_t) 71)

// ADC has timed out during data acquisition
#define PFC_ADC_TIMEOUT                                        ((uint8_t) 72)

// Register configuration checks
#define PFC_REG_CONFIG_ADC1                                    ((uint8_t) 80)
#define PFC_REG_CONFIG_GPIO                                    ((uint8_t) 81)
#define PFC_REG_CONFIG_TIMER                                   ((uint8_t) 82)
#define PFC_REG_CONFIG_UART                                    ((uint8_t) 83)

// Switch appears to be stuck 'on'
#define PFC_SWITCH_STUCK                                       ((uint8_t) 90)

// PFC stored prior to planned mode change
#define PFC_NO_FAULT                                          ((uint8_t) 240)

// Stored as default after reset: usually triggered by iWDT
#define PFC_UNSPECIFIED_FAULT                                 ((uint8_t) 250)

#endif

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
