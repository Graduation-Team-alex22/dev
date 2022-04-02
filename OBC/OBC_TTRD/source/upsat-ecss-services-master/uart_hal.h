#ifndef __UART_HAL_H
#define __UART_HAL_H

#include <stdint.h>
#include "stm32f4xx.h"
#include "services.h"
#include "upsat.h"


void MYDMA_Init(DMA_Stream_TypeDef* DMA_Streamx , uint32_t chx , uint32_t padd , uint32_t madd , uint16_t ndtr);

void MYDMA_Enable(DMA_Stream_TypeDef* DMA_Streamx , uint16_t ndtr);

SAT_returnState hal_kill_uart(TC_TM_app_id app_id);

void UART_PROCESS_DATA(const void* temp, size_t len,struct uart_data *data);
void UART_DMA_rx_check(TC_TM_app_id app_id, struct uart_data *data);

SAT_returnState HAL_uart_tx_check(TC_TM_app_id app_id);

void HAL_uart_tx(TC_TM_app_id app_id, uint8_t *buf, uint16_t size);

SAT_returnState HAL_uart_rx(TC_TM_app_id app_id, struct uart_data *data);


#endif