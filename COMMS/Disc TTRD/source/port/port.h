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

// Set for Discovery board

/////////////////////////////////////////////////////////////////////////

// Heartbeat LED
#define HEARTBEAT_LED_PORT_RCC RCC_AHB1Periph_GPIOD
#define HEARTBEAT_LED_PORT GPIOD
#define HEARTBEAT_LED_PIN  GPIO_Pin_15
#define HEARTBEAT_SafeMode_LED_PIN GPIO_Pin_14


// Switch interface (Button 1)
#define BUTTON1_PORT       	GPIOA
#define BUTTON1_PIN        	GPIO_Pin_0

// UART2 interface
#define UART2_PORT_RCC		 	RCC_AHB1Periph_GPIOA
#define UART2_UART_RCC		 	RCC_APB1Periph_USART2
#define UART2_DMA_RCC		 	 	RCC_AHB1Periph_DMA1
#define UART2_DMA_RX_STREAM	DMA1_Stream5
#define UART2_DMA_TX_STREAM	DMA1_Stream6
#define UART2_DMA_Channel		 DMA_Channel_4
#define UART2_PORT         	GPIOA
#define UART2_UARTX         USART2
#define UART2_TX_PIN       	GPIO_Pin_2
#define UART2_RX_PIN       	GPIO_Pin_3
#define UART2_TX_PIN_SOURCE GPIO_PinSource2
#define UART2_RX_PIN_SOURCE	GPIO_PinSource3

// CC_RX interface 
#define CC_RX_PORT_RCC		 	 RCC_AHB1Periph_GPIOB
#define CC_RX_UART_RCC		 	 RCC_APB1Periph_USART3
#define CC_RX_DMA_RCC		 	 	 RCC_AHB1Periph_DMA1
#define CC_RX_DMA_RX_STREAM	 DMA1_Stream1
#define CC_RX_DMA_Channel		 DMA_Channel_4
#define CC_RX_PORT         	 GPIOB
#define CC_RX_UARTX          USART3
#define CC_RX_AF		         GPIO_AF_USART3 
#define CC_RX_TX_PIN       	 GPIO_Pin_10
#define CC_RX_RX_PIN       	 GPIO_Pin_11
#define CC_RX_TX_PIN_SOURCE  GPIO_PinSource10
#define CC_RX_RX_PIN_SOURCE	 GPIO_PinSource11
#define CC_RX_SET_PIN			 	 GPIO_Pin_12

// CC_TX interface
#define CC_TX_PORT_RCC		 	 RCC_AHB1Periph_GPIOC
#define CC_TX_UART_RCC			 RCC_APB1Periph_UART4
#define CC_TX_DMA_RCC		 		 RCC_AHB1Periph_DMA1
#define CC_TX_DMA_RX_STREAM	 DMA1_Stream2
#define CC_TX_DMA_TX_STREAM	 DMA1_Stream4
#define CC_TX_DMA_Channel	   DMA_Channel_4
#define CC_TX_PORT        	 GPIOC
#define CC_TX_UARTX          UART4 
#define CC_TX_AF		         GPIO_AF_UART4 
#define CC_TX_TX_PIN      	 GPIO_Pin_11
#define CC_TX_RX_PIN      	 GPIO_Pin_10
#define CC_TX_TX_PIN_SOURCE	 GPIO_PinSource11
#define CC_TX_RX_PIN_SOURCE	 GPIO_PinSource10
#define CC_TX_SET_PIN			 	 GPIO_Pin_9

/////////////////////////////////////////// MXCONSTANT ///////////////////////////////////////

//#define PA_CNTRL_Pin GPIO_Pin_5
//#define PA_CNTRL_GPIO_Port GPIOA
//#define RESETN_RX_Pin GPIO_Pin_1
//#define RESETN_RX_GPIO_Port GPIOB
//#define CC_GPIO2_START_END_OF_PACKET_Pin GPIO_Pin_9
//#define CC_GPIO2_START_END_OF_PACKET_GPIO_Port GPIOE
//#define CS_SPI2_RX_Pin GPIO_Pin_15
//#define CS_SPI2_RX_GPIO_Port GPIOE
//#define CC_GPIO0_RXFIFO_THR_Pin GPIO_Pin_12
//#define CC_GPIO0_RXFIFO_THR_GPIO_Port GPIOB
//#define RESETN_TX_Pin GPIO_Pin_10
//#define RESETN_TX_GPIO_Port GPIOA
//#define CS_SPI1_TX_Pin GPIO_Pin_15
//#define CS_SPI1_TX_GPIO_Port GPIOA


#endif

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
