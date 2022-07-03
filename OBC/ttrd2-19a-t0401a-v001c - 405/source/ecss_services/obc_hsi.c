#include "obc_hsi.h"
#include "stm32f4xx.h"
#include "obc.h"
#include "../main/main.h"
#include "fatfs.h"
#include "mass_storage_service.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"

extern struct _obc_data obc_data;
extern void DelayOneMillis(void);

#undef __FILE_ID__
#define __FILE_ID__ 18

static uint32_t currentTick;
uint32_t tick = 0;
#define lastTick  0xFFFF0000

void updateCurrentTick(void)
{
	if(currentTick >= lastTick)
	{
		currentTick = 0;
	}
	
		currentTick = currentTick + 30;
}

uint32_t getCurrentTick(void)
{
	return currentTick;
}

void obc_SD_ON() {

  for(uint16_t i = 0; i < 1000; i++){ 
		GPIO_ResetBits(GPIOA, GPIO_Pin_12);
		DelayOneMillis();
		GPIO_SetBits(GPIOA, GPIO_Pin_12);			
		DelayOneMillis();
	}
	GPIO_ResetBits(GPIOA, GPIO_Pin_12);
}

void obc_SD_OFF() {

	GPIO_SetBits(GPIOA, GPIO_Pin_12);
	DelayOneMillis();
}

void OBC_reset_source(uint8_t *src) {

    *src = RCC_GetFlagStatus(RCC_FLAG_BORRST);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_PINRST) << 1);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_PORRST) << 2);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_SFTRST) << 3);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) << 4);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) << 5);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_LPWRRST) << 6);
 
    RCC_ClearFlag();

}

void OBC_sys_setTime(uint8_t hours, uint8_t mins, uint8_t sec) {

	RTC_TimeTypeDef sTime;
	
	sTime.RTC_Hours = hours ;
	sTime.RTC_Minutes = mins ;
	sTime.RTC_Seconds = sec ;
	RTC_SetTime(RTC_Format_BIN , &sTime);
  
}

void OBC_sys_getTime(uint8_t *hours, uint8_t *mins, uint8_t *sec) {

	RTC_TimeTypeDef sTime;
	
	RTC_GetTime(RTC_Format_BIN , &sTime);
	
	*hours = sTime.RTC_Hours ;
	*mins = sTime.RTC_Minutes ;
	*sec = sTime.RTC_Seconds ;
}

void OBC_sys_setDate(uint8_t weekday, uint8_t mon, uint8_t date, uint8_t year) {

		RTC_DateTypeDef sDate ; 
	
	sDate.RTC_WeekDay = weekday;
	sDate.RTC_Month = mon;
	sDate.RTC_Date = date;
	sDate.RTC_Year = year;
	
	RTC_SetDate(RTC_Format_BIN , &sDate);

}

void OBC_sys_getDate(uint8_t *weekday, uint8_t *mon, uint8_t *date, uint8_t *year) {

 	RTC_DateTypeDef sDate ; 
	RTC_GetDate(RTC_Format_BIN , &sDate);
	
	*weekday = sDate.RTC_WeekDay;
	*mon = sDate.RTC_Month;
	*date = sDate.RTC_Date;
	*year = sDate.RTC_Year;
}



void obc_enableBkUpAccess() {
	PWR_BackupAccessCmd(ENABLE);
	PWR_BackupRegulatorCmd(ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
}

uint32_t * obc_BKPSRAM_BASE() {
  return (uint32_t *)BKPSRAM_BASE;
}

void sdcard_gpio_inits()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/* GPIO Ports Clock Enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	/*Configure GPIO pin Output Level */
	GPIO_ResetBits(GPIOA, SD_PWR_EN_Pin);
	
		 
/*Configure GPIO pins : SD_PWR_EN_Pin */
	 GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
   GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz; 
	 GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStruct.GPIO_Pin   = SD_PWR_EN_Pin;
   GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void sdcard_init()
{
	sdcard_gpio_inits();
	MX_FATFS_Init();
	obc_SD_ON();
	mass_storage_init();

	UART2_BUF_O_Write_String_To_Buffer("\n -SDCARD INIT- \n");
	UART2_BUF_O_Send_All_Data();	
}

