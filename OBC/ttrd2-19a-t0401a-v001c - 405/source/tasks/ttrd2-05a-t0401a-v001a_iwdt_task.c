/*----------------------------------------------------------------------------*-

  ttrd2-05a-t0401a-v001a_iwdt_task.c (Release 2017-03-06a)

  --------------------------------------------------------------------------------------
   
  'Watchdog' library for STM32F401RE.

  See 'ERES2' (Chapter 2 and Chapter 5) for further information.

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
#include "ttrd2-05a-t0401a-v001a_iwdt_task.h"

// ------ Private Duplicated Variables ('File Global') -----------------------

uint32_t Watchdog_running_g = WDT_NOT_RUNNING;
uint32_t Watchdog_running_ig = ~WDT_NOT_RUNNING;

/*----------------------------------------------------------------------------*-

  WATCHDOG_Init()

  Set up watchdog timer.
 
  The watchdog timer is driven by the Internal LSI (RC) Oscillator:
  the timing varies (significantly) with temperature.

  Datasheet values: 
  RC oscillator frequency (Minimum) : 17 kHz  
  RC oscillator frequency (Minimum) : 32 kHz 
  RC oscillator frequency (Minimum) : 47 kHz 

  It is suggested that you use twice the required timeout value (approx).
    
  PARAMETERS:
     WDT_COUNT : Will be multiplied by ~125µs to determine the timeout. 
   
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
void WATCHDOG_Init(const uint32_t WDT_COUNT)
   {
   // Enable write access to IWDG_PR and IWDG_RLR registers
   IWDG->KR = 0x5555;

   // Set pre-scalar to 4 (timer resolution is 125µs)
   IWDG->PR = 0x00;

   // Counts down to 0 in increments of 125µs
   // Max reload value is 0xFFF (4095) or ~511 ms (with this prescalar)
   IWDG->RLR = WDT_COUNT;

   // Reload IWDG counter
   IWDG->KR = 0xAAAA;

   // Enable IWDG (the LSI oscillator will be enabled by hardware)
   IWDG->KR = 0xCCCC;

   // Feed watchdog
   WATCHDOG_Update();

   // Store the state
   Watchdog_running_g = WDT_RUNNING;
   Watchdog_running_ig = ~WDT_RUNNING;
   }

/*----------------------------------------------------------------------------*-

  WATCHDOG_Update()

  Feed the watchdog timer.

  See Watchdog_Init() for further information.
   
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

  WCET:
     Not yet determined.

  BCET:
     Not yet determined.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
uint32_t WATCHDOG_Update(void)
   {
		 
	 updateCurrentTick();
		 
   // Feed the watchdog (reload IWDG counter)
   IWDG->KR = 0xAAAA;

   return RETURN_NORMAL_STATE;
   }

/*----------------------------------------------------------------------------*-

  WATCHDOG_Get_State()

  Reports whether the iWDT is running.

  In the STM32 we cannot determine this from the register configuration
  (which is the preferred solution) - we therefore use a flag.
   
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
     Checks data integrity.

  WCET:
     Not yet determined.

  BCET:
     Not yet determined.

  RETURN VALUE:
     WDT_RUNNING, WDT_NOT_RUNNING or WDT_UNKNOWN_STATE.

-*----------------------------------------------------------------------------*/
uint32_t WATCHDOG_Get_State(void)
   {
   uint32_t Return_value = WDT_UNKNOWN_STATE;

   // Check data integrity (file-global variable)
   if (Watchdog_running_g == ~Watchdog_running_ig)
      {
      Return_value = Watchdog_running_g;
      }

   return Return_value;
   }


/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
