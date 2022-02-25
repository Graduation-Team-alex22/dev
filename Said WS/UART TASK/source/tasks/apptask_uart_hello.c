#include "../main/main.h"
#include "apptask_uart_hello.h"
#include "../adcs/services_utilities/time.h"

#define SW_PRESSED (0)

static inline void convert(char* p, uint8_t num){
	p[0] = (num/100) + 48;
	p[1] = (num%100 /10) + 48;
	p[2] = (num%10) + 48;
	
}

void uart_hello_Init(void){
	UART2_BUF_O_Write_String_To_Buffer("\n***RTC test INIT***\n");
	UART2_BUF_O_Send_All_Data();
	// now this will be used for testing
	uint8_t error_code = 1;
	error_code = time_init();
	if(error_code) 
		PROCESSOR_Perform_Safe_Shutdown(error_code);
}

uint32_t uart_hello_Update(void){
	/*
	// if the button is not pressed, do nothing
	uint32_t Button1_input = GPIO_ReadInputDataBit(BUTTON1_PORT, BUTTON1_PIN);
	if(Button1_input != SW_PRESSED) return RETURN_NORMAL_STATE;
	
	// write Hello to buffer
	UART2_BUF_O_Write_String_To_Buffer("\n***APPTASK_HELLO_WORLD***\n");
	*/
	time_update();
	time_keeping_adcs_t time = time_getTime();
	
	UART2_BUF_O_Write_String_To_Buffer("\n***RTC test: ");
	char temp[3] = {0};
	convert(temp, time.utc.min);
	UART2_BUF_O_Write_String_To_Buffer(temp);
	UART2_BUF_O_Write_String_To_Buffer(" : ");
	convert(temp, time.utc.sec);
	UART2_BUF_O_Write_String_To_Buffer(temp);
	UART2_BUF_O_Write_String_To_Buffer("***");
	
	// send it away
	return UART2_BUF_O_Update();
}
