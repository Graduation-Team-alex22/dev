#include "temp_sensor.h"
#include "../hsi_library/stm32f4xx_i2c.h"
#include "../port/port.h"
#include "ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#include "stdio.h"
#include "string.h"

static uint8_t lower_byte = 0;
static uint8_t upper_byte = 0;
static float temperature_value;
static char buff[8];


static void I2C1_Init(void);
static void I2C1_GPIO_Init(void);
static float TEMP_SENSOR_Calculate_Value(void);



void TEMP_SENSOR_Init(void)
{
	// Init GPIO of I2C1 
	I2C1_GPIO_Init();
	// Init I2C1 with the specified parameters
	I2C1_Init();
}

uint32_t TEMP_SENSOR_update(void)
{
	I2C1_Init();
	// Genetrate I2C start sequence 
	I2C_GenerateSTART(I2C1, ENABLE);
	// Wait for generating the start and take the bus
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	// Send slave address and select master transmitter mode
	I2C_Send7bitAddress(I2C1, TEMP_SENSOR_ADDRESS,  I2C_Direction_Transmitter);
	// Wait for slave to be acknowledged
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	// Send the register address to get the data from it
	I2C_SendData(I2C1, TEMP_REG_ADDRESS);
	// Wait until data has been written in the data register and is shifted out on the bus
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	// Generate repeated start
	I2C_GenerateSTART(I2C1, ENABLE); 
	// Wait for generating the repeated start and take the bus
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	// Send slave address and select master reciever mode
	I2C_Send7bitAddress(I2C1, TEMP_SENSOR_ADDRESS,  I2C_Direction_Receiver);
	// Wait for slave to be acknowledged
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	// Wait till recieving the first byte from the bus
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	upper_byte = I2C_ReceiveData(I2C1);
	// Disable ACK of received data as we recieve only 2 bytes
	I2C_AcknowledgeConfig(I2C1, DISABLE); 
	// Wait till recieving the second byte from the bus
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	lower_byte = I2C_ReceiveData(I2C1);
	// Stop the communication 
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	// Deinitialize the I2C1 peripheral registers to their default reset values
	I2C_DeInit(I2C1);
	
	/*Calculate temperature value and print it using UART*/
	temperature_value = TEMP_SENSOR_Calculate_Value();
	sprintf(buff,"%f", temperature_value);
	UART2_BUF_O_Write_String_To_Buffer("TEMP SENSOR : ");
	UART2_BUF_O_Write_String_To_Buffer(buff);
	UART2_BUF_O_Write_String_To_Buffer("\n");
	 
	return RETURN_NORMAL_STATE;
}


static float TEMP_SENSOR_Calculate_Value(void)
{
	float ret = 0;
		
	upper_byte = upper_byte & 0x1F; //Clear flag bits
	
	if ((upper_byte & 0x10) == 0x10) //TA < 0°C
	{
		upper_byte = upper_byte & 0x0F; //Clear SIGN
		ret = 256 - (upper_byte * 16 + (float)lower_byte / 16);
	}
	else //TA ³ 0°C
	{
		ret = (upper_byte * 16 + (float)lower_byte / 16); //Temperature = Ambient Temperature (°C)
	}
	
	return ret;
}

float TEMP_SENSOR_Get_Value(void)
{
	return temperature_value; 
}

static void I2C1_Init(void)
{
	I2C_InitTypeDef I2C_InitStruct;
	
  // I2C1 clock enable 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	//I2C1 configuration
  I2C_InitStruct.I2C_ClockSpeed = 100000;
  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStruct.I2C_OwnAddress1 = 0;
  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1, &I2C_InitStruct);
	
	// Enable I2C1
  I2C_Cmd(I2C1, ENABLE);	
}

static void I2C1_GPIO_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// GPIOB clock enable 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	// GPIO config
	GPIO_InitStruct.GPIO_Pin   = I2C1_SCL_PIN | GPIO_Pin_7; // I2C1_SDA_PIN : GPIO_Pin_7
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOB, &GPIO_InitStruct);  //I2C1_PORT : GPIOB

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
}
