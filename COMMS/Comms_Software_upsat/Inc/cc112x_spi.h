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

#ifndef __CC1120X_SPI_H
#define __CC1120X_SPI_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "cc_definitions.h"
#include "cc_commands.h"
#include "config.h"
#include "scrambler.h"
#include "cw.h"

#define CC_EXT_ADD 0x2F00

uint8_t
cc_tx_rd_reg (uint16_t addr, uint8_t *data);
uint8_t
cc_tx_wr_reg (uint16_t addr, uint8_t data);
HAL_StatusTypeDef
cc_tx_spi_write_fifo(const uint8_t *data, uint8_t *spi_rx_data, size_t len);
int32_t
cc_tx_data_continuous (const uint8_t *data, size_t size, uint8_t *rec_data,
		       size_t timeout_ms);
int32_t
cc_tx_cw(const cw_pulse_t *in, size_t len);
uint8_t
cc_tx_cmd (uint8_t CMDStrobe);

HAL_StatusTypeDef
cc_rx_spi_read_fifo(uint8_t *out, size_t len);
HAL_StatusTypeDef
cc_rx_spi_write_fifo(uint8_t *data, uint8_t *spi_rx_data, size_t len);
uint8_t
cc_rx_rd_reg (uint16_t addr, uint8_t *data);
uint8_t
cc_rx_wr_reg (uint16_t addr, uint8_t data);
int32_t
cc_rx_data_packet (uint8_t *out, size_t len, size_t timeout_ms);
uint8_t
cc_rx_cmd (uint8_t CMDStrobe);

uint8_t
cc_rx_check_fifo_status();

#endif
