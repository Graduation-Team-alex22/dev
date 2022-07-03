#include "UART_task.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#include "obc.h"
#include "obc_hsi.h"
#include "../tasks/COMMS_comm_task.h"
#include "../tasks/EPS_comm_task.h"
#include "../tasks/ADCS_comm_task.h"


//static void DMA_USART_Rx_Init(void);
uint8_t SUBSYS1[UART_DMA_BUF_SIZE];

void UART_Init(void)
{	

	 COMMS_U4_init(9600);
	 EPS_U1_init(9600);
	 ADCS_U6_init(9600);
	
   pkt_pool_INIT();
   obc_enableBkUpAccess();
   bkup_sram_INIT();

   uint8_t rsrc = 0;
   OBC_reset_source(&rsrc);
   set_reset_source(rsrc);

   uint32_t b_cnt = 0;
   get_boot_counter(&b_cnt);

   update_boot_counter();
	
}

uint32_t UART_Update(void)
{
	
	UART2_BUF_O_Update();
	

  EPS_U1_update();
  EPS_U1_BUF_O_Update();
	
  COMMS_U4_update();
  COMMS_U4_BUF_O_Update();

  ADCS_U6_update();
  ADCS_U6_BUF_O_Update();	

	return RETURN_NORMAL_STATE;
}


//void DMA_USART_Rx_Init()
//{
//	
//	DMA_InitTypeDef DMA_InitStructure;
//  USART_InitTypeDef USART_InitStructure;
//   GPIO_InitTypeDef GPIO_InitStructure;

//   GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);
//   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
//   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
//   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//   GPIO_Init(GPIOD, &GPIO_InitStructure);
//	
//	   USART_InitStructure.USART_BaudRate = 230400;
//	   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	   USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	   USART_InitStructure.USART_Parity = USART_Parity_No;
//	   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	   USART_InitStructure.USART_Mode = USART_Mode_Rx;
//	   USART_Init(USART3, &USART_InitStructure);
//	   USART_Cmd(USART3, ENABLE);


//	  // DMA_DeInit(DMA1_Stream1);
//		 DMA_StructInit(&DMA_InitStructure);
//	   DMA_InitStructure.DMA_BufferSize = UART_DMA_BUF_SIZE;
//	   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	   DMA_InitStructure.DMA_Channel = DMA_Channel_4 ;
//	   DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)SUBSYS1;
//	   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
//	   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	   DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
//	   DMA_InitStructure.DMA_FIFOMode =DMA_FIFOMode_Disable;
//	   DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
//		 DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	   DMA_Init(DMA1_Stream1, &DMA_InitStructure);
//	   USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
//	   DMA_Cmd(DMA1_Stream1,ENABLE);

//}

