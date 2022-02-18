/*
 * adcs_error_handler.c
 *
 *  Created on: Jul 16, 2016
 *      Author: azisi
 */

#include "stm32f4xx_hal.h"
#include "adcs_error_handler.h"
#include "adcs_manager.h"

#define TR_ERROR_OK         0x05
#define TR_ERROR_USRESOLVED 0x10
#define TR_ERROR_SGP4       0x15
#define TR_ERROR_TLE        0x20
#define TR_ERROR_TIME       0x25
#define TR_ERROR_SENSOR     0x30
#define TR_ERROR_FLASH      0x35
#define TR_ERROR_ACTUATOR   0x40
#define TR_ERROR_GPS        0x45
#define TR_ERROR_HAL_INIT   0x50

extern IWDG_HandleTypeDef hiwdg;

adcs_error_status error_status = ERROR_OK;
uint8_t trasmit_error_status;

static uint8_t wdg_error_cnt = 0;

/**
 * Checks the status, if it is a known issue from the enumeration, tries
 * to solve it in software else a soft reset is trigerred. if the error status
 * is ok, the watchdog is updated.
 * @param error Error status is enumeration.
 */
void error_handler(adcs_error_status error) {
    switch (error) {
    case ERROR_OK:
        HAL_IWDG_Refresh(&hiwdg);
        trasmit_error_status = TR_ERROR_OK;
        break;
    case ERROR_SGP4:
        /* Send from the ground new TLE or update the TLE from GPS */
        HAL_IWDG_Refresh(&hiwdg);
        trasmit_error_status = TR_ERROR_SGP4;
        break;
    case ERROR_TLE:
        /* Send from the ground new TLE or update the TLE from GPS */
        HAL_IWDG_Refresh(&hiwdg);
        trasmit_error_status = TR_ERROR_TLE;
        break;
    case ERROR_TIME:
        /* Resolve in main loop, request time from OBC or from GPS */
        HAL_IWDG_Refresh(&hiwdg);
        trasmit_error_status = TR_ERROR_TIME;
        break;
    case ERROR_SENSOR:
    case ERROR_ACTUATOR:
        /* Both devices are in the same I2C bus */
        wdg_error_cnt += 1;
        if (wdg_error_cnt < WDG_REFRESH) {
            HAL_IWDG_Refresh(&hiwdg);
        }
        if (error == ERROR_SENSOR) {
            trasmit_error_status = TR_ERROR_SENSOR;
        } else {
            trasmit_error_status = TR_ERROR_ACTUATOR;
        }
        /* Close the power of sensors */
        for (uint8_t i = 0; i < 50; i++) {
            adcs_pwr_switch(SWITCH_ON, SENSORS);
            HAL_Delay(1);
            adcs_pwr_switch(SWITCH_OFF, SENSORS);
            HAL_Delay(1);
        }
        /* Software I2C reset */
        for (uint8_t i = 0; i < 8; i++) {
            uint8_t read_sda_pin = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11);
            if (read_sda_pin == GPIO_PIN_RESET) {
                HAL_Delay(1);
                GPIOB->BSRR = GPIO_PIN_10; //SET
                HAL_Delay(1);
                GPIOB->BSRR = (uint32_t) GPIO_PIN_11 << 16U; // RESET
            } else {
                break;
            }
        }
        /* Initialize sensors */
        init_measured_vectors();
        break;
    case ERROR_FLASH:
        HAL_IWDG_Refresh(&hiwdg);
        trasmit_error_status = TR_ERROR_FLASH;
        break;
    case ERROR_GPS:
        HAL_IWDG_Refresh(&hiwdg);
        trasmit_error_status = TR_ERROR_GPS;
        break;
    case ERROR_HAL_INIT:
        trasmit_error_status = TR_ERROR_HAL_INIT;
        break;
    case ERROR_UNRESOLVED:
        trasmit_error_status = TR_ERROR_USRESOLVED;
        break;
    default:
        break;
    }
}

/**
 * Get the current error status and if it's OK return previous error status else
 * return the current error status.
 * @param error Error status is enumeration.
 * @return Error status is enumeration.
 */
adcs_error_status error_propagation(adcs_error_status current_error) {
    if (current_error == ERROR_OK) {
        return error_status;
    }
    return current_error;
}
