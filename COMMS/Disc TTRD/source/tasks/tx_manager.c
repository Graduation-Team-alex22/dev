#include "tx_manager.h"

int32_t ax25_send (uint8_t *out, const uint8_t *in, size_t len, uint8_t is_wod);
static uint8_t tmp_buf[AX25_PREAMBLE_LEN + AX25_POSTAMBLE_LEN
		       + AX25_MAX_FRAME_LEN + AX25_MAX_ADDR_LEN];

int32_t tx_data(const uint8_t *in, size_t len, uint8_t *dev_rx_buffer, uint8_t is_wod,
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
  // ret = cc_tx_data_continuous(tmp_buf, ret, dev_rx_buffer, timeout_ms); // NEEDS REDESIGN
  if(ret < 1){
    return ret;
  }
  return len;	
}
