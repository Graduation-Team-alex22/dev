#ifndef MGN_DRIVER_H__
#define	MGN_DRIVER_H__

#include "adcs_actuators.h"

/* Set Up PWM Start up Duty-Cycle */
#define MAGNETO_TORQUER_PERIOD      65535   // 1.282 KHz
#define MAGNETO_TORQUER_RESISTANCE  60      // in Ohm two R in parallel 120Ohm
#define MAX_VOLT_MAGNETO_TORQUER    5000    // in mV
#define MAX_CURR_MAGNETO_TORQUER    85      // in mA

typedef struct {
    int8_t current_z;
    int8_t current_y;
    uint32_t duty_cycle_z;
    uint32_t duty_cycle_y;
} magneto_handle_t;

void magneto_torquer_init(magneto_handle_t *pMagneto_handle);
void magneto_torquer_update(magneto_handle_t *pMagneto_handle);
// void magneto_torquer_on(); same as magneto_torquer_update
void magneto_torquer_off();


#endif
