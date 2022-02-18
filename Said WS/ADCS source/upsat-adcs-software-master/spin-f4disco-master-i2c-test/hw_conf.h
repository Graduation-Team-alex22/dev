/*
 * hw_conf.h
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





#define tic		TIM2->CNT=0
#define toc		TIM2->CNT



void initHw(void);
char *u16toa(u16 n, char *s, u8 b, u8 ra);
void delay_us(u32 delay);

#endif /* HW_CONF_H_ */
