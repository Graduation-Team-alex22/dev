/*-------------------------------------------------------------------------------*-
  Temperature sensor module
	This module use I2C1 peripheral to read temperature value from temperature sensor
	
-*--------------------------------------------------------------------------------*/
#include "temp_sensor.h"
#include "../port/port.h"
#include "ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#include "stdio.h"



// ------ Private Identifiers --------------------------------------------------
// Device I2C Address
#define TEMP_SENSOR_ADDRESS		(0x18<<1)
// temperature value register
#define TEMP_REG_ADDRESS			(0x05)

#define I2C_EVENT_WAIT				200			// microseconds

// ------ Private variables --------------------------------------------------
static float temperature_value;
static char buff[8];
static uint8_t data[2];

// ------ Private function prototypes ----------------------------------------
static void I2Cx_Init(I2C_TypeDef* I2Cx);
static void I2C1_GPIO_Init(void);
static float TEMP_SENSOR_Calculate_Value(void);


/*----------------------------------------------------------------------------*-

  TEMP_SENSOR_Init()

  Initialisation function for Temperature sensor module.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None 
   
  MCU HARDWARE:
     I2C1 pins.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void TEMP_SENSOR_Init(I2C_TypeDef* I2Cx)
{
	// Init GPIO of I2C1 
	I2C1_GPIO_Init();
	// Init I2C1 with the specified parameters
	I2Cx_Init(I2Cx);
}

/*----------------------------------------------------------------------------*-

  TEMP_SENSOR_update()

  Temperature reading task.
   
  PARAMETERS:
     None.

  LONG-TERM DATA:
     temperature_value  
   
  MCU HARDWARE:
     I2C1 pins.

  PRE-CONDITION CHECKS:
     Data integrity checks. --> Not implemented yet
     Register configuration checks. --> Not implemented yet

     If any PreC check fails, we force a shutdown
     [other behaviour may be more appropriate in your system.]

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING: --> Not implemented yet
     See PreCs and PostCs.

  WCET:
     Not yet determined.

  BCET:
     Not yet determined.

  RETURN VALUE:
     RETURN_NORMAL_STATE (fixed).

-*----------------------------------------------------------------------------*/
uint32_t TEMP_SENSOR_update(void)
{
	// Check pre-conditions (START)
	
	// Check pre-conditions (END)
	
	uint8_t data[2];
	uint8_t error_code;
	
	error_code = I2Cx_Recv_Bytes(I2C1, TEMP_SENSOR_ADDRESS, TEMP_REG_ADDRESS, data, 2, I2C_EVENT_WAIT);
	if(error_code)
	{
		PROCESSOR_Perform_Safe_Shutdown(error_code);
	}
	// Calculate temperature based on updated data
	temperature_value = TEMP_SENSOR_Calculate_Value();
	
	// Check post-conditions (START)
	
  // Check post-conditions (END)
	
	
	sprintf(buff,"%f", temperature_value);
	UART2_BUF_O_Write_String_To_Buffer("TEMP SENSOR : ");
	UART2_BUF_O_Write_String_To_Buffer(buff);
	UART2_BUF_O_Write_String_To_Buffer("\n");
	 
	return RETURN_NORMAL_STATE;
}
/*----------------------------------------------------------------------------*-

  TEMP_SENSOR_Calculate_Value()

  Calculate current temperature sensor value.
   
  PARAMETERS:
     None.
     
  LONG-TERM DATA:
     None.
   
  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     Temperature value.

-*----------------------------------------------------------------------------*/
static float TEMP_SENSOR_Calculate_Value(void)
{
	float ret = 0;
		
	data[0] = data[0] & 0x1F; //Clear flag bits
	
	if ((data[0] & 0x10) == 0x10) //TA < 0°C
	{
		data[0] = data[0] & 0x0F; //Clear SIGN
		ret = 256 - (data[0] * 16 + (float)data[1] / 16);
	}
	else //TA ³ 0°C
	{
		ret = (data[0] * 16 + (float)data[1] / 16); //Temperature = Ambient Temperature (°C)
	}
	
	return ret;
}

/*----------------------------------------------------------------------------*-
  TEMP_SENSOR_Get_Value()

  Return current temperature sensor value.
   
  PARAMETERS:
     None.
     
  LONG-TERM DATA:
     None.
   
  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     Temperature value.

-*----------------------------------------------------------------------------*/
float TEMP_SENSOR_Get_Value(void)
{
	return temperature_value; 
}
/*----------------------------------------------------------------------------*-

  I2Cx_Init(I2C_TypeDef* I2Cx)

  Set up I2Cx configurations.

  PARAMETER:
     I2Cx.

  LONG-TERM DATA:
     Timeout_us_g (W) --> Not implemented yet
     Timeout_us_ig (W) --> Not implemented yet

  MCU HARDWARE:
     I2C1.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
static void I2Cx_Init(I2C_TypeDef* I2Cx)
{
	I2C_InitTypeDef I2C_InitStruct;
	
	// I2Cx clock enable 
	if(I2Cx == I2C1)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	else if(I2Cx == I2C2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	else if(I2Cx == I2C3)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
	
	//I2Cx configuration
  I2C_InitStruct.I2C_ClockSpeed = TEMP_I2C_CLOCK;
  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStruct.I2C_OwnAddress1 = 0;
  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2Cx, &I2C_InitStruct);
	
	
	// Enable I2C1
	if(I2Cx == I2C1)
		I2C_Cmd(I2C1, ENABLE);	
	else if(I2Cx == I2C2)
		I2C_Cmd(I2C2, ENABLE);	
	else if(I2Cx == I2C3)
		I2C_Cmd(I2C3, ENABLE);		
}
/*----------------------------------------------------------------------------*-

  I2C1_GPIO_Init()

  Set up I2C1 GPIO pins.

  PARAMETER:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     I2C1 GPIO pins.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
static void I2C1_GPIO_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// GPIOB clock enable 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	// GPIO config
	GPIO_InitStruct.GPIO_Pin   = I2C1_SCL_PIN | I2C1_SDA_PIN; 
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(I2C1_PORT, &GPIO_InitStruct);  

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
}
