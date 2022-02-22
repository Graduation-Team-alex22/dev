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

#include "comms_manager.h"
#include "ax25.h"
#include "status.h"
#include "cc_tx_init.h"
#include "cc_rx_init.h"
#include "persistent_mem.h"
#include "utils.h"
#include "log.h"
#include "services.h"
#include "pkt_pool.h"
#include "service_utilities.h"
#include "comms.h"
#include "verification_service.h"
#include "large_data_service.h"
#include "sensors.h"
#include "wod_handling.h"
#include "sha256.h"
#include "sysview.h"

#undef __FILE_ID__
#define __FILE_ID__ 25

static uint8_t interm_buf[AX25_PREAMBLE_LEN + AX25_POSTAMBLE_LEN + AX25_MAX_FRAME_LEN + 2];
static uint8_t spi_buf[AX25_PREAMBLE_LEN
		       + AX25_POSTAMBLE_LEN
		       + AX25_MAX_FRAME_LEN + AX25_MAX_ADDR_LEN];
static uint8_t recv_buffer[AX25_MAX_FRAME_LEN];
static uint8_t send_buffer[AX25_MAX_FRAME_LEN];

volatile extern uint8_t rx_sync_flag;

extern UART_HandleTypeDef huart5;
extern IWDG_HandleTypeDef hiwdg;
extern struct _comms_data comms_data;
comms_rf_stat_t comms_stats;

/**
 * Timer that keeps track of the Command and Control phase
 */
comms_cmd_ctrl_t cmd_and_ctrl;

/**
 * Used to delay the update of the internal statistics and save some cycles
 */
static uint32_t stats_tick;
static uint32_t rx_fifo_tick;

static SHA256_CTX sha_ctx;
/**
 * Disables the TX RF
 */
static inline void
rf_tx_shutdown()
{
  comms_rf_stats_sw_cmd_received(&comms_stats, 0);
  comms_write_persistent_word(__COMMS_RF_OFF_KEY, __COMMS_RF_KEY_FLASH_OFFSET);
}

/**
 * Enables the TX RF
 */
static inline void
rf_tx_enable()
{
  comms_rf_stats_sw_cmd_received(&comms_stats, 1);
  comms_write_persistent_word(__COMMS_RF_ON_KEY, __COMMS_RF_KEY_FLASH_OFFSET);
}

/**
 * Checks if the TX is enabled or not.
 * To avoid the loss of TX due to flash error, this function check if the
 * COMMS TX is enabled by comparing with the COMMS OFF key. If the stored value
 * is equals the OFF key, COMMS is considered in OFF state. Otherwise it is
 * considered ON state.
 *
 * @return 0 if the TX is disabled, 1 if it is enabled
 */
uint8_t
is_tx_enabled()
{
  uint32_t val;

  val = comms_read_persistent_word(__COMMS_RF_KEY_FLASH_OFFSET);
  if(val != __COMMS_RF_OFF_KEY) {
    return 1;
  }
  return 0;
}

/**
 * Returns if the COMMS is during a command and control state.
 *
 * @return 1 if it is during command and control, 0 otherwise
 */
uint8_t
is_cmd_ctrl_enabled()
{
  uint32_t now;
  now = HAL_GetTick();
  if(cmd_and_ctrl.is_armed){
    /* Maybe the armed state is obsolete */
    if (now < cmd_and_ctrl.time_ms
	|| now - cmd_and_ctrl.time_ms > __COMMS_CMD_AND_CTRL_PERIOD_MS) {
      cmd_and_ctrl.is_armed = 0;
      return 0;
    }
    else {
      return 1;
    }
  }
  return 0;
}

/**
 * Sets the satellite on/off command and control phase
 * @param enable true in case the command and control period should be activated,
 * false otherwise
 */
void
set_cmd_and_ctrl_period(uint8_t enable)
{
  cmd_and_ctrl.is_armed = enable;
  cmd_and_ctrl.time_ms = HAL_GetTick();
}

/**
 * Checks if the received frame contains an RF switch command for the COMMS
 * subsystem
 * @param in the payload of the received frame
 * @param len the length of the payload
 * @return 0 in case the received payload was not an RF switch command,
 * 1 if it was.
 */
static inline uint8_t
check_rf_switch_cmd(const uint8_t *in, size_t len)
{
  uint32_t i;
  uint8_t flag = 0;
  uint32_t *sw_key_ptr;
  uint32_t cmd_hdr_len = strlen(__COMMS_RF_SWITCH_CMD);
  uint8_t sha_res[32];

  /* Due to length restrictions, this is definitely not an RF switch command */
  if (len < (cmd_hdr_len + sizeof(__COMMS_RF_ON_KEY))) {
    return 0;
  }

  /*
   * RF switch command is intended only for the COMMS subsystem.
   * There is no ECSS structure here.
   */

  /* This was not an RF switch command. Proceed normally */
  if(strncmp((const char *)in, __COMMS_RF_SWITCH_CMD, cmd_hdr_len) != 0){
    return 0;
  }

  /*
   * Now check if the command is for turning ON or OFF the COMMS.
   * The ON command is constructed using the __COMMS_RF_SWITCH_CMD
   * string followed by the ON key.
   *
   * On the other hand the OFF command, uses the same format having its own
   * key. However, after the key, there is the security key upon which the
   * satellite decides if the command is originating from a valid source.
   */
  sw_key_ptr = (uint32_t *)(in + cmd_hdr_len);
  if(*sw_key_ptr == __COMMS_RF_ON_KEY) {
    rf_tx_enable();
    return 1;
  }
  else if(*sw_key_ptr == __COMMS_RF_OFF_KEY) {
    if(len < cmd_hdr_len + sizeof(__COMMS_RF_ON_KEY) + __COMMS_RF_SWITCH_KEY_LEN){
      comms_rf_stats_sw_cmd_failed(&comms_stats);
      return 0;
    }
    /* Compute the SHA-256 of the received key */
    sha256_init(&sha_ctx);
    sha256_update(&sha_ctx, in + cmd_hdr_len + sizeof(__COMMS_RF_OFF_KEY),
		  __COMMS_RF_SWITCH_KEY_LEN);
    sha256_final(&sha_ctx, sha_res);

    /* Check if the received key matches the stored hash */
    for(i = 0; i < 32; i++){
      if(sha_res[i] != __COMMS_RF_SWITCH_HASH[i]) {
        return 0;
      }
    }
    rf_tx_shutdown();
    return 1;
  }
  else{
    comms_rf_stats_sw_cmd_failed(&comms_stats);
  }
  return 0;
}

/**
 * This function receives a valid frame from the uplink interface and extracts
 * its payload.
 * Valid frames are considered those that have passed the AX.25 CRC-16 and
 * have in the Destination Address field the appropriate destination callsign.
 *
 * @param out the output buffer, large enough to hold a valid payload
 * @param len the size of the buffer
 * @param timeout_ms the timeout limit in milliseconds
 * @return the size of the AX.25 payload or appropriate error code
 */
int32_t
recv_payload(uint8_t *out, size_t len, size_t timeout_ms)
{
  int32_t ret;
  uint8_t check;
  if(len > AX25_MAX_FRAME_LEN) {
    return COMMS_STATUS_BUFFER_OVERFLOW;
  }

  memset(spi_buf, 0, sizeof(spi_buf));
  ret = rx_data(interm_buf, len, timeout_ms);
  if(ret < 1){
    if(ret == AX25_DEC_CRC_FAIL){
      comms_rf_stats_invalid_crc_frame(&comms_stats);
    }
    return ret;
  }

  /* Now check if the frame was indented for us */
  check = ax25_check_dest_callsign(interm_buf, (size_t)ret, __UPSAT_CALLSIGN);
  if(!check){
    comms_rf_stats_invalid_dest_frame(&comms_stats);
    return COMMS_STATUS_INVALID_FRAME;
  }

  /*
   * NOTE: UPSat frames using only Short Address field.
   */
  ret = ax25_extract_payload(out, interm_buf,
			     (size_t) ret, AX25_MIN_ADDR_LEN, 1);

  /* Now check if the received frame contains an RF swicth ON/OFF command */
  if(ret > 0){
    check = check_rf_switch_cmd(out, ret);
    if(check){
      return COMMS_STATUS_RF_SWITCH_CMD;
    }
  }
  return ret;
}

/**
 * Send a payload using AX.25 encoding
 * @param in the payload buffer
 * @param len the length of the payload in bytes
 * @param is_wod set to true if the frame is a WOD
 * @param timeout_ms the timeout limit in milliseconds
 * @return number of bytes sent or appropriate error code
 */
int32_t
send_payload(const uint8_t *in, size_t len, uint8_t is_wod, size_t timeout_ms)
{
  int32_t ret;

  if(len > AX25_MAX_FRAME_LEN) {
    comms_rf_stats_frame_transmitted(&comms_stats, 0,
				     COMMS_STATUS_BUFFER_OVERFLOW);
    return COMMS_STATUS_BUFFER_OVERFLOW;
  }

  /* Check if the TX is enabled */
  if(!is_tx_enabled()){
    /* If the TX is disabled do not count it as error in the RF stats */
    return COMMS_STATUS_RF_OFF;
  }

  memset(spi_buf, 0, sizeof(spi_buf));
  ret = tx_data(in, len, spi_buf, is_wod, timeout_ms);
  comms_rf_stats_frame_transmitted(&comms_stats, ret > 1, ret);
  return ret;
}

/**
 * Sends a payload using CW Morse code
 * @param in the payload buffer
 * @param len the length of the payload in bytes
 * @return the number of bytes sent or an appropriate error code
 */
int32_t
send_payload_cw(const uint8_t *in, size_t len)
{
  int32_t ret;

  if(len > AX25_MAX_FRAME_LEN) {
    return COMMS_STATUS_BUFFER_OVERFLOW;
  }

  /* Check if the TX is enabled */
  if(!is_tx_enabled()){
    return COMMS_STATUS_RF_OFF;
  }

  ret = tx_data_cw(in, len);
  return ret;
}

/**
 * Sends a CW beacons based on the internal COMMS statistics tracking mechanism
 * @return a negative number in case of error
 */
int32_t
send_cw_beacon()
{
  size_t i = 0;

  /* Check if the satellite is during command and control phase */
  if(is_cmd_ctrl_enabled()) {
    return 0;
  }

  memset(send_buffer, 0, AX25_MAX_FRAME_LEN);
  send_buffer[i++] = 'U';
  send_buffer[i++] = 'P';
  send_buffer[i++] = 'S';
  send_buffer[i++] = 'A';
  send_buffer[i++] = 'T';
  send_buffer[i++] = cw_get_bat_voltage_char(&comms_stats);
  send_buffer[i++] = cw_get_bat_current_char(&comms_stats);
  send_buffer[i++] = cw_get_temp_char(&comms_stats);
  send_buffer[i++] = cw_get_uptime_hours_char(&comms_stats);
  send_buffer[i++] = cw_get_uptime_mins_char(&comms_stats);
  send_buffer[i++] = cw_get_cont_errors_char(&comms_stats);
  send_buffer[i++] = cw_get_last_error_char(&comms_stats);
  return send_payload_cw(send_buffer, i);
}

/**
 * This dispatcher checks which communication related task should execute.
 * The task may be:
 * 	1. Serve an RX operation because the corresponding IRQ was triggered
 * 	2. Transmit the CW beacon
 * 	3. Flush and restart the RX operation if the CC1120 reached an invalid
 * 	   state.
 *
 * 	NOTE: Normal FSK TX operations are triggered automatically by the
 * 	ECSS services subsystem and more precisely from the route() function.
 *
 * @param send_cw pointer to a boolean indicating if the COMMS should transmit
 * a CW beacon. It will be reset from this function only if the beacon
 * has been successfully transmitted or the TX subsystem encountered an error
 * during transmission. This is happening because we give a priority to
 * the RX event.
 * @return COMMS_STATUS_OK on success or an appropriate error code.
 */
int32_t
comms_routine_dispatcher(comms_tx_job_list_t *tx_jobs)
{
  int32_t ret = COMMS_STATUS_OK;
  uint32_t now;

  if(tx_jobs == NULL){
    return COMMS_STATUS_NO_DATA;
  }

  /* A frame is received */
  if(rx_sync_flag){
    rx_sync_flag = 0;
    ret = recv_payload(recv_buffer, AX25_MAX_FRAME_LEN,
		       COMMS_DEFAULT_TIMEOUT_MS);
    if(ret > 0) {
      ret = rx_ecss(recv_buffer, ret);
      if(ret == SATR_OK){
	set_cmd_and_ctrl_period(1);
	comms_rf_stats_frame_received(&comms_stats, FRAME_OK, 0);
      }
      else{
	comms_rf_stats_frame_received(&comms_stats, !FRAME_OK, ret);
      }
    }
    else{
      comms_rf_stats_frame_received(&comms_stats, !FRAME_OK, ret);
    }
  }
  else if(tx_jobs->tx_cw){
    tx_jobs->tx_cw = 0;
    ret = send_cw_beacon();
  }
  else if(tx_jobs->tx_wod){
    tx_jobs->tx_wod = 0;
    ret = comms_wod_tx(0);
  }
  else if(tx_jobs->tx_ext_wod){
    tx_jobs->tx_ext_wod = 0;
    ret = comms_ex_wod_tx();
  }
  else{
    import_pkt (OBC_APP_ID, &comms_data.obc_uart);
    export_pkt (OBC_APP_ID, &comms_data.obc_uart);
    now = HAL_GetTick ();

    /* High frequency checks on the RX FIFO affects drastically performance */
    if(now - rx_fifo_tick > __CC1120_RX_FIFO_CHK_INTERVAL_MS){
      rx_fifo_tick = now;
      /* Check the RX FIFO status and act accordingly */
      cc_rx_check_fifo_status();
    }

    /*
     * Update the statistics of the COMMS and reset the watchdog
     * if there are strong reasons to do so.
     *
     * Also, refresh some internal system utilities.
     */
    if (now - stats_tick > COMMS_STATS_PERIOD_MS) {
      stats_tick = now;

      comms_rf_stats_update (&comms_stats);
      if (comms_stats.rx_failed_cnt < 10 && comms_stats.tx_failed_cnt < 5) {
    	  HAL_IWDG_Refresh (&hiwdg);
      }

      large_data_IDLE ();
      pkt_pool_IDLE(now);
    }
  }

  return ret;
}

/**
 * Make all the necessary initializations for the COMMS subsystem
 */
void
comms_init ()
{
  uint8_t cc_id_tx;
  uint8_t cc_id_rx;

  /* fetch TX ID */
  cc_tx_rd_reg (0x2F8F, &cc_id_tx);

  /* fetch RX ID */
  cc_rx_rd_reg (0x2F8F, &cc_id_rx);

  /* Configure TX CC1120 */
  tx_registerConfig ();

  HAL_Delay (10);
  cc_tx_rd_reg (0x2f8F, &cc_id_tx); //i think it's useless..

  /* Configure RX CC1120 */
  rx_register_config ();
  HAL_Delay (10);
  cc_rx_rd_reg (0x2f8F, &cc_id_rx); //i think it's useless also..

  /* Calibrate TX */
  tx_manualCalibration ();
  cc_tx_rd_reg (0x2f8F, &cc_id_tx);

  /* Calibrate RX */
  rx_manual_calibration ();
  cc_rx_rd_reg (0x2f8F, &cc_id_tx); //Why?..

  /* Initialize the TX and RX routines */
  rx_init();
  cw_init();

  large_data_init();

  /*Initialize the COMMS statistics mechanism */
  comms_rf_stats_init(&comms_stats);

  /* Initialize the CC1120 in RX mode */
  cc_rx_cmd(SRX);

  pkt_pool_INIT ();

  comms_wod_init();
  comms_ex_wod_init();

  /* Initialize all the time counters */
  stats_tick = HAL_GetTick();
  rx_fifo_tick = stats_tick;
  set_cmd_and_ctrl_period(0);
}
