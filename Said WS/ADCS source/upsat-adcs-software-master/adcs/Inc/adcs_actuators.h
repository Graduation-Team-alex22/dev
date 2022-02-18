/*
 * adcs_actuators.h
 *
 *  Created on: May 21, 2016
 *      Author: azisi
 */

#ifndef INC_ADCS_ACTUATORS_H_
#define INC_ADCS_ACTUATORS_H_

/* Spin Torquer */
#define SPIN_ID         0x03
#define SPIN_TIMEOUT    10000
#define MAX_RPM         15000
#define CNT2RPM(x)      48000000*15/(x)
#define RPM2CNT(x)      CNT2RPM(x)
/* MOTOR In sync to bridge pulses, but not locked  yet to reference RPM (MOTOR_INSYNC)
 * MOTOR In Synchronous mode to  reference RPM (MOTOR_LOCKED)
 * MOTOR stalled (that is bad, unless RPMr <100, MOTOR_STALL)
 */
typedef enum {
    MOTOR_ERROR = 0,
    MOTOR_INSYNC,
    MOTOR_LOCKED,
    MOTOR_STALL,
    MOTOR_STOP
} _adcs_spin_status;

typedef struct {
    int32_t RPM;
    uint32_t rampTime;
    uint32_t crc;
    int32_t m_RPM;
    _adcs_spin_status status;
} _adcs_spin;

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
} _adcs_magneto;

typedef struct {
    _adcs_spin spin_torquer;
    _adcs_magneto magneto_torquer;
} _adcs_actuator;

extern _adcs_actuator adcs_actuator;

void init_magneto_torquer(_adcs_actuator *actuator);
void update_magneto_torquer(_adcs_actuator *actuator);
void magneto_torquer_off();
_adcs_spin_status init_spin_torquer(_adcs_actuator *actuator);
_adcs_spin_status get_spin_state(_adcs_actuator *actuator);
_adcs_spin_status update_spin_torquer(_adcs_actuator *actuator);

#endif /* INC_ADCS_ACTUATORS_H_ */
