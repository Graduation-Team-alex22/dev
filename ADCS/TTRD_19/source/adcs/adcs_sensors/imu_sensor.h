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
#include "../services_utilities/common.h"

/************ Public structures ************/
typedef struct {
   float Ax, Ay, Az;                      /*!< Accelerometer raw data */
   float Gx, Gy, Gz;                      /*!< Gyroscope raw data */
   float Mx, My, Mz;                      /*!< Magnetometer raw data */
   double xm_norm;                        /*!< magnometer vector magnitude */
   int16_t Ax_Raw, Ay_Raw, Az_Raw;        /*!< Accelerometer raw data */
   int16_t Gx_Raw, Gy_Raw, Gz_Raw;        /*!< Gyroscope raw data */
   int16_t Mx_Raw, My_Raw, Mz_Raw;        /*!< Magnetometer raw data */
   int16_t	Temprature_Raw;               /*!< Temperature raw data */
   float AMult, GMult, MMult;
   float M_Calib[3];
   float xm_filtered[3];                  /*!<Filtered mgn output */
   float xm_prev[3];
   float gyro_filtered[3];                /*!<Filtered gyro output */
   float gyro_prev[3];
   device_status_e status;
} imu_sensor_t;

/************ Public Interfaces ************/
/*
   IMU_Sensor_Init

   Initialize      Imu sensor and check if it is connected or not

   @note           The I2C peripheral must be already iniialized before using this funcion.

   @param I2Cx,    I2C Peripheral to be used.

   @return error_code,  An error code, Or Zero if no Error.

*/
uint32_t IMU_Sensor_Init(I2C_TypeDef* I2Cx);

/*
  IMU_Sensor_Update
  
  Receive updated GPS data from the sensor over a specified I2C peripheral
	
	@note    None
   
   @param   None

  @return Zero if no Error.

*/
uint32_t IMU_Sensor_Update(I2C_TypeDef* I2Cx);

/*
   IMU_Sensor_GetData

   get last updated IMU values

   @note    None.

   @param   None

   @return temperature value.

*/
imu_sensor_t IMU_Sensor_GetData(void);

#endif
