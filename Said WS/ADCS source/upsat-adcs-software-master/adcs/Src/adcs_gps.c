/*
 * adcs_gps.c
 *
 *  Created on: May 18, 2016
 *      Author: azisi
 */

#include "adcs_gps.h"
#include "adcs_flash.h"
#include "adcs_switch.h"
#include "adcs_configuration.h"

#include "sysview.h"

static void reset_gps_flag();
static uint8_t get_gps_flag();

static uint8_t gps_buffer[NMEA_MAX_LEN];
static uint8_t gps_flag = false;

static uint8_t gps_sentence[NMEA_MAX_LEN];
static uint8_t gps_parser[NMEA_MAX_FIELDS][NMEA_MAX_FIELD_SIZE];

extern UART_HandleTypeDef huart4;
extern RTC_HandleTypeDef hrtc;

/**
 * Initialize GPS serial.
 */
void init_gps_uart() {
    /* Set up UART4 for GPS */
    memset(gps_sentence, 0, NMEA_MAX_LEN);
    HAL_UART_Receive_IT(&huart4, gps_buffer, NMEA_MAX_LEN);
}

/**
 * Get 16 sentences and check if is correct. If it's correct get the arguments.
 */
void get_gps_sentences() {
    /* Get 16 sentences for GPS to parse */
    for (uint8_t i = 0; i < 2 * NMEA_NUM_SENTENCES; i++) {
        if (get_gps_flag() == true) {
            reset_gps_flag();
            gps_parse_fields(&gps_sentence, NMEA_MAX_LEN, gps_parser);
            gps_parse_logic(&gps_parser, &gps_state);
            SYSVIEW_PRINT("%s", gps_sentence);
        }
    }
}

/**
 * RTC alarm handler and set the next RTC alarm
 * @param gps_state_value: update the gps_state status
 */
void HAL_GPS_Alarm_Handler(_gps_state *gps_state_value) {

    struct time_utc gps_utc_handler;
    get_time_UTC(&gps_utc_handler);

    switch (gps_state_value->status) {
    case GPS_OFF:
        adcs_pwr_switch(SWITCH_ON, GPS);
        gps_state_value->status = HAL_SetAlarm_GPS_LOCK(gps_utc_handler,
        GPS_ALARM_UNLOCK);
        SYSVIEW_PRINT("GPS UNLOCK");
        break;
    case GPS_UNLOCK:
        gps_state_value->reset_flag = true;
        gps_state_value->status = HAL_SetAlarm_GPS_LOCK(gps_utc_handler,
        GPS_ALARM_RESET);
        SYSVIEW_PRINT("GPS RESET");
        break;
    case GPS_RESET:
        adcs_pwr_switch(SWITCH_OFF, GPS);
        gps_state_value->status = HAL_SetAlarm_GPS_ON(gps_utc_handler);
        SYSVIEW_PRINT("GPS OFF");
        break;
    default:
        break;
    }

}

/**
 * This function handles UART interrupt request.
 * @param  huart: pointer to a UART_HandleTypeDef structure that contains
 *                the configuration information for the specified UART module.
 * @retval None
 */
void HAL_GPS_UART_IRQHandler(UART_HandleTypeDef *huart) {
    uint32_t gps_uart_tmp1, gps_uart_tmp2;

    gps_uart_tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
    gps_uart_tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
    /* UART in mode Receiver ---------------------------------------------------*/
    if ((gps_uart_tmp1 != RESET) && (gps_uart_tmp2 != RESET)) {
        UART_GPS_Receive_IT(huart);
    }
}

/**
 * Receives an amount of data in non blocking mode
 * @param  huart: pointer to a UART_HandleTypeDef structure that contains
 *                the configuration information for the specified UART module.
 * @retval HAL status
 */
void UART_GPS_Receive_IT(UART_HandleTypeDef *huart) {
    uint8_t c;

    c = (uint8_t) (huart->Instance->DR & (uint8_t) 0x00FFU);
    if (huart->RxXferSize == huart->RxXferCount && c == '$') {
        *huart->pRxBuffPtr++ = c;
        huart->RxXferCount--;
        //start timeout
    } else if (c == '$') {
        huart->RxXferCount = huart->RxXferSize;

        *huart->pRxBuffPtr++ = c;
        huart->RxXferCount--;
        //error
        //event log
        //reset buffers & pointers
        //start timeout
    } else if (c == 0xa) {
        *huart->pRxBuffPtr++ = c;
        huart->RxXferCount--;
        *huart->pRxBuffPtr++ = 0;
        gps_flag = true;
        huart->pRxBuffPtr = &gps_buffer;
        memset(gps_sentence, 0, NMEA_MAX_LEN);
        for (uint8_t i = 0; i < NMEA_MAX_LEN; i++) {
            gps_sentence[i] = gps_buffer[i];
        }
        huart->RxXferCount = huart->RxXferSize;
    } else if (huart->RxXferSize > huart->RxXferCount) {
        *huart->pRxBuffPtr++ = c;
        huart->RxXferCount--;
    }
    /* Errror */
    if (huart->RxXferCount == 0U) {
        //huart->pRxBuffPtr = &gps;
        huart->RxXferCount = huart->RxXferSize;
    }

}

/**
 * Reset GPS flag when the parser get a correct sentence
 */
void reset_gps_flag() {
    gps_flag = false;
}

/**
 * Get GPS flag to check if the parser has a correct sentence
 */
uint8_t get_gps_flag() {
    return gps_flag;
}

/**
 * Set up GPS alarm, to open GPS every 5 hours and save in flash the alarm time
 * and GPS status.
 * @param gps_utc: Get UTC time to set RTC alarm
 * @return _gps_status
 */
_gps_status HAL_SetAlarm_GPS_ON(struct time_utc gps_utc) {

    uint8_t gps_flash[4];
    memset(gps_flash, 0, 4);

    gps_flash[0] = (gps_utc.hour + GPS_ALARM_ON) % 24;
    gps_flash[1] = gps_utc.min;
    gps_flash[2] = gps_utc.sec;
    gps_flash[3] = GPS_OFF;
    HAL_SetAlarm_GPS(gps_flash[0], gps_flash[1], gps_flash[2]);
    /* Write to flash GPS alarm to recover after reset */
    if (flash_erase_block4K(GPS_ALARM_BASE_ADDRESS) == FLASH_NORMAL) {
        flash_write_page(gps_flash, sizeof(gps_flash), GPS_ALARM_BASE_ADDRESS);
    } else {
        return GPS_ERROR_FLASH;
    }

    return GPS_OFF;

}

/**
 * Set up GPS alarm, to wait in total 18 minutes for cold start - reset -
 * cold start and save in flash the alarm and GPS status.
 * @param gps_utc: Get UTC time to set RTC alarm
 * @return _gps_status
 */
_gps_status HAL_SetAlarm_GPS_LOCK(struct time_utc gps_utc,
        uint8_t gps_alarm_ulock_min) {

    uint8_t gps_flash[4];

    memset(gps_flash, 0, 4);

    if (gps_utc.min + gps_alarm_ulock_min > 60) {
        gps_flash[0] = gps_utc.hour + 1;
    } else {
        gps_flash[0] = gps_utc.hour;
    }

    gps_flash[1] = (gps_utc.min + gps_alarm_ulock_min) % 60;
    gps_flash[2] = gps_utc.sec;
    HAL_SetAlarm_GPS(gps_flash[0], gps_flash[1], gps_flash[2]);
    /* Write to flash the correct GPS state */
    if (gps_alarm_ulock_min == GPS_ALARM_UNLOCK) {
        gps_flash[3] = GPS_UNLOCK;
    } else {
        gps_flash[3] = GPS_RESET;
    }
    /* Write to flash GPS alarm to recover after reset */
    if (flash_erase_block4K(GPS_ALARM_BASE_ADDRESS) == FLASH_NORMAL) {
        flash_write_page(gps_flash, sizeof(gps_flash), GPS_ALARM_BASE_ADDRESS);
    } else {
        return GPS_ERROR_FLASH;
    }

    if (gps_alarm_ulock_min == GPS_ALARM_UNLOCK) {
        return GPS_UNLOCK;
    } else {
        return GPS_RESET;
    }

}

/**
 * Set GPS alarm to interrupt every day at specific time.
 * @param gps_alarm_hour
 * @param gps_alarm_min
 * @param gps_alarm_sec
 * @return HAL_StatusTypeDef: return type to evaluate if HAL works correctly
 */
HAL_StatusTypeDef HAL_SetAlarm_GPS(uint8_t gps_alarm_hour,
        uint8_t gps_alarm_min, uint8_t gps_alarm_sec) {

    RTC_AlarmTypeDef gps_alarm;

    gps_alarm.AlarmTime.Hours = gps_alarm_hour;
    gps_alarm.AlarmTime.Minutes = gps_alarm_min;
    gps_alarm.AlarmTime.Seconds = gps_alarm_sec;
    gps_alarm.AlarmTime.SubSeconds = 0;
    gps_alarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    gps_alarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    gps_alarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
    gps_alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    gps_alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    gps_alarm.AlarmDateWeekDay = 1;
    gps_alarm.Alarm = RTC_ALARM_A;
    HAL_RTC_SetAlarm_IT(&hrtc, &gps_alarm, RTC_FORMAT_BIN);

    return HAL_OK;

}
