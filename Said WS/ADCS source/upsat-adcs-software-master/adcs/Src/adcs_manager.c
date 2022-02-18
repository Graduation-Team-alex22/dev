/*
 * adcs_manager.c
 *
 *  Created on: Aug 5, 2016
 *      Author: azisi
 */

#include "adcs_manager.h"

uint32_t adcs_boot_cnt;
static uint8_t eps_cnt = 0;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim4;

/**
 * Initialize flash memory and health check by getting correct ID
 * @return error to get the error handler
 */
adcs_error_status init_mem() {
    uint8_t flash_id = 0;
    /* Initialize flash memory */
    flash_init();
    if (flash_readID(&flash_id) != FLASH_NORMAL) {
        return ERROR_FLASH;
    }
    return error_propagation(ERROR_OK);
}

/**
 * Calculate ADCS boot counter and save in flash. The boot counter is available
 * in global variable adcs_boot_counter.
 * @return The status of Flash memory and if the boot counter is valid
 */
adcs_error_status increment_boot_counter() {
    adcs_boot_cnt = 0;
    uint8_t adcs_boot_cnt_8[4] = { 0 };
    uint32_t flash_read_address = BOOT_CNT_BASE_ADDRESS;

    for (uint8_t i = 0; i < 4; i++) {
        if (flash_read_byte(&adcs_boot_cnt_8[i], flash_read_address)
                == FLASH_NORMAL) {
            flash_read_address = flash_read_address + BOOT_CNT_OFFSET_ADDRESS;
        } else {
            return ERROR_FLASH;
        }
    }
    cnv8_32(adcs_boot_cnt_8, &adcs_boot_cnt);
    adcs_boot_cnt++;
    if (flash_erase_block4K(BOOT_CNT_BASE_ADDRESS) == FLASH_ERROR) {
        return ERROR_FLASH;
    }
    cnv32_8(adcs_boot_cnt, &adcs_boot_cnt_8);
    if (flash_write_page(adcs_boot_cnt_8, sizeof(adcs_boot_cnt_8),
    BOOT_CNT_BASE_ADDRESS) == FLASH_ERROR) {
        return FLASH_ERROR;
    }
    return error_propagation(ERROR_OK);

}

/**
 * Update EPS counter to send a packet every ~2min to show if ADCS is alive
 */
void update_eps_pkt() {
    /* Send to EPS test packet every 2min */
    if (eps_cnt > TEST_EPS_PKT_TIME) {
        eps_cnt = 0;
        sys_refresh();
    } else {
        eps_cnt++;
    }
}

/**
 * Initialize communication with OBC and report boot counter and reset source
 * @param boot_cnt
 * @return always in ERROR_OK that means no error occurs
 */
adcs_error_status init_obc_communication(uint32_t boot_cnt) {

    uint8_t rst_src = 0;

    pkt_pool_INIT();

    HAL_reset_source(&rst_src);
    set_reset_source(rst_src);
//    event_boot(rst_src, boot_cnt);

    /* Initialize UART2 for OBC */
    HAL_UART_Receive_IT(&huart2, adcs_data.obc_uart.uart_buf, UART_BUF_SIZE);

    return error_propagation(ERROR_OK);

}

/**
 * Communicate with OBC or other subsystems
 * @return always ERROR_OK that means no error occurs
 */
adcs_error_status update_obc_communication() {
    uint32_t time_sys_tick = 0;

    time_sys_tick = HAL_sys_GetTick();
//    uart_killer(OBC_APP_ID, &adcs_data.obc_uart, time_sys_tick);
    pkt_pool_IDLE(time_sys_tick);
    queue_IDLE(OBC_APP_ID);
    import_pkt(OBC_APP_ID, &adcs_data.obc_uart);
    export_pkt(OBC_APP_ID, &adcs_data.obc_uart);

    return error_propagation(ERROR_OK);
}

/**
 * Initialize TLE, read flash to get TLE and update in SGP4 algorithm. If the
 * flash memory is corrupted get the first TLE.
 * @return
 */
adcs_error_status init_tle() {

    adcs_error_status error_status_value = ERROR_OK;
    uint8_t tle_string[TLE_SIZE];
    memset(tle_string, 0, TLE_SIZE);

    /* Read TLE from flash */
    if (flash_read_tle(&temp_tle) == FLASH_ERROR) {
        error_status_value = ERROR_FLASH;
    }
    /* Update if it is correct else get the first TLE */
    if (update_tle(&upsat_tle, temp_tle) == TLE_ERROR) {
        error_status_value = ERROR_TLE;
        sprintf(tle_string,
                "1 25544U 98067A   16229.19636472  .00005500  00000-0  87400-4 0  9991\n2 25544  51.6439 118.5889 0001926 134.0246   3.7037 15.55029964 14324");
        temp_tle = read_tle(tle_string);
        update_tle(&upsat_tle, temp_tle);
    }
    return error_propagation(error_status_value);
}

/**
 * Update the global variables p_eci, v_eci with SGP4 propagator
 * @return The state of SGP4 propagator
 */
adcs_error_status update_sgp4() {

    sgp4_status satpos_status;

    /* Update position and velocity of satellite */
    satpos_status = satpos_xyz(adcs_time.jd, &p_eci, &v_eci);
    /* Check if the SGP4 is correct */
    if (satpos_status == SGP4_ZERO_ECC || satpos_status == SGP4_NEAR_SIMP
            || satpos_status == SGP4_NEAR_NORM) {
        return ERROR_OK;
    }

    return ERROR_SGP4;
}

/**
 * Initialize ADCS actuators and return error status
 * @return
 */
adcs_error_status init_actuators() {
    init_magneto_torquer(&adcs_actuator);
    if (init_spin_torquer(&adcs_actuator) == MOTOR_ERROR) {
        return ERROR_ACTUATOR;
    }
    return error_propagation(ERROR_OK);
}

/**
 * Update the state of actuators and return error status
 * @return
 */
adcs_error_status update_actuators() {
    _adcs_spin_status spin_status_value;

    update_magneto_torquer(&adcs_actuator);
    spin_status_value =  get_spin_state(&adcs_actuator);

    if (spin_status_value == MOTOR_ERROR) {
        return ERROR_ACTUATOR;
    } else if (spin_status_value == MOTOR_STALL) {
        adcs_actuator.spin_torquer.RPM = 0;
    }
    update_spin_torquer(&adcs_actuator);

    return error_propagation(ERROR_OK);
}

/**
 * Close magneto-torquers
 */
void magneto_torquer_off() {
    /* Set OFF z side magneto-torquer */
    htim4.Instance->CCR1 = 0;
    htim4.Instance->CCR2 = 0;
    /* Set OFF y side magneto-torquer */
    htim4.Instance->CCR3 = 0;
    htim4.Instance->CCR4 = 0;
}

/**
 * Open magneto-torquers
 */
void magneto_torquer_on() {
    update_magneto_torquer(&adcs_actuator);
}

/**
 * Switch ON and initialize sensors
 * @return error state
 */
adcs_error_status init_measured_vectors() {

    adcs_error_status error_status_value = ERROR_OK;
    /* Switch ON sensors */
    adcs_pwr_switch(SWITCH_ON, SENSORS);
    HAL_Delay(100);
    /* Initialize sensors */
    if (init_lsm9ds0_gyro(&adcs_sensors) == DEVICE_ERROR) {
        error_status_value = ERROR_SENSOR;
    }
    calib_lsm9ds0_gyro(&adcs_sensors);
    if (init_rm3100(&adcs_sensors) == DEVICE_ERROR) {
        error_status_value = ERROR_SENSOR;
    }
    if (init_lsm9ds0_xm(&adcs_sensors) == DEVICE_ERROR) {
        error_status_value = ERROR_SENSOR;
    }
    if (init_sun_sensor(&adcs_sensors) == DEVICE_ERROR) {
        error_status_value = ERROR_SENSOR;
    }
    if (init_adt7420(&adcs_sensors) == DEVICE_ERROR) {
        error_status_value = ERROR_SENSOR;
    }

    return error_propagation(error_status_value);

}

/**
 * Convert UTC to Decyimal year and Julian day.
 * @return
 */
adcs_error_status time_converter() {

    decyear(&adcs_time);
    julday(&adcs_time);

    return error_propagation(ERROR_OK);
}

/**
 * Update reference vectors, IGRF and sun position vectors
 * @return
 */
adcs_error_status update_reference_vectors() {

    /* Calculate IGRF reference vectors */
    xyz_t p_ecef = { .x = 0, .y = 0, .z = 0 };
    llh_t p_ecef_llh = { .lat = 0, .lon = 0, .alt = 0 };

    ECI2ECEF(adcs_time.jd, p_eci, &p_ecef);
    cart2spher(p_ecef, &p_ecef_llh);
    igrf_vector.sdate = adcs_time.decyear;
    igrf_vector.latitude = p_ecef_llh.lat;
    igrf_vector.longitude = p_ecef_llh.lon;
    igrf_vector.alt = p_ecef_llh.alt;
    geomag(&igrf_vector); // Xm,Ym,Zm in NED
    igrf_vector.norm = norm(igrf_vector.Xm, igrf_vector.Ym, igrf_vector.Zm);

    /* Calculate sun position reference vectors */
    sun_vector.JD_epoch = adcs_time.jd;
    update_sun(&sun_vector); // Sun position in ECI
    ECI2NED(sun_vector.sun_pos_eci, &sun_vector.sun_pos_ned, upsat_tle.ascn,
            upsat_tle.eqinc, upsat_tle.argp + upsat_tle.mnan); // Convert to NED
    sun_vector.norm = norm(sun_vector.sun_pos_ned.x, sun_vector.sun_pos_ned.y,
            sun_vector.sun_pos_ned.z);

    return error_propagation(ERROR_OK);

}

/**
 * Update sensor values
 * @return
 */
adcs_error_status update_measured_vectors() {

    adcs_error_status error_status_value = ERROR_OK;

    /* Calculate measurement vectors */
    if (update_adt7420(&adcs_sensors) == DEVICE_ERROR) {
        error_status_value = ERROR_SENSOR;
    }
    if (update_sun_sensor(&adcs_sensors) == DEVICE_ERROR) {
        error_status_value = ERROR_SENSOR;
    }
    if (update_lsm9ds0_gyro(&adcs_sensors) == DEVICE_ERROR) {
        error_status_value = ERROR_SENSOR;
    }
    if ((update_rm3100(&adcs_sensors) == DEVICE_ERROR)) {
        error_status_value = ERROR_SENSOR;
    }
    if ((update_lsm9ds0_xm(&adcs_sensors) == DEVICE_ERROR)) {
        error_status_value = ERROR_SENSOR;
    }

    return error_propagation(error_status_value);

}

/**
 * Initialize attitude determination algorithm
 * @return
 */
adcs_error_status init_attitude_determination() {

    /* Initialize Attitude determination */
    initWahbaStruct(&WahbaRot, LOOP_TIME);

    WahbaRot.run_flag = false;

    return error_propagation(ERROR_OK);

}

/**
 * Update attitude determination algorithm, calculate Euler angles, angular
 * velocities, rotation matrix and quartenions. Check the sensors values and
 * choose the appropriate.
 * @return
 */
adcs_error_status update_attitude_determination() {

    float gyroscope[3] = { 0 };

    adcs_error_status error_status_value = ERROR_OK;

    /* Set IGRF reference vectors */
    WahbaRot.w_m[0] = igrf_vector.Xm / igrf_vector.norm;
    WahbaRot.w_m[1] = igrf_vector.Ym / igrf_vector.norm;
    WahbaRot.w_m[2] = igrf_vector.Zm / igrf_vector.norm;

    /* Set Sun position reference vectors */
    WahbaRot.w_a[0] = sun_vector.sun_pos_ned.x / sun_vector.norm;
    WahbaRot.w_a[1] = sun_vector.sun_pos_ned.y / sun_vector.norm;
    WahbaRot.w_a[2] = sun_vector.sun_pos_ned.z / sun_vector.norm;

    /* Check if the sun sensor is available */
    if (adcs_sensors.sun.sun_status == DEVICE_ENABLE) {
        WahbaRot.sun_sensor_gain = 1;
    } else if (adcs_sensors.sun.sun_status == DEVICE_DISABLE) {
        /* Propagate the sun sensor vector */
        WahbaRot.sun_sensor_gain = 0.1;
        mulMatrVec(adcs_sensors.sun.sun_xyz, WahbaRot.RotM, WahbaRot.w_a);
    } else {
        /* Error in sun sensor */
        adcs_sensors.sun.sun_xyz[0] = 0;
        adcs_sensors.sun.sun_xyz[1] = 0;
        adcs_sensors.sun.sun_xyz[2] = 0;
        WahbaRot.sun_sensor_gain = 0;
    }
    /* Check magneto-meter values according to norm of IGRF */
    if (adcs_sensors.mgn.rm_norm > MAX_IGRF_NORM
     || adcs_sensors.mgn.rm_norm < MIN_IGRF_NORM) {
        adcs_sensors.mgn.rm_status = DEVICE_ERROR;
        error_status_value = ERROR_SENSOR;
    }
    if (adcs_sensors.imu.xm_norm > MAX_IGRF_NORM
     || adcs_sensors.imu.xm_norm < MIN_IGRF_NORM) {
        adcs_sensors.mgn.rm_status = DEVICE_ERROR;
        error_status_value = ERROR_SENSOR;
    }
    /* Check the magneto-meter sensor to run attitude algorithm */
    if ((adcs_sensors.mgn.rm_status == DEVICE_NORMAL
      && adcs_sensors.imu.xm_status == DEVICE_NORMAL)
      || adcs_sensors.mgn.rm_status == DEVICE_NORMAL) {
        WahbaRotM(adcs_sensors.sun.sun_xyz, gyroscope,
                adcs_sensors.mgn.rm_f, &WahbaRot);
        WahbaRot.run_flag = true;
    } else if (adcs_sensors.imu.xm_status == DEVICE_NORMAL) {
        WahbaRotM(adcs_sensors.sun.sun_xyz, gyroscope,
                adcs_sensors.imu.xm_f, &WahbaRot);
        WahbaRot.run_flag = true;
    }
    return error_propagation(error_status_value);
}

/**
 * Run control for de-tumbling and pointing. Changes between controllers are done
 * according to angular velocities and availability of sun sensor.
 * @return Error status usual is ERROR_OK
 */
adcs_error_status attitude_control() {

    float angular_velocities[3] = { 0 };
    /* Reset control signals */
    adcs_actuator.magneto_torquer.current_z = 0;
    adcs_actuator.magneto_torquer.current_y = 0;
    control.Iz = 0;
    control.Iy = 0;
    control.sp_rpm = 0;
    /* Choose the correct velocities */
    if (WahbaRot.run_flag == false) {
        angular_velocities[0] = adcs_sensors.imu.gyr_f[0];
        angular_velocities[1] = adcs_sensors.imu.gyr_f[1];
        angular_velocities[2] = adcs_sensors.imu.gyr_f[2];

        control.sp_rpm = 0;
    } else {
        angular_velocities[0] = WahbaRot.W[0];
        angular_velocities[1] = WahbaRot.W[1];
        angular_velocities[2] = WahbaRot.W[2];

        spin_torquer_controller(angular_velocities[1], &control);
    }
    /* Run B-dot and spin torquer controller if the angular velocities are bigger than thresholds */
    if (fabsf(angular_velocities[0]) > WX_THRES
     || fabsf(angular_velocities[1]) > WY_THRES
     || fabsf(angular_velocities[2]) > WZ_THRES) {
        /* Check the magneto-meter sensor */
        if ((adcs_sensors.mgn.rm_status == DEVICE_NORMAL
          && adcs_sensors.imu.xm_status == DEVICE_NORMAL)
          || adcs_sensors.mgn.rm_status == DEVICE_NORMAL) {
            b_dot(adcs_sensors.mgn.rm_f, adcs_sensors.mgn.rm_prev,
                    adcs_sensors.mgn.rm_norm, &control);
            /* Set the currents to magneto-torquers in mA*/
            adcs_actuator.magneto_torquer.current_z = (int8_t) (control.Iz * 1000);
            adcs_actuator.magneto_torquer.current_y = (int8_t) (control.Iy * 1000);
        } else if (adcs_sensors.imu.xm_status == DEVICE_NORMAL) {
            b_dot(adcs_sensors.imu.xm_f, adcs_sensors.imu.xm_prev,
                    adcs_sensors.imu.xm_norm, &control);
            /* Set the currents to magneto-torquers in mA*/
            adcs_actuator.magneto_torquer.current_z = (int8_t) (control.Iz * 1000);
            adcs_actuator.magneto_torquer.current_y = (int8_t) (control.Iy * 1000);
        }
    /* If angular velocities are smaller than the thresholds then run pointing controller */
    } else {
        /* Run pointing controller if the sun sensor is available */
        if (adcs_sensors.sun.sun_status == DEVICE_ENABLE
         && WahbaRot.run_flag == true
         && adcs_sensors.mgn.rm_status == DEVICE_NORMAL) {
            /* Run pointing controller when the sun sensor is available */
            pointing_controller(adcs_sensors.mgn.rm_f, adcs_sensors.imu.xm_norm,
                    &WahbaRot, &control);
            /* Set the currents to magneto-torquers in mA*/
            adcs_actuator.magneto_torquer.current_z = (int8_t) (control.Iz * 1000);
            adcs_actuator.magneto_torquer.current_y = (int8_t) (control.Iy * 1000);
            b_dot(adcs_sensors.mgn.rm_f, adcs_sensors.mgn.rm_prev,
                    adcs_sensors.mgn.rm_norm, &control);
            /* Set the currents to magneto-torquers in mA*/
            adcs_actuator.magneto_torquer.current_z += (int8_t) (control.Iz * 1000);
            adcs_actuator.magneto_torquer.current_y += (int8_t) (control.Iy * 1000);
        }
    }

    /* Set spin torquer RPM */
    adcs_actuator.spin_torquer.RPM = control.const_rmp + control.sp_rpm;

    return error_propagation(ERROR_OK);

}

/**
 * Initialize GPS serial and RTC alarm.
 * @param gps_utc: get UTC time to calculate the next RTC alarm
 * @param gps_state_value: initialize the global struct gps_state
 * @return adcs_error_status: return error for error handling and report
 */
adcs_error_status init_gps(struct time_utc gps_utc, _gps_state *gps_state_value) {
    adcs_error_status error_status_value;

    init_gps_uart();
    /* Set RTC alarm to open GPS because the time isn't set from OBC */
    HAL_SetAlarm_GPS(gps_utc.hour, GPS_ALARM_ON_INIT, gps_utc.sec);
    gps_state_value->status = GPS_OFF;
    error_status_value = ERROR_OK;

    return error_propagation(error_status_value);
}

/**
 * Update GPS alarm, after the system got correct time. In the case of the
 * system couldn't recover the time from flash, update the RTC to open after 5H.
 * @param gps_utc Get the UTC to set the RTC in ERROR FLASH
 * @param gps_state_value Update global gps_state variable
 * @return for error handling and report
 */
adcs_error_status update_gps_alarm_from_flash(struct time_utc gps_utc,
        _gps_state *gps_state_value) {

    uint32_t flash_read_address = GPS_ALARM_BASE_ADDRESS;
    uint8_t gps_flash[4];
    adcs_error_status error_status_value;

    /* Read previous state of GPS */
    memset(gps_flash, 0, 4);
    /* gps_flash[0]: Hours, gps_flash[1]: Min, gps_flash[2]: sec, gps_flash[3]: GPS status */
    for (uint8_t i = 0; i < 4; i++) {
        if (flash_read_byte(&gps_flash[i], flash_read_address)
                == FLASH_NORMAL) {
            flash_read_address = flash_read_address + GPS_ALARM_OFFSET_ADDRESS;
        } else {
            error_status_value = ERROR_FLASH;
            break;
        }
    }
    /* Check if values are correct */
    if (gps_flash[0] > 24 || gps_flash[1] > 60 || gps_flash[2] > 60
            || gps_flash[3] > 5) {
        error_status_value = ERROR_FLASH;
        HAL_SetAlarm_GPS_ON(gps_utc);
        gps_state_value->status = GPS_OFF;
    } else {
        /* Set the alarm according to the previous status */
        gps_state_value->status = gps_flash[3];
        if (gps_state.status == GPS_UNLOCK || gps_state.status == GPS_RESET) {
            gps_state_value->status = HAL_SetAlarm_GPS_LOCK(gps_utc,
            GPS_ALARM_UNLOCK);
            error_status_value = ERROR_OK;
        } else {
            HAL_SetAlarm_GPS(gps_flash[0], gps_flash[1], gps_flash[2]);
            gps_state_value->status = GPS_OFF;
            error_status_value = ERROR_OK;
        }
    }
    return error_propagation(error_status_value);
}

/**
 * Update GPS state and get the correct arguments.
 * @param gps_state_value Fill the global struct gps_state if GPS is fixed.
 * @return adcs_error_status return error for error handling and report
 */
adcs_error_status update_gps(_gps_state *gps_state_value) {

    adcs_error_status error_status_value = ERROR_OK;
    uint8_t gps_flash[7] = { 0 };

    if (gps_state_value->status == GPS_UNLOCK
            || gps_state_value->status == GPS_RESET) {
        /* Reset GPS, to try again */
        if (gps_state_value->reset_flag == true) {
            adcs_pwr_switch(SWITCH_OFF, GPS);
            HAL_Delay(100);
            adcs_pwr_switch(SWITCH_ON, GPS);
            gps_state_value->reset_flag = false;
        }
        /* Get the GPS sentences and if is valid put in gps_state struct */
        get_gps_sentences();
        /* Check if the gps_struct is valid and update the time and TLE */
        if (gps_state_value->d3fix == 3 && gps_state_value->num_sat >= 3
                && gps_state_value->sec != 0 && gps_state_value->week != 0
                && gps_state_value->p_gps_ecef.x != 0
                && gps_state_value->p_gps_ecef.y != 0
                && gps_state_value->p_gps_ecef.z != 0
                && gps_state_value->v_gps_ecef.x != 0
                && gps_state_value->v_gps_ecef.y != 0
                && gps_state_value->v_gps_ecef.z != 0) {
            /* Update time and set ADCS RTC */
            adcs_time.gps_sec = gps_state_value->sec;
            adcs_time.gps_week = gps_state_value->week;
            gps2utc(&adcs_time);
            set_time_UTC(adcs_time.utc);
            /* Update time of OBC */
            time_management_force_time_update(OBC_APP_ID);
            /* Calculate new TLE */
            tle_epoch(&adcs_time);
            temp_tle = calculate_tle(gps_state_value->p_gps_ecef,
                    gps_state_value->v_gps_ecef, adcs_time.tle_epoch);
            if (update_tle(&upsat_tle, temp_tle) == TLE_NORMAL) {
                if (flash_write_tle(&upsat_tle) == FLASH_ERROR) {
                    error_status_value = ERROR_FLASH;
                }
            } else {
                error_status_value = ERROR_TLE;
            }
            /* Set GPS alarm to hit in 5 hours */
            adcs_pwr_switch(SWITCH_OFF, GPS);
            get_time_UTC(&adcs_time.utc);
            gps_state_value->status = HAL_SetAlarm_GPS_ON(adcs_time.utc);
            if (gps_state_value->status == GPS_ERROR_FLASH) {
                error_status_value = ERROR_FLASH;
                gps_state_value->status = GPS_OFF;
            }
            /* Write to flash GPS lock time to recover after reset */
            gps_flash[0] = gps_state.num_sat;
            cnv16_8(gps_state.week, &gps_flash[1]);
            cnv32_8((uint32_t) gps_state.sec, &gps_flash[3]);
            if (flash_erase_block4K(GPS_LOCK_BASE_ADDRESS) == FLASH_NORMAL) {
                flash_write_page(gps_flash, sizeof(gps_flash),
                GPS_LOCK_BASE_ADDRESS);
            } else {
                error_status_value = ERROR_FLASH;
            }
            /* Initialize again GPS state */
            gps_state_value->p_gps_ecef.x = 0;
            gps_state_value->p_gps_ecef.y = 0;
            gps_state_value->p_gps_ecef.z = 0;
            gps_state_value->v_gps_ecef.x = 0;
            gps_state_value->v_gps_ecef.y = 0;
            gps_state_value->v_gps_ecef.z = 0;
            gps_state_value->sec = 0;
            gps_state_value->utc_time = 0;
            gps_state_value->week = 0;
            gps_state_value->d3fix = 0;
            gps_state_value->num_sat = 0;
        }
    } else if (gps_state_value->status == GPS_OFF) {
        adcs_pwr_switch(SWITCH_OFF, GPS);
        error_status_value = ERROR_OK;
    } else if (gps_state_value->status == GPS_ERROR_FLASH) {
        error_status_value = ERROR_FLASH;
        gps_state_value->status = GPS_OFF;
    }

    return error_propagation(error_status_value);
}
