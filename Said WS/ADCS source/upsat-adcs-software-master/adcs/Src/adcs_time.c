/*
 * adcs_time.c
 *
 *  Created on: Jun 17, 2016
 *      Author: azisi
 */

#include "stm32f4xx_hal.h"
#include <math.h>
#include <stdlib.h>

#include "adcs_time.h"
#include "adcs_common.h"

time_keeping_adcs adcs_time = { .set_time = false, .utc.year = 0, .utc.month = 0,
                                .utc.day = 0, .utc.hour = 0, .utc.min = 0,
                                .utc.sec = 0, .utc.weekday = 0,
                                .tle_epoch.ep_year = 0, .tle_epoch.ep_day = 0,
                                .decyear = 0.0, .jd = 0.0, .gps_sec = 0.0,
                                .gps_sec = 0.0, .gps_week = 0.0 };

/**
 * Conversion from UTC date to decimal year
 * @param t
 */
void decyear(time_keeping_adcs *t) {

    uint16_t year = 2000 + t->utc.year;
    int days[] = { 0, 31, 59, 90, 120, 151, 182, 212, 243, 273, 304, 334 };
    int isleap = (((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0)));

    int ndays = isleap ? 366 : 365;
    double day_no = (days[t->utc.month - 1] + t->utc.day
            + (t->utc.month > 2 ? isleap : 0));
    double day_hour = t->utc.hour + (t->utc.min / 60) + (t->utc.sec / 3600);

    t->decyear = ((double) year + (day_no / ndays))
            + day_hour / SOLAR_DAY_HOURS / ndays;

}

/**
 * Conversion from UTC date to TLE epoch
 * @param t
 */
void tle_epoch(time_keeping_adcs *t) {

    uint16_t year = 2000 + t->utc.year;
    int days[] = { 0, 31, 59, 90, 120, 151, 182, 212, 243, 273, 304, 334 };
    int isleap = (((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0)));

    t->tle_epoch.ep_year = t->utc.year;
    t->tle_epoch.ep_day = days[t->utc.month - 1] + t->utc.day
            + (t->utc.month > 2 ? isleap : 0) + (t->utc.hour / SOLAR_DAY_HOURS)
            + (t->utc.min / SOLAR_DAY_MIN) + (t->utc.sec / SOLAR_DAY_SEC);

}

/**
 * Conversion from UTC date to Julian days from 1st Jan 1900
 * Reference:
 * http://aa.usno.navy.mil/faq/docs/JD_Formula.php
 * Vallado       2007, 189, alg 14, ex 3-14
 * @param t
 */
void julday(time_keeping_adcs *t) {

    uint16_t year = 2000 + t->utc.year;
    float sgn = 100.0 * year + t->utc.month - 190002.5;

    if (sgn > 0) {
        sgn = 1;
    } else if (sgn < 0) {
        sgn = -1;
    } else {
        sgn = 0;
    }

    t->jd = 367.0 * year
            - floor((7 * (year + floor((t->utc.month + 9) / 12.0))) * 0.25)
            + floor(275 * t->utc.month / 9.0) + t->utc.day + 1721013.5
            + ((t->utc.sec / 60.0 + t->utc.min) / 60.0 + t->utc.hour) / 24.0
            - 0.5 * sgn + 0.5;

}

/**
 * Conversion from GPS time and week to UTC.
 * In calculation we take into account leap seconds.
 * @param t
 */
void gps2utc(time_keeping_adcs *t) {

    double tmp_hour, tmp_min, UT = 0;
    volatile uint32_t iJD, L, N, tmp_year, tmp_month, tmp_day = 0;

    t->jd = (t->gps_week + (t->gps_sec - LEAP_SECOND) / 604800.0)
            * 7.0 + JULIAN_GPS_TIME;

    iJD = (uint32_t) (t->jd + 0.5);
    L = t->jd + 68569;

    N = (4 * L / 146097);
    L = (L - (146097 * N + 3) / 4);
    tmp_year = (4000 * (L + 1) / 1461001);
    L = (L - 1461 * tmp_year / 4 + 31);
    tmp_month = (80 * L / 2447);
    tmp_day = (L - 2447 * tmp_month / 80);
    L = (tmp_month / 11);
    tmp_month = (tmp_month + 2 - 12 * L);
    tmp_year = 100 * (N - 49) + tmp_year + L;

    t->utc.year = (uint8_t) (tmp_year - 2000);
    t->utc.month = (uint8_t) tmp_month;
    t->utc.day = (uint8_t) tmp_day;
    t->utc.weekday = adcs_time.utc.weekday;

    UT = (t->jd - iJD + 0.5) * 24;
    tmp_hour = UT;
    t->utc.hour = (uint8_t) tmp_hour;
    tmp_min = (tmp_hour - t->utc.hour) * 60;
    t->utc.min = (uint8_t) tmp_min;
    t->utc.sec = (uint8_t) ((tmp_min - t->utc.min) * 60);

}
