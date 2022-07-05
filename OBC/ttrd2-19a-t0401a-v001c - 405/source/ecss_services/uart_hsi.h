#ifndef __UART_HSI_H
#define __UART_HSI_H

#include <stdint.h>
#include "stm32f4xx.h"
#include "services.h"
#include "cubesat_mod.h"
#include "../tasks/COMMS_comm_task.h"
#include "../tasks/EPS_comm_task.h"
#include "../tasks/ADCS_comm_task.h"


void MYDMA_Init(DMA_Stream_TypeDef* DMA_Streamx , uint32_t chx , uint32_t padd , uint32_t madd , uint16_t ndtr);

void MYDMA_Enable(USART_TypeDef *USARTx,DMA_Stream_TypeDef* DMA_Streamx , uint16_t ndtr);


void UART_PROCESS_DATA(const void* temp, size_t len,struct uart_data *data);
void UART_DMA_rx_check(TC_TM_app_id app_id, struct uart_data *data);


void send_packet(TC_TM_app_id app_id, uint8_t *buf, uint16_t size);

SAT_returnState UART_rx(TC_TM_app_id app_id, struct uart_data *data);

SAT_returnState recieve_packet(TC_TM_app_id app_id, struct uart_data *data);

#endif

