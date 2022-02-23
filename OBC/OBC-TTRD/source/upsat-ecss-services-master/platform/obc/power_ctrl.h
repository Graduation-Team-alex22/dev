#ifndef __POWER_CTRL_H
#define __POWER_CTRL_H

#include <stdint.h>
#include "services.h"

extern void HAL_obc_SD_ON();
extern void HAL_obc_SD_OFF();
extern void HAL_sys_delay(uint32_t sec);

SAT_returnState power_control_api(FM_dev_id did, FM_fun_id fid, uint8_t *state);

#endif
