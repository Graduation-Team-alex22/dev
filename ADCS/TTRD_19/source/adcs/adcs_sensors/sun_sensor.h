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

typedef enum {
	NO_ERROR = 0,
	ADC_ERROR
}sun_sensor_status_e;

typedef struct {
    uint16_t v_sun_raw[5];						// Raw Values
    float v_sun[5];										// Raw Values converted to volts
		float sun_rough[3];								// sun vector in polar co-ordinates: Alpha, Beta, Magnitude = 1
    float sun_xyz[3];									// polar co-ordinates converted to cartizian co-ordinates
    sun_sensor_status_e sun_status;		// Status of sun sensors system
} sun_sensor_t;


sun_sensor_status_e init_sun_sensor(sun_sensor_t *pSunSensor);
sun_sensor_status_e update_sun_sensor(sun_sensor_t *pSunSensor);
sun_sensor_status_e update_ad7682(uint8_t ch, uint16_t *v_raw);




#endif
