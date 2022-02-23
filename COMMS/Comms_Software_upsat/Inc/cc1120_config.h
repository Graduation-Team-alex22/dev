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

#ifndef INC_CC1120_CONFIG_H_
#define INC_CC1120_CONFIG_H_

typedef struct
{
  unsigned int addr;
  unsigned short dat;
} registerSetting_t;

#define VCDAC_START_OFFSET 2
#define FS_VCO2_INDEX 0
#define FS_VCO4_INDEX   1
#define FS_CHP_INDEX   2

/*****************************************************************************
 ********************* TX related definitions ********************************
 ****************************************************************************/

#define CC1120_TX_MAX_FRAME_LEN 255
#define CC1120_TX_FIFO_SIZE 128
#define CC1120_TXFIFO_THR 63
#define CC1120_TXFIFO_IRQ_THR (127 - CC1120_TXFIFO_THR)
#define CC1120_TXFIFO_AVAILABLE_BYTES (CC1120_TX_FIFO_SIZE - CC1120_TXFIFO_IRQ_THR + 2)

/*****************************************************************************
 ********************* RX related definitions ********************************
 ****************************************************************************/
#define CC1120_RX_FIFO_SIZE 128
#define CC1120_RXFIFO_THR 64
#define CC1120_BYTES_IN_RX_FIF0 (CC1120_RXFIFO_THR + 1)

#define CC1120_INFINITE_PKT_LEN 0x40
#define CC1120_FIXED_PKT_LEN 0x00

#endif /* INC_CC1120_CONFIG_H_ */
