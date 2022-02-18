
/* Includes */
#include "stdio.h"
#include "string.h"

#include "arm_math.h"
//#include "math.h"

#include "hw_conf.h"
#include "WahbaRotM.h"

//volatile uint8_t UsbRxBuf[8192], UsbRxLen;
volatile int I2C_Error=0;
I2C_InitTypeDef  I2C_InitStructure;


#define loopTime 10e3 //in us
// Sensor is at PB10, PB11


WahbaRotMStruct WStruct;

int main(void)
{
	volatile u32 tocs[4];
	char str[128];
	u32 stat;
	volatile u32 start;

	initWahbaStruct(&WStruct,1.0/loopFreq);


	initHw();
	delay_us(2e6);
	tic;
	Led1(1);
	SysTick_Config(SystemCoreClock/loopFreq);

	while(1){
	}

}

void SysTick_Handler(void){
	static int cnt=0;
	static unsigned int prev_toc=0;
	static int started=0;

	u8 stat;
	stat=updGyro();
	if(stat)
		stat=updAcc();
	if(stat)
		stat=updMag();
	if(stat)
		stat=updTemp();
	if(stat==0){
		sens9DInit();
		delay_us(1000000);
		Led2(1);
	}
	scaleSens();


	volatile u32 tocs[10];
	tocs[0]=toc;
	WahbaRotM(sens.Acc,sens.Gyr,sens.Mag,&WStruct);
	tocs[1]=toc;

	static int len;
	static char str[512];
	len = 0;
	len = sprintf(str,"%d,%6d,  % 8.5f,% 8.5f,% 8.5f,% 8.5f, % 8.5f,% 8.5f,% 8.5f\n",stat,tocs[1]-tocs[0],WStruct.dth,WStruct.W[0],WStruct.W[1],WStruct.W[2],sens.Gyr[0],sens.Gyr[1],sens.Gyr[2]);
	len += sprintf(&str[len],"Rot Det:% 8.5f\n % 8.5f,% 8.5f,% 8.5f\n% 8.5f,% 8.5f,% 8.5f\n% 8.5f,% 8.5f,% 8.5f\n",detMatr(WStruct.RotM),WStruct.RotM[0][1],WStruct.RotM[0][2],WStruct.RotM[0][3],WStruct.RotM[1][1],WStruct.RotM[1][2],WStruct.RotM[1][3],WStruct.RotM[2][1],WStruct.RotM[2][2],WStruct.RotM[2][3]);
	len += sprintf(&str[len],"Euler: % 8.5f,% 8.5f,% 8.5f\n\n",WStruct.Euler[0]*180.0/M_PI,WStruct.Euler[1]*180.0/M_PI,WStruct.Euler[2]*180.0/M_PI);
	cdc_DataTx(str,len,2e3);
}


void DISCOVERY_EXTI_IRQHandler(void){
	//STM32F4_Discovery_LEDToggle(LED5);
	//cdc_DataTx("123\n",4);
}
