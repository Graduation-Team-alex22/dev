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

#ifndef INC_WOD_HANDLING_H_
#define INC_WOD_HANDLING_H_

#include <stdint.h>
#include <stdlib.h>
#include "utils.h"
#include "ax25.h"
#include <math.h>

/**
 * The number of bytes in every WOD data set WITHOUT the mode bit
 */
#define WOD_DATASET_SIZE 7

/**
 * The maximum number of datasets that each WOD can carry
 */
#define WOD_MAX_DATASETS 32

#define WOD_SIZE (sizeof(uint32_t)				\
	+ ((WOD_DATASET_SIZE*8+1) * WOD_MAX_DATASETS / 8))

/**
 * WOD packet as it handled inside the COMMS
 */
typedef struct {
  uint8_t wod[WOD_SIZE];
  size_t len;
  size_t tx_cnt;
  uint8_t valid;
} comms_wod_t;

typedef struct {
  uint8_t ex_wod[AX25_MAX_FRAME_LEN];
  size_t len;
  size_t tx_cnt;
  uint8_t valid;
} comms_ex_wod_t;


uint8_t
wod_convert_temperature(float val);

void
store_wod_obc(const uint8_t *obc_wod, size_t len);
void
store_ex_wod_obc(const uint8_t *obc_wod, size_t len);

void
comms_wod_init();
void
comms_ex_wod_init();

uint8_t
bat_voltage_valid(uint8_t val);
int32_t
comms_wod_tx(uint8_t bypass_check);
int32_t
comms_ex_wod_tx();
uint8_t
bat_current_valid(uint8_t val);
uint8_t
bus_3300mV_current_valid(uint8_t val);
uint8_t
bus_5000mV_current_valid(uint8_t val);
uint8_t
comms_temp_valid(uint8_t val);
uint8_t
eps_temp_valid(uint8_t val);
uint8_t
bat_temp_valid(uint8_t val);

#endif /* INC_WOD_HANDLING_H_ */
