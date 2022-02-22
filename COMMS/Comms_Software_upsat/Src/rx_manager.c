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

#include "rx_manager.h"
#include "ax25.h"
#include "cc112x_spi.h"
#include "status.h"
#include "log.h"
#include "scrambler.h"

volatile extern uint8_t rx_sync_flag;
volatile extern uint8_t rx_finished_flag;
volatile extern uint8_t rx_thr_flag;

uint8_t tmp_buf[AX25_MAX_FRAME_LEN + 2];
ax25_handle_t h_ax25dec;

/**
 * Resets all the IRQ flags involved in RX operations
 */
static inline void
reset_rx_irqs()
{
  rx_sync_flag = 0;
  rx_finished_flag = 0;
  rx_thr_flag = 0;
}

void
rx_init()
{
  ax25_rx_init(&h_ax25dec);
}


int32_t
rx_data_continuous (uint8_t *out, size_t len, size_t timeout_ms)
{
  HAL_StatusTypeDef ret;
  uint32_t start_tick;
  uint8_t timeout = 1;
  int32_t ax25_status;
  size_t decode_len;

  cc_tx_wr_reg(PKT_CFG0, CC1120_INFINITE_PKT_LEN);

  reset_rx_irqs();
  ax25_rx_reset(&h_ax25dec);

  /* Enter the chip into RX mode */
  cc_rx_cmd(SFRX);
  cc_rx_cmd(SRX);

  start_tick = HAL_GetTick();

  /* Now wait for the SYNC word to be received */
  while (HAL_GetTick () - start_tick < timeout_ms) {
    if (rx_sync_flag) {
      timeout = 0;
      break;
    }
  }

  /* Timeout occurred, just return */
  if (timeout) {
    cc_rx_cmd (SFRX);
    cc_rx_cmd (SIDLE);
    return COMMS_STATUS_TIMEOUT;
  }

  /* Try to decode an AX.25 message inside the given amount of time */
  while(HAL_GetTick() - start_tick < timeout_ms) {
    timeout = 1;
    while (HAL_GetTick () - start_tick < timeout_ms) {
      if (rx_thr_flag) {
	timeout = 0;
	break;
      }
    }

    if (timeout) {
      cc_rx_cmd (SFRX);
      cc_rx_cmd(SIDLE);
      return COMMS_STATUS_TIMEOUT;
    }

    /* We can now dequeue CC1120_BYTES_IN_RX_FIF0 bytes */
    ret = cc_rx_spi_read_fifo(tmp_buf, CC1120_BYTES_IN_RX_FIF0);
    /*Reset the FIFO flag */
    rx_thr_flag = 0;

    if(ret) {
      cc_rx_cmd(SFRX);
      cc_rx_cmd(SIDLE);
      return COMMS_STATUS_NO_DATA;
    }

    /* Proceed with the AX.25 decoding */
    ax25_status = ax25_recv(&h_ax25dec, out, &decode_len, tmp_buf,
			    CC1120_BYTES_IN_RX_FIF0);
    //LOG_UART_DBG(&huart5, "AX25 status %d", ax25_status);
    if(ax25_status == AX25_DEC_OK) {
      return (int32_t) decode_len;
    }
  }

  /* If this point is reached, that was due to a timeout */
  cc_rx_cmd (SFRX);
  cc_rx_cmd(SIDLE);
  return COMMS_STATUS_TIMEOUT;
}


/**
 * Received and decodes using the AX.25 encapsulation a new frame.
 * This is a blocking call. It will block either until a frame is received
 * or the timeout limit is reached
 * @param out the output buffer
 * @param len the length of the output buffer
 * @param timeout_ms the timeout limit in milliseconds
 * @return the number of bytes received and decoded or appropriate error code.
 * Note that this function does not perform any AX.25 header extraction
 */
int32_t
rx_data(uint8_t *out, size_t len, size_t timeout_ms)
{
  int32_t ret;
  size_t decode_len;
  ax25_rx_reset(&h_ax25dec);

  memset(tmp_buf, 0, sizeof(tmp_buf));
  ret = cc_rx_data_packet(tmp_buf, AX25_MAX_FRAME_LEN, timeout_ms);
  if(ret < 1){
    return ret;
  }

  /* Frame received. Try to decode it using the AX.25 encapsulation */
  ret = ax25_recv(&h_ax25dec, out, &decode_len, tmp_buf, ret);
  if(ret == AX25_DEC_OK){
    return (int32_t) decode_len;
  }
  return ret;
}
