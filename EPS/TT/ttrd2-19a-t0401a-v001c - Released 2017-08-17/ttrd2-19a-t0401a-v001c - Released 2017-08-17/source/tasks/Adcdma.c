#include "Adcdma.h"

void ADC1_External_Init(void)
{
// typedefs declaration  
	GPIO_InitTypeDef GPIO_InitStructure;
	// Enable ADC clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//ADC1 IN0 connected to PA0 Pin
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//Activate the GPIO configurations
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  uint32_t              ADC_ref;
 
   //
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	 // Set up ADC in independent mode
   ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
   ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
   ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
   ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
   ADC_CommonInit(&ADC_CommonInitStructure);
	// Apply reset values
   ADC_StructInit(&ADC_InitStructure);
   ADC_CommonStructInit(&ADC_CommonInitStructure);
	 
   // Enable ADC1 
   ADC_Cmd(ADC1, ENABLE); 
	 // Channel 1,3
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_1,  1,  ADC_SampleTime_15Cycles);
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_4,  2,  ADC_SampleTime_15Cycles);
	 ADC_ContinuousModeCmd(ADC1,DISABLE);

   // Store the register configuration
   REG_CONFIG_CHECKS_ADC1_Store();

   // Set up timeout mechanism
   TIMEOUT_T3_USEC_Init();		 
}

/*----------------------------------------------------------------------------*-

  ADC1_Update()
 RETURN VALUE:
     RETURN_NORMAL_STATE (fixed).

-*----------------------------------------------------------------------------*/
uint32_t ADC1_External_Update(void)
 {
   // Take A1,A4 reading
	
   uint32_t reading1= ADC1_Read_Channel(ADC_Channel_1,ADC_SampleTime_480Cycles);
	 UART2_BUF_O_Write_String_To_Buffer("A1: ");
	 UART2_BUF_O_Write_Number04_To_Buffer(reading1);
   uint32_t reading3= ADC1_Read_Channel(ADC_Channel_4,ADC_SampleTime_480Cycles);
	 	 
	 
	 UART2_BUF_O_Write_String_To_Buffer("\nA4: ");
	 UART2_BUF_O_Write_Number04_To_Buffer(reading3);
	 UART2_BUF_O_Write_String_To_Buffer("\n");

   // Always return this state (no backup-task options)
   return RETURN_NORMAL_STATE;
}