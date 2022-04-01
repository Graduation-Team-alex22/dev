#include "UART_task.h"

#include "obc.h"
#include "mxconstants.h"
#include "obc_hsi.h"

static void GPIO_USART_Tx_Init();
static void DMA_USART_Rx_Init();

uint8_t SUBSYS1[UART_DMA_BUF_SIZE];

void     UART_Init(void)
{	
	 GPIO_USART_Tx_Init();
	
	 /******************************* IS25LP128  eeprom *********************************************/
	 GPIO_SetBits(GPIOA, GPIO_Pin_11);
	 GPIO_SetBits(GPIOB, GPIO_Pin_12);
	
   pkt_pool_INIT();
   HAL_obc_enableBkUpAccess();
   bkup_sram_INIT();

   uint8_t rsrc = 0;
   HAL_reset_source(&rsrc);
   set_reset_source(rsrc);

   uint32_t b_cnt = 0;
   get_boot_counter(&b_cnt);

   update_boot_counter();

   HAL_obc_SD_ON();
	
}

uint32_t UART_Update(void)
{
    import_pkt(EPS_APP_ID, &obc_data.eps_uart);
    export_pkt(EPS_APP_ID, &obc_data.eps_uart);

    import_pkt(COMMS_APP_ID, &obc_data.comms_uart);
    export_pkt(COMMS_APP_ID, &obc_data.comms_uart);

    import_pkt(ADCS_APP_ID, &obc_data.adcs_uart);
    export_pkt(ADCS_APP_ID, &obc_data.adcs_uart);

	return RETURN_NORMAL_STATE;
}

void GPIO_USART_Tx_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

   // GPIOA clock enable 
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	 GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8; 
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOD, &GPIO_InitStructure);

	 USART_InitStructure.USART_BaudRate = 115200;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Tx;
   USART_Init(USART3, &USART_InitStructure);
	 USART_Cmd(USART3, ENABLE);
}

void DMA_USART_Rx_Init()
{
	
	DMA_InitTypeDef DMA_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = 115200;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART2, &USART_InitStructure);
   USART_Cmd(USART2, ENABLE);

   DMA_DeInit(DMA1_Stream5);
   DMA_InitStructure.DMA_BufferSize = UART_DMA_BUF_SIZE;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
   DMA_InitStructure.DMA_Channel = DMA_Channel_4 ;
   DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)SUBSYS1;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
 // DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
   //DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
   DMA_Init(DMA1_Stream5, &DMA_InitStructure);
	 USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
	 DMA_Cmd(DMA1_Stream5,ENABLE);
	
}