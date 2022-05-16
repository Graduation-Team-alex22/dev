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
// getter called when no available date is there
#define ERROR_OBC_COMM_BAD_GETTER            ((error_t) 1801)

#define PROJECT_I2C1_CLOCK (400000)

/****************** Sensors config **********************/
#define MAX_IGRF_NORM   0.8         // in guass, to check if the magneto-meter values are valid
#define MIN_IGRF_NORM   0.1         // in guass, to check if the magneto-meter values are valid

/* GPS */
#define GPS_NO_DATA_TIME   50       // how much time (in cycles) with no new data to wait
                                    // before switching to the other GPS module

/* Low Pass Filter Co-ef */
#define A_GYRO ((float)0.7)            // Coefficient < 1.0
#define A_MGN  ((float)0.7)            // Coefficient < 1.0
#define A_XM   ((float)0.7)            // Coefficient < 1.0

/* Solar panel */
#define SLR_NUM               (6)      // don't change
#define ADC2VOLTS_COEF        ((float) 0.000625)
#define DAY_LIGHT_THRESHOLD   ((float) 0.7)  // in volts

/**************** Controller config **********************/
#define WX_THRES        RAD(0.3)       // Threshold to enable B-dot controller (in deg/s)
#define WY_THRES        RAD(0.3)       // Threshold to enable B-dot controller (in deg/s)
#define WZ_THRES        RAD(0.3)       // Threshold to enable B-dot controller (in deg/s)

#define BDOT_FILTER          0.5f      // is 0 < BDOT_FILTER < 1
#define BDOT_GAIN            10        // Gain of B-dot controller, *0.1Nms
#define POINTING_GAIN_W      1         // Point gain kp1
#define POINTING_GAIN_Q      20        // Point gain kp2
#define SPIN_TORQUER_GAIN    1         // Spin torquer gain, *0.001Nms
#define SPIN_TORQUER_REF_RPM 5000      // Reference RMP in rmp

/**************** Actuators config **********************/
#define A_COIL                1.55f       // in m^2 = nA, n: number of turns, A: area of one turn
#define I_SPIN_TORQUER        2.12E-07f   // Fly wheel moment of inertia kg*m^2
#define SATURATION_RPM        2000        // Limit for RPM of spin torquer
#define SPIN_TORQUER_FILTER_P 0.99f       // Pole in spin torquer filter
#define SPIN_TORQUER_FILTER_Z 1           // Zero in spin torquer filter
#define LOOP_TIME             1           // in sec

//=========================================================//
/********************** PIN / PORTS ************************/
// sensor activation pins
#define SENSOR_ACTIVATION_PORT   GPIOA
#define GPS_ACTIVATION_PIN       GPIO_Pin_8
#define IMU_ACTIVATION_PIN       GPIO_Pin_9
#define MGN_ACTIVATION_PIN       GPIO_Pin_10
#define TMP_ACTIVATION_PIN       GPIO_Pin_13

void Project_MSP_Init(void);
void HardFault_Handler(void);

#endif
