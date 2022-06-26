#include "Adcdma.h"

/*----------------------------------------------------------------------------*-

  ADC1_External_Init()
 RETURN VALUE:
     RETURN_NORMAL_STATE (fixed).

-*----------------------------------------------------------------------------*/
void ADC1_External_Init(void)
{
  // typedefs declaration  
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
	// Enable ADC clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//ADC1 IN0 connected to PA0, PA4, PA5 Pin
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1  | GPIO_Pin_4  | GPIO_Pin_5  |
	                              GPIO_Pin_6  | GPIO_Pin_10 | GPIO_Pin_11 ;
																//| GPIO_Pin_18 | GPIO_Pin_19 | GPIO_Pin_20 | GPIO_Pin_21;  // for the rest of channels in L152
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//Activate the GPIO configurations
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
  //uint32_t              ADC_ref; % previosely defined in the ttrd ADC task
   
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
	 // Channel 1,4,5,6,10,11 
	 //`18,19,20,21
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_1,  1,  ADC_SampleTime_15Cycles);
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_4,  2,  ADC_SampleTime_15Cycles);
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_5,  3,  ADC_SampleTime_15Cycles);
	 
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_6,   6,  ADC_SampleTime_15Cycles);
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_10,  10,  ADC_SampleTime_15Cycles);
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_11,  11,  ADC_SampleTime_15Cycles);
	 
	 ADC_ContinuousModeCmd(ADC1,DISABLE);
	 
	 //DMA Init
	 ADC_DMACmd(ADC1, ENABLE);
	 
	 ////////////////////ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);

   // Store the register configuration
   REG_CONFIG_CHECKS_ADC1_Store();

   // Set up timeout mechanism ()
   TIMEOUT_T3_USEC_Init();




}

/*----------------------------------------------------------------------------*-

  ADC1_External_Update()
 RETURN VALUE:
     RETURN_NORMAL_STATE (fixed).

-*----------------------------------------------------------------------------*/
uint32_t ADC1_External_Update(void)
 {
   // Take A1,A4,A5 reading
	
   uint32_t reading1= ADC1_Read_Channel(ADC_Channel_1,ADC_SampleTime_480Cycles);
	 UART2_BUF_O_Write_String_To_Buffer("A1: ");
	 UART2_BUF_O_Write_Number04_To_Buffer(reading1);
   
	
	 UART2_BUF_O_Write_String_To_Buffer("\nA4: ");
	 uint32_t reading4= ADC1_Read_Channel(ADC_Channel_4,ADC_SampleTime_480Cycles);
	 UART2_BUF_O_Write_Number04_To_Buffer(reading4);
	 
	 UART2_BUF_O_Write_String_To_Buffer("\nA5: ");
	 uint32_t reading5= ADC1_Read_Channel(ADC_Channel_5,ADC_SampleTime_480Cycles);
	 UART2_BUF_O_Write_Number04_To_Buffer(reading5);
	 
	 UART2_BUF_O_Write_String_To_Buffer("\n");
   UART2_BUF_O_Write_String_To_Buffer("\nA17: ");
	 uint32_t reading17= ADC1_Read_Channel(ADC_Channel_17,ADC_SampleTime_480Cycles);
	 UART2_BUF_O_Write_Number04_To_Buffer(reading17);
	 UART2_BUF_O_Write_String_To_Buffer("\n");
	 
	 
	 UART2_BUF_O_Write_String_To_Buffer("\nA6: ");
	 uint32_t reading6= ADC1_Read_Channel(ADC_Channel_6,ADC_SampleTime_480Cycles);
	 UART2_BUF_O_Write_Number04_To_Buffer(reading6);
	 UART2_BUF_O_Write_String_To_Buffer("\n");
	 
	 UART2_BUF_O_Write_String_To_Buffer("\nPC0: ");
	 uint32_t reading10= ADC1_Read_Channel(ADC_Channel_10,ADC_SampleTime_480Cycles);
	 UART2_BUF_O_Write_Number04_To_Buffer(reading10);
	 UART2_BUF_O_Write_String_To_Buffer("\n");
	 
	 UART2_BUF_O_Write_String_To_Buffer("\nPC1: ");
	 uint32_t reading11= ADC1_Read_Channel(ADC_Channel_11,ADC_SampleTime_480Cycles);
	 UART2_BUF_O_Write_Number04_To_Buffer(reading11);
	 UART2_BUF_O_Write_String_To_Buffer("\n");
	 
	 /* 
	 for stm32l152 additional channels of adc
	  UART2_BUF_O_Write_String_To_Buffer("\n");
   UART2_BUF_O_Write_String_To_Buffer("\nA17: ");
	 uint32_t reading18= ADC1_Read_Channel(ADC_Channel_18,ADC_SampleTime_480Cycles);
	 UART2_BUF_O_Write_Number04_To_Buffer(reading18);
	 UART2_BUF_O_Write_String_To_Buffer("\n");
	 
	 
	 UART2_BUF_O_Write_String_To_Buffer("\nA6: ");
	 uint32_t reading19= ADC1_Read_Channel(ADC_Channel_19,ADC_SampleTime_480Cycles);
	 UART2_BUF_O_Write_Number04_To_Buffer(reading19);
	 UART2_BUF_O_Write_String_To_Buffer("\n");
	 
	 UART2_BUF_O_Write_String_To_Buffer("\nPC0: ");
	 uint32_t reading20= ADC1_Read_Channel(ADC_Channel_20,ADC_SampleTime_480Cycles);
	 UART2_BUF_O_Write_Number04_To_Buffer(reading20);
	 UART2_BUF_O_Write_String_To_Buffer("\n");
	 
	 UART2_BUF_O_Write_String_To_Buffer("\nPC1: ");
	 uint32_t reading21= ADC1_Read_Channel(ADC_Channel_21,ADC_SampleTime_480Cycles);
	 UART2_BUF_O_Write_Number04_To_Buffer(reading21);
	 UART2_BUF_O_Write_String_To_Buffer("\n");
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 */
	 
	 
	 
	 
	 
   // Always return this state (no backup-task options)
   return RETURN_NORMAL_STATE;
}

 
