#ifndef ADCS_SENSORS_H__
#define ADCS_SENSORS_H__

#include <stdint.h>
#include <math.h>
#include <string.h>

#include "stm32f4xx.h"
#include "../hsi_library/stm32f4xx_gpio.h"
#include "../port/port.h"

#include "../services_utilities/time.h"
#include "../services_utilities/services.h"

#include "sun_sensor.h"
#include "mgn_sensor.h"
#include "tmp_sensor.h"
#include "imu_sensor.h"
#include "gps_sensor.h"

typedef struct {
    temp_sensor_t temp;
    imu_sensor_t imu;
    mgn_sensor_t mgn;
    sun_sensor_t sun;
} adcs_sensors_t;


/* Power switch: switches the power for sensors/gps */
typedef enum {
    SWITCH_ON = 0, SWITCH_OFF
} switch_state_e;

typedef struct {
    switch_state_e sens_sw;
    switch_state_e gps_sw;
} adcs_switch_state_t;


// static adcs_switch_state_t adcs_switch_state = {.sens_sw=SWITCH_ON, .gps_sw=SWITCH_ON};


void sensors_init(void);
void sensors_update(void);
adcs_sensors_t sensors_getData(void);




/* ON-OFF Switches */
//void adcs_pwr_switch(switch_state_e switch_state, adcs_switch_e adcs_switch);

/*
	sets the switch structure: adcs_switch_state
	and takes effect on the hardware immediately.
*/
void adcs_pwr_switch_set(adcs_switch_state_t* tmp);
/*
	turns on/off the hardware (sensors/gps) based
	on the values in: adcs_switch_state
*/
void adcs_pwr_switch_update(void);
/*
	returns a copy of: adcs_switch_state
*/
adcs_switch_state_t adcs_pwr_switch_get(void);


#endif
