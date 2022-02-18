/* hw_conf.h
 *
 *  Created on: Jul 6, 2012
 *      Author: mobintu
 */

// define USE_STDPERIPH_DRIVER here to enable eclipse indexing of the stdperiph_driver


#ifndef HW_CONF_H_
#define HW_CONF_H_

#include "stm32f4xx.h"
#include "usbd_cdc_core.h"
#include "usbd_cdc.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "math.h"
uint16_t cdc_DataTx   (uint8_t* Buf, uint32_t Len,uint32_t timeout);



#define loopFreq 100

#define tic		TIM2->CNT=0
#define toc		TIM2->CNT

#define TIMEDWHILE(cond,statOk,timeout) {\
		volatile u32 end=toc+timeout;\
		while(cond && statOk && (toc<end));\
		statOk=(toc<end) && statOk;\
}

#define Led1(x) (x==0)?(GPIOB->BSRRH=GPIO_Pin_12):(GPIOB->BSRRL=GPIO_Pin_12)
#define Led2(x) (x==0)?(GPIOB->BSRRH=GPIO_Pin_2):(GPIOB->BSRRL=GPIO_Pin_2)


#define SATUR(x,Lim) (x>Lim)?(Lim):((x<-Lim)?(-Lim):x)
#define SATUR2(x,Low,High) (x>High)?(High):((x<Low)?(Low):x)


#pragma pack(push)
#pragma pack(1)
typedef  struct{
	u8 c;
	u8 stat;
	u32 t;
	s16 GyrRaw[3];
	s16 MagRaw[3];
	s16 AccRaw[3];

	float TempRaw;
	float Gyr[3];
	float Acc[3];
	float Mag[3];
	float CalibGyr[3];
	float CalibAcc[3];
	float CalibMag[3];
}sensorDat;
sensorDat sens;
#pragma pack(pop)

// 9DoF Sensor Section
//8Bit addresses
#define XM_addr 0x3B
#define Gy_addr	0xD7

void LedInit(void);
void GenTimInit(void);
void sens9DInit(void);
u8 updGyro(void);
u8 updAcc(void);
u8 updMag(void);
u8 updTemp(void);

void calibSens(void);
void scaleSens(void);

u8 I2C_read(u8 addr,u8 reg,u8 *buf,u8 len);
u8 I2C_write(u8 addr,u8 reg,u8 *buf,u8 len);


void initHw(void);
char *u16toa(u16 n, char *s, u8 b, u8 ra);
void delay_us(u32 delay);

#endif /* HW_CONF_H_ */
