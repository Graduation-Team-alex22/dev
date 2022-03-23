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
#define HEARTBEAT_LED_PORT GPIOA
#define HEARTBEAT_LED_PIN  GPIO_Pin_5

// Switch interface (Button 1)
#define BUTTON1_PORT       GPIOC
#define BUTTON1_PIN        GPIO_Pin_13

// UART2 Tx interface
#define UART2_PORT         GPIOA
#define UART2_TX_PIN       GPIO_Pin_2
/////////////////////////////////////////////////////

/*
 * eps_configuration.h
 *
 *  Created on: May 2, 2016
 *      Author: Aris Stathakis
 */

#ifndef INC_EPS_CONFIGURATION_H_
#define INC_EPS_CONFIGURATION_H_

#include "stm32l1xx_hal.h"
#include "eps_soft_error_handling.h"

/** @addtogroup eps_configuration
  * @{
  */

#define DEPLOY_BURNOUT_DELAY ((uint32_t)6000)/*TIME IN MILLISECONDS TO BURN THE RESISTOR IN DEPLOYMENT SYSTEM.*/

#define MPPT_STEP_SIZE ((uint32_t)1)
#define MPPT_STARTUP_PWM_DUTYCYCLE ((uint32_t) 16)/*1 - 160 for 0 -100%duty cycle - must not start from 0*/
#define MPPT_VOLTAGE_THRESHOLD ((uint32_t) 1240)/*solar cell voltage under which we reset mppt search: 0.5Volt/Volt: 62012bitadc/Volt so 2volt = 1240*/
/* battery temperature sensors i2c device address*/
#define TC74_ADDRESS_A TC74_A2
#define TC74_ADDRESS_B TC74_A5

#define CPU_TO_BATTERY_TEMPERATURE_OFFSET ((int32_t) 2)/*value to subtract from cpu temp to "match" the battery pack temperature.*/

#define ADC_VALUE_3V_BAT_VOLTAGE ((uint16_t) 670)/*3 volt value measured at adc...for obc service. 0.54Volt in the adc given the 0.18v/v voltage divider - x/4095 *3.3 = 0.54 =>x=670 the adc measurement for 3Volt*/
#define ADC_VALUE_1A_BAT_CURRENT ((uint16_t) 3475)/*1 ampere value measured at adc...for obc service. 2.8Volt in the adc given the 2.8v/A voltage divider - x/4095 *3.3 = 2.8 =>x=3475 the adc measurement for 1ampere*/
/*Set up power modules pwm timer channels*/
#define PWM_TIM_CHANNEL_TOP TIM_CHANNEL_3
#define PWM_TIM_CHANNEL_BOTTOM TIM_CHANNEL_1
#define PWM_TIM_CHANNEL_LEFT TIM_CHANNEL_2
#define PWM_TIM_CHANNEL_RIGHT TIM_CHANNEL_4
/*Set up power modules adc current channels*/
#define ADC_CURRENT_CHANNEL_TOP ADC_CHANNEL_5
#define ADC_CURRENT_CHANNEL_BOTTOM ADC_CHANNEL_20
#define ADC_CURRENT_CHANNEL_LEFT ADC_CHANNEL_11
#define ADC_CURRENT_CHANNEL_RIGHT ADC_CHANNEL_18
/*Set up power modules adc voltage channels*/
#define ADC_VOLTAGE_CHANNEL_TOP ADC_CHANNEL_6
#define ADC_VOLTAGE_CHANNEL_BOTTOM ADC_CHANNEL_21
#define ADC_VOLTAGE_CHANNEL_LEFT ADC_CHANNEL_10
#define ADC_VOLTAGE_CHANNEL_RIGHT ADC_CHANNEL_19

/*Setup eps state adc channels*/
#define ADC_VBAT ADC_CHANNEL_1;
#define ADC_IBAT_PLUS ADC_CHANNEL_2;
#define ADC_IBAT_MINUS ADC_CHANNEL_3;
#define ADC_I3V3 ADC_CHANNEL_4;
#define ADC_I5V ADC_CHANNEL_12;


/**
 * @brief Umbillical connector status.
 *
 * Umbilical is the external connector of the satellite. When connected means that the satellite is under debug/development
 * and when connected the sattelite is ready to deploy and start running. This status is used to enter debug mode or not.
 */
typedef enum {
	UMBILICAL_NOT_CONNECTED,/**<  Umbilicall connector is not connected - satellite is ready ti deploy*/
	UMBILICAL_CONNECTED,/**<  Umbilicall connector is   connected - satellite is in debug mode*/
	UMBILICAL_CONNECTOR_UNDEFINED_STATE,/**<  Umbilicall connector is in undefined state*/
	UMBILICAL_LAST_VALUE/**<  Umbilicall connector is in undefined state*/
}EPS_umbilical_status;

extern volatile EPS_umbilical_status EPS_umbilical_mode;/* initialize global umbilical flag to connected - When umbillical is connected no deployment stage occurs.*/



#endif /* INC_EPS_CONFIGURATION_H_ */
/**
  * @}
  */
















#endif

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
