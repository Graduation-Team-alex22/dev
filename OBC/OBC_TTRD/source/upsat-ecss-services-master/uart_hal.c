#include "uart_hal.h"

#include "obc.h"
#include "hldlc.h"
#include "../tasks/UART_task.h"

extern uint8_t SUBSYS1[UART_DMA_BUF_SIZE];

#undef __FILE_ID__
#define __FILE_ID__ 17

struct _uart_timeout
{
    uint32_t su;
    uint32_t adcs;
    uint32_t eps;
    uint32_t comms;
};

static struct _uart_timeout uart_timeout; 
extern uint8_t uart_temp[];

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
void MYDMA_Enable(DMA_Stream_TypeDef* DMA_Streamx, uint16_t ndtr)
{

	// close DMA transmission
	DMA_Cmd(DMA_Streamx, DISABLE);

	// Guarantee DMA Can be set
	while(DMA_GetCmdStatus(DMA_Streamx) != DISABLE)
	{

	}

	// Set the amount of data transfer
	DMA_SetCurrDataCounter(DMA_Streamx, ndtr);

	// Turn on DMA transmission
	DMA_Cmd(DMA_Streamx, ENABLE);
}

SAT_returnState hal_kill_uart(TC_TM_app_id app_id) {

//    UART_HandleTypeDef *huart;
//    HAL_StatusTypeDef res;
//    SAT_returnState ret = SATR_ERROR;

//    if(app_id == EPS_APP_ID) { huart = &huart1; }
//    else if(app_id == DBG_APP_ID) { huart = &huart3; }
//    else if(app_id == COMMS_APP_ID) { huart = &huart4; }
//    else if(app_id == ADCS_APP_ID) { huart = &huart6; }

//    HAL_UART_DeInit(huart);
//    res = HAL_UART_Init(huart);
//    if (res == HAL_OK)
//    {
//      ret = SATR_OK;
//    }
//    return ret;
}

SAT_returnState HAL_uart_tx_check(TC_TM_app_id app_id) {
    
//    HAL_UART_StateTypeDef res;
//    UART_HandleTypeDef *huart;

//    if(app_id == EPS_APP_ID) { huart = &huart1; }
//    else if(app_id == DBG_APP_ID) { huart = &huart3; }
//    else if(app_id == COMMS_APP_ID) { huart = &huart4; }
//    else if(app_id == ADCS_APP_ID) { huart = &huart6; }

//    res = HAL_UART_GetState(huart);
//    if(res == HAL_UART_STATE_BUSY ||
//       res == HAL_UART_STATE_BUSY_TX ||
//       res == HAL_UART_STATE_BUSY_TX_RX) { return SATR_ALREADY_SERVICING; }

//    return SATR_OK;
}

void HAL_uart_tx(TC_TM_app_id app_id, uint8_t *buf, uint16_t size)
{
    USART_TypeDef *huart;

    if(app_id == EPS_APP_ID) {
    	MYDMA_Init(DMA2_Stream7, DMA_Channel_4, (uint32_t)&(USART1->DR), (uint32_t)buf, size);
    	DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
			MYDMA_Enable(DMA2_Stream7, size);
    }

    else if(app_id == COMMS_APP_ID) {
    	MYDMA_Init(DMA1_Stream4, DMA_Channel_4, (uint32_t)&(UART4->DR), (uint32_t)buf, size);
    	DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);
			MYDMA_Enable(DMA1_Stream4, size);
    }
    else if(app_id == ADCS_APP_ID) {
    	MYDMA_Init(DMA2_Stream7, DMA_Channel_5, (uint32_t)&(USART6->DR), (uint32_t)buf, size);
    	DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
			MYDMA_Enable(DMA2_Stream7, size);
    }	
 
}


static uint32_t size = 0;

SAT_returnState HAL_uart_rx(TC_TM_app_id app_id, struct uart_data *data) {

    data->uart_size = size;
    for(uint16_t i = 0; i < data->uart_size; i++) { data->uart_unpkt_buf[i] = data->uart_buf[i]; }
    return SATR_EOT;
		
    return SATR_OK;
}


uint16_t err;
static uint32_t old_pos;
static	uint32_t pos;
#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))
#define True 1
#define False 0	
uint8_t flag = False;

void UART_DMA_rx_check(TC_TM_app_id app_id, struct uart_data *data)
{
	uint8_t *pData;
	DMA_Stream_TypeDef* DMAy_Streamx;
	
    if(app_id == EPS_APP_ID)
      {
				pData = SUBSYS1;
				DMAy_Streamx = DMA1_Stream5;
			}
    else if(app_id == COMMS_APP_ID) 
      {
				DMAy_Streamx = DMA1_Stream5;
			}
    else if(app_id == ADCS_APP_ID)
      {
				DMAy_Streamx = DMA1_Stream5; 
			}
		
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
   for ( uint32_t i =0; len > 0; len-- ,++d ,i++ )
      {

        if( (c == len) && (*d == HLDLC_START_FLAG) )
					{
						data->uart_buf[i] = *d; 
            //uart_timeout_start(huart);
          }  
				 else if(*d  == HLDLC_START_FLAG)
					 {
             data->uart_buf[i] = *d; 
						 size = i;
						 if(pos > old_pos)
						 {
						   old_pos = old_pos + i;							 
						 }
						 else if((pos < old_pos) && (old_pos +i < UART_DMA_BUF_SIZE))
						 {
						   old_pos = old_pos + i;		
               flag = True;						 
						 }						 
						 else if(pos > 0)
						 {
               	flag = False;
                old_pos = i;							 
						 }
						 break;
           }
		     else if( c > len )
		       {
						 data->uart_buf[i] = *d; 
           }	   
      }	
}

void uart_timeout_start(USART_TypeDef *huart) {

  // uint32_t t = HAL_GetTick();
  // if(huart == &huart1)      { uart_timeout. = t; }
  // else if(huart == &huart2) { uart_timeout. = t; }
  // else if(huart == &huart3) { uart_timeout. = t; }
  // else if(huart == &huart4) { uart_timeout. = t; }
  // else if(huart == &huart6) { uart_timeout. = t; }
}

void uart_timeout_stop(USART_TypeDef *huart) {

  // uint32_t t = HAL_GetTick();
  // if(huart == &huart1)      { uart_timeout. = 0; }
  // else if(huart == &huart2) { uart_timeout. = 0; }
  // else if(huart == &huart3) { uart_timeout. = 0; }
  // else if(huart == &huart4) { uart_timeout. = 0; }
  // else if(huart == &huart6) { uart_timeout. = 0; }
}

void uart_timeout_check(USART_TypeDef *huart){

  // uint32_t t = HAL_GetTick();
  // if(huart == &huart1 && uart_timeout. != 0 && (t - uart_timeout. > TIMEOUT)) { 
  //   HAL_UART_Receive_IT(huart, data->uart_buf, UART_BUF_SIZE);
  // } else if(huart == &huart2 && uart_timeout. != 0 && (t - uart_timeout. > TIMEOUT)) { 
  // } else if(huart == &huart3 && uart_timeout. != 0 && (t - uart_timeout. > TIMEOUT)) { 
  // } else if(huart == &huart4 && uart_timeout. != 0 && (t - uart_timeout. > TIMEOUT)) { 
  // } else if(huart == &huart6 && uart_timeout. != 0 && (t - uart_timeout. > TIMEOUT)) { 
  //   HAL_UART_Receive_IT(huart, &su_inc_buffer[22], 173);
  // }
}
