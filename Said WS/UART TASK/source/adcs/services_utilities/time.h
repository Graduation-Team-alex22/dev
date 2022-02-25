/**********************************************************
Implementation of the time module. This module provides a
real-time clock for other modules. It supports different time
formats.
It depends basically on:
	- RTC peripheral
	- obc_comm module

The module assumes:
	- 24-hour format
	- calender data is stored in BCD format

Notes: changing clock settings will affect the RTC tick.

Module Category:	Services

Auther: Mohamed Said (AKA: Alpha_Arslan)
Date:		2022-02-23

**********************************************************/
#ifndef TIME_H__
#define TIME_H__

#include "stdint.h"
#include "../hsi_library/stm32f4xx_rtc.h"
#include "common.h"

#define TM_MONTH_JANUARY        ((uint8_t)0x01U)
#define TM_MONTH_FEBRUARY       ((uint8_t)0x02U)
#define TM_MONTH_MARCH          ((uint8_t)0x03U)
#define TM_MONTH_APRIL          ((uint8_t)0x04U)
#define TM_MONTH_MAY            ((uint8_t)0x05U)
#define TM_MONTH_JUNE           ((uint8_t)0x06U)
#define TM_MONTH_JULY           ((uint8_t)0x07U)
#define TM_MONTH_AUGUST         ((uint8_t)0x08U)
#define TM_MONTH_SEPTEMBER      ((uint8_t)0x09U)
#define TM_MONTH_OCTOBER        ((uint8_t)0x10U)
#define TM_MONTH_NOVEMBER       ((uint8_t)0x11U)
#define TM_MONTH_DECEMBER       ((uint8_t)0x12U)

#define MAX_YEAR 22

#define MIN_QB_SECS 2678400
#define MAX_QB_SECS 662774400


typedef struct{
    uint8_t weekday;
    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} time_utc_t;

typedef struct{
    int ep_year; 			/* Year of epoch, e.g. 94 for 1994, 100 for 2000AD */
    double ep_day; 		/* Day of epoch from 00:00 Jan 1st ( = 1.0 ) */
} tle_epoch_t; // _tle_epoch

typedef struct {
    uint8_t set_time;
    time_utc_t utc;
    tle_epoch_t tle_epoch; 			// TLE epoch
    double decyear; 						// Decimal year, for IGRF
    double jd; 									// Julian days from 1st Jan 1900, for SGP4
    double gps_sec; 						// Time in sec of "gps_week" week
    uint16_t gps_week; 					// Number of weeks from GPS starting date
} time_keeping_adcs_t;

static time_keeping_adcs_t time_keeping_adcs = {0};


/**********************************************************
										SUPPORTED APIs
**********************************************************/
/*
	Initializes the RTC peripheral and sets the clock
	from OBC.

	NOTE: OBC communication must be initialized first.
*/
uint8_t time_init(void);

/*
	Updates time_keeping_adcs from the RTC peripheral.
*/
void time_update(void);

/*
	Gets time_keeping_adcs
*/
time_keeping_adcs_t time_getTime(void);


/******** conversion functions ***********/
/*
	calculates time_keeping_adcs.tle_epoch from 
	time_keeping_adcs.UTC
*/
void tle_epoch(void);

/*
	calculates time_keeping_adcs.decyear from 
	time_keeping_adcs.UTC
*/
void decyear(void);

/*
	calculates time_keeping_adcs.jd from 
	time_keeping_adcs.UTC
*/
void julday(void);

/*
	Conversion from GPS time and week to UTC.
*/
void gps2utc(void);

/**
  * @brief  Convert from 2 digit BCD to Binary.
  * @param  Value: BCD value to be converted.
  * @retval Converted word
  */
static uint8_t RTC_Bcd2ToByte(uint8_t Value);

#endif
