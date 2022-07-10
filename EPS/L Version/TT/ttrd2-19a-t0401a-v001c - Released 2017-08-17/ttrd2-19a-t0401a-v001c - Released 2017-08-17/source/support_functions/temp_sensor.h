#ifndef __TEMP_SENSOR_H__
#define __TEMP_SENSOR_H__

#include <stdint.h>

#define TEMP_SENSOR_ADDRESS		(0x18<<1)
#define TEMP_REG_ADDRESS			(0x05)

void TEMP_SENSOR_Init(void);
uint32_t TEMP_SENSOR_update(void);
float TEMP_SENSOR_Get_Value(void);
#endif
