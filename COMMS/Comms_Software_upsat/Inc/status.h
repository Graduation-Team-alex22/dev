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

#ifndef INC_STATUS_H_
#define INC_STATUS_H_


enum {
  COMMS_STATUS_DMA_ERROR = -9,
  COMMS_STATUS_RF_OFF = -8,
  COMMS_STATUS_RF_SWITCH_CMD = -7,
  COMMS_STATUS_RXFIFO_ERROR = -6,
  COMMS_STATUS_INVALID_FRAME = -5,
  COMMS_STATUS_TIMEOUT = -4,
  COMMS_STATUS_NO_DATA = -3,
  COMMS_STATUS_BUFFER_OVERFLOW = -2,
  COMMS_STATUS_BUFFER_UNDERFLOW = -1,
  COMMS_STATUS_OK = 0,
};

#endif /* INC_STATUS_H_ */
