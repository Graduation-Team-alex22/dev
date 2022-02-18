/*
 * hw_conf.c
 *
 *  Created on: Jul 6, 2012
 *      Author: mobintu
 */
#include "hw_conf.h"
__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;
#define adaf
//extern volatile int I2C_Error;

void initHw(){

	LedInit();
	GenTimInit();

	delay_us(500e3);
	sens9DInit();

	USBD_Init(&USB_OTG_dev,  USB_OTG_FS_CORE_ID,&USR_desc,&USBD_CDC_cb,&USR_cb);
	NVIC_SetPriority(SysTick_IRQn, 0);

}

void LedInit(void){
	GPIO_InitTypeDef      GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void GenTimInit(void){
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = (84-1); //us CHECK prescaler
	TIM_TimeBaseStructure.TIM_ClockDivision = 1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_Cmd(TIM2, ENABLE);
}




void sens9DInit(void){
	//PB10/I2C2_SCL, PB11/I2C2_SDA
	// XM Read Address 8bit 0x3B, Gyro 8bit Read 0xD7
	GPIO_InitTypeDef      GPIO_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;
	u8 statOk=!0;
	int i;



	I2C_DeInit(I2C2);
	I2C_Cmd(I2C2, DISABLE);
	I2C_SoftwareResetCmd(I2C2, ENABLE);
	I2C_SoftwareResetCmd(I2C2, DISABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIOB->BSRRL=GPIO_Pin_10|GPIO_Pin_11;

	i=8;
	while( (i--) && ((GPIOB->IDR&GPIO_Pin_11)==0)){
		delay_us(200);
		GPIOB->BSRRH=GPIO_Pin_10;
		delay_us(200);
		GPIOB->BSRRL=GPIO_Pin_10;
	}

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10, GPIO_AF_I2C2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11, GPIO_AF_I2C2);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x01;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed=100e3;
	I2C_InitStructure.I2C_DutyCycle=I2C_DutyCycle_16_9;
	I2C_Init(I2C2, &I2C_InitStructure);
	I2C_StretchClockCmd(I2C2,ENABLE);
	//I2C_GeneralCallCmd(I2C1,ENABLE);
	//I2C_AnalogFilterCmd(I2C2,ENABLE);
	//I2C_DigitalFilterConfig(I2C2,0x02); Not available on 407.. :(
	I2C_Cmd(I2C2, ENABLE);


	delay_us(1e3);

	//CTRL_REG1_G to CTRL_REG5_G 0x20..0x24
	//190Hz, 25Hz cut
	//0.45Hz HPass
	//Full scale 2000dps
	//HPass ΝΟΤ selected
	//					20h			21h				22h			23h			24h
	u8 GyroCTRreg[5]={0b01111111, 0b00100101,	0b00000000,	0b10100000,	0b00000000 };//24h ->0b00000000 disables LPF2 //Enable 0b00000010
	I2C_write(Gy_addr,0x20|0x80,GyroCTRreg,5);
	// HP filter bypassed
	//					1Fh			20h			21h			22h			23h			24h			25h			26h
	u8 XM_CTRreg[8]={0b00000000, 0b01111111, 0b11001000, 0b00000000, 0b00000000, 0b11110100, 0b00000000, 0b00000000 };//200Hz, 50HzBW, 4G, Magn:100Hz,2Gauss
	I2C_write(XM_addr,0x1F|0x80,XM_CTRreg,8);

}



u8 updGyro(void){
	// Takes ~0.21ms when no error..
	return I2C_read(Gy_addr,0x28|0x80,(u8 *)sens.GyrRaw,6);
}
u8 updAcc(void){
	return I2C_read(XM_addr,0x28|0x80,(u8 *)sens.AccRaw,6);
}
u8 updMag(void){
	return I2C_read(XM_addr,0x08|0x80,(u8 *)sens.MagRaw,6);
}
u8 updTemp(void){
	u16 tmp,ret;
	ret=I2C_read(XM_addr,0x05|0x80,(u8 *)&tmp,2);
	sens.TempRaw=(float)(((s16)(tmp<<4))>>4)/8.0+21;
	return ret;
}

void scaleSens(void){
	float t[3],vec[3];
	int i;
	const float AccSc[9]={ 	 1.0,  0.0,   0.0, \
			0.0,  1.0,   0.0,\
			0.0,  0.0,   1.0 };
	const float AccOf[3]={0.095839126788067, 0.050385565686325, 0.006112150167768};

	const float MagSc[9]={ 	1.19,  0.0,   0.0, \
							0.0,  1.37170,   0.0,\
							0.0,  0.0,   1.3760 };
	const float MagOf[3]={0.2235, 0.1018, 0.1352};



	for(i=0;i<3;i++){
		vec[i]=((float)sens.AccRaw[i])*0.122/1e3;
		sens.Gyr[i]=((float)sens.GyrRaw[i])*4.0*(17.5/1e3)*(M_PI/180.0);//8.75/1e3;
	}

	sens.Acc[0] = AccSc[0]*vec[0]+AccSc[1]*vec[1]+AccSc[2]*vec[2]+AccOf[0];
	sens.Acc[1] = AccSc[3]*vec[0]+AccSc[4]*vec[1]+AccSc[5]*vec[2]+AccOf[1];
	sens.Acc[2] = AccSc[6]*vec[0]+AccSc[7]*vec[1]+AccSc[8]*vec[2]+AccOf[2];

	sens.Mag[0] = ( sens.MagRaw[0]/6500.0 + MagOf[0]);
	sens.Mag[1] = ( sens.MagRaw[1]/6500.0 + MagOf[1]);
	sens.Mag[2] = (-sens.MagRaw[2]/6500.0 + MagOf[2]);

	sens.Mag[0] = 	MagSc[0]*sens.Mag[0] + MagSc[1]*sens.Mag[1] + MagSc[2]*sens.Mag[2];
	sens.Mag[1] =  	MagSc[3]*sens.Mag[0] + MagSc[4]*sens.Mag[1] + MagSc[5]*sens.Mag[2];
	sens.Mag[2] =	MagSc[6]*sens.Mag[0] + MagSc[7]*sens.Mag[1] + MagSc[8]*sens.Mag[2];
}


void calibSens(){
#define N 100
	int i=N,d,cnt,stat;
	cnt=0;

	while(i--){
		stat=updAcc();
		if(stat)
			updGyro();
		if(stat){
			cnt++;
			for(d=0;d<3;d++){
				sens.CalibAcc[d]+=(float)sens.AccRaw[d];
				sens.CalibGyr[d]+=(float)sens.GyrRaw[d];
				delay_us(10e3);
			}
		}
		else{
			sens9DInit();
			Led2(1);
		}
	}
	sens.CalibAcc[0]/=(float)cnt;
	sens.CalibAcc[1]/=(float)cnt;
	sens.CalibAcc[2]=sens.CalibAcc[2]/(float)cnt+1.0/(0.122/1e3);
	for(d=0;d<3;d++){
		sens.CalibGyr[d]/=(float)cnt;
	}
}
/*{
	//static float a[3]={0.122/1e3,0.122/1e3,0.122/1e3},b[3]={0,0,0};
	static double a[3]={1,1,1},b[3]={0,0,0},e_o;

	double mua=0.01;
	double mub=0.01;
	double e,tmp;
	int i;

	e=0;
	for(i=0;i<3;i++){
		sens.Acc[i]=0.122/1e3*sens.AccRaw[i];
	}
	for(i=0;i<3;i++){
		e+=(a[i]*sens.Acc[i]+b[i])*(a[i]*sens.Acc[i]+b[i]);
	}
	e=4*(1-e);
	e=0.01*e+0.99*e_o;
	for(i=0;i<3;i++){
		tmp=e*(a[i]*sens.Acc[i]+b[i]);
		a[i]=a[i]+ mua*tmp*sens.Acc[i];
		b[i]= mub*tmp;
		sens.Acc[i]=a[i]*sens.Acc[i]+b[i];
		aa[i]=a[i];
		bb[i]=b[i];
	}
	e_o=e;
}
 */

void calibGyr(void){
	sens.Gyr[0]=sens.GyrRaw[0]*8.75/1e3;
	sens.Gyr[1]=sens.GyrRaw[1]*8.75/1e3;
	sens.Gyr[2]=sens.GyrRaw[2]*8.75/1e3;
}


/*
u8 scaleSens(scaleSens *dat){

}*/

u8 I2C_write(u8 addr,u8 reg,u8 *buf,u8 len){
	u8 statOk=!0;

	I2C_AcknowledgeConfig(I2C2, ENABLE);
	I2C_GenerateSTART(I2C2,ENABLE);
	TIMEDWHILE((!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT)),statOk,5e3);
	I2C_Send7bitAddress(I2C2,addr,I2C_Direction_Transmitter);
	TIMEDWHILE((!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)),statOk,5e3);

	I2C_SendData(I2C2,reg); //Set MSB for reg. increment
	TIMEDWHILE((!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING)),statOk,50e3);

	while(len--){
		I2C_SendData(I2C2,*buf++);
		TIMEDWHILE(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING),statOk,5e3);
	}
	TIMEDWHILE(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED),statOk,5e3);
	I2C_GenerateSTOP(I2C2,ENABLE);
	return statOk;
}

u8 I2C_read(u8 addr,u8 reg,u8 *buf,u8 len){
	u8 statOk=!0;

	I2C_AcknowledgeConfig(I2C2, ENABLE);
	I2C_GenerateSTART(I2C2,ENABLE);
	TIMEDWHILE((!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT)),statOk,1e3);
	I2C_Send7bitAddress(I2C2,addr,I2C_Direction_Transmitter);
	TIMEDWHILE((!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)),statOk,1e3);

	I2C_SendData(I2C2,reg); //OUT_X_L_G + MSB
	TIMEDWHILE(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED),statOk,1e3);

	I2C_GenerateSTART(I2C2,ENABLE);
	TIMEDWHILE((!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT)),statOk,1e3);

	I2C_Send7bitAddress(I2C2,addr,I2C_Direction_Receiver);
	TIMEDWHILE((!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)),statOk,50e3);
	len--;
	while(len--){
		TIMEDWHILE((!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED)),statOk,1e3);
		*buf++=I2C_ReceiveData(I2C2);
	}
	I2C_AcknowledgeConfig(I2C2, DISABLE);
	I2C_GenerateSTOP(I2C2, ENABLE);
	TIMEDWHILE((!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED)),statOk,1e3);
	*buf++=I2C_ReceiveData(I2C2);
	return statOk;
}

char *u16toa(u16 n, char *s, u8 b, u8 ra) {

	static char digits[] = "0123456789ABCDEF";
	char tmp[6]; //7for int
	// #65535
	int k,d,i=0;
	do {
		tmp[i++] = digits[n % b];
	} while ((n /= b) > 0);
	i--;
	k=(ra==1)?(5-i):0;
	for(d=0;d<=i;d++){
		s[d+k]=tmp[i-d];
	}
	k=(ra==1)?0:d;
	d=6-d;
	while(d--)
		s[d+k]=' ';
	return s;
}

char *i16toa(int16_t n, char *s, u8 b, u8 ra) {

	if(n<0){
		s[0]='-';
		u16toa((u16)(-n),&s[1],b,ra);
	}
	else{
		s[0]=' ';
		u16toa((u16)(n),&s[1],b,ra);
	}
	return s;
}


void delay_us(u32 delay){
	volatile u32 start=toc;
	while((toc-start)<delay);
}



