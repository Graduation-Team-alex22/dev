#include "tx_manager.h"
#include "../support_functions/cw.h"
static comms_tx_job_list_t tx_jobs;// here i have changed the struct of the list //<---------------------------
struct _comms_data comms_data;

static uint8_t tmp_buf[AX25_PREAMBLE_LEN + AX25_POSTAMBLE_LEN + AX25_MAX_FRAME_LEN + AX25_MAX_ADDR_LEN];
static cw_pulse_t cw_buf[AX25_MAX_FRAME_LEN * 10];
uint32_t tx_job_desc = __COMMS_DEFAULT_HEADLESS_TX_PATTERN;
uint32_t tx_job_cnt = 0;
 
 
//Init
void tx_init(void)
{
	memset((void *)&tx_jobs, 0, sizeof(comms_tx_job_list_t));
}

//update 
uint32_t tx_type_update(void)
{
	static uint32_t tick;
	uint32_t now;
	
	now = getCurrentTick();
	if(now < tick || now - tick > __TX_INTERVAL_MS)//__TX_INTERVAL_MS
		{
			
			tick = now;
			/*
       * Check which type of TX job should be performed. At this time any
       * previous unfinished TX jobs are dropped.
       */
			tx_init();
			/* Zero ID always refer to CW beacon */
			if(tx_job_cnt == 0)
      {
				UART2_BUF_O_Write_String_To_Buffer("TX TYPE UPDATE: CW\n");
				/* Should be reset from comms_routine_dispatcher() when served */
    	  tx_jobs.tx_cw = 1;
      }
			else
      {
				/*An active bit means ex_WOD whereas an inactive a WOD */
				if((tx_job_desc >> tx_job_cnt) & 0x1)
    	  {
					UART2_BUF_O_Write_String_To_Buffer("TX TYPE UPDATE: EXT WOD\n");
    		  tx_jobs.tx_ext_wod = 1;
    	  }
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("TX TYPE UPDATE: WOD\n");
    		 tx_jobs.tx_wod = 1;
				}
      }
			tx_job_cnt++;
      tx_job_cnt = tx_job_cnt % 8;
			
			/*
       * Now it's a good time to re-read the flash, in case the
       * headless transmission scheme altered
       */
			tx_job_desc = comms_read_persistent_word(__COMMS_HEADLESS_TX_FLASH_OFFSET); //<------
			/*
       * In case the ex_WOD number is greater than the WOD, shit happened.
       * So fall back to the default
       */
			if(bit_count(tx_job_desc) > 4)
			{
    	  tx_job_desc = __COMMS_DEFAULT_HEADLESS_TX_PATTERN;
      }
		}
	return RETURN_NORMAL_STATE;;
}
uint32_t tx_update(void)
{
	static uint32_t tick;
	uint32_t now;
	int32_t ret = COMMS_STATUS_OK;
	now = getCurrentTick();
	
	if(now < tick || now - tick > 1000)//__TX_INTERVAL_MS
	{
		tick = now;
		if(tx_jobs.tx_cw)
		{
			tx_jobs.tx_wod = 0;
			tx_jobs.tx_cw = 0;
			UART2_BUF_O_Write_String_To_Buffer("TX UPDATE: CW\n");
			ret = send_cw_beacon(); // <------------ still using the normal wod
		}
		else if(tx_jobs.tx_wod)
		{
			UART2_BUF_O_Write_String_To_Buffer("TX UPDATE: WOD\n");
			tx_jobs.tx_wod = 0;
			ret = comms_wod_send(0);
		}
		else if(tx_jobs.tx_ext_wod)
		{
			UART2_BUF_O_Write_String_To_Buffer("TX UPDATE: EXT WOD\n");
			tx_jobs.tx_ext_wod = 0;
			ret = comms_ex_wod_send();
		}
	}
	return RETURN_NORMAL_STATE;
}

/**
 * This routine sends the data using the AX.25 encapsulation
 * @param in the input buffer containing the raw data
 * @param len the length of the input buffer
 * @param dev_rx_buffer a buffer that will hold the SPI resulting bytes
 * @param is_wod set to true if the frame is WOD
 * @param timeout_ms the timeout in milliseconds
 * @return the number of bytes sent or appropriate error code
 */
//Getters
int32_t tx_data(const uint8_t *in, size_t len, uint8_t *dev_rx_buffer, uint8_t is_wod, size_t timeout_ms)
{
  int32_t ret = 0;
  /* This routine can not handle large payloads */
  if(len == 0) 
	{
    return COMMS_STATUS_NO_DATA;
  }
  if(len > AX25_MAX_FRAME_LEN) 
	{
    return COMMS_STATUS_BUFFER_OVERFLOW;
  }

  /* Prepare the AX.25 frame */
  ret = ax25_send(tmp_buf, in, len, is_wod);
  if(ret < 1)
	{
    return COMMS_STATUS_NO_DATA;
  }

  /* Issue the frame at the CC1120 */
  ret = CC_TX_data_packet(tmp_buf, ret);
  if(ret < 1)
	{
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
int32_t tx_data_cw (const uint8_t *in, size_t len)
{
  int32_t ret = 0;
  size_t symbols_num;
  /* This routine can not handle large payloads */
  if (len == 0) 
	{
    return COMMS_STATUS_NO_DATA;
  }

  if (len > AX25_MAX_FRAME_LEN) 
	{
    return COMMS_STATUS_BUFFER_OVERFLOW;
  }

  ret = cw_encode(cw_buf, &symbols_num, in, len);
  if(ret != CW_OK)
	{
    return COMMS_STATUS_INVALID_FRAME;
  }

//	ret = CC_TX_data_packet_cw(cw_buf, symbols_num); //<----------------------------- redesign needed it needs to transmit for a period of time and stop for another
	
 // ret = cc_tx_cw(cw_buf, symbols_num);
  if(ret != CW_OK) {
    return COMMS_STATUS_INVALID_FRAME;
  }
  return len;
}
