#include "../main/main.h"
#include "apptask_uart_hello.h"
#include "stdio.h"

//#define 	TEST_TIME
//#define			TEST_IMU
//#define			TEST_MGN
//#define			I2C_SCANNER
#define    TEST_GPS

#ifdef TEST_IMU
	#include "../adcs/adcs_sensors/imu_sensor.h"
#endif

#ifdef TEST_MGN
	#include "../adcs/adcs_sensors/mgn_sensor.h"
#endif

#ifdef TEST_GPS
	#include "../adcs/adcs_sensors/gps_sensor.h"
	
#endif

#define SW_PRESSED (0)

char buf[96];

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
	
#ifdef TEST_MGN
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
  I2C_InitStruct.I2C_ClockSpeed = MGN_I2C_CLOCK;
  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStruct.I2C_OwnAddress1 = 0;
  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	
	I2C_Init(I2C1, &I2C_InitStruct);
	
	// Enable I2C1
  I2C_Cmd(I2C1, ENABLE);
	
	/******* initializing MGN Sensor *******/
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
	
#endif

#ifdef I2C_SCANNER
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
  I2C_InitStruct.I2C_ClockSpeed = MGN_I2C_CLOCK;
  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStruct.I2C_OwnAddress1 = 0;
  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	
	I2C_Init(I2C1, &I2C_InitStruct);
	
	// Enable I2C1
  I2C_Cmd(I2C1, ENABLE);
	
	/******** Start Scanning********/
	TIMEOUT_T3_USEC_Init();
	TIMEOUT_T3_USEC_Start();
	while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(50000));
	
	
	UART2_BUF_O_Write_String_To_Buffer("I2C Scanner:\n");
	UART2_BUF_O_Send_All_Data();

#endif

#ifdef TEST_GPS

  //----------set up GPIO pin A1 as usart4 RX pin-----------
  GPIO_InitTypeDef GPIO_InitStructure;
  // GPIOB clock enable 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  // GPIO config
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1; 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);

  GPS_Sensor_Init(UART4, DMA1_Stream2, DMA_Channel_4);
	UART2_BUF_O_Write_String_To_Buffer("GPS TEST INIT\n");
	UART2_BUF_O_Send_All_Data();

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

#ifdef TEST_MGN
	uint8_t error_code = MGN_Sensor_Update();
	if(error_code)
	{
		PROCESSOR_Perform_Safe_Shutdown(error_code);
	}
	
	mgn_sensor_t t = MGN_Sensor_GetData();
	/*
	sprintf(buf, "X: %+.4f  Y: %+.4f  Z:%+.4f\n  %6d   %6d   %6d \n",
								t.mag[0], t.mag[1], t.mag[2], t.raw[0], t.raw[1], t.raw[2]);
	*/
	
	sprintf(buf, "X: %+.4f  Y: %+.4f  Z:%+.4f\n",
								t.mag[0], t.mag[1], t.mag[2]);
	
	UART2_BUF_O_Write_String_To_Buffer(buf);
	
#endif

#ifdef I2C_SCANNER
	uint8_t found_devices[5] = {0};
	I2Cx_Slave_Scanner(I2C1, found_devices, 5, 20);
	
	sprintf(buf, "Looking..\n");
	UART2_BUF_O_Write_String_To_Buffer(buf);
	
	for(int i = 0 ; i < 5 ; i++)
	{
		if(found_devices[i] == 0)
		{
			sprintf(buf, " --");
		}
		else
		{
			sprintf(buf, " %X", found_devices[i]);
		}
		
		UART2_BUF_O_Write_String_To_Buffer(buf);
	}
	
#endif
	
	
#ifdef TEST_GPS
	uint16_t sn = GPS_Sensor_Update();
	UART2_BUF_O_Write_String_To_Buffer("SN: ");
	UART2_BUF_O_Write_Number03_To_Buffer(sn);
	UART2_BUF_O_Write_Char_To_Buffer('\n');
	
	GPS_Sensor_GetData(buf);
	UART2_BUF_O_Write_String_To_Buffer(buf);
	GPS_Sensor_GetData(buf);
	UART2_BUF_O_Write_String_To_Buffer(buf);
	GPS_Sensor_GetData(buf);
	UART2_BUF_O_Write_String_To_Buffer(buf);
	
	
#endif
	// send it away
	//UART2_BUF_O_Send_All_Data();
	return 0;
}
