/*
 * eps_power_module.c
 *
 *  Created on: May 2, 2016
 *      Author: Aris Stathakis
 */

#include "eps_power_module.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "eps_state.h"
#include "../port/port.h"
#include "../port/eps_configuration.h"
//TIM_HandleTypeDef htim3;
//extern ADC_HandleTypeDef hadc;
//extern TIM_HandleTypeDef htim3;
/* 

PWM example

///////
//////  **************************************************************************
//////  * @file    main.c
//////  * @author  Ac6
//////  * @version V1.0
//////  * @date    01-December-2013
//////  * @brief   Default main function.
//////  **************************************************************************
//////*/
////////#include "defines.h"
//////#include "stm32f4xx.h"
//////#include "stm32f4xx_rcc.h"
//////#include "stm32f4xx_gpio.h"
//////#include "stm32f4xx_tim.h"

//////void TM_LEDS_Init(void) {
//////	GPIO_InitTypeDef GPIO_InitStruct;

//////	/* Clock for GPIOD */
//////    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

//////	/* Alternating functions for pins */
//////	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
//////	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
//////	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
//////	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);

//////	/* Set pins */
//////	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
//////	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
//////	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
//////    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
//////    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
//////    GPIO_Init(GPIOD, &GPIO_InitStruct);
//////}

//////void TM_TIMER_Init(void) {
//////	TIM_TimeBaseInitTypeDef TIM_BaseStruct;

//////	/* Enable clock for TIM4 */
//////	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
///////*
//////	TIM4 is connected to APB1 bus, which has on F407 device 42MHz clock
//////	But, timer has internal PLL, which double this frequency for timer, up to 84MHz
//////	Remember: Not each timer is connected to APB1, there are also timers connected
//////	on APB2, which works at 84MHz by default, and internal PLL increase
//////	this to up to 168MHz

//////	Set timer pre-scaler
//////	Timer count frequency is set with

//////	timer_tick_frequency = Timer_default_frequency / (prescaller_set + 1)

//////	In our case, we want a max frequency for timer, so we set prescaller to 0
//////	And our timer will have tick frequency

//////	timer_tick_frequency = 84000000 / (0 + 1) = 84000000
//////*/
//////	TIM_BaseStruct.TIM_Prescaler = 32000;
//////	/* Count up */
//////    TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
///////*
//////	Set timer period when it have reset
//////	First you have to know max value for timer
//////	In our case it is 16bit = 65535
//////	To get your frequency for PWM, equation is simple

//////	PWM_frequency = timer_tick_frequency / (TIM_Period + 1)

//////	If you know your PWM frequency you want to have timer period set correct

//////	TIM_Period = timer_tick_frequency / PWM_frequency - 1

//////	In our case, for 10Khz PWM_frequency, set Period to

//////	TIM_Period = 84000000 / 10000 - 1 = 8399

//////	If you get TIM_Period larger than max timer value (in our case 65535),
//////	you have to choose larger prescaler and slow down timer tick frequency
//////*/
//////    TIM_BaseStruct.TIM_Period = 262; /* 10kHz PWM */
//////    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
//////    TIM_BaseStruct.TIM_RepetitionCounter = 0;
//////	/* Initialize TIM4 */
//////    TIM_TimeBaseInit(TIM4, &TIM_BaseStruct);
//////	/* Start count on TIM4 */
//////    TIM_Cmd(TIM4, ENABLE);
//////}

//////void TM_PWM_Init(void) {
//////	TIM_OCInitTypeDef TIM_OCStruct;

//////	/* Common settings */

//////	/* PWM mode 2 = Clear on compare match */
//////	/* PWM mode 1 = Set on compare match */
//////	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
//////	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
//////	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;

///////*
//////	To get proper duty cycle, you have simple equation

//////	pulse_length = ((TIM_Period + 1) * DutyCycle) / 100 - 1

//////	where DutyCycle is in percent, between 0 and 100%

//////	25% duty cycle: 	pulse_length = ((8399 + 1) * 25) / 100 - 1 = 2099
//////	50% duty cycle: 	pulse_length = ((8399 + 1) * 50) / 100 - 1 = 4199
//////	75% duty cycle: 	pulse_length = ((8399 + 1) * 75) / 100 - 1 = 6299
//////	100% duty cycle:	pulse_length = ((8399 + 1) * 100) / 100 - 1 = 8399

//////	Remember: if pulse_length is larger than TIM_Period, you will have output HIGH all the time
//////*/
//////	TIM_OCStruct.TIM_Pulse = 65; /* 25% duty cycle */
//////	TIM_OC1Init(TIM4, &TIM_OCStruct);
//////	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

//////	TIM_OCStruct.TIM_Pulse = 130; /* 50% duty cycle */
//////	TIM_OC2Init(TIM4, &TIM_OCStruct);
//////	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

//////	TIM_OCStruct.TIM_Pulse = 196; /* 75% duty cycle */
//////	TIM_OC3Init(TIM4, &TIM_OCStruct);
//////	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

//////	TIM_OCStruct.TIM_Pulse = 262; /* 100% duty cycle */
//////	TIM_OC4Init(TIM4, &TIM_OCStruct);
//////	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
//////}

//////int main(void) {
//////	/* Initialize system */
//////	SystemInit();
//////	/* Init leds */
//////	TM_LEDS_Init();
//////	/* Init timer */
//////	TM_TIMER_Init();
//////	/* Init PWM */
//////	TM_PWM_Init();

//////	while (1) {

//////	}
//////}


/**
  * @brief Initializes power module instance.
  * @param  module_X: pointer to the power module instance to set.
  * @param  starting_pwm_dutycycle: initial duty cycle at which the power module will start the mppt !0 is not a good start.
  * @param  htim: pointer to timer peripheral handle for pwm generation.
  * @param  timer_channel: timer peripheral channel used for this pwm module.
  * @param  hadc_power_module: pointer to adc peripheral handle that holds the current/voltage measurements.
  * @param  ADC_channel_current: pointer to adc channel with the current measurement.
  * @param  ADC_channel_voltage:  pointer to adc channel with the voltage measurement.
  * @retval None.
  */
void EPS_PowerModule_init(EPS_PowerModule *module_X, uint32_t starting_pwm_dutycycle, TIM_HandleTypeDef *htim, uint32_t timer_channel, ADC_HandleTypeDef *hadc_power_module, uint32_t ADC_channel_current, uint32_t ADC_channel_voltage){
	//initialize properly all power module entries.
	module_X->module_state = POWER_MODULE_ON;
	module_X->current =0;
	module_X->voltage =0;
	module_X->previous_power =0;
	module_X->previous_voltage =0;
	module_X->pwm_duty_cycle = starting_pwm_dutycycle;
	module_X->htim_pwm = htim;
	module_X->timChannel = timer_channel;
	module_X->incremennt_flag = 1;//start by incrementing
	module_X->hadc_power_module = hadc_power_module;
	module_X->ADC_channel_current = ADC_channel_current;
	module_X->ADC_channel_voltage = ADC_channel_voltage;

	/*Start pwm with initialized from cube mx pwm duty cycle for timerX at timer_channel.*/
//  uint16_t HAL_TIM_PWM_Start(htim, timer_channel);
  TIM_TimeBaseInitTypeDef  TIM_3_InitStruct ;
	// 	Enable TIM clock 
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_3_InitStruct.TIM_ClockDivision =TIM_CKD_DIV1;
	TIM_3_InitStruct.TIM_CounterMode = TIM_CounterMode_Up ;
	TIM_3_InitStruct.TIM_Period = 0xa0;
	TIM_3_InitStruct.TIM_Prescaler=0;
	
	TIM_TimeBaseInit(TIM3, &TIM_3_InitStruct);
	
	//
	
	

	
	
	
	
	//
	
	 TIM_OCInitTypeDef sConfigOC;
	 sConfigOC.TIM_OCMode= TIM_OCMode_PWM1;
	 sConfigOC.TIM_OutputState=TIM_OutputState_Enable;
	 sConfigOC.TIM_Pulse= 0x00;
	 sConfigOC.TIM_OCPolarity=TIM_OCPolarity_High;
	 TIM_OC1FastConfig(TIM3,TIM_OCFast_Enable);
	 TIM_OC1Init(TIM3, &sConfigOC);
	 TIM_OC2Init(TIM3, &sConfigOC);
	 TIM_OC3Init(TIM3, &sConfigOC);
   TIM_OC4Init(TIM3, &sConfigOC);
	 TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Reset);
	 TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Disable);
	 
		//@brief  Selects the TIM Output Compare Mode.
  //* @note   This function disables the selected channel before changing the Output
  //*         Compare Mode. If needed, user has to enable this channel using
  //*         TIM_CCxCmd() and TIM_CCxNCmd() functions.
	TIM_SelectOCxM( TIM3, TIM_Channel_1, TIM_OCMode_PWM1);
	TIM_SelectOCxM( TIM3, TIM_Channel_2, TIM_OCMode_PWM1);
	TIM_SelectOCxM( TIM3, TIM_Channel_3, TIM_OCMode_PWM1);
	TIM_SelectOCxM( TIM3, TIM_Channel_4, TIM_OCMode_PWM1);
}

/**
  * @brief Initializes all satellite power module instances.
  * @param  module_top: pointer to top solar panel side power module.
  * @param  module_bottom: pointer to bottom solar panel side power module.
  * @param  module_left: pointer to left solar panel side power module.
  * @param  module_right: pointer to right solar panel side power module
  * @retval Error status for handling and debugging.
  */
EPS_soft_error_status EPS_PowerModule_init_ALL(EPS_PowerModule *module_top, EPS_PowerModule *module_bottom, EPS_PowerModule *module_left, EPS_PowerModule *module_right)
{

  uint16_t error_status = EPS_SOFT_ERROR_POWER_MODULE_INIT_ALL;
  
	
	TIM_HandleTypeDef htim3;
	htim3.Instance = TIM3;
  htim3.Init.TIM_Prescaler = 0;
  htim3.Init.TIM_CounterMode = TIM_CounterMode_Up;
  htim3.Init.TIM_Period = 0xa0;
  htim3.Init.TIM_ClockDivision = TIM_CKD_DIV1;
	//
	
	ADC_HandleTypeDef hadc; 
	
	hadc.Instance = ADC1;
  hadc.Comm_Init.ADC_Prescaler = ADC_Prescaler_Div2;
  hadc.Init.ADC_Resolution = ADC_Resolution_12b;
  hadc.Init.ADC_DataAlign = ADC_DataAlign_Right;
  hadc.Init.ADC_ScanConvMode = ENABLE;
  //hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  //hadc.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
  //hadc.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
  //hadc.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
  hadc.Init.ADC_ContinuousConvMode = DISABLE;// Enable in eclipse
  hadc.Init.ADC_NbrOfConversion = 14;
  //hadc.Init.DiscontinuousConvMode = DISABLE;
  //hadc.Init.ADC_ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
  //hadc.Init.DMAContinuousRequests = ENABLE;
	
	/*start timer3 pwm base generation (initialized pwm duty cycle from mx must be 0) */
	//HAL_TIM_Base_Start(&htim3);

	/* initialize all power modules and dem mppt cotrol blocks.
	
	for the stm32l152 please don't forget to uncomment the right and the bottom module init
	
	*/
	error_status = EPS_SOFT_ERROR_POWER_MODULE_INIT_TOP;
	EPS_PowerModule_init(module_top, MPPT_STARTUP_PWM_DUTYCYCLE, &htim3, PWM_TIM_CHANNEL_TOP, &hadc, ADC_CURRENT_CHANNEL_TOP, ADC_VOLTAGE_CHANNEL_TOP);
	error_status = EPS_SOFT_ERROR_POWER_MODULE_INIT_BOTTOM;
	// EPS_PowerModule_init(module_bottom, MPPT_STARTUP_PWM_DUTYCYCLE, &htim3, PWM_TIM_CHANNEL_BOTTOM, &hadc, ADC_CURRENT_CHANNEL_BOTTOM, ADC_VOLTAGE_CHANNEL_BOTTOM);
	error_status = EPS_SOFT_ERROR_POWER_MODULE_INIT_LEFT;
	EPS_PowerModule_init(module_left, MPPT_STARTUP_PWM_DUTYCYCLE, &htim3, PWM_TIM_CHANNEL_LEFT, &hadc, ADC_CURRENT_CHANNEL_LEFT, ADC_VOLTAGE_CHANNEL_LEFT);
	error_status = EPS_SOFT_ERROR_POWER_MODULE_INIT_RIGHT;
	//EPS_PowerModule_init(module_right, MPPT_STARTUP_PWM_DUTYCYCLE, &htim3, PWM_TIM_CHANNEL_RIGHT, &hadc, ADC_CURRENT_CHANNEL_RIGHT, ADC_VOLTAGE_CHANNEL_RIGHT);
  uint16_t EPS_SOFT_ERROR_POWER_MODULE_INIT_ALL_COMPLETE;
	return EPS_SOFT_ERROR_POWER_MODULE_INIT_ALL_COMPLETE;
}

/**
  * @brief Update power module instance adc measurements.
  * @param  power_module: pointer to  power module instance for which to get adc measurements.
  * @retval None.
  */
void EPS_update_power_module_state(EPS_PowerModule *power_module,ADC_InitTypeDef hadc1){

	/*initialize adc handle*/
	power_module->hadc_power_module->Init.ADC_NbrOfConversion = 2;
	power_module->hadc_power_module->NbrOfCurrentConversionRank = 2;
	ADC_Init(ADC1,&hadc1);

	/*setup conversion sequence for */
	ADC_ChannelConfTypeDef sConfig;
	sConfig.SamplingTime = ADC_SAMPLETIME_192CYCLES;

	/*power module current*/
	sConfig.Channel = power_module->ADC_channel_current ;
	sConfig.Rank = 1;
	HAL_ADC_ConfigChannel(power_module->hadc_power_module, &sConfig);

	/*power module voltage*/
	sConfig.Channel = power_module->ADC_channel_voltage ;
	//sConfig.Rank = 2;
	//HAL_ADC_ConfigChannel(power_module->hadc_power_module, &sConfig);



	/*start dma transfer from adc to memory.*/

	uint32_t adc_measurement_dma_power_modules[67]= { 0 };//2*64 +1

	//adc_reading_complete = ADC_TRANSFER_NOT_COMPLETED;//external global flag defined in main and shared with the adc complete transfer interrupt handler.
	//HAL_ADC_Start_DMA(power_module->hadc_power_module, adc_measurement_dma_power_modules, 66);

	/*Process Measurements*/
	uint32_t voltage_avg =0;
	uint32_t current_avg =0;

	/*Wait till DMA ADC sequence transfer is ready*/
	//while(adc_reading_complete==ADC_TRANSFER_NOT_COMPLETED){
		//wait for dma transfer complete.

	/*ADC must be stopped in the adc dma transfer complete callback.*/

	//HAL_ADC_Stop_DMA(power_module->hadc_power_module);

	/*de-interleave and sum voltage and current measurements.*/
	for (uint32_t  sum_index = 2 ; sum_index < 66; sum_index+=2) {
		/*top*/
		current_avg = current_avg + adc_measurement_dma_power_modules[sum_index];
		voltage_avg = voltage_avg + adc_measurement_dma_power_modules[sum_index+1];
	}

	/*filter ting*/
	/*average of 16 concecutive adc measurements.skip the first to avoid adc power up distortion.*/
	//power_module->voltage = voltage_avg>>5;
	//power_module->current = current_avg>>5;
}



/**
  * @brief Update power module instance MPPT next step pwm duty cycle.
  * @param  moduleX: pointer to  power module instance for which to calculate pwm duty cycle based on P&O (protrube and observe) mppt sstrategy algorithm.
  * @retval None.
  */
void EPS_PowerModule_mppt_update_pwm(EPS_PowerModule *moduleX){



	/*power calculation*/

	volatile uint32_t power_now_avg = moduleX->voltage * moduleX->current;
	uint32_t duty_cycle = moduleX->pwm_duty_cycle;


	/*if solar cell voltage is below threshold, reset mppt point search starting from startup dutycycle*/
	if(moduleX->voltage<MPPT_VOLTAGE_THRESHOLD){

		duty_cycle = MPPT_STARTUP_PWM_DUTYCYCLE;

	}
	else{

		uint32_t step_size = MPPT_STEP_SIZE;

		/*decide duty cycle orientation - set increment flag.*/
		if (power_now_avg  <(moduleX->previous_power)){

			if (moduleX->incremennt_flag>0){

				if(moduleX->voltage  <(moduleX->previous_voltage -5)){
					moduleX->incremennt_flag = 0;
				}

			}
			else{
				moduleX->incremennt_flag = 1;
			}
		}
		/*add appropriate offset - create new duty cycle.*/

		if(moduleX->incremennt_flag){
			duty_cycle = duty_cycle+step_size;
		}
		else{
			duty_cycle = duty_cycle-step_size;
		}
		/*Check for Overflow and Underflow*/
		if (duty_cycle>(160+MPPT_STEP_SIZE)){//first check for underflow
			duty_cycle= MPPT_STARTUP_PWM_DUTYCYCLE;//
		}
		if (duty_cycle==(160+MPPT_STEP_SIZE)){//then check for overflow
			duty_cycle=160;
		}

	}

	  moduleX->previous_power = power_now_avg;
	  moduleX->previous_voltage = moduleX->voltage;
	  moduleX->pwm_duty_cycle = duty_cycle;

}

/**
  * @brief Apply calculated MPPT step pwm duty cycle.
  * @param  moduleX: pointer to  power module instance for which to apply the calculated pwm dutycycle.
  * @retval None.
  */
void EPS_PowerModule_mppt_apply_pwm(EPS_PowerModule *moduleX){
	
	
		/*power calculation*/

	volatile uint32_t power_now_avg = moduleX->voltage * moduleX->current;
	uint32_t duty_cycle = moduleX->pwm_duty_cycle;


	/*if solar cell voltage is below threshold, reset mppt point search starting from startup dutycycle*/
	if(moduleX->voltage<MPPT_VOLTAGE_THRESHOLD){

		duty_cycle = MPPT_STARTUP_PWM_DUTYCYCLE;

	}
	else{

		uint32_t step_size = MPPT_STEP_SIZE;

		/*decide duty cycle orientation - set increment flag.*/
		if (power_now_avg  <(moduleX->previous_power)){

			if (moduleX->incremennt_flag>0){

				if(moduleX->voltage  <(moduleX->previous_voltage -5)){
					moduleX->incremennt_flag = 0;
				}

			}
			else{
				moduleX->incremennt_flag = 1;
			}
		}
		/*add appropriate offset - create new duty cycle.*/

		if(moduleX->incremennt_flag){
			duty_cycle = duty_cycle+step_size;
		}
		else{
			duty_cycle = duty_cycle-step_size;
		}
		/*Check for Overflow and Underflow*/
		if (duty_cycle>(160+MPPT_STEP_SIZE)){//first check for underflow
			duty_cycle= MPPT_STARTUP_PWM_DUTYCYCLE;//
		}
		if (duty_cycle==(160+MPPT_STEP_SIZE)){//then check for overflow
			duty_cycle=160;
		}

	}

	  moduleX->previous_power = power_now_avg;
	  moduleX->previous_voltage = moduleX->voltage;
	  moduleX->pwm_duty_cycle = duty_cycle;
}
void EPS_Power_Task_Init(void){}
