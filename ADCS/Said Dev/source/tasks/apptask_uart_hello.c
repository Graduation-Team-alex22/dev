#include "../main/main.h"
#include "apptask_uart_hello.h"
//#include "../adcs/services_utilities/uart_dma_rx.h"
#include "../adcs/adcs_sensors/imu_sensor.h"
#include "stdio.h"

#define SW_PRESSED (0)

//#define 	TEST_TIME
#define			TEST_IMU


char buf[100];

void uart_hello_Init(void){
#ifdef TEST_TIME
	UART2_BUF_O_Write_String_To_Buffer("\n***RTC test INIT***\n");
	UART2_BUF_O_Send_All_Data();
	// now this will be used for testing
	uint8_t error_code = 1;
	error_code = time_init();
	if(error_code) 
		PROCESSOR_Perform_Safe_Shutdown(error_code);
#endif
	
#ifdef TEST_IMU
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
	
	// I2C1 Init
	I2C_InitTypeDef I2C_InitStruct;
	
	// I2C1 clock enable 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	//I2C1 configuration
  I2C_InitStruct.I2C_ClockSpeed = IMU_I2C_CLOCK;
  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStruct.I2C_OwnAddress1 = 0;
  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	
	I2C_Init(I2C1, &I2C_InitStruct);
	
	// Enable I2C1
  I2C_Cmd(I2C1, ENABLE);
	
	/******* initializing IMU Sensor *******/
	uint8_t error_code = IMU_Init(I2C1);
	if(error_code)
	{
		PROCESSOR_Perform_Safe_Shutdown(error_code);
	}
	
	// print calibration values
	imu_sensor_t t = IMU_Get_Data();
	char buf[100];
	sprintf(buf, "Calib: %+.3f %+.3f %+.3f \n",
								 t.M_Calib[0], t.M_Calib[1], t.M_Calib[2]);
	UART2_BUF_O_Write_String_To_Buffer(buf);
	UART2_BUF_O_Update();
	
#endif
	
}

uint32_t uart_hello_Update(void){
#ifdef TEST_TIME
	time_update();
	time_keeping_adcs_t time = time_getTime();
	
	UART2_BUF_O_Write_String_To_Buffer("\nRTC test: ");
	char temp[PRINT_TIME_STR_LENGTH] = {'X'};
	print_time(temp);
	UART2_BUF_O_Write_String_To_Buffer(temp);
#endif
	
#ifdef TEST_IMU
	uint8_t error_code = IMU_Update(I2C1);
	if(error_code)
	{
		PROCESSOR_Perform_Safe_Shutdown(error_code);
	}
	
	imu_sensor_t t = IMU_Get_Data();
	
	sprintf(buf, "A: %+.3f %+.3f %+.3f  G: %+.3f %+.3f %+.3f  M: %+.3f %+.3f %+.3f\n",
								t.Ax, t.Ay, t.Az, t.Gx, t.Gy, t.Gz, t.Mx, t.My, t.Mz);
	UART2_BUF_O_Write_String_To_Buffer(buf);
#endif

	// send it away
	return UART2_BUF_O_Update();
}
