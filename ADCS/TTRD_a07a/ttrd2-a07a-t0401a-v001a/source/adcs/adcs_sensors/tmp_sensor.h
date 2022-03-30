
#ifndef __TEMP_SENSOR_H__
#define __TEMP_SENSOR_H__ 1

#include "driver_i2c.h"

#define 	TEMP_I2C_CLOCK				400000U

// ------ Public function prototypes (declarations) --------------------------
uint8_t TMP_Sensor_Init(I2C_TypeDef* I2Cx);
uint8_t TMP_Sensor_update(void);
float TMP_Sensor_GetData(void); 

#endif

