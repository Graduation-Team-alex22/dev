/*
 * adcs_sensors.h
 *
 *  Created on: Jun 9, 2016
 *      Author: azisi
 */

#ifndef INC_ADCS_SENSORS_H_
#define INC_ADCS_SENSORS_H_

/* IMU LSM9DS0 */
#define GYRO_ADDR       0x6B
#define GYRO_VAL        0x28
#define CTRL_REG1_G     0x20
#define WHO_AM_I_G      0x0F
#define GYRO_ID         0xD4
#define GYRO_GAIN       8.75 / 1e3	// DPS

#define XM_ADDR         0x1D
#define XM_CTR_REG      0x1F
#define WHO_AM_I_XM     0x0F
#define XM_ID           0x49
#define XM_VAL          0x08
#define XM_GAIN         8           // nT/LSB

#define LSM9DS0_TIMEOUT 500         // in ms
#define LSM9DS0_MASK    0x80
/* Gyroscope offsets */
#define AUTO_GYRO_CALIB 0
#define GYRO_N          100         // Calculate gyroscope offset
#define GYRO_OFFSET_X   72.38
#define GYRO_OFFSET_Y   381.63
#define GYRO_OFFSET_Z   409.60

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

/* ADT7420 temperature sensor */
#define ADT7420_ADDRESS             0x48    // ADT7420 address, IC2
#define ADT7420_DEFAULT_ID          0xCB    // ADT7420 default ID
#define ADT7420_REG_TEMP_MSB        0x00    // Temperature value MSB
#define ADT7420_REG_TEMP_LSB        0x01    // Temperature value LSB
#define ADT7420_REG_STATUS          0x02    // Status
#define ADT7420_REG_CONFIG          0x03    // Configuration
#define ADT7420_REG_T_HIGH_MSB      0x04    // Temperature HIGH set point MSB
#define ADT7420_REG_T_HIGH_LSB      0x05    // Temperature HIGH set point LSB
#define ADT7420_REG_T_LOW_MSB       0x06    // Temperature LOW set point MSB
#define ADT7420_REG_T_LOW_LSB       0x07    // Temperature LOW set point LSB
#define ADT7420_REG_T_CRIT_MSB      0x08    // Temperature CRIT set point MSB
#define ADT7420_REG_T_CRIT_LSB      0x09    // Temperature CRIT set point LSB
#define ADT7420_REG_HIST            0x0A    // Temperature HYST set point
#define ADT7420_REG_ID              0x0B    // ID
#define ADT7420_REG_RESET           0x2F    // Software reset
#define ADT7420_TIMEOUT             500     // in ms
/* ADT7420 configure */
#define ADT7420_16BIT               0x80
#define ADT7420_OP_MODE_1_SPS       0x40
#define ADT7420_OP_MODE_CONT_CONV   0x00

/* AD7689 */
#define AD7682_CFG      0x80        // Configuration update
#define AD7682_INCC     0x70        // Input channel configuration
#define AD7682_BW       0x01        // Bandwidth for low-pass filter
#define AD7682_REF      0x20        // Reference/buffer selection
#define AD7682_SEQ      0x00        // Channel sequencer
#define AD7682_RB       0x00        // Read back the CFG register
#define AD7682_CH5      0x0A        // Channel 5
#define AD7682_CH4      0x08        // Channel 4
#define AD7682_CH3      0x06        // Channel 3
#define AD7682_CH2      0x04        // Channel 2
#define AD7682_CH1      0x02        // Channel 1
#define AD7682_COEF     0.0000625   // Convert digital measure to analog
#define AD7682_TIMEOUT	100         // in ms

/* Sun Sensor Coefficient */
#define SUN_SENSOR_VALID    0.7 // Threshold for valid values
#define S_SUN_SENSOR        5   // For fine measure, 5-8
#define SUN_FINE            0   // 1 for fine measurements

/* LP Filter */
#define A_GYRO 0.7    // Coefficient < 1.0
#define A_MGN  0.7    // Coefficient < 1.0
#define A_XM   0.7    // Coefficient < 1.0

typedef enum {
    DEVICE_ERROR = 0,
    DEVICE_NORMAL,
    DEVICE_ENABLE,
    DEVICE_DISABLE
} _adcs_sensor_status;

typedef struct {
    uint16_t temp_raw;
    float temp_c;
    _adcs_sensor_status temp_status;
} _adt7420_temp_sensor;

typedef struct {
    int16_t gyr_raw[3];
    float gyr[3];
    float gyr_prev[3];
    float gyr_f[3];
    float calib_gyr[3];
    _adcs_sensor_status gyr_status;

    int16_t xm_raw[3];
    float xm_norm;
    float xm[3];
    float xm_prev[3];
    float xm_f[3];
    _adcs_sensor_status xm_status;
} _lsm9ds0_sensor;

typedef struct {
    int32_t rm_raw[3];
    float rm_norm;
    float rm[3];
    float rm_prev[3];
    float rm_f[3];
    _adcs_sensor_status rm_status;
} _rm3100_sensor;

typedef struct {
    uint16_t v_sun_raw[5];
    float v_sun[5];
    float sun_rough[3];
    float sun_fine[3];
    float sun_xyz[3];
    _adcs_sensor_status sun_status;
} _ssbv_sun_sensor;

typedef struct {
    _adt7420_temp_sensor temp;
    _lsm9ds0_sensor imu;
    _rm3100_sensor mgn;
    _ssbv_sun_sensor sun;
} _adcs_sensors;

extern _adcs_sensors adcs_sensors;

_adcs_sensor_status init_lsm9ds0_gyro(_adcs_sensors *sensors);
void calib_lsm9ds0_gyro(_adcs_sensors *sensors);
_adcs_sensor_status update_lsm9ds0_gyro(_adcs_sensors *sensors);
_adcs_sensor_status init_lsm9ds0_xm(_adcs_sensors *adcs_sensors);
_adcs_sensor_status update_lsm9ds0_xm(_adcs_sensors *sensors);

_adcs_sensor_status init_rm3100(_adcs_sensors *sensors);
_adcs_sensor_status update_rm3100(_adcs_sensors *sensors);

_adcs_sensor_status init_adt7420(_adcs_sensors *sensors);
_adcs_sensor_status update_adt7420(_adcs_sensors *sensors);

_adcs_sensor_status init_sun_sensor(_adcs_sensors *sensors);
_adcs_sensor_status update_sun_sensor(_adcs_sensors *sensors);
_adcs_sensor_status update_ad7682(uint8_t ch, uint16_t *v_raw);

#endif /* INC_ADCS_SENSORS_H_ */
