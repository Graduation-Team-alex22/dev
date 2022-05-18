#include "time.h"
#include "math.h"
#include "../main/project.h"
#include "../hsi_library/stm32f4xx_rtc.h"
#include "common.h"

/************* LOCAL DEFINITIONS ***************/
#define RTC_TR_RESERVED_MASK    ((uint32_t)0x007F7F7F)
#define RTC_DR_RESERVED_MASK    ((uint32_t)0x00FFFF3F)

/************* LOCAL VARIABLES ***************/
static time_t time_keeping_adcs = {0};

/************* PRIVATE FUNCTIONS ************/
static uint8_t RTC_Bcd2ToByte(uint8_t Value);

// -- PUBLIC FUNCTIONS' IMPLEMENTATION ---------------------------
uint32_t time_init(void){
   
   /******* init the RTC peripheral *******/
   // to set up RTC clock control registers in the RCC peripheral,
   // We need to enable access to those register as they are proteced
   // by PWR register
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
   PWR_BackupAccessCmd(ENABLE);
   // now we can setup RTC peripheral clock and clock source
   RCC_LSEConfig(RCC_LSE_ON);                         // start LSE clock
   while( (RCC->BDCR & RCC_BDCR_LSERDY) == 0);        // wait untill LSE stablizes
   RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);            // choose LSE as clock source
   RCC_RTCCLKCmd(ENABLE);                             // enable the clock 

   /******* init the RTC parameters *******/
   // RTC is initialized with default parameters: 24h format, default prescalars for LSE
   RTC_InitTypeDef rtc_init_struct;
   RTC_StructInit(&rtc_init_struct);
   if(RTC_Init(&rtc_init_struct) == ERROR)
   {
      return ERROR_RTC_INIT;
   }
   
   /******* RTC Clock time check *******/
   // check if the time is set correctly
   time_update();
   
   // time is correctly set. No error.
   if(time_keeping_adcs.utc.year >= CURRENT_YEAR)
   {
      return NO_ERROR;         
   }
   else
   {
      uint32_t rtc_calender[2] = {0x00124530, 0x00170227};

      //obc_comm_get_time(rtc_calender); // rtc_calender[0] = TR, rtc_calender[1] = DR

      /******* set the time and date of RTC *******/
      // disable write protection and enter initialization mode
      RTC_WriteProtectionCmd(DISABLE);
      if(RTC_EnterInitMode() == ERROR) return ERROR_RTC_ENTER_INIT_MODE;
      RTC->WPR = 0xCA;
      RTC->WPR = 0x53;

      // write to TR and DR registers
      RTC->TR = rtc_calender[0] & RTC_TR_RESERVED_MASK;
      RTC->DR = rtc_calender[1] & RTC_DR_RESERVED_MASK;

      // enable write protection and exit initialization mode
      RTC->WPR = 0xFF; 
      RTC_ExitInitMode();
      RTC_WriteProtectionCmd(ENABLE);
   }
   PWR_BackupAccessCmd(DISABLE);								// Re-disable access to protected registers

   return NO_ERROR;		// no error	
}

void time_update(void)
{
   uint32_t temp_reg = (RTC->TR & RTC_TR_RESERVED_MASK);
   time_keeping_adcs.utc.sec = RTC_Bcd2ToByte((uint8_t) temp_reg);
   time_keeping_adcs.utc.min = RTC_Bcd2ToByte((uint8_t) (temp_reg >> 8));
   time_keeping_adcs.utc.hour = RTC_Bcd2ToByte((uint8_t) (temp_reg >> 16) & 0x3F);
   temp_reg = (RTC->DR & RTC_DR_RESERVED_MASK);
   time_keeping_adcs.utc.day = RTC_Bcd2ToByte((uint8_t) temp_reg);
   time_keeping_adcs.utc.month = RTC_Bcd2ToByte((uint8_t) (temp_reg >> 8) & 0x1F);
   time_keeping_adcs.utc.weekday = RTC_Bcd2ToByte((uint8_t) (temp_reg >> 13) & 0x7);
   time_keeping_adcs.utc.year = RTC_Bcd2ToByte((uint8_t) (temp_reg >> 16));
   time_keeping_adcs.utc.year += 2000;
   
   // update other time formats
   tle_epoch();
   decyear();
   julday();
}


time_t time_getTime(void){
   time_update();
   return time_keeping_adcs;
}

void tle_epoch(void){
   time_t* t = &time_keeping_adcs;

   uint16_t year = 2000 + t->utc.year;
   int days[] = { 0, 31, 59, 90, 120, 151, 182, 212, 243, 273, 304, 334 };
   int isleap = (((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0)));

   t->tle_epoch.ep_year = t->utc.year;
   t->tle_epoch.ep_day = days[t->utc.month - 1] + t->utc.day
                        + (t->utc.month > 2 ? isleap : 0) + (t->utc.hour / SOLAR_DAY_HOURS)
                        + (t->utc.min / SOLAR_DAY_MIN) + (t->utc.sec / SOLAR_DAY_SEC);
}

void decyear(void){
   time_t* t = &time_keeping_adcs;

   uint16_t year = 2000 + t->utc.year;
   int days[] = { 0, 31, 59, 90, 120, 151, 182, 212, 243, 273, 304, 334 };
   int isleap = (((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0)));

   int ndays = isleap ? 366 : 365;
   double day_no = (days[t->utc.month - 1] + t->utc.day
                  + (t->utc.month > 2 ? isleap : 0));
   double day_hour = t->utc.hour + (t->utc.min / 60) + (t->utc.sec / 3600);

   t->decyear = ((double) year + (day_no / ndays))
               + day_hour / SOLAR_DAY_HOURS / ndays;
}

void julday(void)
{
   uint16_t Y = time_keeping_adcs.utc.year;
   uint8_t M = time_keeping_adcs.utc.month;
   uint8_t D = time_keeping_adcs.utc.day;
   
   if( M <= 2)
   {
      Y -= 1;
      M += 12;
   }
   
   uint8_t A = Y / 100;
   uint8_t B = A / 4;
   int8_t  C = 2 - A + B;
   double  E = 365.25 * ( Y + 4716);
   double  F = 30.6001 * (M + 1);
   double  J = C + D + E + F - 1524.5;
   
   time_keeping_adcs.Julian_Date = J;
}

static uint8_t RTC_Bcd2ToByte(uint8_t Value)
{
   uint8_t tmp = 0;
   tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
   return (tmp + (Value & (uint8_t)0x0F));
}

static inline void convert(char* p, uint8_t num){
   p[0] = (num%100 /10) + 48;
   p[1] = (num%10) + 48;
}

void print_time(char *s){
   // "20XX-XX-XX XX:XX:XX";
   char temp[2];
   s[0] = '2';
   s[1] = '0';
   convert(temp, time_keeping_adcs.utc.year);
   s[2] = temp[0];
   s[3] = temp[1];
   s[4] = '-';
   convert(temp, time_keeping_adcs.utc.month);
   s[5] = temp[0];
   s[6] = temp[1];
   s[7] = '-'; 
   convert(temp, time_keeping_adcs.utc.day);
   s[8] = temp[0];
   s[9] = temp[1];
   s[10] = ' '; 
   convert(temp, time_keeping_adcs.utc.hour);
   s[11] = temp[0];
   s[12] = temp[1];
   s[13] = ':';
   convert(temp, time_keeping_adcs.utc.min);
   s[14] = temp[0];
   s[15] = temp[1];
   s[16] = ':'; 
   convert(temp, time_keeping_adcs.utc.sec);
   s[17] = temp[0];
   s[18] = temp[1]; 
   s[19] = 0; 
}
