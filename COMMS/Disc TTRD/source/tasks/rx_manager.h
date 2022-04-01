#ifndef INC_RX_MANAGER_H_
#define INC_RX_MANAGER_H_

// Processor Header
#include "../main/main.h"
#include "../support_functions/ax_25.h"
//#include "../support_functions/cc_definitions.h"
//#include "../support_functions/cc_init.h"


#ifdef CC1101_UART
/***************** CC_RX ******************************************************/
//Init
void rx_init();

//Setters
int32_t rx_data_continuous (uint8_t *out, size_t maxlen, size_t timeout_ms);

int32_t rx_data (uint8_t *out, size_t len, size_t timeout_ms);
/***********************************************************************************/
#elif CC1120

#elif CC1101_SPI

#endif

#endif /* INC_RX_MANAGER_H_ */
