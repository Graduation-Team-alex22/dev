#include "../main/main.h"
#include "apptask_uart_hello.h"
#include "../adcs/services_utilities/uart_dma_rx.h"

#define SW_PRESSED (0)

//#define 	TEST_TIME
#define			TEST_UART_DMA

void uart_hello_Init(void){
#ifdef TEST_TIME
	UART2_BUF_O_Write_String_To_Buffer("\n***RTC test INIT***\n");
	UART2_BUF_O_Send_All_Data();
	// now this will be used for testing
	uint8_t error_code = 1;
	error_code = time_init();
	if(error_code) 
		PROCESSOR_Perform_Safe_Shutdown(error_code);
#endif
	
#ifdef TEST_UART_DMA
	UART2_BUF_O_Write_String_To_Buffer("\n***UART_DMA test INIT***\n");
	uint16_t error_code = uart_dma_init();
	if(error_code) PROCESSOR_Perform_Safe_Shutdown(error_code);
#endif
	
}

uint32_t uart_hello_Update(void){
#ifdef TEST_TIME
	time_update();
	time_keeping_adcs_t time = time_getTime();
	
	UART2_BUF_O_Write_String_To_Buffer("\nRTC test: ");
	char temp[PRINT_TIME_STR_LENGTH] = {'X'};
	print_time(temp);
	UART2_BUF_O_Write_String_To_Buffer(temp);
#endif
	
#ifdef TEST_UART_DMA
	uart_dma_update();
#endif

	// send it away
	return UART2_BUF_O_Update();
}
