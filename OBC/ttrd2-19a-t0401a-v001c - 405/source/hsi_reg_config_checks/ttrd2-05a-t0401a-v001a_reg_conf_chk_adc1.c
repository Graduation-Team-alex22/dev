/*----------------------------------------------------------------------------*-

  ttrd2-05a-t0401a-v001a_reg_conf_chk_adc1.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
  Supports storage and checking of ADC1 configuration.

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

// Processor Module
#include "../main/main.h"

// ------ Private (inverted) copies of register configurations ---------------

static uint32_t ADC_ccr_ig;
static uint32_t ADC1_cr1_ig;
static uint32_t ADC1_cr2_ig;
static uint32_t ADC1_sqr1_ig;

/*----------------------------------------------------------------------------*-

  REG_CONFIG_CHECKS_ADC1_Store()

  Stores a copy of the ADC register configuration. 
   
  PARAMETERS:
     None.

  LONG-TERM DATA:
     uint32_t ADC_ccr_ig (W)
     uint32_t ADC1_cr1_ig (W)
     uint32_t ADC1_cr2_ig (W)
     uint32_t ADC1_sqr1_ig (W)
   
  MCU HARDWARE:
     ADC1

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void REG_CONFIG_CHECKS_ADC1_Store(void)
   {
   ADC_ccr_ig   = ~(ADC->CCR);    
   ADC1_cr1_ig  = ~(ADC1->CR1);
   ADC1_cr2_ig  = ~(ADC1->CR2);
   ADC1_sqr1_ig = ~(ADC1->SQR1);
   }

/*----------------------------------------------------------------------------*-
   
  REG_CONFIG_CHECKS_ADC1_Check()

  Checks whether the stored (inverted) copy of the ADC1 register configuration 
  matches the current register settings.    

  Forces shutdown in the event of data corruption.
   
  PARAMETERS:
     None.

  LONG-TERM DATA:
     uint32_t ADC_ccr_ig (R)
     uint32_t ADC1_cr1_ig (R)
     uint32_t ADC1_cr2_ig (R)
     uint32_t ADC1_sqr1_ig (R)
   
  MCU HARDWARE:
     ADC1

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
void REG_CONFIG_CHECKS_ADC1_Check(void)
   {
   uint32_t Result = REGISTERS_OK;

   if (ADC_ccr_ig != ~(ADC->CCR))
      {
      Result = REGISTERS_CHANGED;  
      }  
   else if (ADC1_cr1_ig != ~(ADC1->CR1))
      {
      Result = REGISTERS_CHANGED;  
      } 
   else if (ADC1_cr2_ig != ~(ADC1->CR2))
      {
      Result = REGISTERS_CHANGED;  
      } 
   else if (ADC1_sqr1_ig != ~(ADC1->SQR1))
      {
      Result = REGISTERS_CHANGED;  
      } 

   // Shut down if registers have been altered.
   // Other design solutions may be more appropriate in your system.
   if (Result == REGISTERS_CHANGED)
      {
      PROCESSOR_Perform_Safe_Shutdown(PFC_REG_CONFIG_ADC1);
      }
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
