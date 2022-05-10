#include "wod_handling.h"

#undef __FILE_ID__
#define __FILE_ID__ 38

static comms_wod_t last_wod;
static comms_ex_wod_t last_ex_wod;
extern comms_rf_stat_t comms_stats;

//Private functions
/**
 * This functions takes a WOD frame from the OBC and transforms it properly
 * in order to transmit it at the Ground Station. At the same time it updates
 * the statistics of the COMMS by tracking the WOD values of the most
 * recent observations.
 *
 * @param wod the COMMS internal WOD structure
 * @param obc_wod the buffer with the WOD frame from the OBC
 * @param len the size of the \p obc_wod buffer
 * @return COMMS_STATUS_OK on success or -1 on error.
 */
static int32_t prepare_wod(comms_wod_t *wod, const uint8_t *obc_wod, size_t len)
{
  size_t i = 0;
  size_t j = 0;
  size_t idx = 0;
  size_t bits_cnt = 0;
  size_t bytes_cnt = 0;
  uint8_t valid;
  uint8_t out_b = 0x0;
  const size_t datasets_num = (len - sizeof(uint32_t)) / WOD_DATASET_SIZE;

  if (wod == NULL || obc_wod == NULL || datasets_num == 0|| datasets_num > WOD_MAX_DATASETS) 
	{
    return -1;
  }

  /* Update the COMMS stats based on the most recent WOD data */
  comms_rf_stats_wod_received(&comms_stats, obc_wod);

  /* The first 4 bytes are the timestamp */
  memcpy(wod->wod, obc_wod, sizeof(uint32_t));

  idx += sizeof(uint32_t);
  bytes_cnt += sizeof(uint32_t);

  /* Iterate through the data sets and check the validity of the measurements */
  for(i = 0; i < datasets_num; i++)
	{
    valid = bat_voltage_valid(obc_wod[idx]);
    valid &= bat_current_valid(obc_wod[idx + 1]);
    valid &= bus_3300mV_current_valid(obc_wod[idx + 2]);
    valid &= bus_5000mV_current_valid(obc_wod[idx + 3]);
    valid &= comms_temp_valid(obc_wod[idx + 4]);
    valid &= eps_temp_valid(obc_wod[idx + 5]);
    valid &= bat_temp_valid(obc_wod[idx + 6]);

    out_b <<= 1;
    out_b |= valid & 0x1;
    bits_cnt++;
    if(bits_cnt == 8 )
		{
      bits_cnt = 0;
      wod->wod[bytes_cnt++] = out_b;
    }

    for(j = 0; j < WOD_DATASET_SIZE * 8; j++)
		{
      out_b <<= 1;
      out_b |= ((obc_wod[idx + j/8] >> (7 - (j % 8))) & 0x1);
      bits_cnt++;
      if (bits_cnt == 8) 
			{
				bits_cnt = 0;
				wod->wod[bytes_cnt++] = out_b;
      }
    }
    idx += WOD_DATASET_SIZE;
  }

  /* Zero padding may be necessary */
  if(bits_cnt)
	{
    out_b <<= (8 - bits_cnt);
    wod->wod[bytes_cnt++] = out_b;
  }
  wod->len = bytes_cnt;
  return COMMS_STATUS_OK;
}
//Init
/**
 * Initializes the COMMS WOD structure to the initial defaults
 */
void comms_wod_init(void)
{
	memset((void *)&last_wod, 0, sizeof(comms_wod_t));
}
void comms_ex_wod_init(void)
{
  memset((void *)&last_ex_wod, 0, sizeof(comms_ex_wod_t));
}

//Getters
/**
 * Sends the last stored valid WOD. If now WOD has been
 * received, the transmitted WOD has all its fields zeroed.
 *
 * @param bypass_check if set to true, the check of command and control
 * period is bypassed.
 * @return number of bytes sent or negative error code
 */
int32_t comms_wod_send(uint8_t bypass_check) 
{
//	UART2_BUF_O_Write_String_To_Buffer("Test1");
  int32_t ret = 0;

  /* Check if the satellite is during command and control phase */
  if(is_cmd_ctrl_enabled() && !bypass_check) 
	{
//		UART2_BUF_O_Write_String_To_Buffer("Test2");
    return 0;
  }

  /* If the last OBC WOD was valid, send it but for a finite number of times */
  if(last_wod.valid && last_wod.tx_cnt < __WOD_VALID_REPEATS) 
	{
//		UART2_BUF_O_Write_String_To_Buffer("Test3");
//		UART2_BUF_O_Write_StringByLength_To_Buffer(last_wod.wod, last_wod.len);
    ret = send_payload(last_wod.wod, last_wod.len, 1, COMMS_DEFAULT_TIMEOUT_MS);
    if(ret > 0)
		{
      last_wod.tx_cnt++;
    }
  }
  else
	{
	//	UART2_BUF_O_Write_String_To_Buffer("Test4");
    /*
     * Send a WOD with all zeros to the Ground. This will indicate the
     * communication problem with the OBC
     */
		//UART2_BUF_O_Write_StringByLength_To_Buffer(last_wod.wod, last_wod.len);
    memset(last_wod.wod, 0, WOD_SIZE);
    ret = send_payload(last_wod.wod, WOD_SIZE, 1, COMMS_DEFAULT_TIMEOUT_MS);
  }
	//UART2_BUF_O_Write_String_To_Buffer("Test5");
  return ret;
}
/**
 * Sends the last stored valid exWOD. If now exWOD has been
 * received from OBC, the transmitted exWOD has only the information
 * originated from the COMMS subsystem.
 *
 * @return number of bytes sent or negative error code
 */
int32_t comms_ex_wod_send(void) 
{
  int32_t ret;
  tc_tm_pkt *temp_pkt = 0;
  HK_struct_id sid;

  /* Check if the satellite is during command and control phase */
  if(is_cmd_ctrl_enabled()) 
	{
    return 0;
  }

  if(last_ex_wod.valid && last_ex_wod.tx_cnt < __WOD_VALID_REPEATS)
	{
    temp_pkt = get_pkt(PKT_NORMAL);
    if(!C_ASSERT(temp_pkt != NULL)) 
		{
      return COMMS_STATUS_NO_DATA;
    }

    crt_pkt(temp_pkt, (TC_TM_app_id)SYSTEM_APP_ID, TM, TC_ACK_NO, TC_HOUSEKEEPING_SERVICE, TM_HK_PARAMETERS_REPORT, GND_APP_ID);
    memcpy(temp_pkt->data, last_ex_wod.ex_wod, last_ex_wod.len);
    /* Set the SID and the size explicitly */
    temp_pkt->data[0] = EXT_WOD_REP;
    temp_pkt->len = last_ex_wod.len;

    ret = send_ecss (temp_pkt);
    if(ret > 0) 
		{
      last_ex_wod.tx_cnt++;
    }
    free_pkt(temp_pkt);
    if(ret != SATR_OK)
		{
      ret = COMMS_STATUS_INVALID_FRAME;
    }
  }
  else
	{
    sid = EXT_WOD_REP;
    hk_crt_empty_pkt_TM(&temp_pkt, GND_APP_ID, sid);

    /* TX the exWOD. We deallocate the packet here explicitly */
    ret = send_ecss (temp_pkt);
    free_pkt(temp_pkt);
    if(ret != SATR_OK)
		{
      ret = COMMS_STATUS_INVALID_FRAME;
    }
  }
  return ret;
}
/**
 * Converts a floating point temperature into a WOD compatible 1 byte value
 * @param val the temperature
 * @return 1 byte with the WOD compatible temperature
 */
uint8_t wod_convert_temperature(float val)
{
  /* clamp between min and max value */
  val = maxf(val, -15.0);
  val = minf(val, 48.75);
  return (uint8_t)((val + 15.0) / 0.25);
}

uint8_t bat_voltage_valid(uint8_t val)
{
  if(val > 129 && val < 197)
	{
    return 1;
  }
  return 0;
}

uint8_t bat_current_valid(uint8_t val)
{
  if(val > 64)
	{
    return 1;
  }
  return 0;
}

uint8_t bus_3300mV_current_valid(uint8_t val)
{
  if(val < 115)
	{
    return 1;
  }
  return 0;
}

uint8_t bus_5000mV_current_valid(uint8_t val) // this function returns a call to the getter bus_3300mV_current_valid
{
	return bus_3300mV_current_valid(val);
}

uint8_t comms_temp_valid(uint8_t val)
{
  if(val > 79)
	{
    return 1;
  }
  return 0;
}

uint8_t eps_temp_valid(uint8_t val) // this function returns a call to the getter comms_temp_valid
{
	return comms_temp_valid(val);
}

uint8_t bat_temp_valid(uint8_t val) // this function returns a call to the getter comms_temp_valid
{
	return comms_temp_valid(val);
}

//Setters
void store_wod_obc(const uint8_t *obc_wod, size_t len) // obc_wod is the buffer with the WOD, as it comes from the OBC
{
  int32_t ret;
  ret = prepare_wod(&last_wod, obc_wod, len);
  if(ret == COMMS_STATUS_OK)
	{
    last_wod.valid = 1;
    last_wod.tx_cnt = 0;
    //SYSVIEW_PRINT("WOD from OBC stored");
		UART2_BUF_O_Write_String_To_Buffer("WOD from OBC stored\n");
  }
}

void store_ex_wod_obc(const uint8_t *obc_wod, size_t len)
{
  if(len > AX25_MAX_FRAME_LEN)
	{
    return;
  }
  memcpy(last_ex_wod.ex_wod, obc_wod, len);
  last_ex_wod.len = len;
  last_ex_wod.tx_cnt = 0;
  last_ex_wod.valid = 1;
}



