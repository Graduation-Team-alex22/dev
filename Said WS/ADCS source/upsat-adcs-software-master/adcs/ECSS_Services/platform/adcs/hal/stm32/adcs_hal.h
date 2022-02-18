#ifndef __ADCS_HAL_H
#define __ADCS_HAL_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "services.h"
#include "hldlc.h"
#include "upsat.h"

//ToDo
//  improve return types
//  need to make the change from the two different typedefs

extern struct _adcs_data adcs_data;
extern UART_HandleTypeDef huart2;

SAT_returnState HAL_takeMutex(TC_TM_app_id app_id);

SAT_returnState HAL_giveMutex(TC_TM_app_id app_id);

void HAL_adcs_SD_ON();

void HAL_adcs_SD_OFF();

void HAL_sys_delay(uint32_t sec);

void HAL_ADCS_UART_IRQHandler(UART_HandleTypeDef *huart);

void UART_ADCS_Receive_IT(UART_HandleTypeDef *huart);

SAT_returnState hal_kill_uart(TC_TM_app_id app_id);

SAT_returnState HAL_uart_tx_check(TC_TM_app_id app_id);

void HAL_uart_tx(TC_TM_app_id app_id, uint8_t *buf, uint16_t size);

SAT_returnState HAL_uart_rx(TC_TM_app_id app_id, struct uart_data *data);

void HAL_reset_source(uint8_t *src);

void HAL_sys_setTime(uint8_t hours, uint8_t mins, uint8_t sec);

void HAL_sys_getTime(uint8_t *hours, uint8_t *mins, uint8_t *sec);

void HAL_sys_setDate(uint8_t weekday, uint8_t mon, uint8_t date, uint8_t year);

void HAL_sys_getDate(uint8_t *weekday, uint8_t *mon, uint8_t *date, uint8_t *year);

uint32_t HAL_sys_GetTick();

#endif
