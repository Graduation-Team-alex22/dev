#ifndef TIME_H__
#define TIME_H__

#include "stdint.h"

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

#define MAX_YEAR 21

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


/*
typedef struct{
    uint32_t epoch;
    uint32_t elapsed;
    time_utc_t utc;
} time_keeping_t;
*/

typedef struct{
    int ep_year; /* Year of epoch, e.g. 94 for 1994, 100 for 2000AD */
    double ep_day; /* Day of epoch from 00:00 Jan 1st ( = 1.0 ) */
} tle_epoch_t; // _tle_epoch


// time_utc_t utc;

typedef struct {
    uint8_t set_time;
    time_utc_t utc;
    tle_epoch_t tle_epoch; // TLE epoch
    double decyear; // Decimal year, for IGRF
    double jd; // Julian days from 1st Jan 1900, for SGP4
    double gps_sec; // Time in sec of "gps_week" week
    uint16_t gps_week; // Number of weeks from GPS starting date
} time_keeping_adcs;


// extern SAT_returnState crt_pkt(tc_tm_pkt *pkt, TC_TM_app_id app_id, uint8_t type, uint8_t ack, uint8_t ser_type, uint8_t ser_subtype, TC_TM_app_id dest_id);
// extern SAT_returnState route_pkt(tc_tm_pkt *pkt);

void cnv_UTC_QB50(time_utc_t utc, uint32_t *qb);
void set_time_QB50(uint32_t qb);
void set_time_UTC(time_utc_t utc);
void get_time_QB50(uint32_t *qb);
void get_time_UTC(time_utc_t *utc);
uint32_t return_time_QB50();

// SAT_returnState time_management_app(tc_tm_pkt *pck);
// SAT_returnState time_management_report_time_in_utc(tc_tm_pkt *pkt, TC_TM_app_id dest_id);
// SAT_returnState time_management_report_time_in_qb50(tc_tm_pkt *pkt, TC_TM_app_id dest_id);
// SAT_returnState time_management_crt_pkt_TC(tc_tm_pkt *pkt, uint8_t sid, TC_TM_app_id app_id);
// SAT_returnState time_management_crt_pkt_TM(tc_tm_pkt *pkt, uint8_t sid, TC_TM_app_id app_id);
// SAT_returnState time_management_request_time_in_utc( TC_TM_app_id dest_id);
// SAT_returnState time_management_force_time_update( TC_TM_app_id dest_id);


// extern time_keeping_adcs adcs_time;

void tle_epoch(time_keeping_adcs *t);
void decyear(time_keeping_adcs *t);
void julday(time_keeping_adcs *t);
void gps2utc(time_keeping_adcs *t);
/**
 * Convert UTC to Decyimal year and Julian day.
 * @return
 */
void time_converter();

#endif
