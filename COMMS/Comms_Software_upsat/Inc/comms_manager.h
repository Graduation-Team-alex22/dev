/*
 * upsat-comms-software: Communication Subsystem Software for UPSat satellite
 *
 *  Copyright (C) 2016, Libre Space Foundation <http://librespacefoundation.org/>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INC_COMMS_MANAGER_H_
#define INC_COMMS_MANAGER_H_

#include "rx_manager.h"
#include "tx_manager.h"
#include "config.h"

#define FRAME_OK	1

/**
 * Struct containing a set of TX jobs that the dispatcher should take care
 */
typedef struct {
  uint8_t tx_cw; 	/**< TX a CW beacon */
  uint8_t tx_wod;	/**< TX a WOD beacon */
  uint8_t tx_ext_wod;   /**< TX an exWOD beacon */
} comms_tx_job_list_t;

typedef struct {
  uint32_t time_ms;
  uint8_t is_armed;
} comms_cmd_ctrl_t;

int32_t
recv_payload(uint8_t *out, size_t len, size_t timeout_ms);

int32_t
send_payload(const uint8_t *in, size_t len, uint8_t is_wod, size_t timeout_ms);

int32_t
send_payload_cw(const uint8_t *in, size_t len);

int32_t
send_cw_beacon();

uint8_t
is_tx_enabled();

uint8_t
is_cmd_ctrl_enabled();

void
set_cmd_and_ctrl_period(uint8_t enable);

void
comms_init();

int32_t
comms_routine_dispatcher(comms_tx_job_list_t *tx_jobs);


#endif /* INC_COMMS_MANAGER_H_ */
