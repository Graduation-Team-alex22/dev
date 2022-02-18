/*
 * adcs_manager.h
 *
 *  Created on: Aug 5, 2016
 *      Author: azisi
 */

#ifndef ADCS_MANAGER_H_
#define ADCS_MANAGER_H_

#include "stm32f4xx_hal.h"

#include "adcs_error_handler.h"
#include "adcs_configuration.h"
#include "adcs_flash.h"
#include "adcs_switch.h"
#include "adcs_time.h"
#include "adcs_frame.h"

#include "WahbaRotM.h"
#include "adcs_control.h"
#include "adcs_sensors.h"
#include "adcs_actuators.h"

#include "adcs_gps.h"

#include "sgp4.h"
#include "geomag.h"
#include "sun_pos.h"

#include "adcs_hal.h"
#include "adcs.h"
#include "service_utilities.h"
#include "event_reporting_service.h"
#include "queue.h"

extern uint32_t adcs_boot_cnt;

adcs_error_status init_mem();
adcs_error_status increment_boot_counter();
void update_eps_pkt();
adcs_error_status init_obc_communication(uint32_t boot_cnt);
adcs_error_status update_obc_communication();
adcs_error_status init_tle();
adcs_error_status update_sgp4();
adcs_error_status init_actuators();
adcs_error_status update_actuators();
void magneto_torquer_off();
void magneto_torquer_on();
adcs_error_status time_converter();
adcs_error_status update_reference_vectors();
adcs_error_status init_measured_vectors();
adcs_error_status update_measured_vectors();
adcs_error_status init_attitude_determination();
adcs_error_status update_attitude_determination();
adcs_error_status attitude_control();
adcs_error_status init_gps(struct time_utc gps_utc, _gps_state *gps_state_value);
adcs_error_status update_gps_alarm_from_flash(struct time_utc gps_utc,
        _gps_state *gps_state_value);
adcs_error_status update_gps(_gps_state *gps_state_value);

#endif /* ADCS_MANAGER_H_ */
