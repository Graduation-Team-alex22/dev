#ifndef MGN_SENSOR_H__
#define MGN_SENSOR_H__

#include "sensors_common.h"

/* RM3100 Magnetometer, SPI1 */
#define PNI_POLL        0x00    // Polls for a Single Measurement
#define PNI_CMM         0x01    // Initiates Continuous Measurement Mode
#define PNI_CCX         0x04    // Cycle Count Register – X Axis
#define PNI_CCY         0x06    // Cycle Count Register – Y Axis
#define PNI_CCZ         0x08    // Cycle Count Register – Z Axis
#define PNI_TMRC        0x0B    // Sets Continuous Measurement Mode Data Rate
#define PNI_ALLX        0x0C    // Alarm Lower Limit – X Axis
#define PNI_AULX        0x0F    // Alarm Upper Limit – X Axis
#define PNI_ALLY        0x12    // Alarm Lower Limit – Y Axis
#define PNI_AULY        0x15    // Alarm Upper Limit – Y Axis
#define PNI_ALLZ        0x18    // Alarm Lower Limit – Z Axis
#define PNI_AULZ        0x1B    // Alarm Upper Limit – Z Axis
#define PNI_ADLX        0x1E    // Alarm Hysteresis Value – X Axis
#define PNI_ADLY        0x20    // Alarm Hysteresis Value – Y Axis
#define PNI_ADLZ        0x22    // Alarm Hysteresis Value – Z Axis
#define PNI_MX          0x24    // Measurement Results – X Axis
#define PNI_MY          0x27    // Measurement Results – Y Axis
#define PNI_MZ          0x2A    // Measurement Results – Z Axis
#define PNI_BIST        0x33    // Built-In Self Test
#define PNI_STATUS      0x34    // Status of DRDY
#define PNI_HSHAKE      0x35    // Handshake Register
#define PNI_REVID       0x36    // MagI2C Revision Identification
#define SM_ALL_AXIS     0x70    // Single measurement mode
#define STATUS_MASK     0x80    // To get status of data ready
#define PNI_CyclesMSB   0x00
#define PNI_CyclesLSB   0xC8
#define PNI_GAIN        1e3/75.0  // nT/LSB
#define PNI_DEFAULT_ID  0x22
#define PNI_TIMEOUT     500       // in ms

typedef struct {
    int32_t rm_raw[3];
    float rm_norm;
    float rm[3];
    float rm_prev[3];
    float rm_f[3];
    sensor_status_e rm_status;
} mgn_sensor_t;

sensor_status_e mgn_sensor_init(mgn_sensor_t *pMgnSensor);
sensor_status_e mgn_sensor_update(mgn_sensor_t *pMgnSensor);


#endif
