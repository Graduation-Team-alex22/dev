/*----------------------------------------------------------------------------*-

  ttrd2-05a-t0401a-v001a_reg_conf_chk_uart.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
  Supports storage and checking of UART configuration on STM32F401RE.
  
  This MCU has three UARTs (USARTs): 1, 2 and 6.

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

// Processor header
#include "../main/main.h"

// Module header
#include "ttrd2-05a-t0401a-v001a_reg_conf_chk_uart.h"

// ------ Private (inverted) copies of register configurations ---------------

static uint32_t uart1_cr1_ig;
static uint32_t uart1_cr2_ig;
static uint32_t uart1_cr3_ig;
static uint32_t uart1_brr_ig;

static uint32_t uart2_cr1_ig;
static uint32_t uart2_cr2_ig;
static uint32_t uart2_cr3_ig;
static uint32_t uart2_brr_ig;

static uint32_t uart6_cr1_ig;
static uint32_t uart6_cr2_ig;
static uint32_t uart6_cr3_ig;
static uint32_t uart6_brr_ig;

/*----------------------------------------------------------------------------*-

  REG_CONFIG_CHECKS_UART_Store()

  Stores a copy of the register configuration for the specified UART. 
   
  PARAMETERS:
     USART_TypeDef* USARTx : Identifies the required UART.

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
void REG_CONFIG_CHECKS_UART_Store(USART_TypeDef* pUSARTx)
   {
   // Valid UART? 
   if (((pUSARTx != USART1) &&
        (pUSARTx != USART2) &&
        (pUSARTx != USART6)))
      {
      // We treat this as a Fatal Platform Failure
      PROCESSOR_Perform_Safe_Shutdown(PFC_INVALID_UART);
      }

   if (pUSARTx == USART1)
      {
      uart1_cr1_ig = ~(pUSARTx->CR1);    
      uart1_cr2_ig = ~(pUSARTx->CR2);    
      uart1_cr3_ig = ~(pUSARTx->CR3);    
      uart1_brr_ig = ~(pUSARTx->BRR);    
      }
   else if (pUSARTx == USART2)
      {
      uart2_cr1_ig = ~(pUSARTx->CR1);    
      uart2_cr2_ig = ~(pUSARTx->CR2);    
      uart2_cr3_ig = ~(pUSARTx->CR3);    
      uart2_brr_ig = ~(pUSARTx->BRR);    
      }
   else
      {
      uart6_cr1_ig = ~(pUSARTx->CR1);    
      uart6_cr2_ig = ~(pUSARTx->CR2);    
      uart6_cr3_ig = ~(pUSARTx->CR3);    
      uart6_brr_ig = ~(pUSARTx->BRR);    
      }
   }

/*----------------------------------------------------------------------------*-
   
  REG_CONFIG_CHECKS_UART_Check()

  Checks whether the stored (inverted) copy of the register configuration 
  for the specified UART matches the current register settings.    

  Forces shutdown in the event of data corruption.
   
  PARAMETERS:
     USART_TypeDef* USARTx : Identifies the required UART.

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
void REG_CONFIG_CHECKS_UART_Check(USART_TypeDef* pUSARTx)
   {
   uint32_t Result = REGISTERS_OK;

   // Valid UART? 
   if (((pUSARTx != USART1) &&
        (pUSARTx != USART2) &&
        (pUSARTx != USART6)))
      {
      // We treat this as a Fatal Platform Failure
      PROCESSOR_Perform_Safe_Shutdown(PFC_INVALID_UART);
      }

   if (pUSARTx == USART1)
      {
      if (uart1_cr1_ig != ~(pUSARTx->CR1))
         {
         Result = REGISTERS_CHANGED;  
         }  
      else if (uart1_cr2_ig != ~(pUSARTx->CR2))
         {
         Result = REGISTERS_CHANGED;  
         } 
      else if (uart1_cr3_ig != ~(pUSARTx->CR3))
         {
         Result = REGISTERS_CHANGED;  
         } 
      else if (uart1_brr_ig != ~(pUSARTx->BRR))
         {
         Result = REGISTERS_CHANGED;  
         } 
      }
   else if (pUSARTx == USART2)
      {
      if (uart2_cr1_ig != ~(pUSARTx->CR1))
         {
         Result = REGISTERS_CHANGED;  
         }  
      else if (uart2_cr2_ig != ~(pUSARTx->CR2))
         {
         Result = REGISTERS_CHANGED;  
         } 
      else if (uart2_cr3_ig != ~(pUSARTx->CR3))
         {
         Result = REGISTERS_CHANGED;  
         } 
      else if (uart2_brr_ig != ~(pUSARTx->BRR))
         {
         Result = REGISTERS_CHANGED;  
         } 
      }
   else
      {
      if (uart6_cr1_ig != ~(pUSARTx->CR1))
         {
         Result = REGISTERS_CHANGED;  
         }  
      else if (uart6_cr2_ig != ~(pUSARTx->CR2))
         {
         Result = REGISTERS_CHANGED;  
         } 
      else if (uart6_cr3_ig != ~(pUSARTx->CR3))
         {
         Result = REGISTERS_CHANGED;  
         } 
      else if (uart6_brr_ig != ~(pUSARTx->BRR))
         {
         Result = REGISTERS_CHANGED;  
         } 
      }

   // Shut down if registers have been altered.
   // Other design solutions may be more appropriate in your system.
   if (Result == REGISTERS_CHANGED)
      {
      PROCESSOR_Perform_Safe_Shutdown(PFC_REG_CONFIG_UART);
      }
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
