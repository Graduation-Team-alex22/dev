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
#define HEARTBEAT_LED_PIN  GPIO_Pin_15

// UART2 LED
#define UART2_LED_PORT GPIOD
#define UART2_LED_PIN  GPIO_Pin_12

// Switch interface (Button 1)
#define BUTTON1_PORT       GPIOA
#define BUTTON1_PIN        GPIO_Pin_0

// UART2 Tx interface
#define UART2_PORT         GPIOA
#define UART2_TX_PIN       GPIO_Pin_2

// SDCARD enable pin
#define SD_PWR_EN_Pin  GPIO_Pin_12
#define SD_PWR_EN_GPIO_Port GPIOA

// COMMS_U4 interface
#define COMMS_U4_PORT_RCC		 	   RCC_AHB1Periph_GPIOA
#define COMMS_U4_UART_RCC			   RCC_APB1Periph_UART4
#define COMMS_U4_DMA_RCC		 		 RCC_AHB1Periph_DMA1
#define COMMS_U4_DMA_RX_STREAM	 DMA1_Stream2
#define COMMS_U4_DMA_TX_STREAM	 DMA1_Stream4
#define COMMS_U4_DMA_Channel	   DMA_Channel_4
#define COMMS_U4_PORT        	   GPIOA
#define COMMS_U4_UARTX           UART4 
#define COMMS_U4_AF		           GPIO_AF_UART4 
#define COMMS_U4_TX_PIN      	   GPIO_Pin_0
#define COMMS_U4_RX_PIN      	   GPIO_Pin_1
#define COMMS_U4_TX_PIN_SOURCE	 GPIO_PinSource0
#define COMMS_U4_RX_PIN_SOURCE	 GPIO_PinSource1

// ADCS_U6 interface
#define ADCS_U6_PORT_RCC		 	 RCC_AHB1Periph_GPIOC
#define ADCS_U6_UART_RCC			 RCC_APB2Periph_USART6
#define ADCS_U6_DMA_RCC		 		 RCC_AHB1Periph_DMA2
#define ADCS_U6_DMA_RX_STREAM	 DMA2_Stream2
#define ADCS_U6_DMA_TX_STREAM	 DMA2_Stream6
#define ADCS_U6_DMA_Channel	   DMA_Channel_5
#define ADCS_U6_PORT        	 GPIOC
#define ADCS_U6_UARTX          USART6 
#define ADCS_U6_AF		         GPIO_AF_USART6 
#define ADCS_U6_TX_PIN      	 GPIO_Pin_6
#define ADCS_U6_RX_PIN      	 GPIO_Pin_7
#define ADCS_U6_TX_PIN_SOURCE	 GPIO_PinSource6
#define ADCS_U6_RX_PIN_SOURCE	 GPIO_PinSource7

// EPS_U1 interface
#define EPS_U1_PORT_RCC		 	   RCC_AHB1Periph_GPIOA
#define EPS_U1_UART_RCC			   RCC_APB2Periph_USART1
#define EPS_U1_DMA_RCC		 		 RCC_AHB1Periph_DMA2
#define EPS_U1_DMA_RX_STREAM	 DMA2_Stream5
#define EPS_U1_DMA_TX_STREAM	 DMA2_Stream7
#define EPS_U1_DMA_Channel	   DMA_Channel_4
#define EPS_U1_PORT        	   GPIOA
#define EPS_U1_UARTX           USART1 
#define EPS_U1_AF		           GPIO_AF_USART1 
#define EPS_U1_TX_PIN      	   GPIO_Pin_9
#define EPS_U1_RX_PIN      	   GPIO_Pin_10
#define EPS_U1_TX_PIN_SOURCE	 GPIO_PinSource9
#define EPS_U1_RX_PIN_SOURCE	 GPIO_PinSource10



#endif

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
