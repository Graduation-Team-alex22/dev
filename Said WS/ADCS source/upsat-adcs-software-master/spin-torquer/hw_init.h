/*
 * hw_init.h
 *
 *  Created on: Aug 22, 2014
 *      Author: mobintu
 */

#ifndef HW_INIT_H_
#define HW_INIT_H_

#include "stm32f0xx.h"
#include "math.h"



#define FM_BOARD

#ifdef FM_BOARD
#define HallAp_Ch ADC_Channel_0
#define HallAp_WD_Ch ADC_AnalogWatchdog_Channel_0

#define HallCp_Ch ADC_Channel_3
#define HallCp_WD_Ch ADC_AnalogWatchdog_Channel_3

#else

#define HallAp_Ch ADC_Channel_2
#define HallAp_WD_Ch ADC_AnalogWatchdog_Channel_2
#define HallCp_Ch ADC_Channel_0
#define HallCp_WD_Ch ADC_AnalogWatchdog_Channel_0

#endif

//#define DBG
#ifdef DBG
#define Led1(x) ((x)==0)?(GPIOB->BRR=GPIO_Pin_6):(GPIOB->BSRR=GPIO_Pin_6)
#define Led2(x) ((x)==0)?(GPIOB->BRR=GPIO_Pin_7):(GPIOB->BSRR=GPIO_Pin_7)
#else
#define Led1(x) (x==0)
#define Led2(x) (x==0)
#endif

//PA8/T1C1
static inline void AU(uint32_t x){ ((x)==0)?(GPIOA->BRR=GPIO_Pin_8):(GPIOA->BSRR=GPIO_Pin_8);}
//PA10/T1C3
static inline void CU(uint32_t x){ ((x)==0)?(GPIOA->BRR=GPIO_Pin_10):(GPIOA->BSRR=GPIO_Pin_10);}
//PA9/T1C2
static inline void BU(uint32_t x){ ((x)==0)?(GPIOA->BRR=GPIO_Pin_9):(GPIOA->BSRR=GPIO_Pin_9);}


//PA7/T1C1N
static inline void AD(uint32_t x){	TIM1->CCR1=(x);}
//PB1/T1C3N
static inline void CD(uint32_t x){	TIM1->CCR3=(x);}
//PB0/T1C2N
static inline void BD(uint32_t x){	TIM1->CCR2=(x);}


#define toc		TIM2->CNT

#define CNT2RPM(x) 			48000000*15/(x) //CNT in 1/48 us, 4 pairs of poles
#define RPM2CNT(x)			CNT2RPM(x)

#define M_PWM_TickPeriod 1000 //used -1 for period


#define I2C_Address 		3
#define pck_Payload 		16
#define I2C_Idle_Mode 		0
#define I2C_TX_Mode 		1
#define I2C_RX_Mode 		2
#define Pck_Flag_NewAndReady 	1<<7

// MOTOR In sync to bridge pulses, but not locked  yet to reference RPM
#define MOTOR_INSYNC 	1
// MOTOR In Synchronous mode to  reference RPM
#define MOTOR_LOCKED 	2
// MOTOR stalled (that is bad, unless RPMr <100)
#define MOTOR_STALL  	3

//#endif

#define SATUR(x,Lim) ((x)>(Lim))?(Lim):(((x)<-(Lim))?(-(Lim)):(x))
#define SATUR2(x,Low,High) ((x)>(High))?(High):(((x)<(Low))?(Low):(x))


// KEEP THEM PACKED..!
typedef volatile struct {
	uint32_t 	flag;
	int32_t 	CNTr;
	uint32_t 	rampTime;
	uint32_t 	crc;
}I2C_RX_pck;

typedef volatile struct {
	uint32_t 	flag;
	int32_t 	CNTm;
	uint32_t 	dummy;
	uint32_t 	crc;
}I2C_TX_pck;


//extern volatile int32_t RPMr_;
extern volatile int32_t CNTr_;
extern volatile int32_t CNTm_;
extern volatile int32_t direction_;
extern volatile int32_t state;
extern int32_t io_mtx;
extern I2C_RX_pck rx_pck;
extern I2C_TX_pck tx_pck;



void hw_init(void);
void I2C_init(void);

void delay_us(uint32_t delay);
int32_t setDirection(int32_t CNTr);

#endif /* HW_INIT_H_ */
