/*
 * adcs_control.h
 *
 *  Created on: Jul 16, 2016
 *      Author: azisi
 */

#ifndef ADCS_CONTROL_H_
#define ADCS_CONTROL_H_

#include "WahbaRotM.h"

#define A_COIL                1.55     // in m^2 = nA, n: number of turns, A: area of one turn
#define BDOT_FILTER           0.5      // is 0 < BDOT_FILTER < 1
#define I_SPIN_TORQUER        2.12E-07 // Fly wheel moment of inertia kg*m^2
#define SATURATION_RPM        2000     // Limit for RPM of spin torquer
#define SPIN_TORQUER_FILTER_P 0.99     // Pole in spin torquer filter
#define SPIN_TORQUER_FILTER_Z 1        // Zero in spin torquer filter

typedef struct {
    /* B-dot Controller */
    float b_dot[3];
    float b_dot_prev[3];
    uint16_t k_bdot;
    /* Pointing controller */
    uint16_t k_pointing[2];
    int16_t sp_yaw;
    int16_t sp_pitch;
    int16_t sp_roll;
    /* Control output for magneto torquers */
    float Ix, Iy, Iz;
    /* Spin torquer control */
    uint16_t k_spin;
    float sp_rpm;
    int32_t const_rmp;
} _adcs_control;

extern _adcs_control control;

void b_dot(float b[3], float b_prev[3], float b_norm, _adcs_control *control_struct);
void spin_torquer_controller(float w, _adcs_control *control_struct);
void pointing_controller(float b[3], float b_norm, WahbaRotMStruct *WStruct,
        _adcs_control *control_struct);

#endif /* ADCS_CONTROL_H_ */
