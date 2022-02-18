/*
 * adcs_configuration.h
 *
 *  Created on: May 8, 2016
 *      Author: azisi
 */

#ifndef INC_ADCS_CONFIGURATION_H_
#define INC_ADCS_CONFIGURATION_H_

#include "stm32f4xx_hal.h"

#include "adcs_common.h"

#define ADCS_UART_BUF_LEN   2048
#define ADCS_UART_DBG_EN    0
#define UART_DBG_TIMEOUT    1000

#define TEST_EPS_PKT_TIME   120 // in sec

#define EPS_DELAY_READY     1000 // in ms

#define OBC_REQUEST_TIME_UPDATE 10000 // in ms

#define LOOP_TIME           1       // in sec
#define LOOP_TIME_TICKS     50000   // in ticks

#define GPS_ALARM_ON_INIT   30 // in min
#define GPS_ALARM_ON        5  // in hours
#define GPS_ALARM_UNLOCK    12 // in min
#define GPS_ALARM_RESET     8  // in min

#define WDG_REFRESH         50 // Times for reset (WDG), in sec ~61sec

#define MAX_IGRF_NORM   80000 // in nT, to check if the magneto-meter values are valid
#define MIN_IGRF_NORM   10000 // in nT, to check if the magneto-meter values are valid

#define WX_THRES        RAD(0.3) // Threshold to enable B-dot controller (in deg/s)
#define WY_THRES        RAD(0.3) // Threshold to enable B-dot controller (in deg/s)
#define WZ_THRES        RAD(0.3) // Threshold to enable B-dot controller (in deg/s)

#define BDOT_GAIN            10     // Gain of B-dot controller, *0.1Nms
#define POINTING_GAIN_W      1      // Point gain kp1
#define POINTING_GAIN_Q      20     // Point gain kp2
#define SPIN_TORQUER_GAIN    1      // Spin torquer gain, *0.001Nms
#define SPIN_TORQUER_REF_RPM 5000   // Reference RMP in rmp

typedef enum {
    TIMED_EVENT_SERVICED = 0,
    TIMED_EVENT_NOT_SERVICED,
    TIMED_EVENT_LAST_VALUE
}ADCS_timed_event_status;

extern TIM_HandleTypeDef htim7;
extern ADCS_timed_event_status ADCS_event_period_status;

/* Configure control loop timer */
HAL_StatusTypeDef kick_TIM7_timed_interrupt(uint32_t control_loop); // in ticks

#endif /* INC_ADCS_CONFIGURATION_H_ */
