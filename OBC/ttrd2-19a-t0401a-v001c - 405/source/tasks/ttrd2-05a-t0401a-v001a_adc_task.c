/*----------------------------------------------------------------------------*-

  ttrd2-05a-t0401a-v001a_adc_task.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
  Simple ADC library for STM32F401RE.
  
  Designed to use internal temperature sensor.

  See 'ERES2' (Chapter 5) for further information.

-*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*-

  This code is copyright (c) 2014-2017 SafeTTy Systems Ltd.
  
  This code forms part of a Time-Triggered Reference Design (TTRD) that is 
  documented in the following book: 

   Pont, M.J. (2016) 
   "The Engineering of Reliable Embedded Systems (Second Edition)", 
   Published by SafeTTy Systems Ltd. ISBN: 978-0-9930355-3-1.

  Both the TTRDs and the above book ("ERES2") describe patented 
  technology and are subject to copyright and other restrictions.

  This code may be used without charge: [i] by universities and colleges on 
  courses for which a degree up to and including 'MSc' level (or equivalent) 
  is awarded; [ii] for non-commercial projects carried out by individuals 
  and hobbyists.

  Any and all other use of this code and / or the patented technology 
  described in ERES2 requires purchase of a ReliabiliTTy Technology Licence:
  http://www.safetty.net/technology/reliabilitty-technology-licences

  Please contact SafeTTy Systems Ltd if you require clarification of these 
  licensing arrangements: http://www.safetty.net/contact

   CorrelaTTor, DuplicaTTor, MoniTTor, PredicTTor, ReliabiliTTy, SafeTTy, 
   SafeTTy Systems and WarranTTor are registered trademarks or trademarks 
   of SafeTTy Systems Ltd in the UK and other countries.

-*----------------------------------------------------------------------------*/

// Processor Header
#include "../main/main.h"

// Support for UART2 / USB reporting
//#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_adc_task.h"

// ------ Private constants --------------------------------------------------

// Range of expected reference voltage values (raw ADC1 readings)
#define ADC1_REF_MIN (1000)
#define ADC1_REF_MAX (2200)
#define ADC1_REF_CHANGE (10)
#define TEMP_REF_CHANGE (20)

// Values from STM32F401RE datasheet
#define ADC_TEMPERATURE_V25       (760)  // mV 
#define ADC_TEMPERATURE_AVG_SLOPE (2500) // mV / degrees Celsius 

// ------ Private Duplicated Variables ('File Global') -----------------------

// The ADC reference value reading (raw ADC reading) 
static uint32_t ADC_ref_v_g;
static uint32_t ADC_ref_v_ig;

// The internal temperature measured via the ADC (raw ADC reading)
static uint32_t ADC1_temperature_raw_g;
static uint32_t ADC1_temperature_raw_ig;

// ------ Private function prototypes ----------------------------------------

                                      

/*----------------------------------------------------------------------------*-

  ADC1_Init()

  Set up ADC1.
   
  PARAMETERS:
     None.
   
  LONG-TERM DATA:
     Stored ADC register configuration: 
     see REG_CONFIG_CHECKS_ADC1_Store();

  MCU HARDWARE:
     ADC1.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     Checks that reference voltage is in expected range
     - Calls PROCESSOR_Perform_Safe_Shutdown() if it is not.     

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void ADC1_Init(void)
   {
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
   ADC_InitStructure.ADC_ScanConvMode = DISABLE;
   ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
   ADC_InitStructure.ADC_ExternalTrigConvEdge = 0;
   ADC_InitStructure.ADC_ExternalTrigConv = 0;
   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
   ADC_InitStructure.ADC_NbrOfConversion = 1;
   ADC_Init(ADC1, &ADC_InitStructure);
  
   // Enable VREF_INT & Temperature channel 
   ADC_TempSensorVrefintCmd(ENABLE);
   
   // Enable ADC1 
   ADC_Cmd(ADC1, ENABLE); 

   // Store the register configuration
   REG_CONFIG_CHECKS_ADC1_Store();

   // Set up timeout mechanism
   TIMEOUT_T3_USEC_Init();

  // The temperature sensor is connected to Channel 16 
  // VREF_INT is connected to Channel 17. 
  // In both cases there is a sample time of 480 ADC clock cycles 
  // and a 12-cycle ramp-up time   

  // Check the reference voltage
  ADC_ref = ADC1_Read_Channel(ADC_Channel_17, ADC_SampleTime_480Cycles);
 
  // Check that the reference value is in range
  if ((ADC_ref < ADC1_REF_MIN) || (ADC_ref > ADC1_REF_MAX))
     {
     // Value is out of range
     // Here we treat as a Fatal Platform Failure
     // OTHER BEHAVIOUR MAY BE MORE APPROPRIATE IN YOUR APPLICATION
     PROCESSOR_Perform_Safe_Shutdown(PFC_ADC_RANGE);
     }  

   // Store the reference value
   ADC_ref_v_g = ADC_ref;
            
   // Read and store the temperature value (raw reading)
   ADC1_temperature_raw_g = ADC1_Read_Channel(ADC_Channel_16, ADC_SampleTime_480Cycles);  
   
   // Note that we don't check the absolute temperature value 
   // (we check only for rapid changes while the system operates)
   
   // Store inverted copies
   ADC_ref_v_ig = ~ADC_ref_v_g;
   ADC1_temperature_raw_ig = ~ADC1_temperature_raw_g;
   }

/*----------------------------------------------------------------------------*-

  ADC1_Update()

  Takes readings from ADC1. 
   
  LONG-TERM DATA:
     ADC_ref_v_g, ADC_ref_v_ig;
     ADC1_temperature_raw_g, ADC1_temperature_raw_ig;

  MCU HARDWARE:
     ADC1.

  PRE-CONDITION CHECKS:
     Check Duplicated Variables.
     Check ADC register configuration.
     Check ADC reference voltage (range check).
     
     If any PreC check fails, we force a shutdown
     [other behaviour may be more appropriate in your system.]

  POST-CONDITION CHECKS:
     Check internal temperature (rate-of-change check).
     
     If any PostC check fails, we force a shutdown
     [other behaviour may be more appropriate in your system.]

  ERROR DETECTION / ERROR HANDLING:
     See PreCs and PostCs.

  WCET:
     Not yet determined.

  BCET:
     Not yet determined.

  RETURN VALUE:
     RETURN_NORMAL_STATE (fixed).

-*----------------------------------------------------------------------------*/
uint32_t ADC1_Update(void)
   {
		 //added
	 ADC_CommonInitTypeDef ADC_CommonInitStructure;
		 
	 ADC_CommonStructInit(&ADC_CommonInitStructure);
	 // Set up ADC in independent mode
   ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
   ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
   ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
   ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
   ADC_CommonInit(&ADC_CommonInitStructure);
		 
	 // Enable VREF_INT & Temperature channel 
   ADC_TempSensorVrefintCmd(ENABLE);
   
   // Enable ADC1 
   ADC_Cmd(ADC1, ENABLE); 
 
		 
		 	 
   // Check pre-conditions (START)

   // Failure of any PreC will force a shutdown in this design
   // Other behaviour may be more appropriate in your system

   // Check data integrity (DVs)
   if ((ADC_ref_v_ig != ~ADC_ref_v_g) ||
       (ADC1_temperature_raw_ig != ~ADC1_temperature_raw_g))
      {
      // Data have been corrupted: Treat here as Fatal Platform Failure
      PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
      }
   
   // Check the ADC1 register configuration
   REG_CONFIG_CHECKS_ADC1_Check();
  
   // Read reference voltage 
   uint32_t ADC_ref = ADC1_Read_Channel(ADC_Channel_17, ADC_SampleTime_480Cycles);
 
   // Check that the reference value is in range 
   // We assume that the reference is stable
   if ((ADC_ref < (ADC_ref_v_g - ADC1_REF_CHANGE)) 
      || (ADC_ref > (ADC_ref_v_g + ADC1_REF_CHANGE)))
      {
      // Value is considered to be out of range
      PROCESSOR_Perform_Safe_Shutdown(PFC_ADC_RANGE);
      }  

   // Check pre-conditions (END)

   // We perform the core task activities if we met the pre-conditions
   // (otherwise we won't get this far)

   // Take temperature reading
   uint32_t ADC_temp = ADC1_Read_Channel(ADC_Channel_16, ADC_SampleTime_480Cycles);      

   // Update stored temperature value
   ADC1_temperature_raw_g = ADC_temp;      
   ADC1_temperature_raw_ig = ~ADC_temp;

   // Before we leave the task, we check the post conditions
   // [Do we have a sensible temperature value?]

   // Check post-conditions (START)
  
   // Is temperature increasing too quickly?  
   if ((ADC_temp > ADC1_temperature_raw_g) 
      && ((ADC_temp - ADC1_temperature_raw_g) > TEMP_REF_CHANGE))
      {
      // Temperature rising too quickly
      // Here we force a shutdown (other behaviour may be more approp. in your design)
      PROCESSOR_Perform_Safe_Shutdown(PFC_ADC_TEMP_CHANGE);
      }       
  
   // Is temperature decreasing too quickly?  
   if ((ADC1_temperature_raw_g > ADC_temp) 
      && ((ADC1_temperature_raw_g - ADC_temp) > TEMP_REF_CHANGE))
      {
      // Temperature falling too quickly
      // Here we force a shutdown (other behaviour may be more approp. in your design)
      PROCESSOR_Perform_Safe_Shutdown(PFC_ADC_TEMP_CHANGE);
      }    

   // Check post-conditions (END)

   // Always return this state (no backup-task options)
   return RETURN_NORMAL_STATE;
   }
   
/*----------------------------------------------------------------------------*-

  ADC1_Get_Temperature_Celsius()

  Reports current CPU temperature (Celsius). 
  
  See data sheet (values are not very precise ...).
   
  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.
   
  MCU HARDWARE:
     ADC1.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  WCET:
     Not yet determined.

  BCET:
     Not yet determined.

  RETURN VALUE:
     Temperature (Celsius).

-*----------------------------------------------------------------------------*/
int32_t ADC1_Get_Temperature_Celsius(void) 
   {
   int32_t  Temp_celsius = 0;
   int32_t  Temp_mv;
   uint32_t ADC_steps_per_volt;
   
   // Check pre-conditions (START)
   
   // Check data integrity (stored temperature value)
   if (ADC1_temperature_raw_ig != ~ADC1_temperature_raw_g)
      {
      // Data have been corrupted: Treat here as Fatal Platform Failure
      PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
      }   
    
   // Check pre-conditions (END)

   // We perform the core task activities only if we meet the pre-conditions

   // Assume 1.2V reference voltage
   ADC_steps_per_volt = (ADC_ref_v_g * 10) / 12;
 
   // Calculate temperature 
   Temp_mv      = ((uint32_t)ADC1_temperature_raw_g * 1000) 
                  / ADC_steps_per_volt;
                  
   Temp_celsius = (Temp_mv - ADC_TEMPERATURE_V25) / 25 + 25;
    
   // Return result  
   return Temp_celsius;
   }      

/*----------------------------------------------------------------------------*-

  ADC1_Read_Channel()

  Read from specified ADC channel. 
   
  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.
   
  MCU HARDWARE:
     ADC1.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  WCET:
     Not yet determined.

  BCET:
     Not yet determined.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
uint32_t ADC1_Read_Channel(const uint32_t CHANNEL, 
                           const uint32_t ADC_SAMPLE_TIME) 
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
          && (COUNTING == (T3 = TIMEOUT_T3_USEC_Get_Timer_State(20))));

   if (T3 == TIMED_OUT)
      {
      // The ADC conversion did not complete within the time interval
      // Here we force a shutdown (other behaviour may be more approp. in your design)
      PROCESSOR_Perform_Safe_Shutdown(PFC_ADC_TIMEOUT);    
      }
 
   // Return result  
   return ADC_GetConversionValue(ADC1);
   }   

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
