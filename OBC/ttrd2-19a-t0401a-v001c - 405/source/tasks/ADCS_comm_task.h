#ifndef _ADCS_COMM_H
#define _ADCS_COMM_H 1

// Processor Header
#include "../main/main.h"
#include "ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"


struct ADCS_U6_FLAGS_t
{
	uint8_t SetParam_Flag;// Set Parameters
	uint8_t Sent_Flag;
	
	uint8_t Start1_Flag;
	uint8_t End1_Flag;
  uint8_t* StartPtr1;
	uint8_t* EndPtr1;
	uint16_t Length1;
	
	uint8_t Start2_Flag;
	uint8_t End2_Flag;
	uint8_t* StartPtr2;
	uint8_t* EndPtr2;
	uint16_t Length2;
	
	uint8_t Start3_Flag;
	uint8_t End3_Flag;
	uint8_t* StartPtr3;
	uint8_t* EndPtr3;
	uint16_t Length3;
	
	uint8_t ID_Flag;
	uint8_t Error_Flag;
	uint8_t Frame_Flag;

};
extern struct ADCS_U6_FLAGS_t ADCS_U6_FLAGS;


//Init
void ADCS_U6_init(uint32_t BAUD_RATE);

//Update
uint32_t ADCS_U6_update(void);

uint32_t ADCS_U6_BUF_O_Update(void);
	
//Getters


//Setters
/***************** ADCS_U6 ******************************************************/
#define ADCS_U6_RX_BUFFER_SIZE_BYTES 500
#define ADCS_U6_TX_BUFFER_SIZE_BYTES 500

extern uint8_t ADCS_U6_SP_Flag;
extern char ADCS_U6_SP_Command[10];

void     ADCS_U6_BUF_O_Init(uint32_t BAUD_RATE); 
void 		 ADCS_U6_DMA_CHECK(struct uart_data *uart_data);
void 		 ADCS_U6_PROCESS_DATA(void* data, size_t len);
SAT_returnState ADCS_U6_UART_recv(struct uart_data *uart_data);
void ADCS_U6_UART_send( uint8_t *buf, uint16_t size);


void     ADCS_U6_BUF_O_Send_All_Data(void); 

void 		 ADCS_U6_BUF_O_Write_Frame_To_Buffer(const void* data, size_t len);
void     ADCS_U6_BUF_O_Write_String_To_Buffer(const char* const); 
void     ADCS_U6_BUF_O_Write_Char_To_Buffer(const char); 


#endif

