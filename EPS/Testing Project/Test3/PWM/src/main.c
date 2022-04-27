/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"
//#include "defines.h"
//#include "tm_stm32f4_gpio.h"

int main(void)
{ //Enable Required Buses
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);


	//GPIO structure to config the GPIO pins
	GPIO_InitTypeDef GPIO_InitStructure;
	//DMA structure to config DMA
	DMA_InitTypeDef DMA_InitStructure;
	//ADC cong structures to set ADC configurations
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;


	// creating variable to save the readings
	uint16_t ADC1_CH0_Value = 0;


	//ADC1 IN0 connected to PA0 Pin
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//Activate the GPIO configurations
	GPIO_Init(GPIOA, &GPIO_InitStructure);



	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; // We set the ADC Mode as Independent to work.
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; // We have set the prescaler value. Received data will bi divided by 4.
	ADC_CommonInit(&ADC_CommonInitStructure); //Started the above configurations.

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b; // this means we are going to divide the value by 20mV.
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure); //Set configuration of ADC1

	//Enable ADC1
	ADC_Cmd(ADC1, ENABLE);

	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &ADC1->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &ADC1_CH0_Value;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 2;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	//here we select the DMA Stream to use
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	//Here we start the related DMA
	DMA_Cmd(DMA2_Stream0, ENABLE);

	//////////////

	uint16_t ADC_GetConversionValue(ADC_TypeDef* ADC1)
	{
	  /* Check the parameters */
	  assert_param(IS_ADC_ALL_PERIPH(ADC1));
	  /* Return the selected ADC conversion value */
	  return (uint16_t) ADC1->DR;
	}

	while(1)
		{
			int delaytime = ADC1_CH0_Value * 500;
			GPIO_SetBits(GPIOB, GPIO_Pin_0);
			//ADC1_CH0_Value = ADC_GetConversionValue(ADC1);
			while(delaytime ) delaytime --;
			//ADC1_CH0_Value = ADC_GetConversionValue(ADC_TypeDef* ADC1);
			GPIO_ResetBits(GPIOB, GPIO_Pin_0);
			while(delaytime ) delaytime --;
			//ADC1_CH0_Value = ADC_GetConversionValue(ADC_TypeDef* ADC1);

		}



}

