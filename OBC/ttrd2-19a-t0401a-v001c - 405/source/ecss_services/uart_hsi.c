#include "uart_hsi.h"

#include "obc.h"
#include "hldlc.h"
#include "../tasks/UART_task.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"

extern uint8_t SUBSYS1[UART_DMA_BUF_SIZE];
uint32_t size = 0;
uint8_t packet_flag = 0;
uint16_t err;
static uint32_t old_pos;
static	uint32_t pos;
#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))
#define True 1
#define False 0	
uint8_t flag = False;


#undef __FILE_ID__
#define __FILE_ID__ 17




//DMA initialization
void MYDMA_Init(DMA_Stream_TypeDef* DMA_Streamx, uint32_t chx, uint32_t padd, uint32_t madd, uint16_t ndtr)
{

	DMA_InitTypeDef DMA_InitStructure;



	// Judge whether it is DMA1 perhaps DMA2
	if ((u32)DMA_Streamx > (u32)DMA2)
	{

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	}
	else
	{

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	}

	// Uninitialize
	DMA_DeInit(DMA_Streamx);

	// wait for DMA You can configure the
	while(DMA_GetCmdStatus(DMA_Streamx) != DISABLE)
	{

	}


	// To configure DMA Stream


	// Channel selection
	DMA_InitStructure.DMA_Channel = chx;
	//DMA Peripheral address
	DMA_InitStructure.DMA_PeripheralBaseAddr = padd;
	//DMA Memory 0 Address
	DMA_InitStructure.DMA_Memory0BaseAddr = madd;
	// Memory to peripheral mode
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	// Data transmission volume
	DMA_InitStructure.DMA_BufferSize = ndtr;
	// Peripheral non incremental mode
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// Memory incremental mode
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	// Peripheral data length 8 position
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	// Memory data length 8 position
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	// Use normal mode
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	// medium priority
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	// close FIFO Pattern
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	// choice FIFO threshold   Has not started FIFO  Whatever you choose
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	// Memory burst single transmission
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	// Peripheral burst single transmission
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	// initialization
	DMA_Init(DMA_Streamx, &DMA_InitStructure);
}

//DMA Can make
void MYDMA_Enable(USART_TypeDef *USARTx,DMA_Stream_TypeDef* DMA_Streamx, uint16_t ndtr)
{

	// close DMA transmission
	DMA_Cmd(DMA_Streamx, DISABLE);

	// Guarantee DMA Can be set
	while(DMA_GetCmdStatus(DMA_Streamx) != DISABLE)
	{

	}

	// Set the amount of data transfer
	DMA_SetCurrDataCounter(DMA_Streamx, ndtr);
	
	USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);

	// Turn on DMA transmission
	DMA_Cmd(DMA_Streamx, ENABLE);
}


void send_packet(TC_TM_app_id app_id, uint8_t *buf, uint16_t size)
{
   UART2_BUF_O_Write_String_To_Buffer("--> Size: ");
	 UART2_BUF_O_Write_Number04_To_Buffer(size);
	 UART2_BUF_O_Write_String_To_Buffer("\n");
	
	if(app_id == EPS_APP_ID) {
		MYDMA_Init(DMA2_Stream7, DMA_Channel_4, (uint32_t)&(USART1->DR), (uint32_t)buf, size);
		DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
		MYDMA_Enable(USART1,DMA2_Stream7, size);
		//EPS_U1_UART_send(buf, size);
		UART2_BUF_O_Write_String_To_Buffer("EPS: SENDING msg...\n");
		UART2_BUF_O_Send_All_Data();
	}

	else if(app_id == COMMS_APP_ID) {
		 MYDMA_Init(DMA1_Stream4, DMA_Channel_4, (uint32_t)&(UART4->DR), (uint32_t)buf, size);
		 DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);
		 MYDMA_Enable(UART4,DMA1_Stream4, size);
//			COMMS_U4_UART_send(buf, size);
		UART2_BUF_O_Write_String_To_Buffer("COMMS: SENDING msg...\n");
		UART2_BUF_O_Send_All_Data();			
	}
	else if(app_id == ADCS_APP_ID) {
		MYDMA_Init(DMA2_Stream7, DMA_Channel_5, (uint32_t)&(USART6->DR), (uint32_t)buf, size);
		DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
		MYDMA_Enable(USART6,DMA2_Stream7, size);
//		ADCS_U6_UART_send(buf, size);			
		UART2_BUF_O_Write_String_To_Buffer("ADCS: SENDING msg...\n");
		UART2_BUF_O_Send_All_Data();			
	}	 
}


SAT_returnState UART_rx(TC_TM_app_id app_id, struct uart_data *data) {

	if(packet_flag == true && size > TC_MIN_PKT_SIZE)
	{
      data->uart_size = size;
      for(uint16_t i = 0; i < data->uart_size; i++) { data->uart_unpkt_buf[i] = data->uart_buf[i]; }
      return SATR_EOT;
	}
    return SATR_OK;
}




void UART_DMA_rx_check(TC_TM_app_id app_id, struct uart_data *data)
{
	uint8_t *pData;
	DMA_Stream_TypeDef* DMAy_Streamx;
	
    if(app_id == EPS_APP_ID)
      {
				pData = SUBSYS1;
				DMAy_Streamx = DMA1_Stream1;
			}
    else if(app_id == COMMS_APP_ID) 
      {
				pData = SUBSYS1;
				DMAy_Streamx = DMA1_Stream1;
			}
    else if(app_id == ADCS_APP_ID)
      {
				pData = SUBSYS1;
				DMAy_Streamx = DMA1_Stream1; 
			}
	if (pos>=UART_DMA_BUF_SIZE)
		pos=0;		
	   	pos = UART_DMA_BUF_SIZE - DMA_GetCurrDataCounter(DMAy_Streamx);
	   	   if (pos != old_pos) {                       /* Check change in received data */
					  if (pos > old_pos) {                    /* Current position is over previous one */
	               /*
	                * Processing is done in "linear" mode.
	                *
	                * Application processing is fast with single data block,
	                * length is simply calculated by subtracting pointers
	                *
	                * [   0   ]
	                * [   1   ] <- old_pos |------------------------------------|
	                * [   2   ]            |                                    |
	                * [   3   ]            | Single block (len = pos - old_pos) |
	                * [   4   ]            |                                    |
	                * [   5   ]            |------------------------------------|
	                * [   6   ] <- pos
	                * [   7   ]
	                * [ N - 1 ]
	                */
	        	   UART_PROCESS_DATA(&pData[old_pos], pos - old_pos,data);
	       }
				 else {
            /*
             * Processing is done in "overflow" mode..
             *
             * Application must process data twice,
             * since there are 2 linear memory blocks to handle
             *
             * [   0   ]            |---------------------------------|
             * [   1   ]            | Second block (len = pos)        |
             * [   2   ]            |---------------------------------|
             * [   3   ] <- pos
             * [   4   ] <- old_pos |---------------------------------|
             * [   5   ]            |                                 |
             * [   6   ]            | First block (len = N - old_pos) |
             * [   7   ]            |                                 |
             * [ N - 1 ]            |---------------------------------|
             */
            UART_PROCESS_DATA(&pData[old_pos], ARRAY_LEN(pData) - old_pos,data);
            if (pos > 0 && (flag == False) ) {
                UART_PROCESS_DATA(&pData[0], pos,data);
            }
					}			 
				}
}


void UART_PROCESS_DATA(const void* temp, size_t len,struct uart_data *data)
{
	 const uint8_t* d = temp;
	 uint8_t c = len;
	 uint8_t err =0;
   for ( uint32_t i =0; len > 0; len-- ,++d ,++i )
      {

        if( (c == len) && (*d == HLDLC_START_FLAG) )
					{
						data->uart_buf[i] = *d;
          }
				 else if(*d == HLDLC_START_FLAG)
					 {
             data->uart_buf[i] = *d;
						 size = i+1;
						 if(pos > old_pos)
						 {
						   old_pos = old_pos + size +err;
							 packet_flag =1;
							 UART2_BUF_O_Write_String_To_Buffer("recieved data complete\n");
               UART2_BUF_O_Send_All_Data();							 
						 }
						 else if((pos < old_pos) && (old_pos +i < UART_DMA_BUF_SIZE))
						 {
						   old_pos = old_pos +size +err;
               flag = True;
						 }
						 else if(pos > 0)
						 {
               	flag = False;
                old_pos = size +err;
						 }
						 break;
           }
		     else if( c > len )
		       {
						 data->uart_buf[i] = *d;
           }
		     else
		     {
		    	 i--;
		    	 len++;
		    	 err++;
		     }
      }	
}


SAT_returnState recieve_packet(TC_TM_app_id app_id, struct uart_data *data)
{
	SAT_returnState res;
	
	if( app_id == EPS_APP_ID)
	{
		EPS_U1_DMA_CHECK(data);
		res = EPS_U1_UART_recv(data);
	}
	else if( app_id == COMMS_APP_ID)
	{
		COMMS_U4_DMA_CHECK(data);
		res = COMMS_U4_UART_recv(data);
	}
	else if(app_id == ADCS_APP_ID)
	{
		ADCS_U6_DMA_CHECK(data);
		res = ADCS_U6_UART_recv(data);
	}	
	
  return res;
}
