/*----------------------------------------------------------------------------*-
  
   port.h (Release 2017-03-06a)

  ----------------------------------------------------------------------------

   The "Port Header".
 
   This file documents the use of port pins on this processor.

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

#ifndef _PORT_H
#define _PORT_H 1

// Processor Header
#include "../main/main.h"

// Set for Nucleo board

// Heartbeat LED
#define HEARTBEAT_LED_PORT GPIOD
#define HEARTBEAT_LED_PIN  GPIO_Pin_13

// Switch interface (Button 1)
#define BUTTON1_PORT       GPIOA
#define BUTTON1_PIN        GPIO_Pin_0

// UART2 Tx interface
#define UART2_PORT         GPIOA
#define UART2_TX_PIN       GPIO_Pin_2



/////////////////////////////////////////// MXCONSTANT ///////////////////////////////////////
#define PA_CNTRL_Pin GPIO_PIN_5
#define PA_CNTRL_GPIO_Port GPIOA
#define RESETN_RX_Pin GPIO_PIN_1
#define RESETN_RX_GPIO_Port GPIOB
#define CC_GPIO2_START_END_OF_PACKET_Pin GPIO_PIN_9
#define CC_GPIO2_START_END_OF_PACKET_GPIO_Port GPIOE
#define CS_SPI2_RX_Pin GPIO_PIN_15
#define CS_SPI2_RX_GPIO_Port GPIOE
#define CC_GPIO0_RXFIFO_THR_Pin GPIO_PIN_12
#define CC_GPIO0_RXFIFO_THR_GPIO_Port GPIOB
#define RESETN_TX_Pin GPIO_PIN_10
#define RESETN_TX_GPIO_Port GPIOA
#define CS_SPI1_TX_Pin GPIO_PIN_15
#define CS_SPI1_TX_GPIO_Port GPIOA

#endif

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
