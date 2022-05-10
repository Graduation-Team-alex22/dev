#ifndef INC_CC_RX_H_
#define INC_CC_RX_H_

// Processor Header
#include "../main/main.h"
#include "ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"

struct CC_RX_FLAGS_t
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
extern struct CC_RX_FLAGS_t CC_RX_FLAGS;

//Init
void CC_RX_init(uint32_t BAUD_RATE);


//Update needs to be updated every 214 ms
uint32_t CC_RX_update(void);

uint32_t CC_RX_BUF_O_Update(void);
//Getters


//Setters
#ifdef CC1101_UART
/***************** CC_RX ******************************************************/
#define CC_RX_TX_BUFFER_SIZE_BYTES 20
#define CC_RX_RX_BUFFER_SIZE_BYTES 500

extern char CC_RX_SP_Command[10];

int32_t  CC_RX_data_packet(uint8_t *out, size_t max_len);

void 	   CC_RX_Clear_Command(void);
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
void CC_RX_PROCESS_DATA(void* data, size_t len); //implemented

/***********************************************************************************/

#elif CC1120


#elif CC1101_SPI


#endif

#endif /* INC_CC_RX_H_ */
