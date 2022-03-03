#include "uart_dma_rx.h"


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
	
	// init DMA1
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_InitTypeDef dma_init;
	DMA_StructInit(&dma_init);
	
	
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

