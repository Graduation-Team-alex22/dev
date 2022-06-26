#ifndef _ADCDMA_H
#define _ADCDMA_H 

#include "stm32f4xx_adc.h"
#include "../hsi_reg_config_checks/ttrd2-05a-t0401a-v001a_reg_conf_chk_adc1.h"
#include "../support_functions/ttrd2-05a-t0401a-v001a_timeout_t3.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_adc_task.h"

#define ADC_SOFTWARE_START               ((uint32_t)0x00000010)

void ADC1_External_Init(void);
uint32_t ADC1_External_Update(void);
#endif
