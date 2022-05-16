/**********************************************************
Implementation of the sun sensor module. This module provides a rough estimate
of the Cube-Sun-Vector (the vector pointing from the cudeSat to the sun). It
depends of light intensity readings from 6 LDRs positioned on the cube sides
and facing outwards.

It depends basically on:
	- ADC peripheral (6 Channels)

The module assumes:
	- NONE

Notes: periodic check on the ADC reference channel is required to insure ADC integrity.

Module Category:	Sensors

Auther: Mohamed Said (AKA: Alpha_Arslan)
Date:		2022-02-27

**********************************************************/

#ifndef SUN_SENSOR_H__
#define SUN_SENSOR_H__

#include "stdint.h"
#include "../services_utilities/common.h"
#include "../main/project.h"

typedef struct {
   // solar panel arrangement: X+ , X- , Y+ , Y- , Z+ , Z-
   uint16_t v_sun_raw[SLR_NUM];		// Raw Values
   float v_sun[SLR_NUM];				// Raw Values converted to volts
   float sun_rough[3];			      // sun vector in polar co-ordinates: Alpha, Beta, Magnitude = 1
   float sun_xyz[3];				      // polar co-ordinates converted to cartizian co-ordinates
   device_status_e status;		      // Status of sun sensors system
} sun_sensor_t;

uint8_t SUN_Sensor_Init(void);
uint8_t SUN_Sensor_Update(void);
sun_sensor_t SUN_Sensor_GetData(void);

#endif
