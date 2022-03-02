#ifndef IMU_SENSOR_H__
#define IMU_SENSOR_H__

#include "sensors_common.h"

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

typedef struct {
    int16_t gyr_raw[3];
    float gyr[3];
    float gyr_prev[3];
    float gyr_f[3];
    float calib_gyr[3];
    sensor_status_e gyr_status;

    int16_t xm_raw[3];
    float xm_norm;
    float xm[3];
    float xm_prev[3];
    float xm_f[3];
    sensor_status_e xm_status;
} imu_sensor_t;

void calib_lsm9ds0_gyro(imu_sensor_t *pImuSensor);
sensor_status_e imu_gyro_init(imu_sensor_t *pImuSensor);
sensor_status_e imu_gyro_update(imu_sensor_t *pImuSensor);
sensor_status_e imu_xm_init(imu_sensor_t *pImuSensor);
sensor_status_e imu_xm_update(imu_sensor_t *pImuSensor);


#endif
