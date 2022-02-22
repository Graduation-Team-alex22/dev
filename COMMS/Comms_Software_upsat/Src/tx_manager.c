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

#include "tx_manager.h"
#include "ax25.h"
#include "cc112x_spi.h"
#include "status.h"
#include "cw.h"

static uint8_t tmp_buf[AX25_PREAMBLE_LEN + AX25_POSTAMBLE_LEN
		       + AX25_MAX_FRAME_LEN + AX25_MAX_ADDR_LEN];
static cw_pulse_t cw_buf[AX25_MAX_FRAME_LEN * 10];

/**
 * This routine sends the data using the AX.25 encapsulation
 * @param in the input buffer containing the raw data
 * @param len the length of the input buffer
 * @param dev_rx_buffer a buffer that will hold the SPI resulting bytes
 * @param is_wod set to true if the frame is WOD
 * @param timeout_ms the timeout in milliseconds
 * @return the number of bytes sent or appropriate error code
 */
int32_t
tx_data(const uint8_t *in, size_t len, uint8_t *dev_rx_buffer, uint8_t is_wod,
	size_t timeout_ms)
{
  int32_t ret = 0;
  /* This routine can not handle large payloads */
  if(len == 0) {
    return COMMS_STATUS_NO_DATA;
  }

  if(len > AX25_MAX_FRAME_LEN) {
    return COMMS_STATUS_BUFFER_OVERFLOW;
  }

  /* Prepare the AX.25 frame */
  ret = ax25_send(tmp_buf, in, len, is_wod);
  if(ret < 1){
    return COMMS_STATUS_NO_DATA;
  }

  /* Issue the frame at the CC1120 */
  ret = cc_tx_data_continuous(tmp_buf, ret, dev_rx_buffer, timeout_ms);
  if(ret < 1){
    return ret;
  }
  return len;
}

/**
 * Sends data over the air using CW
 * @param in the input data
 * @param len the length of the input data
 * @return the number of the bytes sent or an appropriate negative error code
 */
int32_t
tx_data_cw (const uint8_t *in, size_t len)
{
  int32_t ret = 0;
  size_t symbols_num;
  /* This routine can not handle large payloads */
  if (len == 0) {
    return COMMS_STATUS_NO_DATA;
  }

  if (len > AX25_MAX_FRAME_LEN) {
    return COMMS_STATUS_BUFFER_OVERFLOW;
  }

  ret = cw_encode(cw_buf, &symbols_num, in, len);
  if(ret != CW_OK){
    return COMMS_STATUS_INVALID_FRAME;
  }

  ret = cc_tx_cw(cw_buf, symbols_num);
  if(ret != CW_OK) {
    return COMMS_STATUS_INVALID_FRAME;
  }
  return len;
}

