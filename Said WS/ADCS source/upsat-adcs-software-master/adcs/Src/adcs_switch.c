/*
 * adcs_common.c
 *
 *  Created on: Jun 17, 2016
 *      Author: azisi
 */

#include "adcs_switch.h"
#include "stm32f4xx_hal.h"

_adcs_switch_state adcs_switch_state;

void adcs_pwr_switch(_switch_state switch_state, _adcs_switch adcs_switch) {
    GPIO_PinState gpio_write_value;

    if (switch_state == SWITCH_ON) {
        gpio_write_value = GPIO_PIN_RESET;
    } else {
        gpio_write_value = GPIO_PIN_SET;
    }

    switch (adcs_switch) {
    case SENSORS:
        HAL_GPIO_WritePin(SENS_EN_GPIO_Port, SENS_EN_Pin, gpio_write_value);
        adcs_switch_state.sens_sw = switch_state;
        break;
    case GPS:
        HAL_GPIO_WritePin(GPS_EN_GPIO_Port, GPS_EN_Pin, gpio_write_value);
        adcs_switch_state.gps_sw = switch_state;
        break;
    default:
        ;/* ERROR */
        break;
    }
}
