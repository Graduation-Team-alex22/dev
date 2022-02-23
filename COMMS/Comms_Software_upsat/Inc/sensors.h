#ifndef SENSORS_H_
#define SENSORS_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

/* ADT7420 address, IC2*/
#define ADT7420_ADDRESS		0x48
/* ADT7420 default ID */
#define ADT7420_DEFAULT_ID	0xCB
/* ADT7420 registers */
#define ADT7420_REG_TEMP_MSB	0x00	/* Temperature value MSB */
#define ADT7420_REG_TEMP_LSB	0x01	/* Temperature value LSB */
#define ADT7420_REG_STATUS	0x02	/* Status */
#define ADT7420_REG_CONFIG	0x03	/* Configuration */
#define ADT7420_REG_T_HIGH_MSB	0x04	/* Temperature HIGH setpoint MSB */
#define ADT7420_REG_T_HIGH_LSB	0x05	/* Temperature HIGH setpoint LSB */
#define ADT7420_REG_T_LOW_MSB	0x06	/* Temperature LOW setpoint MSB */
#define ADT7420_REG_T_LOW_LSB	0x07	/* Temperature LOW setpoint LSB */
#define ADT7420_REG_T_CRIT_MSB	0x08	/* Temperature CRIT setpoint MSB */
#define ADT7420_REG_T_CRIT_LSB	0x09	/* Temperature CRIT setpoint LSB */
#define ADT7420_REG_HIST	0x0A	/* Temperature HYST setpoint */
#define ADT7420_REG_ID		0x0B	/* ID */
#define ADT7420_REG_RESET	0x2F	/* Software reset */
#define ADT7420_TIMEOUT		100
/* ADT7420 configure */
#define ADT7420_16BIT		0x80
#define ADT7420_OP_MODE_1_SPS	0x40
#define ADT7420_OP_MODE_CONT_CONV 0x00


/* STM32 internal temperature sensor settings */

#define STM32_TS_SLOPE		2.5

/**
 * The analog voltage supply in mV where the calibration was performed
 */
#define STM32_VDDA_CAL		3300

#define STM32_ADC_SAMPLES_NUM	6
#define STM32_ADC_BIT_NUM	12
#define STM32_TS_REF_mV		760.0
#define STM32_TS_REF_C		25.0

void
init_adt7420 ();

float
update_adt7420 ();

uint16_t
get_raw_adt7420 ();

float
get_temp_adt7420 ();

uint32_t
get_timestamp_adt7420 ();

int32_t
cc1120_get_temp(int8_t *temperature, uint32_t timeout_ms);

int32_t
stm32_get_temp(ADC_HandleTypeDef *h, float *temperature);

#endif
