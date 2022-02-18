#include "../main/main.h"
#include "apptask_uart_hello.h"

#define SW_PRESSED (0)

void     uart_hello_Init(void){
	// just nothing 
}

uint32_t uart_hello_Update(void){
	// if the button is not pressed, do nothing
	uint32_t Button1_input = GPIO_ReadInputDataBit(BUTTON1_PORT, BUTTON1_PIN);
	if(Button1_input != SW_PRESSED) return RETURN_NORMAL_STATE;
	
	// write Hello to buffer
	UART2_BUF_O_Write_String_To_Buffer("\n***APPTASK_HELLO_WORLD***\n");
	
	// send it away
	return UART2_BUF_O_Update();
}
