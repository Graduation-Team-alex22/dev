//#define STM32F40XX
//#define USE_STDPERIPH_DRIVER

/* Includes */
#include "hw_conf.h"
#include "stdio.h"
#include "string.h"

typedef struct {
	uint32_t 	flag;
	int32_t 	RPRr;
	uint32_t 	rampTime;
	uint32_t 	crc;
}I2C_pck;

I2C_pck tx_pck;
I2C_pck rx_pck;
uint32_t pckSize = 16;

/* Private macro */
/* Private variables */
volatile uint8_t UsbRxBuf[8192], UsbRxLen;
volatile int I2C_Error=0;
I2C_InitTypeDef  I2C_InitStructure;
/* Private function prototypes */
/* Private functions */
uint32_t I2C_SendPck(int32_t RPMr, uint32_t time, uint32_t timeout);
uint32_t I2C_RcvPck(uint32_t timeout);
/**
 **===========================================================================
 **
 **  Abstract: main program
 **
 **===========================================================================
 */

int main(void)
{

	//I2C1 SCL/PB6, SDA/PB7

	GPIO_InitTypeDef  GPIO_InitStructure;

	//LEDS
	// *************************************************
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	// *************************************************


	//USER BUTTON PA0
	// *************************************************
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// *************************************************

	// I2C
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7, GPIO_AF_I2C1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x01;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed=100e3;
	I2C_InitStructure.I2C_DutyCycle=I2C_DutyCycle_16_9;
	I2C_Init(I2C1, &I2C_InitStructure);


	I2C_StretchClockCmd(I2C1,ENABLE);
	//I2C_GeneralCallCmd(I2C1,ENABLE);
	I2C_Cmd(I2C1, ENABLE);


	initHw();

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);

	STM32F4_Discovery_LEDOn(LED4);


	delay_us(3000000);
	int len;
	char ss[1024];
	len=sprintf(ss,"Start:\n");
	cdc_DataTx(ss,len);


	while(1){

		if(GPIOA->IDR&0x01){
			I2C_SendPck(1000,0,10000);
			delay_us(100000);
			I2C_RcvPck(10000);

			len=sprintf(ss,"RPMm: %d,CRC: %u,Flag: %u\n",rx_pck.RPRr,rx_pck.crc,rx_pck.flag);
			cdc_DataTx(ss,len);
			// Check I2C_Error..
			delay_us(100000);
			STM32F4_Discovery_LEDOff(LED3);
		}

		if(UsbRxLen>0){
			if(UsbRxBuf[1]==UsbRxLen){
			}
			UsbRxLen=0; //req new pck
		}
	}
}


uint32_t I2C_SendPck(int32_t RPMr, uint32_t time, uint32_t timeout){
	int i;
	uint8_t  *ptr8;
	uint32_t *ptr32;

	uint32_t len;
	TIM_SetCompare1(TIM2,toc+timeout);
	I2C_Error=0;
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
	TIM_ITConfig(TIM2, TIM_IT_CC1,ENABLE);

	tx_pck.RPRr = RPMr;
	tx_pck.rampTime = time;

	ptr8 = (uint8_t *)&tx_pck;
	ptr32 = (uint32_t *)&tx_pck;

	CRC_ResetDR();
	CRC->DR = *ptr32++;
	CRC->DR = *ptr32++;
	CRC->DR = *ptr32++;
	tx_pck.crc = CRC->DR;

	STM32F4_Discovery_LEDOff(LED5); //Reset error LED
	STM32F4_Discovery_LEDOn(LED3);

	I2C_SoftwareResetCmd(I2C1,ENABLE);
	I2C_Cmd(I2C1, DISABLE);
	I2C_SoftwareResetCmd(I2C1,DISABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
	delay_us(100);

	I2C_CalculatePEC(I2C1,DISABLE);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_GenerateSTART(I2C1,ENABLE);

	while((!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)) && (!I2C_Error) );
	I2C_Send7bitAddress(I2C1,0x03,I2C_Direction_Transmitter);

	while((!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (!I2C_Error));

	for(i=0;i<16;i++){
		I2C_SendData(I2C1,*ptr8++); // Pck_Len
		while((!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING)) && (!I2C_Error));
	}
	STM32F4_Discovery_LEDOff(LED3);
	while((!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (!I2C_Error));
	I2C_GenerateSTOP(I2C1,ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_CC1,DISABLE);

	return 0;
}


uint32_t I2C_RcvPck(uint32_t timeout){
	int i;
	uint8_t  *ptr8;
	uint32_t *ptr32;

	uint32_t len;
	TIM_SetCompare1(TIM2,toc+timeout);
	I2C_Error=0;
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
	TIM_ITConfig(TIM2, TIM_IT_CC1,ENABLE);


	ptr8 = (uint8_t  *) &rx_pck;

	CRC_ResetDR();
	CRC->DR = *ptr32++;
	CRC->DR = *ptr32++;
	CRC->DR = *ptr32++;
	tx_pck.crc = CRC->DR;

	STM32F4_Discovery_LEDOff(LED5); //Reset error LED
	STM32F4_Discovery_LEDOn(LED3);

	I2C_SoftwareResetCmd(I2C1,ENABLE);
	I2C_Cmd(I2C1, DISABLE);
	I2C_SoftwareResetCmd(I2C1,DISABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
	delay_us(100);

	I2C_CalculatePEC(I2C1,DISABLE);

	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_GenerateSTART(I2C1,ENABLE);

	while((!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)) && (!I2C_Error) );
	I2C_Send7bitAddress(I2C1,0x03,I2C_Direction_Receiver);
	while((!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && (!I2C_Error));

	len = pckSize-1; // Do not send last one
	for(i=0;i<len;i++){
		while((!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)) && (!I2C_Error));
		*ptr8++ = I2C_ReceiveData(I2C1);
	}

	STM32F4_Discovery_LEDOff(LED3);
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	I2C_GenerateSTOP(I2C1,ENABLE);

	while((!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)) && (!I2C_Error));
	*ptr8++ = I2C_ReceiveData(I2C1);

	TIM_ITConfig(TIM2, TIM_IT_CC1,DISABLE);

	return 0;
}

void DISCOVERY_EXTI_IRQHandler(void){
}
