/**********************************************************
Implementation of the Magnometer module. This module provides an interface
to QMC-5883L chip.
		
It depends basically on:
   - driver_i2c


Notes:   - the device has an I2C ready time of 200 uC
         - the device has a Measurement ready time 50 mS
         - I2C peripheral to be used must be already initialized
           before using any available functions.

Module Category:	Sensors

Update Stages: Single-Stage

Auther: Mohamed Said & Ali Fakharany
Date:		2022-03-18

**********************************************************/

#ifndef MGN_SENSOR_H__
#define MGN_SENSOR_H__

#include "stdint.h"
#include "driver_i2c.h"
#include "../support_functions/ttrd2-05a-t0401a-v001a_timeout_t3.h"

/************* PUBLIC MACROS ***************/
#define  MGN_I2C_ADD          ((uint8_t)0x1A) 
#define  MGN_I2C_CLOCK        400000U
	
// OVER_SAMPLING_X
#define        MGN_OVER_SAMPLING_512		((uint8_t)0x00)
#define        MGN_OVER_SAMPLING_256		((uint8_t)0x40)
#define			MGN_OVER_SAMPLING_128		((uint8_t)0x80)
#define			MGN_OVER_SAMPLING_64       ((uint8_t)0xC0)
#define MGN_IS_OVER_SAMPLING_X(n)         (((n) == MGN_OVER_SAMPLING_512) || \
                                                ((n) == MGN_OVER_SAMPLING_256) || \
                                                ((n) == MGN_OVER_SAMPLING_128) || \
                                                ((n) == MGN_OVER_SAMPLING_64))
                                                
// OUTPUT_RATE_X
#define        MGN_OUTPUT_RATE_10         ((uint8_t)0x00)
#define        MGN_OUTPUT_RATE_50         ((uint8_t)0x04)
#define        MGN_OUTPUT_RATE_100        ((uint8_t)0x08)
#define        MGN_OUTPUT_RATE_200        ((uint8_t)0x0C)
#define MGN_IS_OUTPUT_RATE_X(n)           (((n) == MGN_OUTPUT_RATE_10) || \
                                                ((n) == MGN_OUTPUT_RATE_50) || \
                                                ((n) == MGN_OUTPUT_RATE_100) || \
                                                ((n) == MGN_OUTPUT_RATE_200))
// RANGE_X
#define        MGN_RANGE_2                ((uint8_t)0x00)			
#define        MGN_RANGE_8                ((uint8_t)0x10)	
#define        MGN_IS_RANGE_X(n)          (((n) == MGN_RANGE_2) || \
                                                ((n) == MGN_RANGE_8))	
// OP_MODE_X
#define        MGN_OP_MODE_STANDBY        ((uint8_t)0x00)			
#define        MGN_OP_MODE_CONT           ((uint8_t)0x01)	 // continuous mode
#define        MGN_IS_OP_MODE_X(n)        (((n) == MGN_OP_MODE_CONT) || \
                                                ((n) == MGN_OP_MODE_STANDBY))		

/************ Public structures ************/
typedef struct {
   uint8_t over_sampling;              /*@ OVER_SAMPLING_X */
   uint8_t output_rate;                /*Hz ,@ OUTPUT_RATE_X */
   uint8_t range;                      /* in Gauss @ RANGE_X */
   uint8_t op_mode;                    /* operation mode @ OP_MODE_X */
} mgn_init_t;

typedef struct {
   int16_t raw[3];
   float 	mag[3];                     // readings in Gauss 
} mgn_sensor_t;


/************ Public Interfaces ************/
/*
   MGN_Sensor_Init
   Inititialize magnometer sensor and check if it is connected or not

   @note    The I2C peripheral must be already iniialized before using this funcion.

   @param I2Cx,         I2C Peripheral to be used.
   @param mgn_init,     Structure of magnometer parameters.

   @return error_code,  An error code, Or Zero if no Error.

*/
uint8_t MGN_Sensor_Init(I2C_TypeDef* I2Cx, mgn_init_t* mgn_init);

/*
   MGN_Sensor_Update
   Receive updated magnometer values from the sensor over a specified I2C peripheral

   @note    The I2C peripheral must be already iniialized before using this funcion.

   @param I2Cx,   None
   @return error_code,  An error code, Or Zero if no Error.

*/
uint8_t MGN_Sensor_Update(void);

/*
  MGN_Sensor_GetData
  
  get last updated magnometer values
	
	@note    None.
   
   @param   None

  @return structure contains magnometer valuse.

*/
mgn_sensor_t MGN_Sensor_GetData(void);

#endif
