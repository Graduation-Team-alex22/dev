
#include "IDLE_task.h"
#include "queue.h"
#include "mxconstants.h"

static void GPIO_Inits();

void IDLE_Init(void)
{
	GPIO_Inits();
}

uint32_t IDLE_Update(void)
{
	 // pkt_pool_IDLE(time);
   queue_IDLE(EPS_APP_ID);
   queue_IDLE(DBG_APP_ID);
   queue_IDLE(COMMS_APP_ID);
   queue_IDLE(ADCS_APP_ID);
	
	return RETURN_NORMAL_STATE;
}	

void GPIO_Inits()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/* GPIO Ports Clock Enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
	
	/*Configure GPIO pin Output Level */
	GPIO_ResetBits(GPIOC, COMMS_EN_Pin);
	GPIO_ResetBits(GPIOB, FLASH_HOLD_Pin);
	GPIO_ResetBits(GPIOA, FLASH_WP_Pin);
	GPIO_ResetBits(GPIOA, FLASH_CS_SPI2_Pin);
	GPIO_ResetBits(GPIOA, SD_PWR_EN_Pin);
	
	 /*Configure GPIO pins : COMMS_EN_Pin */
	 GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz; 
	 GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStruct.GPIO_Pin   = COMMS_EN_Pin;
   GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	 /*Configure GPIO pins :  FLASH_HOLD_Pin */
	 GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz; 
	 GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStruct.GPIO_Pin   = FLASH_HOLD_Pin;
   GPIO_Init(GPIOB, &GPIO_InitStruct);
	 
/*Configure GPIO pins : FLASH_WP_Pin /FLASH_CS_SPI2_Pin /SD_PWR_EN_Pin */
	 GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz; 
	 GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStruct.GPIO_Pin   = FLASH_WP_Pin|FLASH_CS_SPI2_Pin|SD_PWR_EN_Pin;
   GPIO_Init(GPIOA, &GPIO_InitStruct); 	
}