#include "rx_manager.h"
#include "cc_tx.h"


volatile extern uint8_t rx_sync_flag;
static uint8_t recv_buffer[AX25_MAX_FRAME_LEN];
static uint8_t tmp_buf[AX25_MAX_FRAME_LEN + 2];
comms_rf_stat_t comms_stats;


ax25_handle_t h_ax25dec;
//comms_rf_stat_t comms_stats;

#if CC1101_UART
//Init // still need more 
void rx_init(void)
{
  ax25_rx_init(&h_ax25dec);
}


//update 
uint32_t rx_update(void)// DONE!!
{
	uint32_t Return_value = RETURN_NORMAL_STATE;
	
	  /* A frame is received */
  if(rx_sync_flag)
	{
		UART2_BUF_O_Write_String_To_Buffer("in rx update\n");
		int32_t ret = COMMS_STATUS_OK;
		uint16_t length=0;
		rx_sync_flag = 0;
		CC_RX_FLAGS.Frame_Flag=0;
		
		UART2_BUF_O_Write_Number10_To_Buffer(length);
		ret = recv_payload(recv_buffer, length, COMMS_DEFAULT_TIMEOUT_MS); 
		
//		CC_TX_BUF_O_Write_Frame_To_Buffer(recv_buffer,ret);
//		UART2_BUF_O_Write_Frame_To_Buffer(recv_buffer,ret);
//		UART2_BUF_O_Write_Number04_To_Buffer(ret);
		
		if(ret > 0) 
		{
			ret = recv_ecss(recv_buffer, ret);
//			CC_TX_BUF_O_Write_Frame_To_Buffer(recv_buffer,ret);
//			UART2_BUF_O_Write_Number04_To_Buffer(ret);
			if(ret == SATR_OK)
			{
				set_cmd_and_ctrl_period(1);
				comms_rf_stats_frame_received(&comms_stats, FRAME_OK, 0);
			}
			else
			{
				comms_rf_stats_frame_received(&comms_stats, !FRAME_OK, ret);
			}
		}
		else
		{
			comms_rf_stats_frame_received(&comms_stats, !FRAME_OK, ret);
		}
  }
	return Return_value;
}
	

//Setters
/**
 * Resets all the IRQ flags involved in RX operations
 */
static inline void reset_rx_irqs()
{
  rx_sync_flag = 0;
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
int32_t rx_data(uint8_t *out, size_t len, size_t timeout_ms)
{
	//UART2_BUF_O_Write_String_To_Buffer("rx_data\n");
  int32_t ret;
  size_t decode_len;
  ax25_rx_reset(&h_ax25dec);

  memset(tmp_buf, '\0', sizeof(tmp_buf));
  ret = CC_RX_data_packet(tmp_buf, AX25_MAX_FRAME_LEN); 	//this function need to add the frame data from the DMA buffer with the len AX25_MAX_FRAME_LEN to the temp_buf
	if(ret < 1)
	{
    return ret;
  }
//	CC_TX_BUF_O_Write_Frame_To_Buffer(tmp_buf,ret);
	
  /* Frame received. Try to decode it using the AX.25 encapsulation */
  ret = ax25_recv(&h_ax25dec, out, &decode_len, tmp_buf, ret);
//	CC_TX_BUF_O_Write_Frame_To_Buffer(out,decode_len);
//	UART2_BUF_O_Write_Number04_To_Buffer(ret);
	
  if(ret == AX25_DEC_OK)
	{
    return (int32_t) decode_len;
  }
  return ret;
}











#elif CC1120

	
#elif CC1101_SPI
	
	
#endif




