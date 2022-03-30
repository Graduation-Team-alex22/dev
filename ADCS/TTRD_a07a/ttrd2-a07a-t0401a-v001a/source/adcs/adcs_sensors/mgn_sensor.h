/**********************************************************
Implementation of the Magnometer module. This module provides an interface
to the HMC-5883L chip and QMC-5883L.
		
It depends basically on:
	- I2C peripheral (400KHz)


Notes:  - the device has an I2C ready time of 200 uC
        - the device has a Measurement ready time 50 mS

Module Category:	Sensors

Update Stages: Single-Stage

Auther: Mohamed Said & Ali Fakharany
Date:		2022-03-18

**********************************************************/

#ifndef MGN_SENSOR_H__
#define MGN_SENSOR_H__

#include <stdint.h>
#include "driver_i2c.h"

//#define HMC_DEVICE
#define QMC_DEVICE

#ifdef QMC_DEVICE

#endif

/************* PUBLIC DEFINITIONS *************/
#ifdef HMC_DEVICE
	#define			MGN_I2C_ADD				((uint8_t)0x3C)
	#define			MGN_I2C_CLOCK			400000U
#endif

#ifdef QMC_DEVICE
	#define			MGN_I2C_ADD				((uint8_t)0x1A) 
	#define			MGN_I2C_CLOCK			400000U
#endif


// init parameters
#ifdef HMC_DEVICE
	// AVERAGAE_SAMPLES_X
	#define			AVERAGE_SAMPLES_1			((uint8_t)0x00)
	#define			AVERAGE_SAMPLES_2			((uint8_t)0x20)
	#define			AVERAGE_SAMPLES_4			((uint8_t)0x40)
	#define			AVERAGE_SAMPLES_8			((uint8_t)0x60)
	#define IS_AVERAGE_SAMPLES_X(n) 	(((n) == AVERAGE_SAMPLES_1) || \
																		 ((n) == AVERAGE_SAMPLES_2) || \
																		 ((n) == AVERAGE_SAMPLES_4) || \
																		 ((n) == AVERAGE_SAMPLES_8))
	// OUTPUT_RATE_X
	#define			OUTPUT_RATE_0_75			((uint8_t)0x00)
	#define			OUTPUT_RATE_1_5				((uint8_t)0x04)
	#define			OUTPUT_RATE_3					((uint8_t)0x08)
	#define			OUTPUT_RATE_7_5				((uint8_t)0x0C)
	#define			OUTPUT_RATE_15				((uint8_t)0x10)
	#define			OUTPUT_RATE_30				((uint8_t)0x14)
	#define			OUTPUT_RATE_75				((uint8_t)0x18)
	#define IS_OUTPUT_RATE_X(n) 			(((n) == OUTPUT_RATE_0_75) || \
																		 ((n) == OUTPUT_RATE_1_5) || \
																		 ((n) == OUTPUT_RATE_3) || \
																		 ((n) == OUTPUT_RATE_7_5) || \
																		 ((n) == OUTPUT_RATE_15) || \
																		 ((n) == OUTPUT_RATE_30) || \
																		 ((n) == OUTPUT_RATE_75))
	// BIAS_MODE_X
	#define			BIAS_MODE_NO						((uint8_t)0x00)
	#define			BIAS_MODE_POSITIVE			((uint8_t)0x01)
	#define			BIAS_MODE_NEGATIVE			((uint8_t)0x02)
	#define IS_BIAS_MODE_X(n) 				(((n) == BIAS_MODE_NO) || \
																		 ((n) == BIAS_MODE_POSITIVE) || \
																		 ((n) == BIAS_MODE_NEGATIVE))
	// GAIN_X
	#define			GAIN_1370							((uint8_t)0x00)		// recommended range: 0.88 Gauss
	#define			GAIN_1090							((uint8_t)0x20)		// recommended range: 1.3 Gauss
	#define			GAIN_820							((uint8_t)0x40)		// recommended range: 1.9 Gauss
	#define			GAIN_660							((uint8_t)0x60)		// recommended range: 2.5 Gauss
	#define			GAIN_440							((uint8_t)0x80)		// recommended range: 4 Gauss
	#define			GAIN_390							((uint8_t)0xA0)		// recommended range: 4.7 Gauss
	#define			GAIN_330							((uint8_t)0xC0)		// recommended range: 5.6 Gauss
	#define			GAIN_230							((uint8_t)0xE0)		// recommended range: 8.1 Gauss
	#define IS_GAIN_X(n) 						(((n) == GAIN_1370) || \
																		 ((n) == GAIN_1090) || \
																		 ((n) == GAIN_820) || \
																		 ((n) == GAIN_660) || \
																		 ((n) == GAIN_440) || \
																		 ((n) == GAIN_390) || \
																		 ((n) == GAIN_330) || \
																		 ((n) == GAIN_230))
	// HIGH_SPEED_F
	#define			HIGH_SPEED_ENABLE				((uint8_t)0x80)
	#define			HIGH_SPEED_DISABLE			((uint8_t)0x00)
	#define IS_HIGH_SPEED_F(n) 				(((n) == HIGH_SPEED_ENABLE) || \
																		 ((n) == HIGH_SPEED_DISABLE))
	// OP_MODE_X
	#define			OP_MODE_CONT					((uint8_t)0x00)			// continuous
	#define			OP_MODE_SINGLE				((uint8_t)0x01)
	#define			OP_MODE_IDLE					((uint8_t)0x02)
	#define IS_OP_MODE_X(n) 				(((n) == OP_MODE_CONT) || \
																		 ((n) == OP_MODE_SINGLE) || \
																		 ((n) == OP_MODE_IDLE))																
																		 
	typedef struct {
		uint8_t average_samples;				/*samples per measurement @ AVERAGAE_SAMPLES_X */
		uint8_t output_rate;						/*Hz ,@ OUTPUT_RATE_X */
		uint8_t bias_mode;							/*@ BIAS_MODE_X */
		uint8_t gain;										/* gain in Gauss @ GAIN_X */
		uint8_t high_speed;							/* flag @ HIGH_SPEED_F */
		uint8_t op_mode;								/* operation mode @ OP_MODE_X */
	} mgn_init_t;
#endif


#ifdef QMC_DEVICE
	
	// OVER_SAMPLING_X
	#define			MGN_OVER_SAMPLING_512		((uint8_t)0x00)
	#define			MGN_OVER_SAMPLING_256		((uint8_t)0x40)
	#define			MGN_OVER_SAMPLING_128		((uint8_t)0x80)
	#define			MGN_OVER_SAMPLING_64		((uint8_t)0xC0)
	#define MGN_IS_OVER_SAMPLING_X(n) 	(((n) == MGN_OVER_SAMPLING_512) || \
																			((n) == MGN_OVER_SAMPLING_256) || \
																			((n) == MGN_OVER_SAMPLING_128) || \
																			((n) == MGN_OVER_SAMPLING_64))
	// OUTPUT_RATE_X
	#define			MGN_OUTPUT_RATE_10			((uint8_t)0x00)
	#define			MGN_OUTPUT_RATE_50			((uint8_t)0x04)
	#define			MGN_OUTPUT_RATE_100			((uint8_t)0x08)
	#define			MGN_OUTPUT_RATE_200			((uint8_t)0x0C)
	#define MGN_IS_OUTPUT_RATE_X(n) 	(((n) == MGN_OUTPUT_RATE_10) || \
																		 ((n) == MGN_OUTPUT_RATE_50) || \
																		 ((n) == MGN_OUTPUT_RATE_100) || \
																		 ((n) == MGN_OUTPUT_RATE_200))
	// RANGE_X
	#define			MGN_RANGE_2							((uint8_t)0x00)			
	#define			MGN_RANGE_8							((uint8_t)0x10)	
	#define MGN_IS_RANGE_X(n) 				(((n) == MGN_RANGE_2) || \
																			((n) == MGN_RANGE_8))	
	// OP_MODE_X
	#define			MGN_OP_MODE_STANDBY			((uint8_t)0x00)			
	#define			MGN_OP_MODE_CONT				((uint8_t)0x01)			// continuous
	#define MGN_IS_OP_MODE_X(n) 			(((n) == MGN_OP_MODE_CONT) || \
																			((n) == MGN_OP_MODE_STANDBY))		

	typedef struct {
			uint8_t over_sampling;					/*@ OVER_SAMPLING_X */
			uint8_t output_rate;						/*Hz ,@ OUTPUT_RATE_X */
			uint8_t range;									/* in Gauss @ RANGE_X */
			uint8_t op_mode;								/* operation mode @ OP_MODE_X */
		} mgn_init_t;
#endif

typedef struct {
    int16_t raw[3];
    float 	mag[3];			// readings in Gauss 
} mgn_sensor_t;

uint8_t MGN_Sensor_Init(I2C_TypeDef* I2Cx, mgn_init_t* mgn_init);
uint8_t MGN_Sensor_Update(void);
mgn_sensor_t MGN_Sensor_GetData(void);

#endif
