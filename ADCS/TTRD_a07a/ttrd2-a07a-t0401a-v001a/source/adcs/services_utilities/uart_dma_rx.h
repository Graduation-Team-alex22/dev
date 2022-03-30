#ifndef UART_DMA_RX_H__
#define UART_DMA_RX_H__

#include "stm32f4xx_usart.h"
#include "stm32f4xx_dma.h"

uint16_t uart_dma_init(void);

void uart_dma_update(void);





#endif
