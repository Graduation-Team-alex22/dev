/*
 * adcs_gps.h
 *
 *  Created on: May 18, 2016
 *      Author: azisi
 */

#ifndef INC_ADCS_GPS_H_
#define INC_ADCS_GPS_H_

#include "stm32f4xx_hal.h"
#include "adcs_time.h"
#include "gps.h"

void init_gps_uart();
void get_gps_sentences();

void HAL_GPS_Alarm_Handler(_gps_state *gps_state_value);
_gps_status HAL_SetAlarm_GPS_ON(struct time_utc gps_utc);
_gps_status HAL_SetAlarm_GPS_LOCK(struct time_utc gps_utc,
        uint8_t gps_alarm_ulock_min);
HAL_StatusTypeDef HAL_SetAlarm_GPS(uint8_t gps_alarm_hour,
        uint8_t gps_alarm_min, uint8_t gps_alarm_sec);
void HAL_GPS_UART_IRQHandler(UART_HandleTypeDef *huart);
void UART_GPS_Receive_IT(UART_HandleTypeDef *huart);

#endif /* INC_ADCS_GPS_H_ */
