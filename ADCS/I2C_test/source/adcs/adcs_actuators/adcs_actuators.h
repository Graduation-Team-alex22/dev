#ifndef ADCS_ACTUATORS_H__
#define	ADCS_ACTUATORS_H__

#include <stdint.h>

#include "spin_motor_driver.h"
#include "mgn_torquer_driver.h"


typedef struct {
    spin_handle_t spin_torquer;
    magneto_handle_t magneto_torquer;
} adcs_actuator_t;

// extern adcs_actuator_t adcs_actuator;

void actuators_init();
void actuators_update();
void actuators_setParam(adcs_actuator_t* );

#endif
