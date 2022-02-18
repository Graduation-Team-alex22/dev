/*
 * adcs_actuators.c
 *
 *  Created on: May 21, 2016
 *      Author: azisi
 */

#include "stm32f4xx_hal.h"

#include "adcs_actuators.h"
#include <stdlib.h>
#include <string.h>

extern I2C_HandleTypeDef hi2c2;
extern CRC_HandleTypeDef hcrc;
extern TIM_HandleTypeDef htim4;

_adcs_actuator adcs_actuator;

void init_magneto_torquer(_adcs_actuator *actuator) {
    TIM_MasterConfigTypeDef sMasterConfig;

    actuator->magneto_torquer.current_z = 0;
    actuator->magneto_torquer.current_y = 0;
    actuator->magneto_torquer.duty_cycle_z = 0;
    actuator->magneto_torquer.duty_cycle_y = 0;

    /* Set up period */
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 0; // 1.282 KHz
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = MAGNETO_TORQUER_PERIOD;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim4);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig);

    HAL_TIM_Base_Start(&htim4);

    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
}

void update_magneto_torquer(_adcs_actuator *actuator) {

    if (abs(actuator->magneto_torquer.current_z) > MAX_CURR_MAGNETO_TORQUER) {
        actuator->magneto_torquer.current_z = MAX_CURR_MAGNETO_TORQUER;
    }
    if (actuator->magneto_torquer.current_z >= 0) {
        actuator->magneto_torquer.duty_cycle_z =
                actuator->magneto_torquer.current_z * MAGNETO_TORQUER_RESISTANCE
                        * MAGNETO_TORQUER_PERIOD / MAX_VOLT_MAGNETO_TORQUER;

        htim4.Instance->CCR1 = actuator->magneto_torquer.duty_cycle_z;
        htim4.Instance->CCR2 = 0;
    } else {
        actuator->magneto_torquer.duty_cycle_z =
                -actuator->magneto_torquer.current_z
                        * MAGNETO_TORQUER_RESISTANCE * MAGNETO_TORQUER_PERIOD
                        / MAX_VOLT_MAGNETO_TORQUER;

        htim4.Instance->CCR1 = 0;
        htim4.Instance->CCR2 = actuator->magneto_torquer.duty_cycle_z;
    }

    if (abs(actuator->magneto_torquer.current_y) > MAX_CURR_MAGNETO_TORQUER) {
        actuator->magneto_torquer.current_y = MAX_CURR_MAGNETO_TORQUER;
    }
    if (actuator->magneto_torquer.current_y >= 0) {
        actuator->magneto_torquer.duty_cycle_y =
                actuator->magneto_torquer.current_y * MAGNETO_TORQUER_RESISTANCE
                        * MAGNETO_TORQUER_PERIOD / MAX_VOLT_MAGNETO_TORQUER;

        htim4.Instance->CCR3 = actuator->magneto_torquer.duty_cycle_y;
        htim4.Instance->CCR4 = 0;
    } else {
        actuator->magneto_torquer.duty_cycle_y =
                -actuator->magneto_torquer.current_y
                        * MAGNETO_TORQUER_RESISTANCE * MAGNETO_TORQUER_PERIOD
                        / MAX_VOLT_MAGNETO_TORQUER;

        htim4.Instance->CCR3 = 0;
        htim4.Instance->CCR4 = actuator->magneto_torquer.duty_cycle_y;
    }
}

_adcs_spin_status init_spin_torquer(_adcs_actuator *actuator) {

    actuator->spin_torquer.RPM = 10;
    actuator->spin_torquer.rampTime = 0;
    actuator->spin_torquer.crc = 0;
    actuator->spin_torquer.m_RPM = 0;
    actuator->spin_torquer.status = MOTOR_STOP;

    return get_spin_state(actuator);

}

_adcs_spin_status update_spin_torquer(_adcs_actuator *actuator) {
    uint8_t sendbuf[16];

    /* Check if RPM is 0 to set 10, look RPM2CNT */
    if (actuator->spin_torquer.RPM == 0){
        actuator->spin_torquer.RPM = 10;
    /* Check the limits of RPM */
    } else if (actuator->spin_torquer.RPM > MAX_RPM) {
        actuator->spin_torquer.RPM = MAX_RPM;
    } else if (actuator->spin_torquer.RPM < -MAX_RPM) {
        actuator->spin_torquer.RPM = -MAX_RPM;
    }

    memset(sendbuf, 0, 16);
    /* Hardware Calculation or CRC */
    uint32_t cbuf[3] = { actuator->spin_torquer.status, RPM2CNT(
            actuator->spin_torquer.RPM), actuator->spin_torquer.rampTime };
    actuator->spin_torquer.crc = HAL_CRC_Calculate(&hcrc, cbuf, 3);

    sendbuf[3] = (actuator->spin_torquer.status >> 24) & 0x000000FF;
    sendbuf[2] = (actuator->spin_torquer.status >> 16) & 0x000000FF;
    sendbuf[1] = (actuator->spin_torquer.status >> 8) & 0x000000FF;
    sendbuf[0] = (actuator->spin_torquer.status) & 0x000000FF;

    sendbuf[7] = (RPM2CNT(actuator->spin_torquer.RPM) >> 24) & 0x000000FF;
    sendbuf[6] = (RPM2CNT(actuator->spin_torquer.RPM) >> 16) & 0x000000FF;
    sendbuf[5] = (RPM2CNT(actuator->spin_torquer.RPM) >> 8) & 0x000000FF;
    sendbuf[4] = (RPM2CNT(actuator->spin_torquer.RPM)) & 0x000000FF;

    sendbuf[11] = (actuator->spin_torquer.rampTime >> 24) & 0x000000FF;
    sendbuf[10] = (actuator->spin_torquer.rampTime >> 16) & 0x000000FF;
    sendbuf[9] = (actuator->spin_torquer.rampTime >> 8) & 0x000000FF;
    sendbuf[8] = (actuator->spin_torquer.rampTime) & 0x000000FF;

    sendbuf[15] = (actuator->spin_torquer.crc >> 24) & 0x000000FF;
    sendbuf[14] = (actuator->spin_torquer.crc >> 16) & 0x000000FF;
    sendbuf[13] = (actuator->spin_torquer.crc >> 8) & 0x000000FF;
    sendbuf[12] = (actuator->spin_torquer.crc) & 0x000000FF;

    if (HAL_I2C_Mem_Write(&hi2c2, SPIN_ID, sendbuf[0], 1, &sendbuf[1], 15,
    SPIN_TIMEOUT) != HAL_OK) {
        actuator->spin_torquer.status = MOTOR_ERROR;
        return MOTOR_ERROR;
    }
    return get_spin_state(actuator);
}

_adcs_spin_status get_spin_state(_adcs_actuator *actuator) {
    uint8_t getbuf[17];

    memset(getbuf, 0, 17);

    if (HAL_I2C_Mem_Read(&hi2c2, SPIN_ID, getbuf[0], 1, &(getbuf[1]), 15,
    SPIN_TIMEOUT) != HAL_OK) {
        actuator->spin_torquer.status = MOTOR_ERROR;
        return MOTOR_ERROR;
    }
    actuator->spin_torquer.m_RPM = getbuf[9] << 24;
    actuator->spin_torquer.m_RPM |= getbuf[8] << 16;
    actuator->spin_torquer.m_RPM |= getbuf[7] << 8;
    actuator->spin_torquer.m_RPM |= getbuf[6] << 0;
    actuator->spin_torquer.m_RPM = CNT2RPM(actuator->spin_torquer.m_RPM);

    /* Correct sign of measure */
    if (actuator->spin_torquer.RPM < 0) {
        actuator->spin_torquer.m_RPM = - actuator->spin_torquer.m_RPM;
    }

    switch (getbuf[2]) {
    case 1:
        actuator->spin_torquer.status = MOTOR_INSYNC;
        break;
    case 2:
        actuator->spin_torquer.status = MOTOR_LOCKED;
        break;
    case 3:
        if (actuator->spin_torquer.m_RPM <= 10) {
            actuator->spin_torquer.status = MOTOR_STOP;
        } else {
            actuator->spin_torquer.status = MOTOR_STALL;
        }
        break;
    default:
        actuator->spin_torquer.status = MOTOR_ERROR;
        break;
    }

    return actuator->spin_torquer.status;
}
