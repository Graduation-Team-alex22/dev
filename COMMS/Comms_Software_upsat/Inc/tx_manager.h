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

#ifndef INC_TX_MANAGER_H_
#define INC_TX_MANAGER_H_

#include <stdint.h>
#include <stdlib.h>

int32_t
tx_data(const uint8_t *in, size_t len, uint8_t *dev_rx_buffer, uint8_t is_wod,
	size_t timeout_ms);

int32_t
tx_data_cw (const uint8_t *in, size_t len);

#endif /* INC_TX_MANAGER_H_ */
