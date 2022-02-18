/*
 * hw_init.c
 *
 *  Created on: Aug 22, 2014
 *      Author: mobintu
 */

#include "hw_init.h"


//#define HallC_min   1450
//#define HallC_max   2220
// Mean value of Hall sensors (in raw ADC cnts, 12bit)
#define HallC_mean   1835
#define HallA_mean   610+100


//#define ADC0
//
//#ifdef ADC0
#define ADC_WDG_H (HallA_mean + 10)
#define ADC_WDG_L (HallA_mean - 10)
// #define ADC_WDG_L 1800
//#else
//#define ADC_WDG_H 1024
//#define ADC_WDG_L 900
//#endif

volatile uint32_t ADmean;
//volatile uint32_t ADC_WDG_L;
//volatile uint32_t ADC_WDG_H;



void (*AU_ptr)(uint32_t);
void (*BU_ptr)(uint32_t);
void (*CU_ptr)(uint32_t);
void (*AD_ptr)(uint32_t);
void (*BD_ptr)(uint32_t);
void (*CD_ptr)(uint32_t);


//volatile int32_t RPMr_;
volatile int32_t CNTr_;
volatile int32_t CNTm_;
volatile int32_t state;
volatile int32_t direction_;

volatile uint32_t PWMval;
volatile uint32_t CCR1r;
volatile uint32_t Pstate;

volatile uint32_t ADC_IRQ_cnt;



void hw_init(void){


	Pstate=1;
	ADC_IRQ_cnt = 0;
	PWMval = 1000; // (avoid PWM noise)
	CCR1r  = RPM2CNT(100)/6;

	direction_ = 0;
	setDirection(1);
	/*
	 * TIM2 CCR is used for bridge pulse updates
	 * ADC1_COMP Irq is for analog watchdog function. Every event causes the update of CCR, and frequency value
	 */
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;

	// MOTOR CONTROL GPIO / PWM CONFIG
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB,ENABLE);

	// Configure Pins for UP fets
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //should be faster than OD, go up to 3.3V pushing, then up to 5 with external
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStructure);	//AU(1);

	// 1 keeps Upper PMOS off
	AU(1);
	BU(1);
	CU(1);

	// Configure Pins for DOWN fets
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	// Configure Pins for Analog In (Hall,V,I) I
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_Init(GPIOA,&GPIO_InitStructure);


	//Init TIMER1 for PWM Out
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Period = M_PWM_TickPeriod-1;
	TIM_TimeBaseStructure.TIM_Prescaler=0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCNIdleState_Reset;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse=0;
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);
	TIM_OC3Init(TIM1,&TIM_OCInitStructure);

	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource1,GPIO_AF_2);

	//Setup 32bit TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Period=0xFFFFFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler=0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse=48*10000; //10ms
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Disable);
	TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannelPriority = 0; // well it doesn't have minus ones..!
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	TIM_Cmd(TIM2, ENABLE);

	// Analog Init
	ADC_InitTypeDef     ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_DeInit(ADC1);
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_ClockModeConfig(ADC1,ADC_ClockMode_AsynClk);
	ADC_ChannelConfig(ADC1, HallAp_Ch , ADC_SampleTime_71_5Cycles);
	ADC_GetCalibrationFactor(ADC1);


	ADC_AnalogWatchdogThresholdsConfig(ADC1,ADC_WDG_H,0);//> should wait for real ADmean first
	ADC_AnalogWatchdogSingleChannelConfig(ADC1,HallAp_WD_Ch);
	ADC_ClearITPendingBit(ADC1,ADC_IT_AWD);
	ADC_ITConfig(ADC1,ADC_IT_AWD,ENABLE);
	ADC_AnalogWatchdogCmd(ADC1,ENABLE);


	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	ADC_Cmd(ADC1, ENABLE);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));
	ADC_StartOfConversion(ADC1);

	// Enable CRC AHB clock interface
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	CRC_DeInit();
	CRC_ReverseOutputDataCmd(DISABLE);



	//	#define DBG
#ifdef DBG
	//---------------------------------------
	//I2C  - PB6/PB7
	//RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);

	//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9, GPIO_AF_4);
	//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10, GPIO_AF_4);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif

}

void I2C_init(void){
	I2C_InitTypeDef  I2C_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	// ----------------I2C-----------------------------------
	// For discovery I2C1, PB6/SCL, PB7/SDA
	NVIC_InitStructure.NVIC_IRQChannel = I2C1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7, GPIO_AF_1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_SoftwareResetCmd(I2C1);


	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	I2C_InitStructure.I2C_DigitalFilter = 0x00;
	I2C_InitStructure.I2C_OwnAddress1 = I2C_Address;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_Timing =  0xB0420F13;
	//0xPRESC4b | RES4b | SCLDEL4b | SDADEL4b || SCLH8b | SCLL8b
	//	0xB 0 4 2 C3 C7 10kHz @48MHz
	//	0xB 0 4 2 0F 13 100kHz @48MHz
	//	0x5 0 3 3 03 09 400kHz @48MHz
	//	0x5 0 1 0 01 03 1000kHz @48MHz

	I2C_Init(I2C1, &I2C_InitStructure);

	I2C_StretchClockCmd(I2C1,ENABLE);
	I2C_GeneralCallCmd(I2C1,DISABLE);
	I2C_ITConfig(I2C1,I2C_IT_ADDRI,ENABLE);
	I2C_ITConfig(I2C1,I2C_IT_RXI,ENABLE);
	I2C_ITConfig(I2C1,I2C_IT_TXI,ENABLE);
	I2C_ITConfig(I2C1,I2C_IT_TIMEOUT,DISABLE);
	I2C_Cmd(I2C1, ENABLE);
}


int32_t setDirection(int32_t CNTr){
	// Disable IRQ only when direction need to change, to not disturb bridge pulses
	if(CNTr > 0){
		if(direction_ != 1){
			__disable_irq();
			direction_ = 1;
			AU_ptr = AU;
			BU_ptr = BU;
			CU_ptr = CU;
			AD_ptr = AD;
			BD_ptr = BD;
			CD_ptr = CD;
			__enable_irq();
		}
	}
	else{
		CNTr = -CNTr;
		if(direction_ != -1){
			__disable_irq();
			direction_ = -1;
			AU_ptr = CU;
			BU_ptr = BU;
			CU_ptr = AU;

			AD_ptr = CD;
			BD_ptr = BD;
			CD_ptr = AD;
			__enable_irq();
		}
	}
	return CNTr;
}


void ADC1_COMP_IRQHandler(void){
	volatile static uint16_t adc,adc_thd=0;//=ADC_WDG_H;
	volatile static uint32_t t=0,dt,t1,CCRtmp,locked=0;

	t1=toc;
	int i = 2;

	if(ADC_GetITStatus(ADC1, ADC_IT_AWD) != RESET){
		ADC_ClearITPendingBit(ADC1,ADC_IT_AWD);
		ADC_IRQ_cnt = 0;

		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
		adc = ADC1->DR;
		adc = 0;
		while(i--){
			while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
			adc += ADC1->DR;
		}
		adc=adc/2;

		if(adc>adc_thd){
			adc_thd=ADC_WDG_L;
			ADC_AnalogWatchdogThresholdsConfig(ADC1,4095,ADC_WDG_H);
			ADC_ClearITPendingBit(ADC1,ADC_IT_AWD);
		}
		else{
			dt=t1-t;
			t=t1;
			CCRtmp=(200*CCRtmp+56*(dt)/6)/256; //6 steps per Electrical cycle,
			//CCRtmp=(dt)/6; //6 steps per Electrical cycle, faster spin-up, less stable
			Led1(1);
			Led1(0);
			if( 6*CCRtmp > ( RPM2CNT(CNT2RPM(CNTr_) - 10 - locked*200))){ // Was if(6*CCRtmp>(RPM2CNT(RPMr_ - 10 - locked*200))){
				locked=0;
				tx_pck.flag = MOTOR_INSYNC;
				CCR1r=CCRtmp;
				Pstate=1;//1180Hz@3V
				//Pstate=6;TIM2->CCR1=toc+20; //1220Hz@3V, 1700Hz@4.2
				Led1(0);
			}
			else{
				locked=1;
				tx_pck.flag = MOTOR_LOCKED;
				Led1(1);
				CCR1r = CNTr_/6; // Was RPM2CNT(RPMr_)/6;
			}

			CNTm_ = CCRtmp*6;
			tx_pck.CNTm = CNTm_;

			ADC_AnalogWatchdogThresholdsConfig(ADC1,ADC_WDG_H,0);
			ADC_ClearITPendingBit(ADC1,ADC_IT_AWD);
		}
	}
}

void TIM2_IRQHandler(void){

	if(TIM_GetITStatus(TIM2,TIM_IT_CC1)!=RESET){
		TIM2->SR = (uint16_t)~TIM_IT_CC1; //CLEAR FLAG

		ADC_IRQ_cnt ++;
		if(ADC_IRQ_cnt >12){ // No sensor update, reset RPMs.. raise some flags...
			CCR1r = RPM2CNT(100)/6;
			tx_pck.flag = MOTOR_STALL;
			tx_pck.CNTm *= 2;
		}

		CCR1r = SATUR2(CCR1r,RPM2CNT(40000)/6,RPM2CNT(100)/6); //saturate from 50 to 40k RPM, Highr RPM -> Low cnts. Maximum reasonable CCR1r is to guarantee update of CCR, even in RPMr = 0
		TIM2->CCR1 += CCR1r;

		if(CNTr_ < RPM2CNT(100)){ //Was RPMr_ > 100
			switch (Pstate){

			// -----------------PMOS LOGIC-------------------
			//!!!  AU(0) will turn the UP PMOS ON.. obviously
			// -----------------------------------------------
			case 1:
				Led2(1);
				BD_ptr(PWMval);
				AU_ptr(0);
				CD_ptr(0);
				AD_ptr(0);
				CU_ptr(1);
				BU_ptr(1);
				Pstate++;
				Led2(0);
				break;
			case 2:
				CD_ptr(PWMval);
				AU_ptr(0);
				AD_ptr(0);
				BD_ptr(0);
				BU_ptr(1);
				CU_ptr(1);
				Pstate++;
				break;
			case 3:
				CD_ptr(PWMval);
				BU_ptr(0);
				AD_ptr(0);
				BD_ptr(0);
				AU_ptr(1);
				CU_ptr(1);
				Pstate++;
				break;
			case 4:
				AD_ptr(PWMval);
				BU_ptr(0);
				BD_ptr(0);
				CD_ptr(0);
				AU_ptr(1);
				CU_ptr(1);
				Pstate++;
				break;
			case 5:
				AD_ptr(PWMval);
				CU_ptr(0);
				BD_ptr(0);
				CD_ptr(0);
				AU_ptr(1);
				BU_ptr(1);
				Pstate++;
				break;
			case 6:
				BD_ptr(PWMval);
				CU_ptr(0);
				AD_ptr(0);
				CD_ptr(0);
				AU_ptr(1);
				BU_ptr(1);
				Pstate=1;
				break;
			}
		}
		else{
			AU(1);
			BU(1);
			CU(1);
			AD(0);
			BD(0);
			CD(0);
		}
	}
}


void delay_us(uint32_t delay){
	volatile uint32_t start=toc;
	delay=48*delay;
	while((toc-start)<delay);
}

