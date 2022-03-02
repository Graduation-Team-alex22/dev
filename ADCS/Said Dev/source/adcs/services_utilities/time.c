#include "time.h"
#include "math.h"

#define RTC_TR_RESERVED_MASK    ((uint32_t)0x007F7F7F)
#define RTC_DR_RESERVED_MASK    ((uint32_t)0x00FFFF3F)


/**********************************************************
										SUPPORTED APIs
**********************************************************/
/*
	Initializes the RTC peripheral and sets the clock
	from OBC.

	NOTE: OBC communication must be initialized first.
*/
uint8_t time_init(void){
	/******* init the RTC peripheral *******/
	// to set up RTC clock control registers in the RCC peripheral,
	// We need to enable access to those register as they are proteced
	// by PWR register
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	// now we can setup RTC peripheral clock and clock source
	RCC_LSEConfig(RCC_LSE_ON);									// start LSE clock
	while( (RCC->BDCR & RCC_BDCR_LSERDY) == 0);	// wait untill LSE stablizes
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);			// choose LSE as clock source
	RCC_RTCCLKCmd(ENABLE);											// enable the clock 
	
	/******* init the RTC parameters *******/
	// RTC is initialized with default parameters: 24h format, default prescalars for LSE
	RTC_InitTypeDef rtc_init_struct;
	RTC_StructInit(&rtc_init_struct);
	if(RTC_Init(&rtc_init_struct) == ERROR) return 110;
	
	/******* RTC Clock time check *******/
	// check if the time is set correctly
	time_update();		// update time_keeping_adcs according to current time of RTC
	if(time_keeping_adcs.utc.year >= CURRENT_YEAR){
		return 0;				// time is correctly set. No error.
	}else{
		uint32_t rtc_calender[2] = {0x00124530, 0x00220227};
		
		//obc_comm_get_time(rtc_calender); // rtc_calender[0] = TR, rtc_calender[1] = DR
		
		/******* set the time and date of RTC *******/
		// disable write protection and enter initialization mode
		RTC_WriteProtectionCmd(DISABLE);
		if(RTC_EnterInitMode() == ERROR) return 111;
		RTC->WPR = 0xCA;
		RTC->WPR = 0x53;
		
		// write to TR and DR registers
		RTC->TR = rtc_calender[0] & RTC_TR_RESERVED_MASK;
		RTC->DR = rtc_calender[1] & RTC_DR_RESERVED_MASK;
		
		// enable write protection and exit initialization mode
		RTC->WPR = 0xFF; 
		RTC_ExitInitMode();
		RTC_WriteProtectionCmd(ENABLE);
		
		time_update();		// update time_keeping_adcs according to current time of RTC
	}
	PWR_BackupAccessCmd(DISABLE);								// Re-disable access to protected registers

	return 0;		// no error	
}

/*
	Updates time_keeping_adcs from the RTC peripheral.
*/
void time_update(void){
	// update utc
	/* 	all constants are deduced according to how different data
			fields (seconds, minutes, ...) are arranged in RTC registers
			TR (time register) and DR (date register). 
	*/
	uint32_t temp_reg = (RTC->TR & RTC_TR_RESERVED_MASK);
	time_keeping_adcs.utc.sec = RTC_Bcd2ToByte((uint8_t) temp_reg);
	time_keeping_adcs.utc.min = RTC_Bcd2ToByte((uint8_t) (temp_reg >> 8));
	time_keeping_adcs.utc.hour = RTC_Bcd2ToByte((uint8_t) (temp_reg >> 16) & 0x3F);
	temp_reg = (RTC->DR & RTC_DR_RESERVED_MASK);
	time_keeping_adcs.utc.day = RTC_Bcd2ToByte((uint8_t) temp_reg);
	time_keeping_adcs.utc.month = RTC_Bcd2ToByte((uint8_t) (temp_reg >> 8) & 0x1F);
	time_keeping_adcs.utc.weekday = RTC_Bcd2ToByte((uint8_t) (temp_reg >> 13) & 0x7);
	time_keeping_adcs.utc.year = RTC_Bcd2ToByte((uint8_t) (temp_reg >> 16));
	
	// update other time formats
	tle_epoch();
	decyear();
	julday();
	
}


time_keeping_adcs_t time_getTime(void){
	return time_keeping_adcs;
}


/**************** conversion functions *******************/
/*
	calculates time_keeping_adcs.tle_epoch from 
	time_keeping_adcs.UTC
*/
void tle_epoch(void){
	time_keeping_adcs_t* t = &time_keeping_adcs;
	
	uint16_t year = 2000 + t->utc.year;
	int days[] = { 0, 31, 59, 90, 120, 151, 182, 212, 243, 273, 304, 334 };
	int isleap = (((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0)));

	t->tle_epoch.ep_year = t->utc.year;
	t->tle_epoch.ep_day = days[t->utc.month - 1] + t->utc.day
					+ (t->utc.month > 2 ? isleap : 0) + (t->utc.hour / SOLAR_DAY_HOURS)
					+ (t->utc.min / SOLAR_DAY_MIN) + (t->utc.sec / SOLAR_DAY_SEC);
}


/*
	calculates time_keeping_adcs.decyear from 
	time_keeping_adcs.UTC
*/
void decyear(void){
	time_keeping_adcs_t* t = &time_keeping_adcs;
	
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

/*
	calculates time_keeping_adcs.jd from 
	time_keeping_adcs.UTC
*/
void julday(void){
	time_keeping_adcs_t* t = &time_keeping_adcs;
	
	uint16_t year = 2000 + t->utc.year;
	float sgn = 100.0 * year + t->utc.month - 190002.5;

	if (sgn > 0) {
			sgn = 1;
	} else if (sgn < 0) {
			sgn = -1;
	} else {
			sgn = 0;
	}

	t->jd = 367.0 * year
					- floor((7 * (year + floor((t->utc.month + 9) / 12.0))) * 0.25)
					+ floor(275 * t->utc.month / 9.0) + t->utc.day + 1721013.5
					+ ((t->utc.sec / 60.0 + t->utc.min) / 60.0 + t->utc.hour) / 24.0
					- 0.5 * sgn + 0.5;
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
