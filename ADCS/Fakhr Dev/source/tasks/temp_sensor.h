#ifndef __TEMP_SENSOR_H__
#define __TEMP_SENSOR_H__ 1

#include <stdint.h>
#include "driver_i2c.h"

#define 	TEMP_I2C_CLOCK				100000U
#define 	TEMP_I2C_PERIPHERAL		I2C1

// ------ Public function prototypes (declarations) --------------------------
void TEMP_SENSOR_Init(I2C_TypeDef* I2Cx);
uint32_t TEMP_SENSOR_update(void);
float TEMP_SENSOR_Get_Value(void); 

#endif
