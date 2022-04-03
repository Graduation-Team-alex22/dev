/*----------------------------------------------------------------------------*-

  ttrd2-05a-t0401a-v001a_reg_conf_chk_timer.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
  Supports storage and checking of timer configuration.

  STM32F401RE.

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

// ------ Private (inverted) copies of register configurations ---------------

static uint32_t Tim1_cr1_ig;
static uint32_t Tim1_arr_ig;
static uint32_t Tim1_psc_ig;
static uint32_t Tim1_rcr_ig;
static uint32_t Tim1_egr_ig;

static uint32_t Tim2_cr1_ig;
static uint32_t Tim2_arr_ig;
static uint32_t Tim2_psc_ig;
static uint32_t Tim2_egr_ig;

static uint32_t Tim3_cr1_ig;
static uint32_t Tim3_arr_ig;
static uint32_t Tim3_psc_ig;
static uint32_t Tim3_egr_ig;

static uint32_t Tim4_cr1_ig;
static uint32_t Tim4_arr_ig;
static uint32_t Tim4_psc_ig;
static uint32_t Tim4_egr_ig;

static uint32_t Tim5_cr1_ig;
static uint32_t Tim5_arr_ig;
static uint32_t Tim5_psc_ig;
static uint32_t Tim5_egr_ig;

static uint32_t Tim9_cr1_ig;
static uint32_t Tim9_arr_ig;
static uint32_t Tim9_psc_ig;
static uint32_t Tim9_egr_ig;

static uint32_t Tim10_cr1_ig;
static uint32_t Tim10_arr_ig;
static uint32_t Tim10_psc_ig;
static uint32_t Tim10_egr_ig;

static uint32_t Tim11_cr1_ig;
static uint32_t Tim11_arr_ig;
static uint32_t Tim11_psc_ig;
static uint32_t Tim11_egr_ig;

/*----------------------------------------------------------------------------*-

  REG_CONFIG_TIMER_Store()

  Stores a copy of the register configuration for a specified timer. 
   
  PARAMETERS:
     Identifies the timer configuration to store.
     {TIM1, TIM2, TIM3, TIM4, TIM5, TIM9, TIM10, TIM11}

  LONG-TERM DATA:
     Please see start of the file (all *_ig variables are altered)
   
  MCU HARDWARE:
     The specified timer hardware.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void REG_CONFIG_TIMER_Store(TIM_TypeDef* TIMx)
   {
   if (TIMx == TIM1)
      {
      Tim1_cr1_ig = ~(TIMx->CR1);
      Tim1_arr_ig = ~(TIMx->ARR);
      Tim1_psc_ig = ~(TIMx->PSC);
      Tim1_rcr_ig = ~(TIMx->RCR);  // TIM1 only
      Tim1_egr_ig = ~(TIMx->EGR);        
      }
   else if (TIMx == TIM2)
      {
      Tim2_cr1_ig = ~(TIMx->CR1);
      Tim2_arr_ig = ~(TIMx->ARR);
      Tim2_psc_ig = ~(TIMx->PSC);
      Tim2_egr_ig = ~(TIMx->EGR);        
      }
   else if (TIMx == TIM3)
      {
      Tim3_cr1_ig = ~(TIMx->CR1);
      Tim3_arr_ig = ~(TIMx->ARR);
      Tim3_psc_ig = ~(TIMx->PSC);
      Tim3_egr_ig = ~(TIMx->EGR);        
      }
   else if (TIMx == TIM4)
      {
      Tim4_cr1_ig = ~(TIMx->CR1);
      Tim4_arr_ig = ~(TIMx->ARR);
      Tim4_psc_ig = ~(TIMx->PSC);
      Tim4_egr_ig = ~(TIMx->EGR);        
      }
   else if (TIMx == TIM5)
      {
      Tim5_cr1_ig = ~(TIMx->CR1);
      Tim5_arr_ig = ~(TIMx->ARR);
      Tim5_psc_ig = ~(TIMx->PSC);
      Tim5_egr_ig = ~(TIMx->EGR);        
      }
   else if (TIMx == TIM9)
      {
      Tim9_cr1_ig = ~(TIMx->CR1);
      Tim9_arr_ig = ~(TIMx->ARR);
      Tim9_psc_ig = ~(TIMx->PSC);
      Tim9_egr_ig = ~(TIMx->EGR);        
      }
   else if (TIMx == TIM10)
      {
      Tim10_cr1_ig = ~(TIMx->CR1);
      Tim10_arr_ig = ~(TIMx->ARR);
      Tim10_psc_ig = ~(TIMx->PSC);
      Tim10_egr_ig = ~(TIMx->EGR);        
      }
   else if (TIMx == TIM11)
      {
      Tim11_cr1_ig = ~(TIMx->CR1);
      Tim11_arr_ig = ~(TIMx->ARR);
      Tim11_psc_ig = ~(TIMx->PSC);
      Tim11_egr_ig = ~(TIMx->EGR);        
      }
   }

/*----------------------------------------------------------------------------*-
   
  REG_CONFIG_TIMER_Check()

  Checks whether the stored copy of the register configuration for a 
  specified timer matches the current register settings.    

  Forces shutdown in the event of data corruption.
   
  PARAMETERS:
     Identifies the timer configuration to be checked.
     {TIM1, TIM2, TIM3, TIM4, TIM5, TIM9, TIM10, TIM11}

  LONG-TERM DATA:
     Please see start of the file (all *_ig variables are read)
   
  MCU HARDWARE:
     The specified timer hardware.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void REG_CONFIG_TIMER_Check(TIM_TypeDef* TIMx)
   {
   uint32_t Result = REGISTERS_OK;

   if (TIMx == TIM1)
      {
      if ((Tim1_cr1_ig != ~(TIMx->CR1)) ||
          (Tim1_arr_ig != ~(TIMx->ARR)) ||
          (Tim1_psc_ig != ~(TIMx->PSC)) ||
          (Tim1_rcr_ig != ~(TIMx->RCR)) ||  
          (Tim1_egr_ig != ~(TIMx->EGR)))
         {
         Result = REGISTERS_CHANGED;
         }        
      }
   else if (TIMx == TIM2)
      {
      if ((Tim2_cr1_ig != ~(TIMx->CR1)) ||
          (Tim2_arr_ig != ~(TIMx->ARR)) ||
          (Tim2_psc_ig != ~(TIMx->PSC)) ||
          (Tim2_egr_ig != ~(TIMx->EGR)))
         {
         Result = REGISTERS_CHANGED;
         }        
      }
   else if (TIMx == TIM3)
      {
      if ((Tim3_cr1_ig != ~(TIMx->CR1)) ||
          (Tim3_arr_ig != ~(TIMx->ARR)) ||
          (Tim3_psc_ig != ~(TIMx->PSC)) ||
          (Tim3_egr_ig != ~(TIMx->EGR)))
         {
         Result = REGISTERS_CHANGED;
         }        
      }
   else if (TIMx == TIM4)
      {
      if ((Tim4_cr1_ig != ~(TIMx->CR1)) ||
          (Tim4_arr_ig != ~(TIMx->ARR)) ||
          (Tim4_psc_ig != ~(TIMx->PSC)) ||
          (Tim4_egr_ig != ~(TIMx->EGR)))
         {
         Result = REGISTERS_CHANGED;
         }        
      }
   else if (TIMx == TIM5)
      {
      if ((Tim5_cr1_ig != ~(TIMx->CR1)) ||
          (Tim5_arr_ig != ~(TIMx->ARR)) ||
          (Tim5_psc_ig != ~(TIMx->PSC)) ||
          (Tim5_egr_ig != ~(TIMx->EGR)))
         {
         Result = REGISTERS_CHANGED;
         }        
      }
   else if (TIMx == TIM9)
      {
      if ((Tim9_cr1_ig != ~(TIMx->CR1)) ||
          (Tim9_arr_ig != ~(TIMx->ARR)) ||
          (Tim9_psc_ig != ~(TIMx->PSC)) ||
          (Tim9_egr_ig != ~(TIMx->EGR)))
         {
         Result = REGISTERS_CHANGED;
         }        
      }
   else if (TIMx == TIM10)
      {
      if ((Tim10_cr1_ig != ~(TIMx->CR1)) ||
          (Tim10_arr_ig != ~(TIMx->ARR)) ||
          (Tim10_psc_ig != ~(TIMx->PSC)) ||
          (Tim10_egr_ig != ~(TIMx->EGR)))
         {
         Result = REGISTERS_CHANGED;
         }        
      }
   else if (TIMx == TIM11)
      {
      if ((Tim11_cr1_ig != ~(TIMx->CR1)) ||
          (Tim11_arr_ig != ~(TIMx->ARR)) ||
          (Tim11_psc_ig != ~(TIMx->PSC)) ||
          (Tim11_egr_ig != ~(TIMx->EGR)))
         {
         Result = REGISTERS_CHANGED;
         }        
      }
      
   // Shut down if registers have been altered.
   // Other solutions may be more appropriate in your design.
   if (Result == REGISTERS_CHANGED)
      {
      PROCESSOR_Perform_Safe_Shutdown(PFC_REG_CONFIG_TIMER);
      }
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
