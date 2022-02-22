#include "sensors.h"
#include "stm32f4xx_hal.h"
#include "cc112x_spi.h"
#include "status.h"
#include "cc_rx_init.h"
#include <math.h>
#include <string.h>

extern I2C_HandleTypeDef hi2c1;
static uint8_t tmp_buf[256];
/* Append plus  one word for the EOC (End-Of-Conversion) sample*/
static uint32_t adc_vals[STM32_ADC_SAMPLES_NUM + 1];

struct _temp_sensor
{
  /* ADT7420 Temperature */
  uint16_t temp_raw;
  float temp_c;
  uint32_t timestamp;
};

static struct _temp_sensor temp_sensor =
  { .temp_raw = 0, .temp_c = 0.0, .timestamp = 0 };

/* Init ADT7420 */
void
init_adt7420 ()
{
  uint8_t i2c_temp[2];

  /* Set to 0 all state values */
  temp_sensor.temp_raw = 0;
  temp_sensor.temp_c = 0;

  i2c_temp[0] = 0;
  i2c_temp[1] = 0;
  HAL_I2C_Mem_Read (&hi2c1, ( ADT7420_ADDRESS << 1), ADT7420_REG_ID, 1,
            i2c_temp, 1, ADT7420_TIMEOUT);
  if (i2c_temp[0] != ADT7420_DEFAULT_ID) {
    ;
  }
  HAL_Delay (10);
  /* Set operation mode */
  i2c_temp[0] = ADT7420_16BIT | ADT7420_OP_MODE_1_SPS;
  i2c_temp[1] = 0x00;
  HAL_I2C_Mem_Write (&hi2c1, ( ADT7420_ADDRESS << 1), ADT7420_REG_CONFIG, 1,
             i2c_temp, 1, ADT7420_TIMEOUT);
}

/* Update values for adt7420 */
float
update_adt7420 ()
{
  uint8_t lsb, msb;
  uint8_t i2c_temp[2];

  HAL_StatusTypeDef resM;
  HAL_StatusTypeDef resL;
  /* Get Temperature */

  i2c_temp[0] = 0;
  i2c_temp[1] = 0;
  HAL_Delay (10);

  resM = HAL_I2C_Mem_Read (&hi2c1, ( ADT7420_ADDRESS << 1), ADT7420_REG_TEMP_MSB, 1,
            i2c_temp, 1, ADT7420_TIMEOUT);
  msb = i2c_temp[0];
  resL = HAL_I2C_Mem_Read (&hi2c1, ( ADT7420_ADDRESS << 1), ADT7420_REG_TEMP_LSB, 1,
            i2c_temp, 1, ADT7420_TIMEOUT);
  lsb = i2c_temp[0];

  if (resM != HAL_OK || resL != HAL_OK) {
    return 0.0;
  }

  temp_sensor.temp_raw = msb << 8;
  temp_sensor.temp_raw |= lsb;
  if ((temp_sensor.temp_raw >> 15 & 0x1) == 0) {
    temp_sensor.temp_c = (float) temp_sensor.temp_raw / 128.0;
  }
  else {
    temp_sensor.temp_c = ((float) (temp_sensor.temp_raw) - 65536) / 128.0;
  }
  temp_sensor.timestamp = HAL_GetTick();

  return temp_sensor.temp_c;
}

uint16_t
get_raw_adt7420 ()
{
    return temp_sensor.temp_raw;
}

float
get_temp_adt7420 ()
{
  return temp_sensor.temp_c;
}

uint32_t
get_timestamp_adt7420 ()
{
  return temp_sensor.timestamp;
}

int32_t
cc1120_get_temp(int8_t *temperature, uint32_t timeout_ms)
{
  HAL_StatusTypeDef ret;
  uint8_t reg_val;
  uint8_t timeout = 1;
  uint32_t start_tick;
  uint32_t adc_val = 0x0;
  /* String to put radio in debug mode */
  uint8_t tx_buf[19] =
    { BURST_TXFIFO, 0x0F, 0x28, 0x02, 0x90, 0x42, 0x1B, 0x7E, 0x1F, 0xFE, 0xCD,
	0x06, 0x1B, 0x0E, 0xA1, 0x0E, 0xA4, 0x00, 0x3F };
  /* Constants for temperature calculation */
  const float a = -3.3;
  const float b = 992;
  const float c = -2629.9;

  /* Perform the proper configuration of the CC1120 for temperature reading */
  rx_temp_sensor_register_config();
  rx_manual_calibration();

  /* Set the CC1120 in RX mode */
  cc_rx_cmd(SRX);

  /* Check whenever the chip got in RX mode */
  start_tick = HAL_GetTick();
  while(HAL_GetTick() - start_tick < timeout_ms){
    cc_rx_rd_reg(MARCSTATE, &reg_val);
    if(reg_val == 0x6D){
      timeout = 0;
      break;
    }
  }

  if(timeout){
    cc_rx_cmd(SRES);
    rx_register_config();
    return COMMS_STATUS_TIMEOUT;
  }

  /* Set the chip in debug Mode */
  ret = cc_rx_spi_write_fifo(tx_buf, tmp_buf, sizeof(tx_buf));
  if(ret){
    cc_rx_cmd(SRES);
    rx_register_config();
    return -2;
  }

  cc_rx_wr_reg(BIST, 0x1);
  cc_rx_cmd(SIDLE);
  cc_rx_wr_reg(WOR_EVENT0_LSB, 0x1F);
  cc_rx_cmd(SXOFF);

  cc_rx_rd_reg(MARCSTATE, &reg_val);

  /* Check whenever the data from the channel filter are valid */
  start_tick = HAL_GetTick();
  timeout = 1;
  while(HAL_GetTick() - start_tick < timeout_ms){
    cc_rx_rd_reg(CHFILT_I2, &reg_val);
    if(reg_val & 0x8){
      timeout = 0;
      break;
    }
  }

  if(timeout){
    cc_rx_cmd(SRES);
    rx_register_config();
    return -1;
  }
  cc_rx_rd_reg(CHFILT_I2, &reg_val);
  adc_val = ((uint32_t)reg_val) << 16;
  cc_rx_rd_reg(CHFILT_I1, &reg_val);
  adc_val |= (((uint32_t)reg_val) << 8) & 0x0000FF00;
  cc_rx_rd_reg(CHFILT_I0, &reg_val);
  adc_val |= ((uint32_t)reg_val) & 0xFF;

  *temperature = (int8_t) ((-b
      + sqrtf (powf (b, 2.0f) - (4.0 * a * (c - adc_val)))) / (2.0 * a));

  /* Reset the Radio at the initial setup again */
  cc_rx_cmd(SRES);
  rx_register_config();
  rx_manual_calibration();
  return COMMS_STATUS_OK;
}

/**
 * Retrieve the temperature from the internal STM32 sensor
 * @param h pointer to the ADC subsystem
 * @param temperature memory to store the temperature reading
 * @return COMMS_STATUS_OK on success or COMMS_STATUS_NO_DATA in case of
 * invalid ADC pointer or COMMS_STATUS_DMA_ERROR in case of a DMA error
 */
int32_t
stm32_get_temp(ADC_HandleTypeDef *h, float *temperature)
{
  size_t i;
  HAL_StatusTypeDef ret;
  float mean_temp = 0.0;
  float b;
  const float adc_resolution = VDD_VALUE / ((float) (1 << STM32_ADC_BIT_NUM));

  if(h == NULL){
    return COMMS_STATUS_NO_DATA;
  }

  /* Create the linear equation */
  b = STM32_TS_REF_mV - STM32_TS_SLOPE * STM32_TS_REF_C;

  /* Get a batch of ADC measurements */
  memset(adc_vals, 0, (STM32_ADC_SAMPLES_NUM+1) * sizeof(uint32_t));
  ret = HAL_ADC_Start_DMA(h, adc_vals, STM32_ADC_SAMPLES_NUM);
  if(ret != HAL_OK){
    return COMMS_STATUS_DMA_ERROR;
  }

  HAL_Delay(50);
  HAL_ADC_Stop_DMA(h);

  /* Calculate the mean */
  for(i = 0; i < STM32_ADC_SAMPLES_NUM; i++){
    mean_temp += adc_vals[i];
  }
  mean_temp /= (float) STM32_ADC_SAMPLES_NUM;

  mean_temp = ((mean_temp * adc_resolution) - b) / STM32_TS_SLOPE;
  *temperature = mean_temp;
  return COMMS_STATUS_OK;
}
