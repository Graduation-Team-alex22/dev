#ifndef __OBC_HSI_H
#define __OBC_HSI_H

#include <stdint.h>
#include "services.h"


void HAL_obc_SD_ON(void);

void HAL_obc_SD_OFF(void);

void HAL_reset_source(uint8_t *src);

void HAL_sys_setTime(uint8_t hours, uint8_t mins, uint8_t sec);

void HAL_sys_getTime(uint8_t *hours, uint8_t *mins, uint8_t *sec);

void HAL_sys_setDate(uint8_t weekday, uint8_t mon, uint8_t date, uint8_t year);

void HAL_sys_getDate(uint8_t *weekday, uint8_t *mon, uint8_t *date, uint8_t *year);

void HAL_obc_enableBkUpAccess(void);

uint32_t * HAL_obc_BKPSRAM_BASE(void);




#endif
