#include "raa.h"
#include "ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#include "ttrd2-05a-t0401a-v001a_switch_task.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_adc_task.h"
//#include "stm32f4xx.h"
//#include "stm32f4xx_rcc.h"
//#include "stm32f4xx_gpio.h"
//#include "stm32f4xx_adc.h"

uint32_t ADC1_Read_Channel(const uint32_t CHANNEL,const uint32_t ADC_SAMPLE_TIME) 
{
   uint32_t T3;

   // Configure channel
   ADC_RegularChannelConfig(ADC1, CHANNEL, 1, ADC_SAMPLE_TIME);
 
   // Start the conversion
   ADC_SoftwareStartConv(ADC1);
 
   // Wait for conversion
   // Allow 20 microseconds
   TIMEOUT_T3_USEC_Start();
   while((ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
          && (COUNTING == (T3 = TIMEOUT_T3_USEC_Get_Timer_State(200))));

   if (T3 == TIMED_OUT)
      {
      // The ADC conversion did not complete within the time interval
      // Here we force a shutdown (other behaviour may be more approp. in your design)
      PROCESSOR_Perform_Safe_Shutdown(PFC_ADC_TIMEOUT);    
      }
 
   // Return result  
   return ADC_GetConversionValue(ADC1);
}   


uint32_t Rofa_Task_Update(void)
{ 
UART2_BUF_O_Write_String_To_Buffer("A1: \n");
//uint32_t r=RETURN_NORMAL_STATE;
// if (SWITCH_BUTTON1_Get_State() == BUTTON1_NOT_PRESSED)
//{UART2_BUF_O_Write_String_To_Buffer("Hello World\n");
//  UART2_BUF_O_Send_All_Data();}
//
	//Enable Required Buses
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);


//	//GPIO structure to config the GPIO pins
//	GPIO_InitTypeDef GPIO_InitStructure;
//	//DMA structure to config DMA
//	DMA_InitTypeDef DMA_InitStructure;
//	//ADC cong structures to set ADC configurations
//	ADC_InitTypeDef ADC_InitStructure;
//	ADC_CommonInitTypeDef ADC_CommonInitStructure;


//	// creating variable to save the readings
//	uint16_t ADC1_CH0_Value = 0;


//	//ADC1 IN0 connected to PA0 Pin
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	//Activate the GPIO configurations
//	GPIO_Init(GPIOA, &GPIO_InitStructure);



//	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; // We set the ADC Mode as Independent to work.
//	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; // We have set the prescaler value. Received data will bi divided by 4.
//	ADC_CommonInit(&ADC_CommonInitStructure); //Started the above configurations.

//	ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b; // this means we are going to divide the value by 20mV.
//	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
//	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
//	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
//	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
//	ADC_InitStructure.ADC_NbrOfConversion = 1;
//	ADC_Init(ADC1, &ADC_InitStructure); //Set configuration of ADC1

//	//Enable ADC1
//	ADC_Cmd(ADC1, ENABLE);

//	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &ADC1->DR;
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &ADC1_CH0_Value;
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
//	DMA_InitStructure.DMA_BufferSize = 2;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	//here we select the DMA Stream to use
//	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
//	//Here we start the related DMA
//	DMA_Cmd(DMA2_Stream0, ENABLE);

//	//////////////
//uint32_t ADCreff = ADC1_Read_Channel(ADC_Channel_1, ADC_SampleTime_480Cycles);
////uint32_t ADCreading=500000;
//uint16_t ADCreading=ADC_GetConversionValue(ADC1);
////char data=(char)ADC_GetConversionValue(ADC1);
////UART2_BUF_O_Write_Number04_To_Buffer(ADCreff);
//////UART2_BUF_O_Write__To_Buffer(&data);
////UART2_BUF_O_Send_All_Data();

//UART2_BUF_O_Write_Number04_To_Buffer(ADCreading);
////UART2_BUF_O_Write__To_Buffer(&data);
//UART2_BUF_O_Send_All_Data();
return RETURN_NORMAL_STATE;
}

//			int delaytime = ADC1_CH0_Value * 500;
//			GPIO_SetBits(GPIOB, GPIO_Pin_0);
//			//ADC1_CH0_Value = ADC_GetConversionValue(ADC1);
//			while(delaytime ) delaytime --;
//			//ADC1_CH0_Value = ADC_GetConversionValue(ADC_TypeDef* ADC1);
//			GPIO_ResetBits(GPIOB, GPIO_Pin_0);
//			while(delaytime ) delaytime --;
//			//ADC1_CH0_Value = ADC_GetConversionValue(ADC_TypeDef* ADC1);
	
	

///**
//  ******************************************************************************
//  * @file    main.c
//  * @author  Ac6
//  * @version V1.0
//  * @date    01-December-2013
//  * @brief   Default main function.
//  ******************************************************************************
//*/



////#include "defines.h"
////#include "tm_stm32f4_gpio.h"

//int main(void)
//{ 

	//	}



