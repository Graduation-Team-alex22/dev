#ifndef INC_OBC_COMM_H_
#define INC_OBC_COMM_H_

// Processor Header
#include "../main/main.h"
#include "ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"


struct OBC_C_FLAGS_t
{
	uint8_t SetParam_Flag;
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
	
	uint8_t ID_Flag;
	uint8_t Error_Flag;

};

extern struct OBC_C_FLAGS_t OBC_C_FLAGS;
struct _uart_data;
/***************** OBC_C ******************************************************/
#define OBC_C_RX_BUFFER_SIZE_BYTES 500
#define OBC_C_TX_BUFFER_SIZE_BYTES 500

extern uint8_t OBC_C_SP_Flag;
extern char OBC_C_SP_Command[10];

//Init
void OBC_C_init(uint32_t BAUD_RATE);
void OBC_C_BUF_O_Init(uint32_t BAUD_RATE); 

//Update
uint32_t OBC_C_update(void);

uint32_t OBC_C_BUF_O_Update(void);
	

//Setters

void 		 OBC_C_PROCESS_DATA(void* data, size_t len);
int32_t  OBC_C_data_packet(const uint8_t *data, size_t size, uint8_t *rec_data);
void OBC_C_UART_send( uint8_t *buf, uint16_t size);


void     OBC_C_BUF_O_Send_All_Data(void); 

void 		 OBC_C_BUF_O_Write_Frame_To_Buffer(const void* data, size_t len);
void     OBC_C_BUF_O_Write_String_To_Buffer(const char* const); 
void     OBC_C_BUF_O_Write_Char_To_Buffer(const char); 

//Getters

void OBC_C_DMA_CHECK(struct _uart_data *uart_data);
SAT_returnState OBC_C_UART_recv(struct _uart_data *uart_data);


/***********************************************************************************/









#endif /* INC_OBC_COMM_H_ */

