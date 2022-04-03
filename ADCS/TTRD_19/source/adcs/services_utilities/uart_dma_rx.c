#include "uart_dma_rx.h"

#define 	BUFFER_LEN	20

char rx_buffer[BUFFER_LEN];
char tx_buffer[BUFFER_LEN] = "MOTHER RUSSIA WINS";


uint16_t uart_dma_init(void){
	
	// UART4 clock enable 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	// init UART4
	USART_InitTypeDef uart_init_struct;
	uart_init_struct.USART_BaudRate = 115200;
	uart_init_struct.USART_WordLength = USART_WordLength_8b;
  uart_init_struct.USART_StopBits = USART_StopBits_1;
  uart_init_struct.USART_Parity = USART_Parity_No;
  uart_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  uart_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  // uart_init_struct.USART_OverSampling = UART_OVERSAMPLING_16;
	USART_Init( UART4, &uart_init_struct);
	// Enable UART4
  USART_Cmd(UART4, ENABLE);
	
	// enable GPIO pins for UART4
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);
	
	// init DMA1 - UART4_TX
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_InitTypeDef dma_init;
	DMA_StructInit(&dma_init);
	dma_init.DMA_Channel = DMA_Channel_4;			// UART4 channel according to Reference Manual.
	dma_init.DMA_PeripheralBaseAddr = UART4_BASE;
	//dma_init.DMA_Memory0BaseAddr = tx_buffer;
	dma_init.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma_init.DMA_BufferSize = BUFFER_LEN;
	dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_init.DMA_Mode = DMA_Mode_Circular;
	dma_init.DMA_Priority = DMA_Priority_Low;
	dma_init.
	dma_init.
	dma_init.
	
	
	
	return 0;
	
}


void uart_dma_update(void){
	// send over uart4
	// , we are mohamed and abdo in alexandria now wasting\nour youth best years in engineeing none-sense
	char s[] = "Hello\n";
	
	USART_SendData(UART4, s[0]);
	while((USART_GetFlagStatus(UART4, USART_FLAG_TXE) == 0));
	USART_SendData(UART4, s[1]);
	while((USART_GetFlagStatus(UART4, USART_FLAG_TXE) == 0));
	USART_SendData(UART4, s[2]);
	while((USART_GetFlagStatus(UART4, USART_FLAG_TXE) == 0));
	USART_SendData(UART4, s[3]);
	while((USART_GetFlagStatus(UART4, USART_FLAG_TXE) == 0));
	USART_SendData(UART4, s[4]);
	while((USART_GetFlagStatus(UART4, USART_FLAG_TXE) == 0));
	USART_SendData(UART4, s[5]);
	while((USART_GetFlagStatus(UART4, USART_FLAG_TXE) == 0));
	
	
	
}

