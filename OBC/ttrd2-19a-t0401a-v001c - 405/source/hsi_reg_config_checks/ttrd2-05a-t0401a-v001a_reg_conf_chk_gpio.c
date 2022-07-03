/*----------------------------------------------------------------------------*-

  ttrd2-05a-t0401a-v001a_reg_conf_chk_gpio.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
  Supports storage and checking of GPIO configuration.

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

#include "../main/main.h"

// ------ Private (inverted) copies of register configurations ---------------

// RCC config for all ports
static uint32_t GPIOx_rcc_ig; 

// Ports are A-E plus H
static uint32_t GPIOA_moder_ig;
static uint32_t GPIOA_otyper_ig;
static uint32_t GPIOA_ospeedr_ig;
static uint32_t GPIOA_pupdr_ig;

static uint32_t GPIOB_moder_ig;
static uint32_t GPIOB_otyper_ig;
static uint32_t GPIOB_ospeedr_ig;
static uint32_t GPIOB_pupdr_ig;

static uint32_t GPIOC_moder_ig;
static uint32_t GPIOC_otyper_ig;
static uint32_t GPIOC_ospeedr_ig;
static uint32_t GPIOC_pupdr_ig;

static uint32_t GPIOD_moder_ig;
static uint32_t GPIOD_otyper_ig;
static uint32_t GPIOD_ospeedr_ig;
static uint32_t GPIOD_pupdr_ig;

static uint32_t GPIOE_moder_ig;
static uint32_t GPIOE_otyper_ig;
static uint32_t GPIOE_ospeedr_ig;
static uint32_t GPIOE_pupdr_ig;

static uint32_t GPIOH_moder_ig;
static uint32_t GPIOH_otyper_ig;
static uint32_t GPIOH_ospeedr_ig;
static uint32_t GPIOH_pupdr_ig;

/*----------------------------------------------------------------------------*-

  REG_CONFIG_CHECKS_GPIO_Store()

  Stores a copy of the GPIO register configurations (all ports). 
   
  PARAMETERS:
     None.

  LONG-TERM DATA:
     Please see start of the file (all *_ig variables are altered)
   
  MCU HARDWARE:
     GPIO registers (see list at top of file)

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void REG_CONFIG_CHECKS_GPIO_Store(void)
   {
   // We store these register values (set up when port is configured):  
   //    uint32_t MODER;    
   //    uint32_t OTYPER;   
   //    uint32_t OSPEEDR;  
   //    uint32_t PUPDR;    
   
   // We also store the RCC settings  
   // These will be updated by changes to RCC settings for *any* port
   GPIOx_rcc_ig = ~(RCC->AHB1ENR);

   // Store Port A configuration  
   GPIOA_moder_ig   = ~(GPIOA->MODER);
   GPIOA_otyper_ig  = ~(GPIOA->OTYPER);
   GPIOA_ospeedr_ig = ~(GPIOA->OSPEEDR);
   GPIOA_pupdr_ig   = ~(GPIOA->PUPDR);

   // Store Port B configuration  
   GPIOB_moder_ig   = ~(GPIOB->MODER);
   GPIOB_otyper_ig  = ~(GPIOB->OTYPER);
   GPIOB_ospeedr_ig = ~(GPIOB->OSPEEDR);
   GPIOB_pupdr_ig   = ~(GPIOB->PUPDR);     

   // Store Port C configuration  
   GPIOC_moder_ig   = ~(GPIOC->MODER);
   GPIOC_otyper_ig  = ~(GPIOC->OTYPER);
   GPIOC_ospeedr_ig = ~(GPIOC->OSPEEDR);
   GPIOC_pupdr_ig   = ~(GPIOC->PUPDR);

   // Store Port D configuration  
   GPIOD_moder_ig   = ~(GPIOD->MODER);
   GPIOD_otyper_ig  = ~(GPIOD->OTYPER);
   GPIOD_ospeedr_ig = ~(GPIOD->OSPEEDR);
   GPIOD_pupdr_ig   = ~(GPIOD->PUPDR);

   // Store Port E configuration  
   GPIOE_moder_ig   = ~(GPIOE->MODER);
   GPIOE_otyper_ig  = ~(GPIOE->OTYPER);
   GPIOE_ospeedr_ig = ~(GPIOE->OSPEEDR);
   GPIOE_pupdr_ig   = ~(GPIOE->PUPDR);
   
   // Store Port H configuration  
   GPIOH_moder_ig   = ~(GPIOH->MODER);
   GPIOH_otyper_ig  = ~(GPIOH->OTYPER);
   GPIOH_ospeedr_ig = ~(GPIOH->OSPEEDR);
   GPIOH_pupdr_ig   = ~(GPIOH->PUPDR);   
   }

/*----------------------------------------------------------------------------*-
   
  REG_CONFIG_CHECKS_GPIO_Check()

  Checks whether the stored copy of the GPIO register configuration matches
  the current register settings.    

  Forces shutdown in the event of data corruption.
   
  PARAMETERS:
     None.

  LONG-TERM DATA:
     Please see start of the file (all *_ig variables are read)
   
  MCU HARDWARE:
     GPIO registers (see list at start of file)

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
void REG_CONFIG_CHECKS_GPIO_Check(void)
   {
   uint32_t Result = REGISTERS_OK;

   if (GPIOx_rcc_ig != ~(RCC->AHB1ENR))         // RCC settings
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOA_moder_ig != ~(GPIOA->MODER))  // Port A
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOA_otyper_ig != ~(GPIOA->OTYPER))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOA_ospeedr_ig != ~(GPIOA->OSPEEDR))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOA_pupdr_ig != ~(GPIOA->PUPDR))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOB_moder_ig != ~(GPIOB->MODER))  // Port B
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOB_otyper_ig != ~(GPIOB->OTYPER))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOB_ospeedr_ig != ~(GPIOB->OSPEEDR))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOB_pupdr_ig != ~(GPIOB->PUPDR))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOC_moder_ig != ~(GPIOC->MODER))  // Port C
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOC_otyper_ig != ~(GPIOC->OTYPER))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOC_ospeedr_ig != ~(GPIOC->OSPEEDR))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOC_pupdr_ig != ~(GPIOC->PUPDR))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOD_moder_ig != ~(GPIOD->MODER))  // Port D
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOD_otyper_ig != ~(GPIOD->OTYPER))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOD_ospeedr_ig != ~(GPIOD->OSPEEDR))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOD_pupdr_ig != ~(GPIOD->PUPDR))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOE_moder_ig != ~(GPIOE->MODER))  // Port E
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOE_otyper_ig != ~(GPIOE->OTYPER))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOE_ospeedr_ig != ~(GPIOE->OSPEEDR))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOE_pupdr_ig != ~(GPIOE->PUPDR))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOH_moder_ig != ~(GPIOH->MODER))  // Port H
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOH_otyper_ig != ~(GPIOH->OTYPER))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOH_ospeedr_ig != ~(GPIOH->OSPEEDR))
      {
      Result = REGISTERS_CHANGED;
      }
   else if (GPIOH_pupdr_ig != ~(GPIOH->PUPDR))
      {
      Result = REGISTERS_CHANGED;
      }
      
   // Shut down if registers have been altered.
   // Other solutions may be more appropriate in your design.
   if (Result == REGISTERS_CHANGED)
      {
      PROCESSOR_Perform_Safe_Shutdown(PFC_REG_CONFIG_GPIO);
      }
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
