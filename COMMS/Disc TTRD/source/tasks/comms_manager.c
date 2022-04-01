// #include "../main/main.h"
#include "comms_manager.h"

comms_cmd_ctrl_t cmd_and_ctrl;




//Init 
void comms_init()
{
	#ifdef CC1120
	uint8_t cc_id_tx;
  uint8_t cc_id_rx;
	  
	/* fetch TX ID */
  cc_tx_rd_reg (0x2F8F, &cc_id_tx);
	
  /* fetch RX ID */
//cc_rx_rd_reg (0x2F8F, &cc_id_rx);
	
	#elif CC1101_UART
	
	
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



//Getters
uint8_t
is_tx_enabled()
{
  uint32_t val;

  val = comms_read_persistent_word(__COMMS_RF_KEY_FLASH_OFFSET); // 0x0
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

/**
 * Send a payload using AX.25 encoding
 * @param in the payload buffer
 * @param len the length of the payload in bytes
 * @param is_wod set to true if the frame is a WOD
 * @param timeout_ms the timeout limit in milliseconds
 * @return number of bytes sent or appropriate error code
 */
//int32_t send_payload(const uint8_t *in, size_t len, uint8_t is_wod, size_t timeout_ms)
//{
//  int32_t ret;

//  if(len > AX25_MAX_FRAME_LEN) {
//    comms_rf_stats_frame_transmitted(&comms_stats, 0,
//				     COMMS_STATUS_BUFFER_OVERFLOW);
//    return COMMS_STATUS_BUFFER_OVERFLOW;
//  }

//  /* Check if the TX is enabled */
//  if(!is_tx_enabled()){
//    /* If the TX is disabled do not count it as error in the RF stats */
//    return COMMS_STATUS_RF_OFF;
//  }

//  memset(spi_buf, 0, sizeof(spi_buf));
//  ret = tx_data(in, len, spi_buf, is_wod, timeout_ms);
//  comms_rf_stats_frame_transmitted(&comms_stats, ret > 1, ret);
//  return ret;
//}
