/*
 * adcs_control.c
 *
 *  Created on: Jul 16, 2016
 *      Author: azisi
 */
#include "stm32f4xx_hal.h"

#include "adcs_control.h"
#include "adcs_configuration.h"
#include "adcs_common.h"

#include "sysview.h"

_adcs_control control = { .b_dot[0] = 0, .b_dot[1] = 0, .b_dot[2] = 0,
                          .b_dot_prev[0] = 0, .b_dot_prev[1] = 0,
                          .b_dot_prev[2] = 0,.k_bdot = BDOT_GAIN,
                          .k_pointing[0] = POINTING_GAIN_W,
                          .k_pointing[1] = POINTING_GAIN_Q,
                          .sp_yaw = 0, .sp_pitch = 0, .sp_roll = 0,
                          .Ix = 0, .Iy = 0, .Iz = 0 ,
                          .k_spin = SPIN_TORQUER_GAIN,
                          .sp_rpm = 0, .const_rmp = SPIN_TORQUER_REF_RPM };

void b_dot(float b[3], float b_prev[3], float b_norm,
        _adcs_control *control_struct) {

    float b_dot_x, b_dot_y, b_dot_z = 0;

    control_struct->b_dot_prev[0] = control_struct->b_dot[0];
    control_struct->b_dot_prev[1] = control_struct->b_dot[1];
    control_struct->b_dot_prev[2] = control_struct->b_dot[2];
    /* Calculate B-dot */
    control_struct->b_dot[0] = (b[0] - b_prev[0]) / LOOP_TIME;
    control_struct->b_dot[1] = (b[1] - b_prev[1]) / LOOP_TIME;
    control_struct->b_dot[2] = (b[2] - b_prev[2]) / LOOP_TIME;
    /* Moving average for B-dot */
    b_dot_x = BDOT_FILTER * control_struct->b_dot[0]
            + (1 - BDOT_FILTER) * control_struct->b_dot_prev[0];
    b_dot_y = BDOT_FILTER * control_struct->b_dot[1]
            + (1 - BDOT_FILTER) * control_struct->b_dot_prev[1];
    b_dot_z = BDOT_FILTER * control_struct->b_dot[2]
            + (1 - BDOT_FILTER) * control_struct->b_dot_prev[2];
    /* Calculate the currents of coils in A */
    control_struct->Ix = -((float) (control_struct->k_bdot) * 0.1 / A_COIL) * b_dot_x
            * (1 / b_norm);
    control_struct->Iy = -((float) (control_struct->k_bdot) * 0.1 / A_COIL) * b_dot_y
            * (1 / b_norm);
    control_struct->Iz = -((float) (control_struct->k_bdot) * 0.1 / A_COIL) * b_dot_z
            * (1 / b_norm);

}

static float rpm_in_prev = 0;
static float rpm_out_prev = 0;
static float rpm_sum = 0;

void spin_torquer_controller(float w, _adcs_control *control_struct) {

    rpm_in_prev = rpm_sum;
    rpm_out_prev = control_struct->sp_rpm;
    /* Integration of RPM */
    rpm_sum += (-(float) (control_struct->k_spin) * 0.001)
            * (w * RAD2RPM / I_SPIN_TORQUER) * LOOP_TIME;
    /* Check for saturation */
    if (rpm_sum > SATURATION_RPM) {
        rpm_sum = SATURATION_RPM;
    } else if (rpm_sum < -SATURATION_RPM) {
        rpm_sum = -SATURATION_RPM;
    }
    /* Filter the output of RPM */
    control_struct->sp_rpm = rpm_sum - SPIN_TORQUER_FILTER_Z *
            rpm_in_prev + SPIN_TORQUER_FILTER_P * rpm_out_prev;
    /* Check for saturation if output RPM */
    if (control_struct->sp_rpm > SATURATION_RPM) {
        control_struct->sp_rpm = SATURATION_RPM;
    } else if (control_struct->sp_rpm < -SATURATION_RPM) {
        control_struct->sp_rpm = -SATURATION_RPM;
    }

}

void pointing_controller(float b[3], float b_norm, WahbaRotMStruct *WStruct,
        _adcs_control *control_struct) {

    float m_w[3], m_q[3] = { 0 };
    Quat4 sp_quat;
    float sp_rotm[3][3], rotm[3][3] = { 0 };
    float sp_euler[3] = { 0 };

    /* Convert set points to quaternions */
    sp_euler[0] = 0;
    sp_euler[1] = (float) RAD(control_struct->sp_pitch);
    sp_euler[2] = 0;
    euler2rotm((float *) rotm, (const float *) sp_euler);
    mulMatr(sp_rotm, rotm, WStruct->RotM);
    rotmtx2quat((const float *)sp_rotm, &sp_quat);
    /* Calculate control signal from angular velocities */
    m_w[0] = -(float) control_struct->k_pointing[0] * 0.01
            * (b[1] * WStruct->W[2] - b[2] * WStruct->W[1]);
    m_w[1] = -(float) control_struct->k_pointing[0] * 0.01
            * (b[0] * WStruct->W[2] - b[2] * WStruct->W[0]);
    m_w[2] = -(float) control_struct->k_pointing[0] * 0.01
            * (b[0] * WStruct->W[1] - b[1] * WStruct->W[0]);
    SYSVIEW_PRINT("CONTROL %.2f %.2f %.2f", m_w[0], m_w[1], m_w[2]);

    /* Calculate control signal from pitch set point */
    m_q[0] = -(float) control_struct->k_pointing[1] * 0.01
            * (b[1] * sp_quat.z - b[2] * sp_quat.y);
    m_q[1] = -(float) control_struct->k_pointing[1] * 0.01
            * (b[0] * sp_quat.z - b[2] * sp_quat.x);
    m_q[2] = -(float) control_struct->k_pointing[1] * 0.01
            * (b[0] * sp_quat.y - b[1] * sp_quat.x);
    SYSVIEW_PRINT("CONTROL %.2f %.2f %.2f", m_q[0], m_q[1], m_q[2]);

    /* Convert to current in A */
    control_struct->Ix = (m_q[0] + m_w[0]) / b_norm;
    control_struct->Iy = (m_q[1] + m_w[1]) / b_norm;
    control_struct->Iz = (m_q[2] + m_w[2]) / b_norm;
}
