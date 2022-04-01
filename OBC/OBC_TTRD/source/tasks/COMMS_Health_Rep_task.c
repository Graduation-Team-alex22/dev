#include "COMMS_Health_Rep_task.h"

#include "housekeeping_service.h"
#include "housekeeping.h"
#include "mxconstants.h"


static void SPI_flash_Init();

void     COMMS_Health_Rep_task_Init(void)
{
	SPI_flash_Init();
}
uint32_t COMMS_Health_Rep_task_Update(void)
{
  COMMS_Health_Req_SCH();
	
	return RETURN_NORMAL_STATE;
}


void SPI_flash_Init()
{
 GPIO_InitTypeDef GPIOB_InitStructure;
	 SPI_InitTypeDef SPI_InitStructure;
	
	 //Enable peripheral clock
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	 //Enable SCK, MOSI, MISO and NSS GPIO clocks
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	 // GPIO config
	 GPIOB_InitStructure.GPIO_Pin   = FLASH_SCK_SPI2_Pin|FLASH_MISO_SPI2_Pin|FLASH_MOSI_SPI2_Pin; 
   GPIOB_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIOB_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIOB_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
   GPIO_Init(UART2_PORT, &GPIOB_InitStructure);

	 GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);
	 GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2);
	 GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);
	 
	 // SPI2 configuration
	 SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	 SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	 SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	 SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	 SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	 SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	 SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	 SPI_InitStructure.SPI_CRCPolynomial = 10;
	 SPI_Init(SPI2,&SPI_InitStructure);
	 // Enable SPI2
	 SPI_Cmd(SPI2,ENABLE);	
}