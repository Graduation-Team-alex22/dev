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

#ifndef CONFIG_H_
#define CONFIG_H_

#include "cc1120_config.h"

static const char __UPSAT_CALLSIGN[] = "ON02GR";
static const uint8_t __UPSAT_SSID = 0;
static const uint8_t __UPSAT_AX25_CTRL = 0x03;
static const char __UPSAT_DEST_CALLSIGN[] = "GND";
static const uint8_t __UPSAT_DEST_SSID = 0;
/**
 * The destination SSID for WOD frames as specified by QB50-SYS-1.5.13
 */
static const uint8_t __UPSAT_DEST_SSID_WOD = 0x0e;

static const char __COMMS_RF_SWITCH_CMD[] = "RF SW CMD";
static const uint8_t __COMMS_RF_SWITCH_HASH[] =
	{0x09, 0x0d, 0x99, 0x0b, 0x6f, 0xae, 0x19, 0x35, 0xdc,
	0xd4, 0x38, 0x94, 0x56, 0xd8, 0x82, 0x47, 0x56, 0xbe,
	0x99, 0x95, 0x9f, 0xd6, 0xb4, 0x45, 0x7a,
	0x9d, 0x0c, 0xb4, 0xd5, 0x86, 0x45, 0xe5};

static const size_t __COMMS_RF_SWITCH_KEY_LEN = 32;
/**
 * A key indicating that the TX RF of the COMMS is on.
 *
 * We use a random generated key instead of a just a
 * boolean value to be sure that even if some bits of the
 * flash memory are destroyed, the system can recover and
 * properly identify the state of the TX RF.
 */
static const uint32_t __COMMS_RF_ON_KEY = 0x16264e84;

/**
 * A key indicating that the TX RF of the COMMS is OFF.
 *
 * We use a random generated key instead of a just a
 * boolean value to be sure that even if some bits of the
 * flash memory are destroyed, the system can recover and
 * properly identify the state of the TX RF.
 */
static const uint32_t __COMMS_RF_OFF_KEY = 0x669d93a3;

/**
 * The offset from the beginning of the flash block in which
 * the RF key is stored
 */
static const uint32_t __COMMS_RF_KEY_FLASH_OFFSET = 0x0;

/**
 * The offset from the beginning of the flash block in which
 * the headless transmission pattern of the
 */
static const uint32_t __COMMS_HEADLESS_TX_FLASH_OFFSET = 0x4;

static const uint32_t __COMMS_DEFAULT_HEADLESS_TX_PATTERN = 0x48;

/**
 * The period after each Command and Control that the satellite
 * will block the beacon transmissions
 */
static const uint32_t __COMMS_CMD_AND_CTRL_PERIOD_MS = 80000;

/**
 * The scrambler polynomial
 */
static const uint32_t __SCRAMBLER_POLY = 0x21;

/**
 * The initial seed of the scrambler
 */
static const uint32_t __SCRAMBLER_SEED = 0x00;

/**
 * The order of the LFSR that the scrambler will use
 */
static const uint32_t __SCRAMBLER_ORDER = 16;

/**
 * The interval of the WOD and other beacons.
 */
static const uint32_t __TX_INTERVAL_MS = 30000;

/**
 * The number of times that a stored WOD or exWOD can be repeated
 * until it is considered invalid
 */
static const uint32_t __WOD_VALID_REPEATS = 6;

/**
 * The interval of the heart beat message to the EPS system
 */
static const uint32_t __HEARTBEAT_EPS_INTERVAL_MS = 100000;

/**
 * The interval of the communication with the OBC
 */
static const uint32_t __OBS_COMM_INTERVAL_MS = 500;

/**
 * The interval that the COMMS checks the RX FIFO of the
 * CC1120 for overflow/underflow errors
 */
static const uint32_t __CC1120_RX_FIFO_CHK_INTERVAL_MS = 500;

/**
 * The OBC sends a WOD to the COMMS every 1 minute. The WOD
 * interval is far less, so the COMMS transmits in these intervals the same
 * WOD. However, if the threshold is reached, this means that the communication
 * with the OBC has a problem. In this case COMMS will transmit other
 * data to the Ground to report the problem.
 */
static const uint32_t __MAX_WOD_RETRASMISSIONS = 6;

/**
 * Enables/disables the UART debug
 */
#define COMMS_UART_DBG_EN 0
#define COMMS_UART_BUF_LEN 512

/**
 * If set to 1, the UART target is the OBC. If set to 0, the UART target
 * is the FTDI debugging dongle
 */
#define COMMS_UART_DEST_OBC 1
/**
 * The default time out period is 4 seconds
 */
#define COMMS_DEFAULT_TIMEOUT_MS 4000

/**
 * The default COMMS statistics polling period
 */
#define COMMS_STATS_PERIOD_MS 1000



#endif /* CONFIG_H_ */
