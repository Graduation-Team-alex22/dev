#include "ADC1_task.h"

/* Delay for ADC stabilization time.                                        */
/* Maximum delay is 1us (refer to device datasheet, parameter tSTAB).       */
/* Unit: us                                                                 */
#define ADC_STAB_DELAY_US               ((uint32_t) 3U)
//	   /* Delay for ADC stabilization time */
//     /* Compute number of CPU cycles to wait for */
//    counter = (ADC_STAB_DELAY_US * (SystemCoreClock / 1000000U));
//    while(counter != 0U)
//    {
//      counter--;
//    }


//#include "obc.h"


void     ADC1_vbat_Init(void)
{
   ADC_InitTypeDef       ADC_InitStructure;
   ADC_CommonInitTypeDef ADC_CommonInitStructure;
 
   // Apply reset values
   ADC_StructInit(&ADC_InitStructure);
   ADC_CommonStructInit(&ADC_CommonInitStructure);
 
   // Enable ADC clock 
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
 
   // Set up ADC in independent mode
   ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
   ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
   ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
   ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
   ADC_CommonInit(&ADC_CommonInitStructure);
 
   // Set up ADC1: 12-bit; single-conversion 
   ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
   ADC_InitStructure.ADC_ScanConvMode = DISABLE;
   ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
   ADC_InitStructure.ADC_ExternalTrigConvEdge = 0;
   ADC_InitStructure.ADC_ExternalTrigConv = 0;
   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
   ADC_InitStructure.ADC_NbrOfConversion = 1;
   ADC_Init(ADC2, &ADC_InitStructure);
   
   // Enable ADC2 
   //ADC_Cmd(ADC2, ENABLE); 
	 
	 // Enable VRBAT
	 ADC_VBATCmd(ENABLE) ;
	 // Configure channel
	 ADC_RegularChannelConfig(ADC2, ADC_Channel_Vbat, 1, ADC_SampleTime_3Cycles);	 
   // Set up timeout mechanism
   TIMEOUT_T3_USEC_Init();

	 
}

uint32_t ADC1_vbat_Update(void)
{
	__IO uint32_t counter = 0U;
	 uint32_t T3;

	
	  if(obc_data.adc_flag == false) {
			ADC_Cmd(ADC2, ENABLE);
			TIMEOUT_T3_USEC_Start();
   while((COUNTING == (T3 = TIMEOUT_T3_USEC_Get_Timer_State(20))));

   if (T3 == TIMED_OUT)
      {
      // The ADC conversion did not complete within the time interval
      // Here we force a shutdown (other behaviour may be more approp. in your design)
      PROCESSOR_Perform_Safe_Shutdown(PFC_ADC_TIMEOUT);    
      }
			ADC_SoftwareStartConv(ADC2);
      obc_data.adc_flag = true;			 
    }
      
    else if(obc_data.adc_flag == true) {
      obc_data.vbat = (uint16_t) 	ADC_GetConversionValue(ADC2);
			ADC_Cmd(ADC2, DISABLE);
      obc_data.adc_flag = false;
    }

	return RETURN_NORMAL_STATE;
}
 