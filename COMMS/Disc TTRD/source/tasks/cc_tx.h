#ifndef INC_CC_TX_H_
#define INC_CC_TX_H_

// Processor Header
#include "../main/main.h"
#include "ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"


#ifdef CC1101_UART
/***************** CC_TX ******************************************************/
#define CC_TX_RX_BUFFER_SIZE_BYTES 200
#define CC_TX_TX_BUFFER_SIZE_BYTES 500

extern uint8_t CC_TX_SP_Flag;
extern char CC_TX_SP_Command[10];

//Init
void CC_TX_init(uint32_t BAUD_RATE);
void CC_TX_BUF_O_Init(uint32_t BAUD_RATE); 

//Update
uint32_t CC_TX_update(void);

void CC_TX_BUF_O_Update(void);
//Getters
void CC_TX_DMA_CHECK(void); 


//Setters

void 		 CC_TX_Clear_Command(void);
void  	 CC_TX_SET_Parameters(void);
void     CC_TX_PROCESS_DATA(const void* data, size_t len); 
int32_t  CC_TX_data_packet(const uint8_t *data, size_t size);
//int32_t  CC_TX_data_packet_cw(const cw_pulse_t *in, size_t len); //<---------------- not working with this module go for @ref CW_TASK_REF

void     CC_TX_BUF_O_Send_All_Data(void); 

void 		 CC_TX_BUF_O_Write_Frame_To_Buffer(const void* data, size_t len);
void     CC_TX_BUF_O_Write_String_To_Buffer(const char* const); 
void     CC_TX_BUF_O_Write_Char_To_Buffer(const char); 

/***********************************************************************************/

#elif CC1120


#elif CC1101_SPI


#endif


#endif /* INC_CC_TX_H_ */

