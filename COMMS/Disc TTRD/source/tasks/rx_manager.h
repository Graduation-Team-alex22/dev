#ifndef INC_RX_MANAGER_H_
#define INC_RX_MANAGER_H_

// Processor Header
#include "../main/main.h"
#include "../support_functions/ax_25.h"
#include "../tasks/cc_rx.h"
#include "ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#include "../support_functions/stats.h"
#include "ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"

#ifdef CC1101_UART
/***************** CC_RX ******************************************************/

//Init
void rx_init(void);

//update 
uint32_t rx_update(void);

//Getters

int32_t rx_data (uint8_t *out, size_t len);

/***********************************************************************************/
#elif CC1120

#elif CC1101_SPI

#endif

#endif /* INC_RX_MANAGER_H_ */
