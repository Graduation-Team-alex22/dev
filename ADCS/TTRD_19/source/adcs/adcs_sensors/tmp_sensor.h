/**********************************************************
Temperature sensor driver build upon driver_i2c.

Notes:  I2C peripheral to be used must be already initialized
        before using any available functions.

Auther: Mohamed Said & Ali Fakharany
Date:		2022-03-15

**********************************************************/
#ifndef __TEMP_SENSOR_H__
#define __TEMP_SENSOR_H__ 1

#include "stdint.h"
#include "driver_i2c.h"
#include "../support_functions/ttrd2-05a-t0401a-v001a_timeout_t3.h"

// ------ Public Macros --------------------------
#define 	TEMP_I2C_CLOCK				400000U

/************ Public Interfaces ************/
/*
  TMP_Sensor_Init
  
  Initialize temperature sensor and check if it is connected or not
	
	@note    The I2C peripheral must be already iniialized before using this funcion.
	
	@param I2Cx,         I2C Peripheral to be used.

  @return error_code,  An error code, Or Zero if no Error.

*/
uint8_t TMP_Sensor_Init(I2C_TypeDef* I2Cx);

/*
  TMP_Sensor_update
  
  Receive updated temperature value from the sensor over a specified I2C peripheral
	
	@note    The I2C peripheral must be already iniialized before using this funcion.
   
   @param   None

  @return error_code,  An error code, Or Zero if no Error.

*/
uint8_t TMP_Sensor_update(void);

/*
  TMP_Sensor_GetData
  
  get last updated temperature value
	
	@note    None.
   
   @param   None

  @return temperature value.

*/
float TMP_Sensor_GetData(void); 

#endif

