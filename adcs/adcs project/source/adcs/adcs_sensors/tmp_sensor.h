#ifndef TMP_SENSOR_H__
#define TMP_SENSOR_H__

#include "sensors_common.h"

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

typedef struct {
    uint16_t temp_raw;
    float temp_c;
    sensor_status_e temp_status;
} temp_sensor_t;


sensor_status_e temp_sensor_init(temp_sensor_t *pTempSensor);
sensor_status_e temp_sensor_update(temp_sensor_t *pTempSensor);

#endif
