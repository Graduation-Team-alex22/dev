#ifndef INC_RX_MANAGER_H_
#define INC_RX_MANAGER_H_

// Processor Header
#include "../main/main.h"

//Init
void
rx_init();


//Update


//Getters


//Setters
int32_t
rx_data_continuous (uint8_t *out, size_t maxlen, size_t timeout_ms);

int32_t
rx_data (uint8_t *out, size_t len, size_t timeout_ms);


#endif /* INC_RX_MANAGER_H_ */
