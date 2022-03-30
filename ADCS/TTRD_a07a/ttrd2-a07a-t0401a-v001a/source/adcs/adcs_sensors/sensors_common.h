#ifndef SENSORS_COMMON_H__
#define SENSORS_COMMON_H__

#include <stdint.h>

typedef enum {
    DEVICE_ERROR = 0,
    DEVICE_NORMAL,
    DEVICE_ENABLE,
    DEVICE_DISABLE
} sensor_status_e;

#endif
