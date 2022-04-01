#ifndef INC_RX_MANAGER_H_
#define INC_RX_MANAGER_H_

// Processor Header
#include "../main/main.h"
#include "ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"

//Init
void CC_RX_init(uint32_t BAUD_RATE);


//Update
uint32_t CC_RX_update(void);

void CC_RX_BUF_O_Update(void);
//Getters


//Setters
#ifdef CC1101_UART
/***************** CC_RX ******************************************************/
#define CC_RX_TX_BUFFER_SIZE_BYTES 20
#define CC_RX_RX_BUFFER_SIZE_BYTES 4000

extern uint8_t CC_RX_SP_Flag;
extern char CC_RX_SP_Command[10];
void CC_RX_Clear_Command(void);
void     CC_RX_BUF_O_Init(uint32_t BAUD_RATE); //implemented
void  	 CC_RX_SET_Parameters(void);
uint32_t CC_RX_Check_toTransmit(void); //implemented

void     CC_RX_BUF_O_Send_All_Data(void); //implemented

void     CC_RX_BUF_O_Write_String_To_Buffer(const char* const); //implemented
void     CC_RX_BUF_O_Write_Char_To_Buffer(const char); //implemented

//void     CC_RX_BUF_O_Write_Number10_To_Buffer(const uint32_t DATA);
//void     CC_RX_BUF_O_Write_Number04_To_Buffer(const uint32_t DATA);
//void     CC_RX_BUF_O_Write_Number03_To_Buffer(const uint32_t DATA);
//void     CC_RX_BUF_O_Write_Number02_To_Buffer(const uint32_t DATA);

void CC_RX_DMA_CHECK(void); //implemented
void CC_RX_PROCESS_DATA(const void* data, size_t len); //implemented

/***********************************************************************************/

#elif CC1120


#elif CC1101_SPI


#endif

#endif /* INC_RX_MANAGER_H_ */
