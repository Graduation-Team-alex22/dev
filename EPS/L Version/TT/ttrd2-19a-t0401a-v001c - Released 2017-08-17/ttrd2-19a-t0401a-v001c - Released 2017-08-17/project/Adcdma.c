#include "Adcdma.h"

/*GPIO INIT
Pins A0


*/
GPIO_InitTypeDef GPIO_InitStructure;
	//ADC1 IN0 connected to PA0 Pin
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//Activate the GPIO configurations
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	void ADC1_Init(void){
	ADC_InitTypeDef       ADC_InitStructure;
   ADC_CommonInitTypeDef ADC_CommonInitStructure;
   uint32_t              ADC_ref;
 
   // Apply reset values
   ADC_StructInit(&ADC_InitStructure);
   ADC_CommonStructInit(&ADC_CommonInitStructure);
 
   // Enable ADC clock 
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
 
   // Set up ADC in independent mode
   ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
   ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
   ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
   ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
   ADC_CommonInit(&ADC_CommonInitStructure);
 
   // Set up ADC1: 12-bit; single-conversion 
   ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
   ADC_InitStructure.ADC_ScanConvMode = ENABLE;
   ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
   ADC_InitStructure.ADC_ExternalTrigConvEdge = 0;
   ADC_InitStructure.ADC_ExternalTrigConv = ADC_SOFTWARE_START;
   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
   ADC_InitStructure.ADC_NbrOfConversion = 14;
   ADC_Init(ADC1, &ADC_InitStructure);
	
	
	
	
	
}