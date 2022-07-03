#ifndef __OBC_HSI_H
#define __OBC_HSI_H

#include <stdint.h>
#include "services.h"


void updateCurrentTick(void);

uint32_t getCurrentTick(void);

void obc_SD_ON(void);

void obc_SD_OFF(void);

void OBC_reset_source(uint8_t *src);

void OBC_sys_setTime(uint8_t hours, uint8_t mins, uint8_t sec);

void OBC_sys_getTime(uint8_t *hours, uint8_t *mins, uint8_t *sec);

void OBC_sys_setDate(uint8_t weekday, uint8_t mon, uint8_t date, uint8_t year);

void OBC_sys_getDate(uint8_t *weekday, uint8_t *mon, uint8_t *date, uint8_t *year);

void obc_enableBkUpAccess(void);

uint32_t * obc_BKPSRAM_BASE(void);

void sdcard_gpio_inits(void);

void sdcard_init(void);




#endif
