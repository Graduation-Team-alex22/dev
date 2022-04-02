#include "stdint.h"
#include "../port/port.h"
#include "../adcs/adcs_sensors/mgn_sensor.h"

#include "app_sensor_mgn_task.h" 

/*----------------------------------------------------------------------------*-

  App_Sensor_Mgn_Init()

  Set up magnomter sensor.
 
  The magnometer sensor acts as a digital compass providing magnetic
  feild reading to the controller.

  Datasheet values: 
  Power on reset time (Maximum): 350 uS
  I2C ready time (Maximum) : 200 uS
  Measurement ready time (Maximum : 50 ms
  
  PARAMETERS:
     None. 
   
  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     I2C1 peripheral.
     Port B pin 8 (SCL).
     Port B pin 9 (SDA).

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void App_Sensor_Mgn_Init(void)
{	
/******* initializing I2C1 peripheral: SCL @ PB8, SDA @ PB9 *******/
   // GPIO Init
   GPIO_InitTypeDef GPIO_InitStruct;

   // GPIOB clock enable 
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

   // GPIO config
   GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;
   GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
   GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
   GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

   GPIO_Init(GPIOB, &GPIO_InitStruct);

   GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

	 REG_CONFIG_CHECKS_GPIO_Store();
	
   // I2C1 Init
   I2C_InitTypeDef I2C_InitStruct;

   // I2C1 clock enable 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

   //I2C1 configuration
   I2C_InitStruct.I2C_ClockSpeed = MGN_I2C_CLOCK;
   I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
   I2C_InitStruct.I2C_OwnAddress1 = 0;
   I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
   I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
   I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

   I2C_Init(I2C1, &I2C_InitStruct);

   // Enable I2C1
   I2C_Cmd(I2C1, ENABLE);

   //-- initializing MGN Sensor -----------
   mgn_init_t mgn_init;
   mgn_init.over_sampling = MGN_OVER_SAMPLING_512;
   mgn_init.op_mode = MGN_OP_MODE_CONT;
   mgn_init.output_rate = MGN_OUTPUT_RATE_10;
   mgn_init.range = MGN_RANGE_2;

   uint8_t error_code = MGN_Sensor_Init(I2C1, &mgn_init);
   if(error_code)
   {
      PROCESSOR_Perform_Safe_Shutdown(error_code);
   }
}


/*----------------------------------------------------------------------------*-

  App_Sensor_Mgn_Update()

  Update magnomter sensor readings.
 
  The magnometer sensor acts as a digital compass providing magnetic
  feild reading to the controller.

  Datasheet values: 
  Power on reset time (Maximum): 350 uS

  PARAMETERS:
     None. 
   
  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     I2C1 peripheral.
     Port B pin 8 (SCL).
     Port B pin 9 (SDA).

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
uint32_t App_Sensor_Mgn_Update(void)
{
	uint8_t error_code = MGN_Sensor_Update();
	if(error_code)
	{
		PROCESSOR_Perform_Safe_Shutdown(error_code);
	}
   
	return RETURN_NORMAL_STATE;
}
