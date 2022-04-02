#include "obc_hsi.h"
#include "stm32f4xx.h"
#include "obc.h"


extern struct _obc_data obc_data;


#undef __FILE_ID__
#define __FILE_ID__ 18


void osDelay(uint32_t usec)
{

}
 
void HAL_sys_delay(uint32_t usec) {
	osDelay(usec);
}

static uint16_t switch1 = 0;
static uint16_t loop =0;

void HAL_obc_SD_ON() {

    for(;loop < 2000;){
			if(switch1 == 0)
			 {
			   GPIO_ResetBits(GPIOA, GPIO_Pin_12);
			   switch1 ++;
         // osDelay(1);
			 }
			else
				{
				  GPIO_SetBits(GPIOA, GPIO_Pin_12);
				  switch1 --;				
          // osDelay(1);
				}
				loop++;
 				break;			
			}
    GPIO_ResetBits(GPIOA, GPIO_Pin_12);
}

static uint16_t switch2 = 0;
void HAL_obc_SD_OFF() {
	if(switch2 == 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_12);
		switch2++;
	}
	else
	{
		//osDelay(200);
	}
}

void HAL_reset_source(uint8_t *src) {

    *src = RCC_GetFlagStatus(RCC_FLAG_BORRST);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_PINRST) << 1);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_PORRST) << 2);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_SFTRST) << 3);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) << 4);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) << 5);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_LPWRRST) << 6);
 
    RCC_ClearFlag();

}

void HAL_sys_setTime(uint8_t hours, uint8_t mins, uint8_t sec) {

	RTC_TimeTypeDef sTime;
	
	sTime.RTC_Hours = hours ;
	sTime.RTC_Minutes = mins ;
	sTime.RTC_Seconds = sec ;
	RTC_SetTime(RTC_Format_BIN , &sTime);
  
}

void HAL_sys_getTime(uint8_t *hours, uint8_t *mins, uint8_t *sec) {

	RTC_TimeTypeDef sTime;
	
	RTC_GetTime(RTC_Format_BIN , &sTime);
	
	*hours = sTime.RTC_Hours ;
	*mins = sTime.RTC_Minutes ;
	*sec = sTime.RTC_Seconds ;
}

void HAL_sys_setDate(uint8_t weekday, uint8_t mon, uint8_t date, uint8_t year) {

		RTC_DateTypeDef sDate ; 
	
	sDate.RTC_WeekDay = weekday;
	sDate.RTC_Month = mon;
	sDate.RTC_Date = date;
	sDate.RTC_Year = year;
	
	RTC_SetDate(RTC_Format_BIN , &sDate);

}

void HAL_sys_getDate(uint8_t *weekday, uint8_t *mon, uint8_t *date, uint8_t *year) {

 	RTC_DateTypeDef sDate ; 
	RTC_GetDate(RTC_Format_BIN , &sDate);
	
	*weekday = sDate.RTC_WeekDay;
	*mon = sDate.RTC_Month;
	*date = sDate.RTC_Date;
	*year = sDate.RTC_Year;

}



void HAL_obc_enableBkUpAccess() {
 // HAL_PWR_EnableBkUpAccess();
  //HAL_PWREx_EnableBkUpReg();
  //__HAL_RCC_BKPSRAM_CLK_ENABLE();
	PWR_BackupAccessCmd(ENABLE);
	PWR_BackupRegulatorCmd(ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
}

uint32_t * HAL_obc_BKPSRAM_BASE() {
  return (uint32_t *)BKPSRAM_BASE;
}
