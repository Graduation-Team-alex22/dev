#ifndef SUN_SENSOR_H__
#define SUN_SENSOR_H__

#include "sensors_common.h"

/* AD7689; ADC converter for the sun sensor connected throught spi*/
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

typedef struct {
    uint16_t v_sun_raw[5];
    float v_sun[5];
    float sun_rough[3];
    float sun_fine[3];
    float sun_xyz[3];
    sensor_status_e sun_status;
} sun_sensor_t;


sensor_status_e init_sun_sensor(sun_sensor_t *pSunSensor);
sensor_status_e update_sun_sensor(sun_sensor_t *pSunSensor);
sensor_status_e update_ad7682(uint8_t ch, uint16_t *v_raw);




#endif
