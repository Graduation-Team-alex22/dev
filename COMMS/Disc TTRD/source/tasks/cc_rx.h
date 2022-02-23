#ifndef INC_TX_MANAGER_H_
#define INC_TX_MANAGER_H_

// Processor Header
#include "../main/main.h"

int32_t
tx_data(const uint8_t *in, size_t len, uint8_t *dev_rx_buffer, uint8_t is_wod,
	size_t timeout_ms);

int32_t
tx_data_cw (const uint8_t *in, size_t len);



















#endif /* INC_TX_MANAGER_H_ */
