/**********************************************************
Implementation of the time module. This module provides a
real-time clock for other modules. It supports different time
formats.
It depends basically on:
	- RTC peripheral
	- obc_comm module

The module assumes:
	- 24-hour format

Notes: changing clock settings will affect the RTC tick.

Module Category:	Services

Update Stages: Single-Stage

Auther: Mohamed Said (AKA: Alpha_Arslan)
Date:		2022-02-23

**********************************************************/
#ifndef TIME_H__
#define TIME_H__

#include "stdint.h"


/************* PUBLIC MACROS ***************/
#define  TM_MONTH_JANUARY         ((uint8_t)0x01U)
#define  TM_MONTH_FEBRUARY        ((uint8_t)0x02U)
#define  TM_MONTH_MARCH           ((uint8_t)0x03U)
#define  TM_MONTH_APRIL           ((uint8_t)0x04U)
#define  TM_MONTH_MAY             ((uint8_t)0x05U)
#define  TM_MONTH_JUNE            ((uint8_t)0x06U)
#define  TM_MONTH_JULY            ((uint8_t)0x07U)
#define  TM_MONTH_AUGUST          ((uint8_t)0x08U)
#define  TM_MONTH_SEPTEMBER       ((uint8_t)0x09U)
#define  TM_MONTH_OCTOBER         ((uint8_t)0x10U)
#define  TM_MONTH_NOVEMBER        ((uint8_t)0x11U)
#define  TM_MONTH_DECEMBER        ((uint8_t)0x12U)

#define  MAX_YEAR                 2022
#define  CURRENT_YEAR             2022

#define  MIN_QB_SECS              2678400
#define  MAX_QB_SECS              662774400

#define  PRINT_TIME_STR_LENGTH    20

/************ Public structures ************/
typedef struct{
   uint8_t weekday;     // 1 = Monday ---> 7 = Sunday
   uint8_t day;         // day of the month
   uint8_t month;       // 1 ---> 12
   uint16_t year;        // 20xx
   uint8_t hour;        // 00 ---> 23
   uint8_t min;         // 00 ---> 59
   uint8_t sec;         // 00 ---> 59
} time_utc_t;

typedef struct{
   int ep_year;         /* Year of epoch, e.g. 94 for 1994, 100 for 2000AD */
   double ep_day;       /* Day of epoch from 00:00 Jan 1st ( = 1.0 ) */
} tle_epoch_t;          // _tle_epoch

typedef struct {
   uint8_t set_time;
   time_utc_t utc;
   tle_epoch_t tle_epoch;        // TLE epoch
   double decyear;               // Decimal year, for IGRF
   double Julian_Date;           // Julian days from 1st Jan 1900, for SGP4
   double gps_sec;               // Time in sec of "gps_week" week
   uint16_t gps_week;            // Number of weeks from GPS starting date
} time_t;


/************ Public Interfaces ************/

/*
   time_init
   Initializes the RTC peripheral and sets the clock from OBC.

   @note    OBC communication must be iniialized before using this funcion.

   @param   None

   @return  error code or Zero if no error.

*/
uint32_t time_init(void);

/*
   time_update
   Updates time_keeping_adcs from the RTC peripheral.

   @note    None

   @param   None

   @return  None.

*/
void time_update(void);

/*
   time_getTime
   Gets time_keeping_adcs.

   @note    None

   @param   None

   @return  time_keeping_adcs_t.

*/
time_t time_getTime(void);


/******** conversion functions ***********/
/*
   tle_epoch
   calculates time_keeping_adcs.tle_epoch from time_keeping_adcs.UTC.

   @note    None

   @param   None

   @return  None.
*/
void tle_epoch(void);

/*
   decyear
   calculates time_keeping_adcs.decyear from time_keeping_adcs.UTC.

   @note    None

   @param   None

   @return  None.
*/
void decyear(void);

/*
   julday
   Converts UTC date to Julian Date
   The conversion algorithm is found at
   https://quasar.as.utexas.edu/BillInfo/JulianDatesG.html

   @note    None

   @param   None

   @return  None.
*/
void julday(void);

/*
   gps2utc
   Conversion from GPS time and week to UTC.

   @note    None

   @param   None

   @return  None.
*/
void gps2utc(void);

void print_time(char*);

#endif
