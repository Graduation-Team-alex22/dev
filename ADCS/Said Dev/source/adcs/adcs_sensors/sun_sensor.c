#include "sun_sensor.h"



/*
_adcs_sensor_status init_sun_sensor(sun_sensor_t *pSunSensor_t){
	uint8_t spi_in_tmp[4], spi_out_tmp[4];

    memset(pSunSensor_t->v_sun_raw, 0, 4);
    memset(pSunSensor_t->v_sun, 0, 4);
    memset(pSunSensor_t->sun_rough, 0, 3);
    memset(pSunSensor_t->sun_fine, 0, 3);
    memset(pSunSensor_t->sun_xyz, 0, 3);
    pSunSensor_t->sun_status = DEVICE_NORMAL;

    memset(spi_in_tmp, 0, 4);
    spi_in_tmp[0] = AD7682_CFG | AD7682_INCC | AD7682_CH1 | AD7682_BW;
    spi_in_tmp[1] = AD7682_REF | AD7682_SEQ | AD7682_RB;
    memset(spi_out_tmp, 0, 4);

    GPIO_ResetBits(CNV_GPIO_Port, CNV_Pin);
    //HAL_Delay(1);
    GPIO_SetBits(CNV_GPIO_Port, CNV_Pin);
    //HAL_Delay(6);
    GPIO_ResetBits(CNV_GPIO_Port, CNV_Pin);
    //HAL_Delay(1);
    if (HAL_SPI_TransmitReceive(&hspi1, spi_in_tmp, spi_out_tmp, 5,
    AD7682_TIMEOUT) != HAL_OK) {
        sensors->sun.sun_status = DEVICE_ERROR;
        return DEVICE_ERROR;
    }

    //HAL_Delay(1);
    GPIO_ResetBits(CNV_GPIO_Port, CNV_Pin);
    //HAL_Delay(1);
    GPIO_SetBits(CNV_GPIO_Port, CNV_Pin);
    //HAL_Delay(6);
    GPIO_ResetBits(CNV_GPIO_Port, CNV_Pin);
    //HAL_Delay(1);
    if (HAL_SPI_TransmitReceive(&hspi1, spi_in_tmp, spi_out_tmp, 5,
    AD7682_TIMEOUT) != HAL_OK) {
        pSunSensor_t->sun_status = DEVICE_ERROR;
        return DEVICE_ERROR;
    }

    if (spi_out_tmp[3] != (AD7682_REF | AD7682_SEQ | AD7682_RB)) {
        pSunSensor_t->sun_status = DEVICE_ERROR;
        return DEVICE_ERROR;
    }

    return pSunSensor_t->sun_status;
	
}

_adcs_sensor_status update_sun_sensor(sun_sensor_t *pSunSensor_t){
	
	
}

_adcs_sensor_status update_ad7682(uint8_t ch, uint16_t *v_raw){
	
	
}
*/