#ifndef _PROJECT_H
#define _PROJECT_H

#include "stdint.h"

#define DIAGNOSIS_OUTPUT

typedef uint32_t error_t;

// Error Codes
#define NO_ERROR                             ((error_t) 0)
#define ERROR_GPS_NMEA_BUFFER_FULL           ((error_t) 1101)
#define ERROR_IMU_CONNECTED_BASE             ((error_t) 1200)
#define ERROR_IMU_WAKEUP_BASE                ((error_t) 1210)
#define ERROR_IMU_CONFIG_BASE                ((error_t) 1220)
#define ERROR_IMU_CALIBRATED_BASE            ((error_t) 1230)
#define ERROR_IMU_ACC_UPDATE_BASE            ((error_t) 1240)
#define ERROR_IMU_GYRO_UPDATE_BASE           ((error_t) 1250)
#define ERROR_IMU_MGN_UPDATE_BASE            ((error_t) 1260)
#define ERROR_MGN_CONNECTED_BASE             ((error_t) 1300)
#define ERROR_MGN_CONFIG_BASE                ((error_t) 1310)
#define ERROR_MGN_UPDATE_BASE                ((error_t) 1320)
#define ERROR_TMP_CONNECTED_BASE             ((error_t) 1400)
#define ERROR_TMP_UPDATE_BASE                ((error_t) 1410)
#define ERROR_TLE_FLASH_READ                 ((error_t) 1501) 
#define ERROR_SGP4_BAD_INIT_TLE_DATA         ((error_t) 1601) 
#define ERROR_SGP4_BAD_OP_MODE               ((error_t) 1602)
#define ERROR_RTC_INIT                       ((error_t) 1701)
#define ERROR_RTC_ENTER_INIT_MODE            ((error_t) 1702)

#define PROJECT_I2C1_CLOCK (400000)

void Project_MSP_Init(void);

#endif
