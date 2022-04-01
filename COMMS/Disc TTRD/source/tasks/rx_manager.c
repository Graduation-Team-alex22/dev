#include "rx_manager.h"

uint8_t tmp_buf[AX25_MAX_FRAME_LEN + 2];
ax25_handle_t h_ax25dec;


#if CC1101_UART
//Init
void rx_init()
{
  ax25_rx_init(&h_ax25dec);
}

//Setters
int32_t rx_data_continuous (uint8_t *out, size_t maxlen, size_t timeout_ms)
{
  uint32_t start_tick;
  uint8_t timeout = 1;
  int32_t ax25_status;
  size_t decode_len;	
	
	
}












#elif CC1120

	
#elif CC1101_SPI
	
	
#endif




