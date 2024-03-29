/**********************************************************
Implementation of the imu module. This module provides an interface
to the MPU-9255 chip. The chip contains three devices:
		- 3D Accelerometer
		- 3D Gyroscope
		- 3D Compass
		
It depends basically on:
	- driver_i2c

The module assumes:
	- MPU pin AD0 is connected to ground

Notes: None

Module Category:	Sensors

Update Stages: 3-Stages

Auther: Mohamed Said & Ali Fakharany
Date:		2022-03-11

**********************************************************/

#ifndef IMU_SENSOR_H__
#define IMU_SENSOR_H__


#include "stdint.h"
#include "driver_i2c.h"
#include "../support_functions/ttrd2-05a-t0401a-v001a_timeout_t3.h"

/************* PUBLIC MACROS ***************/
#define 	IMU_I2C_CLOCK       400000U

/************ Public structures ************/
typedef struct {
   float Ax, Ay, Az;                      /*!< Accelerometer raw data */
   float Gx, Gy, Gz;                      /*!< Gyroscope raw data */
   float Mx, My, Mz;                      /*!< Magnetometer raw data */
   int16_t Ax_Raw, Ay_Raw, Az_Raw;        /*!< Accelerometer raw data */
   int16_t Gx_Raw, Gy_Raw, Gz_Raw;        /*!< Gyroscope raw data */
   int16_t Mx_Raw, My_Raw, Mz_Raw;        /*!< Magnetometer raw data */
   int16_t	Temprature_Raw;               /*!< Temperature raw data */
   float AMult, GMult, MMult;
   float M_Calib[3];
} imu_sensor_t;

/************ Public Interfaces ************/
/*
   IMU_Init

   Initialize      Imu sensor and check if it is connected or not

   @note           The I2C peripheral must be already iniialized before using this funcion.

   @param I2Cx,    I2C Peripheral to be used.

   @return error_code,  An error code, Or Zero if no Error.

*/
uint8_t IMU_Init(I2C_TypeDef* I2Cx);

/*
  IMU_Update
  
  Receive updated GPS data from the sensor over a specified I2C peripheral
	
	@note    None
   
   @param   None

  @return Zero if no Error.

*/
uint8_t IMU_Update(I2C_TypeDef* I2Cx);

/*
   IMU_Get_Data

   get last updated IMU values

   @note    None.

   @param   None

   @return temperature value.

*/
imu_sensor_t IMU_Get_Data(void);

#endif
