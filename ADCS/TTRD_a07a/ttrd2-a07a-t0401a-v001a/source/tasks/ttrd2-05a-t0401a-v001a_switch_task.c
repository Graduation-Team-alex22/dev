/*----------------------------------------------------------------------------*-

  ttrd2-05a-t0401a-v001a_switch_task.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
  Simple switch interface library for Nucleo F401RE board.

  See 'ERES2' (Chapter 3 and Chapter 5) for further information.

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

// Module header
#include "ttrd2-05a-t0401a-v001a_switch_task.h"

// ------ Private constants --------------------------------------------------

// Used for external processing (not externally visible)

// Allows NO or NC switch to be used (or other wiring variations)
#define SW_PRESSED (0)

// SW_THRES must be > 1 for correct debounce behaviour
#define SW_THRES (3)

// ------ Private Duplicated Variables ('File Global') -----------------------

// The current switch state 
static uint32_t Switch_button1_pressed_g = BUTTON1_NOT_PRESSED;
static uint32_t Switch_button1_pressed_ig = ~BUTTON1_NOT_PRESSED;

/*----------------------------------------------------------------------------*-

  SWITCH_BUTTON1_Init()

  Initialisation function for simple switch-interface module.

  Works with Button 1 on Nucleo board.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Switch_button1_pressed_g (W)
     Switch_button1_pressed_ig (W)   
   
  MCU HARDWARE:
     Switch pin.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void SWITCH_BUTTON1_Init(void)
   {
   GPIO_InitTypeDef GPIO_InitStruct;
  
   // Enable GPIOC clock 
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
   // Configure the switch pin for input
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;
   GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed; 
   GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStruct.GPIO_Pin   = BUTTON1_PIN;

   GPIO_Init(BUTTON1_PORT, &GPIO_InitStruct);

   // Set the initial state  
   Switch_button1_pressed_g = BUTTON1_NOT_PRESSED;
   Switch_button1_pressed_ig = ~BUTTON1_NOT_PRESSED;   
   }

/*----------------------------------------------------------------------------*-

  SWITCH_BUTTON1_Update()

  Switch-reading task.
   
  PARAMETERS:
     None.

  LONG-TERM DATA:
     Duration_s (W)
     Duration_is (W)
     Switch_button1_pressed_g (W)
     Switch_button1_pressed_ig (W)   
   
  MCU HARDWARE:
     Switch pin.

  PRE-CONDITION CHECKS:
     Data integrity checks.
     Register configuration checks. 

     If any PreC check fails, we force a shutdown
     [other behaviour may be more appropriate in your system.]

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     See PreCs and PostCs.

  WCET:
     Not yet determined.

  BCET:
     Not yet determined.

  RETURN VALUE:
     RETURN_NORMAL_STATE (fixed).

-*----------------------------------------------------------------------------*/
uint32_t SWITCH_BUTTON1_Update(void)
   {
   static uint32_t Duration_s = 0;
   static uint32_t Duration_is = ~0;

   // Check pre-conditions (START)

   // Failure of any PreC will force a shutdown in this design
   // Other behaviour may be more appropriate in your system

   // Check data integrity (DVs)
   if ((Duration_s != ~Duration_is) ||
       (Switch_button1_pressed_g != ~Switch_button1_pressed_ig))
      {
      PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
      }

   // Check GPIO registers (*all* registers)
   REG_CONFIG_CHECKS_GPIO_Check();

   // Check pre-conditions (END)

   // We perform the core task activities if we met the pre-conditions
   // (otherwise we won't get this far)

   // Read the pin state
   uint32_t Button1_input = GPIO_ReadInputDataBit(BUTTON1_PORT, BUTTON1_PIN);
   
   if (Button1_input == SW_PRESSED)
      {
      Duration_s += 1;

      if (Duration_s > SW_THRES)
         {
         Duration_s = SW_THRES;

         Switch_button1_pressed_g = BUTTON1_PRESSED;
         }
      else
         {
         // Switch pressed, but not yet for long enough
         Switch_button1_pressed_g = BUTTON1_NOT_PRESSED;
         }
      }
   else
      {
      // Switch not pressed - reset the count
      Duration_s = 0;

      // Update status
      Switch_button1_pressed_g = BUTTON1_NOT_PRESSED;
      }

   // Update the inverted copy of the DVs
   Duration_is = ~Duration_s;
   Switch_button1_pressed_ig = ~Switch_button1_pressed_g;

   // Check post-conditions (START)
   // Check post-conditions (END)

   // Always return this state (no backup-task options)
   return RETURN_NORMAL_STATE;
   }

/*----------------------------------------------------------------------------*-

  SWITCH_BUTTON1_Get_State()

  Get current switch state ('pressed' / 'not pressed').
   
  PARAMETERS:
     None.
     
  LONG-TERM DATA:
     Switch_button1_pressed_g, Switch_button1_pressed_ig;
   
  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     Check Duplicated Variables.
     
     If any PreC check fails, we force a shutdown
     [other behaviour may be more appropriate in your system.]

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     See PreCs.

  RETURN VALUE:
     BUTTON1_PRESSED or BUTTON1_NOT_PRESSED.

-*----------------------------------------------------------------------------*/
uint32_t SWITCH_BUTTON1_Get_State(void)
   {
   // Check data integrity (file-global variable)
   // Here we force a processor shut down if data corruption is detected
   // Other design solutions may be more appropriate in your system.
   if (Switch_button1_pressed_g != ~Switch_button1_pressed_ig)
      {
      PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
      }

   return Switch_button1_pressed_g;
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
