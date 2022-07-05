// #include "../main/main.h"
#include "comms_manager.h"
#include "../support_functions/cw.h"

#undef __FILE_ID__
#define __FILE_ID__ 25

//extern SAT_returnState verification_app (tc_tm_pkt *pkt);
extern int32_t send_payload (const uint8_t *in, size_t len, uint8_t is_wod);

static uint8_t interm_buf[AX25_PREAMBLE_LEN + AX25_POSTAMBLE_LEN + AX25_MAX_FRAME_LEN + 2];
static uint8_t send_buf[TC_MAX_PKT_SIZE];
static uint8_t send_temp_buf[AX25_PREAMBLE_LEN + AX25_POSTAMBLE_LEN + AX25_MAX_FRAME_LEN + AX25_MAX_ADDR_LEN];
static uint8_t send_buffer[AX25_MAX_FRAME_LEN];


comms_cmd_ctrl_t cmd_and_ctrl;
extern comms_rf_stat_t comms_stats;

static uint32_t stats_tick;
static uint32_t rx_fifo_tick;

static SHA256_CTX sha_ctx;

static inline uint8_t check_rf_switch_cmd(const uint8_t *in, size_t len);



//Update

//Init 
void comms_init(void)
{
	#ifdef 	CC1101_UART
	
	CC_RX_init(9600);
					 
  CC_TX_init(9600);
	
	rx_init();
	
	tx_init();

	cw_init();
	
	large_data_init();
	
	/*Initialize the COMMS statistics mechanism */
  comms_rf_stats_init(&comms_stats);
	
	pkt_pool_INIT ();

  comms_wod_init();
  comms_ex_wod_init();

  /* Initialize all the time counters */
  stats_tick = getCurrentTick();
  rx_fifo_tick = stats_tick;
  set_cmd_and_ctrl_period(0);

	
	#elif CC1120
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
	
	#elif CC1101_SPI

	#endif
}

//Setters
/**
 * Sets the satellite on/off command and control phase
 * @param enable true in case the command and control period should be activated,
 * false otherwise
 */
void
set_cmd_and_ctrl_period(uint8_t enable)
{
  cmd_and_ctrl.is_armed = enable;
  cmd_and_ctrl.time_ms = getCurrentTick();
}



/**
 * This function receives a valid frame from the uplink interface and extracts
 * its payload.
 * Valid frames are considered those that have passed the AX.25 CRC-16 and
 * have in the Destination Address field the appropriate destination callsign.
 *
 * @param out the output buffer, large enough to hold a valid payload
 * @param len the size of the buffer
 * @return the size of the AX.25 payload or appropriate error code
 */
int32_t recv_payload(uint8_t *out, size_t len)
{
	int32_t ret;
  uint8_t check;
//	UART2_BUF_O_Write_String_To_Buffer("in recv_payload\n");
//	UART2_BUF_O_Write_Signed_Number_To_Buffer(len);
  if(len > AX25_MAX_FRAME_LEN) 
	{
		return COMMS_STATUS_BUFFER_OVERFLOW;
	}
	// recvd & decoded frame
	ret = rx_data(interm_buf, len);
//	UART2_BUF_O_Write_Number04_To_Buffer(ret);
	
	if(ret < 1)
	{
		if(ret == AX25_DEC_CRC_FAIL)
		{
      comms_rf_stats_invalid_crc_frame(&comms_stats);
    }
    return ret;
  }

	/* Now check if the frame was indented for us */
  check = ax25_check_dest_callsign(interm_buf, (size_t)ret, __UPSAT_CALLSIGN);// check if its for our cubsat or not
//	UART2_BUF_O_Write_Signed_Number_To_Buffer(check);

  if(!check)
	{
    comms_rf_stats_invalid_dest_frame(&comms_stats);
    return COMMS_STATUS_INVALID_FRAME;
  }
	/*
   * NOTE: UPSat frames using only Short Address field.
   */
  ret = ax25_extract_payload(out, interm_buf,(size_t) ret, AX25_MIN_ADDR_LEN, 1);	
	
	  /* Now check if the received frame contains an RF swicth ON/OFF command */
  if(ret > 0)
	{
    check = check_rf_switch_cmd(out, ret);
		//UART2_BUF_O_Write_Signed_Number_To_Buffer(check);
		
    if(check)
		{
      return COMMS_STATUS_RF_SWITCH_CMD;
    }
  }
  return ret;
}



/**
 * This functions handles an incoming ECSS packet.
 * If the ECSS packet is part of a large data transfer consisting from
 * several sequential ECSS packets, it handles them automatically.
 *
 * In other words, there is no need to explicitly check for a fragmented data
 * transfer.
 *
 * @param payload the received payload
 * @param payload_size the size of the payload
 * @return SATR_OK if all went ok or appropriate error code
 */
SAT_returnState recv_ecss (uint8_t *payload, const uint16_t payload_size)
{
  SAT_returnState ret;
  tc_tm_pkt *pkt;

  pkt = get_pkt (payload_size);

	//UART2_BUF_O_Write_pkt_To_Buffer(pkt);
  if (!C_ASSERT(pkt != NULL)) 
	{
    return SATR_ERROR;
  }
  if (unpack_pkt (payload, pkt, payload_size) == SATR_OK) 
	{
//		UART2_BUF_O_Write_pkt_To_Buffer(pkt);
    ret = route_pkt (pkt);
  }

  TC_TM_app_id dest = 0;

  if(pkt->type == TC) 
	{
    dest = pkt->app_id;
  }
  else if(pkt->type == TM) 
	{
    dest = pkt->dest_id;
  }

  if(dest == SYSTEM_APP_ID) 
	{
      free_pkt(pkt);
  }
  return ret;
}


//Getters
/**
 * Sends a CW beacons based on the internal COMMS statistics tracking mechanism
 * @return a negative number in case of error
 */
int32_t send_cw_beacon(void)
{
  size_t i = 0;

  /* Check if the satellite is during command and control phase */
  if(is_cmd_ctrl_enabled()) 
	{
    return 0;
  }

  memset(send_buffer, 0, AX25_MAX_FRAME_LEN);
  send_buffer[i++] = 'A';
  send_buffer[i++] = 'L';
	send_buffer[i++] = 'E';
	send_buffer[i++] = 'X';
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


uint8_t is_tx_enabled()
{
  uint32_t val;

  val = comms_read_persistent_word(__COMMS_RF_KEY_FLASH_OFFSET); // 0x0 //<------ @ref FLASH_CHECK
  if(val != __COMMS_RF_OFF_KEY) 
	{
    return 1;
  }
  return 0;
}

/**
 * Returns if the COMMS is during a command and control state.
 *
 * @return 1 if it is during command and control, 0 otherwise
 */
uint8_t is_cmd_ctrl_enabled()
{
	  uint32_t now;
		now = getCurrentTick();
	
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

SAT_returnState send_ecss(tc_tm_pkt *pkt)
{
	int32_t ret = 0;
	uint16_t size = 0;
	SAT_returnState res;

	if(pkt == NULL)
	{
		comms_rf_stats_frame_transmitted(&comms_stats, 0, SATR_ERROR);
		return SATR_ERROR;
	}

	res = pack_pkt(send_buf, pkt, &size);
	if(res != SATR_OK)
	{
		comms_rf_stats_frame_transmitted(&comms_stats, 0, res);
		return ret;
	}

	ret = send_payload(send_buf, (size_t)size, 0);
	if(ret < 1)
	{
		return SATR_ERROR;
	}
	free_pkt (pkt);
	return SATR_OK;
}

SAT_returnState route_pkt(tc_tm_pkt *pkt)
{
  SAT_returnState res;
  TC_TM_app_id id;

  if ( !C_ASSERT(pkt != NULL && pkt->data != NULL)) 
	{
    free_pkt(pkt);
    return SATR_ERROR;
  }
  if (!C_ASSERT(pkt->type == TC || pkt->type == TM)) 
	{
    free_pkt(pkt);
    return SATR_ERROR;
  }
  if (!C_ASSERT(pkt->app_id < LAST_APP_ID && pkt->dest_id < LAST_APP_ID)) 
	{
    free_pkt(pkt);
    return SATR_ERROR;
  }

  if (pkt->type == TC) 
	{
    id = pkt->app_id;
  }
  else if (pkt->type == TM) 
	{
    id = pkt->dest_id;
  }
  else 
	{
    return SATR_ERROR;
  }

  if(firewall(pkt) != SATR_OK) 
	{
    verification_app (pkt);
    free_pkt (pkt);
    return SATR_OK;
  }

  if (id == SYSTEM_APP_ID && pkt->ser_type == TC_HOUSEKEEPING_SERVICE
      && pkt->ser_subtype == TM_HK_PARAMETERS_REPORT
      && pkt->data[0] == WOD_REP) 
	{
    /*
     * A new WOD arrived from the OBC. Store it and extract the information.
     * The transmission of each WOD is handled by the COMMS dispatcher function
     */
    //SYSVIEW_PRINT("WOD from OBC");
		UART2_BUF_O_Write_String_To_Buffer("WOD from OBC");
    store_wod_obc(pkt->data + 1, pkt->len - 1);
  }
  else if (id == SYSTEM_APP_ID && pkt->ser_type == TC_HOUSEKEEPING_SERVICE
      && pkt->ser_subtype == TM_HK_PARAMETERS_REPORT
      && pkt->data[0] == EXT_WOD_REP) 
	{
    /*
     * A new exWOD arrived from the OBC. Store it and extract the information.
     * The transmission of each exWOD is handled by the COMMS dispatcher function
     */
    //SYSVIEW_PRINT("exWOD from OBC");
		UART2_BUF_O_Write_String_To_Buffer("exWOD from OBC");
    store_ex_wod_obc(pkt->data, pkt->len);
  }
  else if (id == SYSTEM_APP_ID && pkt->ser_type == TC_HOUSEKEEPING_SERVICE) 
	{
    res = hk_app (pkt);
  }
  else if (id == SYSTEM_APP_ID && pkt->ser_type == TC_FUNCTION_MANAGEMENT_SERVICE) 
	{
//    res = function_management_app (pkt); // commented in the upsat also
  }
  else if (id == SYSTEM_APP_ID && pkt->ser_type == TC_LARGE_DATA_SERVICE) 
	{
    res = large_data_app (pkt);
    if (res == SATR_OK) 
		{
      free_pkt (pkt);
      return SATR_OK;
    }
  }
  else if (id == SYSTEM_APP_ID && pkt->ser_type == TC_TEST_SERVICE) 
	{
    //C_ASSERT(pkt->ser_subtype == 1 || pkt->ser_subtype == 2 || pkt->ser_subtype == 9 || pkt->ser_subtype == 11 || pkt->ser_subtype == 12 || pkt->ser_subtype == 13) { free_pkt(pkt); return SATR_ERROR; }
    UART2_BUF_O_Write_String_To_Buffer("TC_TEST_SERVICE");
		res = test_app (pkt);
		UART2_BUF_O_Write_Number04_To_Buffer(res);
  }
  else if (id == EPS_APP_ID) 
	{
    UART2_BUF_O_Write_Number04_To_Buffer(queuePush (pkt, OBC_APP_ID));
		UART2_BUF_O_Write_String_To_Buffer("route_packet: queuePush --> OBC_APP_ID\n");
  }
  else if (id == ADCS_APP_ID) 
	{
    queuePush (pkt, OBC_APP_ID);
  }
  else if (id == OBC_APP_ID) 
	{
    queuePush (pkt, OBC_APP_ID);
  }
  else if (id == IAC_APP_ID) 
	{
    queuePush (pkt, OBC_APP_ID);
  }
  else if (id == GND_APP_ID) 
	{
    if (pkt->len > MAX_PKT_DATA) 
		{
      large_data_downlinkTx_api (pkt);
    }
    else 
		{
      send_ecss (pkt); 
    }
  }
  else if (id == DBG_APP_ID) 
	{
    queuePush (pkt, OBC_APP_ID);
  } 
	else 
	{
    free_pkt(pkt);
		UART2_BUF_O_Write_String_To_Buffer("route_packet didn't Processed");
  }

  return SATR_OK;
}


/**
 * Send a payload using AX.25 encoding
 * @param in the payload buffer
 * @param len the length of the payload in bytes
 * @param is_wod set to true if the frame is a WOD
 * @return number of bytes sent or appropriate error code
 */
int32_t send_payload(const uint8_t *in, size_t len, uint8_t is_wod)
{
  int32_t ret;

  if(len > AX25_MAX_FRAME_LEN) 
	{
    comms_rf_stats_frame_transmitted(&comms_stats, 0, COMMS_STATUS_BUFFER_OVERFLOW);
    return COMMS_STATUS_BUFFER_OVERFLOW;
  }

  /* Check if the TX is enabled */ //<-------------------------------------- may be require some edit
  if(!is_tx_enabled())
	{
    /* If the TX is disabled do not count it as error in the RF stats */
    return COMMS_STATUS_RF_OFF;
  }

  memset(send_temp_buf, 0, sizeof(send_temp_buf));
  ret = tx_data(in, len, send_temp_buf, is_wod);
  comms_rf_stats_frame_transmitted(&comms_stats, ret > 1, ret);
  return ret;
}
/**
 * Sends a payload using CW Morse code
 * @param in the payload buffer
 * @param len the length of the payload in bytes
 * @return the number of bytes sent or an appropriate error code
 */
int32_t send_payload_cw(const uint8_t *in, size_t len)
{
  int32_t ret;
	uint8_t *rx;
  if(len > AX25_MAX_FRAME_LEN) 
	{
    return COMMS_STATUS_BUFFER_OVERFLOW;
  }

  /* Check if the TX is enabled */
  if(!is_tx_enabled())
	{
    return COMMS_STATUS_RF_OFF;
  }

	ret = tx_data(in, len,rx,1);
  //ret = tx_data_cw(in, len); //<---------------- not working with this module go for @ref CW_TASK_REF
  return ret;
}

/**
 * Checks if the received frame contains an RF switch command for the COMMS
 * subsystem
 * @param in the payload of the received frame
 * @param len the length of the payload
 * @return 0 in case the received payload was not an RF switch command,
 * 1 if it was.
 */
static inline uint8_t check_rf_switch_cmd(const uint8_t *in, size_t len) 
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
	/*
	strncmp:This function is the similar to strcmp, except that no more than size characters are compared. In other words, if the two 
	strings are the same in their first size characters, the return value is zero. 
	*/
  if(strncmp((const char *)in, __COMMS_RF_SWITCH_CMD, cmd_hdr_len) != 0)
	{
    return 0;
  }

	// if we are here then it's a CMD
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
  if(*sw_key_ptr == __COMMS_RF_ON_KEY) 
	{
    //rf_tx_enable(); here we should enable the transmitter in my module case the tx always on if it's powered on, we can control the power pin or change the stm8 code
    return 1;
  }
  else if(*sw_key_ptr == __COMMS_RF_OFF_KEY) 
	{
    if(len < cmd_hdr_len + sizeof(__COMMS_RF_ON_KEY) + __COMMS_RF_SWITCH_KEY_LEN)
		{
      comms_rf_stats_sw_cmd_failed(&comms_stats);
      return 0;
    }
    /* Compute the SHA-256 of the received key */ 
    sha256_init(&sha_ctx);
    sha256_update(&sha_ctx, in + cmd_hdr_len + sizeof(__COMMS_RF_OFF_KEY), __COMMS_RF_SWITCH_KEY_LEN);
    sha256_final(&sha_ctx, sha_res);

    /* Check if the received key matches the stored hash */
    for(i = 0; i < 32; i++)
		{
      if(sha_res[i] != __COMMS_RF_SWITCH_HASH[i]) 
			{
        return 0;
      }
    }
    //rf_tx_shutdown(); it closes the tx again after finishing
    return 1;
  }
  else
	{
    comms_rf_stats_sw_cmd_failed(&comms_stats);
  }
  return 0;
}