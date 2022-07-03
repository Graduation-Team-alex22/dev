/*----------------------------------------------------------------------------*-

  ttrd2-05a-t0401a-v001a_heartbeat_sw_task.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
  Simple 'Heartbeat switch' task for STM32F401.

  Targets Nucleo F401RE board.

  Usually released once per second => flashes 0.5 Hz, 50% duty cycle.

  Linked to switch interface: if Button 1 is pressed, LED will not flash.

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

  CorrelaTTor, DecomposiTTor, DuplicaTTor, MoniTTor, PredicTTor, ReliabiliTTy,  
  SafeTTy, SafeTTy Systems, TriplicaTTor and WarranTTor are registered 
  trademarks or trademarks of SafeTTy Systems Ltd in the UK & other countries.

-*----------------------------------------------------------------------------*/

// Module header
#include "ttrd2-05a-t0401a-v001a_heartbeat_sw_task.h"

// Requires switch (Button 1) interface
#include "ttrd2-05a-t0401a-v001a_switch_task.h"

/*----------------------------------------------------------------------------*-

  HEARTBEAT_SW_Init()

  Prepare for HEARTBEAT_SW_Update() function - see below.
  
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
void HEARTBEAT_SW_Init(void)
   {
   GPIO_InitTypeDef GPIO_InitStruct;
     
   // Enable GPIOA clock 
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
     
   // Configure port pin for the LED
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed; 
   GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStruct.GPIO_Pin   = HEARTBEAT_LED_PIN;
   
   GPIO_Init(HEARTBEAT_LED_PORT, &GPIO_InitStruct); 
   }

/*----------------------------------------------------------------------------*-

  HEARTBEAT_SW_Update()

  Flashes at 0.5 Hz.

  Must schedule every second (soft deadline).
   
  PARAMETERS:
     None.

  LONG-TERM DATA:
     Heartbeat_state_s (W)

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     Check Duplicated Variables.
     Check GPIO register configuration.
     
     If any PreC check fails, we force a shutdown
     [other behaviour may be more appropriate in your system.]

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     See PreCs.

  WCET:
     Not yet determined.

  BCET:
     Not yet determined.

  RETURN VALUE:
     RETURN_NORMAL_STATE (fixed).
 
-*----------------------------------------------------------------------------*/
uint32_t HEARTBEAT_SW_Update(void)
   {
   static uint32_t Heartbeat_state_s = 0;
   static uint32_t Heartbeat_state_is = ~0;

   // Check pre-conditions (START)

   // Failure of any PreC will force a shutdown in this design
   // Other behaviour may be more appropriate in your system

   // Check data integrity (Duplicated Variable)
   if (Heartbeat_state_s != ~Heartbeat_state_is)
      {
      // Data have been corrupted: Treat here as Fatal Platform Failure
      PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
      }

   // Check GPIO registers (*all* registers)
   REG_CONFIG_CHECKS_GPIO_Check();

   // Check pre-conditions (END)

   // We perform the core task activities if we met the pre-conditions
   // (otherwise we won't get this far)

   // Only flash the LED if the switch is *not* pressed
   if (SWITCH_BUTTON1_Get_State() == BUTTON1_PRESSED)   
      {
      // Change the LED from OFF to ON (or vice versa)
      if (Heartbeat_state_s == 1)
         {
         Heartbeat_state_s = 0;
         GPIO_ResetBits(HEARTBEAT_LED_PORT, HEARTBEAT_LED_PIN);
         }
      else
         {
         Heartbeat_state_s = 1;
         GPIO_SetBits(HEARTBEAT_LED_PORT, HEARTBEAT_LED_PIN);
         }
      }

   // Update the inverted copy of the state variable
   Heartbeat_state_is = ~Heartbeat_state_s;

   // Check post-conditions (START)
   // No practical post-condition checks identified
   // Check post-conditions (END)

   // Always return this state (no backup-task options)
   return RETURN_NORMAL_STATE;
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
