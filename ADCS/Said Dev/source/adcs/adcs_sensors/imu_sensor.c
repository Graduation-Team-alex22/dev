#include "imu_sensor.h"
#include "../support_functions/ttrd2-05a-t0401a-v001a_timeout_t3.h"

#define ERROR_CODE_TIMEOUT 								1
#define ERROR_CODE_DEVICE_NOT_CONNECTED		2
#define ERROR_CODE_BAD_WHOIAM							3
#define	ERROR_CODE_START_FAIL							4
#define	ERROR_CODE_SEND_FAIL							5
#define	ERROR_CODE_RECV_FAIL							6
#define ERROR_CODE_COMM_FAIL							7

#define	TIMEOUT			1
#define OK 					0
#define MAX_WAIT		500			// microseconds

#define IMU_MAG_PD_WAIT			200			// microseconds

/**************** PRIVATE STRUCT *******************/
static struct	{
	int t;
	
} measure_param;


/***************** PRIVATE VAR *********************/
static imu_sensor_t imu_sensor_data;

/***************** PRIVATE FUNC *********************/
uint8_t Is_IMU_Connected(I2C_TypeDef* I2Cx);
uint8_t	IMU_Wakeup(I2C_TypeDef* I2Cx);
uint8_t	IMU_Configure(I2C_TypeDef* I2Cx);
uint8_t IMU_Mag_Calib(I2C_TypeDef* I2Cx, float* calib);
static inline uint8_t wait_for_event(I2C_TypeDef* I2Cx, uint32_t event, uint32_t timeout );

uint8_t I2Cx_Send_Bytes(I2C_TypeDef* I2Cx, uint8_t device_add, uint8_t reg_add, uint8_t* pData, uint8_t size);
uint8_t I2Cx_Recv_Bytes(I2C_TypeDef* I2Cx, uint8_t device_add, uint8_t reg_add, uint8_t* pData, uint8_t size);


/*----------------------------------------------------------------------------*-

  IMU_Init(I2C_TypeDef* I2Cx)

  Initializes IMU sernsor.
   
  PARAMETERS:
     I2C peripheral to be used in communication.
   
  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     one I2C peripheral.

  PRE-CONDITION CHECKS:
     Checks if the device is connected.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.     

  RETURN VALUE:
     Error code.

-*----------------------------------------------------------------------------*/
uint8_t IMU_Init(I2C_TypeDef* I2Cx)
{
	uint8_t error_code;
	
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	// Set up timeout mechanism
  TIMEOUT_T3_USEC_Init();
	
	// check if the device is connected
	error_code = Is_IMU_Connected(I2Cx);
	if(error_code){ return error_code+100;}

	// reset the device, turn off sleep mode, set clock source
	error_code = IMU_Wakeup(I2Cx);
	if(error_code){ return error_code+120;}
	
	// configuration
	error_code = IMU_Configure(I2Cx);
	if(error_code){ return error_code+160;}
	
	// calibration and multipliers
	imu_sensor_data.AMult = 2.0f / 32768.0f;
	imu_sensor_data.GMult = 250.0f / 32768.0f;
	imu_sensor_data.MMult = 10.0f * 4912.0f / 32768.0f;
	error_code = IMU_Mag_Calib(I2Cx, imu_sensor_data.M_Calib);
	if(error_code){ return error_code+140;}
	
	
	//I2C_GenerateSTOP(I2Cx, ENABLE);
	
	return 0;
	
}


uint8_t Is_IMU_Connected(I2C_TypeDef* I2Cx)
{
	uint8_t error_code;
	uint8_t data[1];
	
	/************ check if the device is connected ************/
	error_code = I2Cx_Recv_Bytes(I2Cx, IMU_I2C_ADD, IMU_WHOIAM_ADD, data, 1);
	if(error_code){ return error_code;}
	
	// check for right WHO_I_AM value
	if(data[0] != IMU_WHOIAM_VAL)
	{
		return ERROR_CODE_BAD_WHOIAM;
	}

	return 0;
}


uint8_t	IMU_Wakeup(I2C_TypeDef* I2Cx)
{
	uint8_t error_code;
	uint8_t data[1];
	
	/************ reset the device ************/
	data[0] = 0x80;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_I2C_ADD, IMU_PWR_MGMT_1, data, 1);
	if(error_code){return error_code;}

	/************ clear sleep mode ************/
	data[0] = 0x00;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_I2C_ADD, IMU_PWR_MGMT_1, data, 1);
	if(error_code){return error_code;}
	
	/************ choose clock source ************/
	data[0] = 0x01;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_I2C_ADD, IMU_PWR_MGMT_1, data, 1);
	if(error_code){return error_code;}
	
	/************  ************/
	data[0] = 0x03;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_I2C_ADD, IMU_PWR_MGMT_1, data, 1);
	if(error_code){return error_code;}
	
	/************ e ************/
	data[0] = 0x01;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_I2C_ADD, IMU_PWR_MGMT_1, data, 1);
	if(error_code){return error_code;}
	
	/************ e ************/
	data[0] = 0x01;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_I2C_ADD, IMU_PWR_MGMT_1, data, 1);
	if(error_code){return error_code;}
	
	
	return 0;
}


uint8_t	IMU_Configure(I2C_TypeDef* I2Cx)
{
	uint8_t error_code;
	uint8_t data;
	
	/******************* MPU9255 Config ******************/
	// enable I2C bypass
	data = 0x02;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_I2C_ADD, IMU_INT_PIN_CFG, &data, 1);
	if(error_code){return error_code;}
	
	return 0;
}

uint8_t IMU_Mag_Calib(I2C_TypeDef* I2Cx, float* calib)
{
	uint8_t error_code;
	uint8_t data[3];
	
	/******** put magnometer in power down mode **********/
	data[0] = 0x00;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_AKM_ADD, IMU_MAG_CTRL, data, 1);
	if(error_code){return error_code;}
	
	/******** wait for IMU_MAG_PD_WAIT **********/
	TIMEOUT_T3_USEC_Start();
	while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(IMU_MAG_PD_WAIT));
	
	/******** put magnometer in FUSE ROM Access **********/
	data[0] = 0x0F;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_AKM_ADD, IMU_MAG_CTRL, data, 1);
	if(error_code){return error_code;}
	
	/******** wait for IMU_MAG_PD_WAIT **********/
	TIMEOUT_T3_USEC_Start();
	while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(IMU_MAG_PD_WAIT));
	
	/******** read calibration values **********/
	error_code = I2Cx_Recv_Bytes(I2Cx, IMU_AKM_ADD, IMU_MAG_ASAX, data, 3);
	if(error_code){ return error_code;}
	
	calib[0] = (float)(data[0] - 128)/256.0 + 1.0;
	calib[1] = (float)(data[1] - 128)/256.0 + 1.0;
	calib[2] = (float)(data[2] - 128)/256.0 + 1.0;
	
	/******** put magnometer in power down mode **********/
	data[0] = 0x00;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_AKM_ADD, IMU_MAG_CTRL, data, 1);
	if(error_code){return error_code;}
	
	/******** wait for IMU_MAG_PD_WAIT **********/
	TIMEOUT_T3_USEC_Start();
	while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(IMU_MAG_PD_WAIT));
	
	/******** put magnometer in Continuous mode **********/
	data[0] = IMU_MAG_CONT1 | IMU_MAG_16BIT;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_AKM_ADD, IMU_MAG_CTRL, data, 1);
	if(error_code){return error_code;}
	
	/******** wait for IMU_MAG_PD_WAIT **********/
	TIMEOUT_T3_USEC_Start();
	while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(IMU_MAG_PD_WAIT));
	
	return 0;
}



uint8_t IMU_Update(I2C_TypeDef* I2Cx)
{
	uint8_t error_code;
	uint8_t data[8];
	
	/**************** Read Accel ****************/
	error_code = I2Cx_Recv_Bytes(I2Cx, IMU_I2C_ADD, IMU_ACCEL_DATA, data, 6);
	if(error_code){ return error_code;}
	
	imu_sensor_data.Ax_Raw = ((int16_t) data[0] << 8) | data[1];
	imu_sensor_data.Ax = (float)imu_sensor_data.Ax_Raw * imu_sensor_data.AMult;
	imu_sensor_data.Ay_Raw = ((int16_t) data[2] << 8) | data[3];
	imu_sensor_data.Ay = (float)imu_sensor_data.Ay_Raw * imu_sensor_data.AMult;
	imu_sensor_data.Az_Raw = ((int16_t) data[4] << 8) | data[5];
	imu_sensor_data.Az = (float)imu_sensor_data.Az_Raw * imu_sensor_data.AMult;
	
	/***************** Read Gyro ****************/
	error_code = I2Cx_Recv_Bytes(I2Cx, IMU_I2C_ADD, IMU_GYRO_DATA, data, 6);
	if(error_code){ return error_code;}
	
	imu_sensor_data.Gx_Raw = ((int16_t) data[0] << 8) | data[1];
	imu_sensor_data.Gx = (float)imu_sensor_data.Gx_Raw * imu_sensor_data.GMult;
	imu_sensor_data.Gy_Raw = ((int16_t) data[2] << 8) | data[3];
	imu_sensor_data.Gy = (float)imu_sensor_data.Gy_Raw * imu_sensor_data.GMult;
	imu_sensor_data.Gz_Raw = ((int16_t) data[4] << 8) | data[5];
	imu_sensor_data.Gz = (float)imu_sensor_data.Gz_Raw * imu_sensor_data.GMult;
	
	/***************** Read Mag *****************/
	imu_sensor_data.Mx =0;
	imu_sensor_data.My =0;
	imu_sensor_data.Mz =0;

	error_code = I2Cx_Recv_Bytes(I2Cx, IMU_AKM_ADD, IMU_MAG_ST1, data, 1);
	if(error_code){ return error_code;}
	
	// check on data ready flag
	if(data[0] & 0x01)
	{
		error_code = I2Cx_Recv_Bytes(I2Cx, IMU_AKM_ADD, IMU_MAG_DATA, data, 7);
		if(error_code){ return error_code;}
		
		// check for magnetic overflow
		if((data[6] & 0x08) == 0x00)
		{
			imu_sensor_data.Mx_Raw = ((int16_t) data[1] << 8) | data[0];
			imu_sensor_data.Mx = (float)imu_sensor_data.Mx_Raw * imu_sensor_data.MMult * imu_sensor_data.M_Calib[0];
			imu_sensor_data.My_Raw = ((int16_t) data[3] << 8) | data[2];
			imu_sensor_data.My = (float)imu_sensor_data.My_Raw * imu_sensor_data.MMult * imu_sensor_data.M_Calib[1];
			imu_sensor_data.Mz_Raw = ((int16_t) data[5] << 8) | data[4];
			imu_sensor_data.Mz = (float)imu_sensor_data.Mz_Raw * imu_sensor_data.MMult * imu_sensor_data.M_Calib[2];
		}
	}
	
	return 0;
}


imu_sensor_t IMU_Get_Data(void){
	return imu_sensor_data;
}


static inline uint8_t wait_for_event(I2C_TypeDef* I2Cx, uint32_t event, uint32_t timeout )
{
	uint32_t T3;
	
	TIMEOUT_T3_USEC_Start();
	while (!I2C_CheckEvent(I2Cx, event) && 
		     (COUNTING == (T3 = TIMEOUT_T3_USEC_Get_Timer_State(timeout))));
	if (T3 == TIMED_OUT) return TIMEOUT;
	return OK;
}



uint8_t I2Cx_Send_Bytes(I2C_TypeDef* I2Cx, uint8_t device_add, uint8_t reg_add, uint8_t* pData, uint8_t size)
{
	
	// generate start
	I2C_GenerateSTART(I2Cx, ENABLE); 
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_TIMEOUT;}
	
	// Send slave address and select master transmitter mode 
	I2C_Send7bitAddress(I2Cx, device_add, I2C_Direction_Transmitter);
	// Wait for slave to be acknowledged
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_DEVICE_NOT_CONNECTED;}
	
	// sending register address
	I2C_SendData(I2Cx, reg_add);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_SEND_FAIL;}
	
	for(int i = 0; i < size; i++)
	{	
		I2C_SendData(I2Cx, pData[i]);
		if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, MAX_WAIT) == TIMEOUT)
		{return ERROR_CODE_SEND_FAIL;}
	}
	
	I2C_GenerateSTOP(I2Cx, ENABLE);
	TIMEOUT_T3_USEC_Start();
	while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(20));
	
	
	return 0;
}


uint8_t I2Cx_Recv_Bytes(I2C_TypeDef* I2Cx, uint8_t device_add, uint8_t reg_add, uint8_t* pData, uint8_t size)
{
	if(size == 0) return 0;
	
	// Generate start
	I2C_GenerateSTART(I2Cx, ENABLE); 
	// Wait for generating the start and take the bus
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_TIMEOUT;}
	
	// Send slave address and select master transmitter mode 
	I2C_Send7bitAddress(I2Cx, device_add, I2C_Direction_Transmitter);
	// Wait for slave to be acknowledged
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_DEVICE_NOT_CONNECTED;}
	
	// send register address
	I2C_SendData(I2Cx, reg_add);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_SEND_FAIL;}
	
	// generate restart
	I2C_GenerateSTART(I2Cx, ENABLE);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_TIMEOUT;}
	
	if(size == 1)
	{
		// disable ack to receive one byte
		I2C_AcknowledgeConfig(I2Cx, DISABLE);
		// Send slave address and select master receiver mode 
		I2C_Send7bitAddress(I2Cx, device_add, I2C_Direction_Receiver);
		
		// wait for a byte to be recieved
		if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
		{return ERROR_CODE_RECV_FAIL;}
		
		pData[0] = I2C_ReceiveData(I2Cx);
	}else{
		// Send slave address and select master receiver mode 
		I2C_Send7bitAddress(I2Cx, device_add, I2C_Direction_Receiver);
		
		for(int i = 0; i< size; i++)
		{
			// wait for a byte to be recieved
			if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
			{return ERROR_CODE_RECV_FAIL;}
			
			if(i == size-2) I2C_AcknowledgeConfig(I2Cx, DISABLE);
			
			pData[i] = I2C_ReceiveData(I2Cx);
		}
	}
	
	// generate stop after receiving one byte
	I2C_GenerateSTOP(I2Cx, ENABLE);
	// re-enable acknoledge
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	
	return 0;
}
