/**********************************************************
Implementation of the imu module. This module provides an interface
to the MPU-9255 chip. The chip contains three devices:
		- 3D Accelerometer
		- 3D Gyroscope
		- 3D Compass
		
It depends basically on:
	- I2C peripheral (400KHz)

The module assumes:
	- MPU pin AD0 is connected to ground

Notes: None

Module Category:	Sensors

Update Stages: Single-Stage

Auther: Mohamed Said (AKA: Alpha_Arslan)
Date:		2022-03-11

**********************************************************/

#ifndef IMU_SENSOR_H__
#define IMU_SENSOR_H__


#include <stdint.h>
#include "../hsi_library/stm32f4xx_i2c.h"

#define 	IMU_I2C_CLOCK       400000U
#define		IMU_I2C_ADD					((uint8_t)0xD0)
#define 	IMU_WHOIAM_ADD			((uint8_t)0x75)
#define 	IMU_WHOIAM_VAL			((uint8_t)0x71)
#define		IMU_PWR_MGMT_1			((uint8_t)0x6B)
#define		IMU_PWR_MGMT_2			((uint8_t)0x6C)

#define		IMU_ACCEL_H					((uint8_t)0x3B)


typedef struct {
    float Ax, Ay, Az;         /*!< Accelerometer raw data */
    float Gx, Gy, Gz;         /*!< Gyroscope raw data */
    float Mx, My, Mz;         /*!< Magnetometer raw data */
    int16_t Ax_Raw, Ay_Raw, Az_Raw;         /*!< Accelerometer raw data */
    int16_t Gx_Raw, Gy_Raw, Gz_Raw;         /*!< Gyroscope raw data */
    int16_t Mx_Raw, My_Raw, Mz_Raw;         /*!< Magnetometer raw data */
    int16_t	Temprature_Raw;									/*!< Temperature raw data */

    float AMult, GMult, MMult;
} imu_sensor_t;

uint8_t IMU_Init(I2C_TypeDef* I2Cx);
uint8_t IMU_Update(I2C_TypeDef* I2Cx);
imu_sensor_t IMU_Get_Data(void);

#endif
