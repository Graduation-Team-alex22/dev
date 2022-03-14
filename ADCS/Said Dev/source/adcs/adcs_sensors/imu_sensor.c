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
#define MAX_WAIT		200			// microseconds


/***************** PRIVATE VAR *********************/
static imu_sensor_t imu_sensor_data;

/***************** PRIVATE FUNC *********************/
uint8_t Is_IMU_Connected(I2C_TypeDef* I2Cx);
uint8_t	IMU_Wakeup(I2C_TypeDef* I2Cx);
static inline uint8_t wait_for_event(I2C_TypeDef* I2Cx, uint32_t event, uint32_t timeout );

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
	if(error_code){ return error_code+200;}
	
	// calibration
	
	// configuration
	
	
	//I2C_GenerateSTOP(I2Cx, ENABLE);
	
	return 0;
	
}


uint8_t Is_IMU_Connected(I2C_TypeDef* I2Cx)
{
	/************ check if the device is connected ************/
	
	// Generate start
	I2C_GenerateSTART(I2Cx, ENABLE); 
	// Wait for generating the start and take the bus
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_TIMEOUT;}
	
	
	// Send slave address and select master transmitter mode 
	I2C_Send7bitAddress(I2Cx, IMU_I2C_ADD, I2C_Direction_Transmitter);
	// Wait for slave to be acknowledged
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_DEVICE_NOT_CONNECTED;}
	
	/***********************************************************/
	
	/***************** Verify device WHO_I_AM ******************/
	// send register address
	I2C_SendData(I2Cx, IMU_WHOIAM_ADD);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_SEND_FAIL;}
	
	// generate restart
	I2C_GenerateSTART(I2Cx, ENABLE);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_TIMEOUT;}
	
	// disable ack to receive one byte
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	
	// Send slave address and select master receiver mode 
	I2C_Send7bitAddress(I2Cx, IMU_I2C_ADD, I2C_Direction_Receiver);
	
	// wait for a byte to be recieved
	TIMEOUT_T3_USEC_Start();
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	uint8_t rcv_byte = I2C_ReceiveData(I2Cx);
	
	// generate stop after receiving one byte
	I2C_GenerateSTOP(I2Cx, ENABLE);
	// re-enable acknoledge
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	
	// check for right WHO_I_AM value
	if(rcv_byte != IMU_WHOIAM_VAL)
	{
		return ERROR_CODE_BAD_WHOIAM;
	}

	return 0;
}


uint8_t	IMU_Wakeup(I2C_TypeDef* I2Cx)
{
	/************ reset the device ************/
	
	// generate restart
	I2C_GenerateSTART(I2Cx, ENABLE); 
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_TIMEOUT;}
	
	// Send slave address and select master transmitter mode 
	I2C_Send7bitAddress(I2Cx, IMU_I2C_ADD, I2C_Direction_Transmitter);
	// Wait for slave to be acknowledged
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_DEVICE_NOT_CONNECTED;}
	
	I2C_SendData(I2Cx, IMU_PWR_MGMT_1);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_SEND_FAIL;}
	
	I2C_SendData(I2Cx, 0x80);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_SEND_FAIL;}

	/************ clear sleep mode ************/
	// generate restart
	I2C_GenerateSTART(I2Cx, ENABLE); 
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_TIMEOUT;}
	
	// Send slave address and select master transmitter mode 
	I2C_Send7bitAddress(I2Cx, IMU_I2C_ADD, I2C_Direction_Transmitter);
	// Wait for slave to be acknowledged
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_DEVICE_NOT_CONNECTED;}
	
	I2C_SendData(I2Cx, IMU_PWR_MGMT_1);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_SEND_FAIL;}
	
	I2C_SendData(I2Cx, 0x00);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_SEND_FAIL;}
	
	/************ choose clock source ************/
	// generate restart
	I2C_GenerateSTART(I2Cx, ENABLE); 
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_TIMEOUT;}
	
	// Send slave address and select master transmitter mode 
	I2C_Send7bitAddress(I2Cx, IMU_I2C_ADD, I2C_Direction_Transmitter);
	// Wait for slave to be acknowledged
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_DEVICE_NOT_CONNECTED;}
	
	I2C_SendData(I2Cx, IMU_PWR_MGMT_1);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_SEND_FAIL;}
	
	I2C_SendData(I2Cx, 0x01);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_SEND_FAIL;}
	
	I2C_GenerateSTOP(I2Cx, ENABLE);
	
	return 0;
}

uint8_t IMU_Update(I2C_TypeDef* I2Cx)
{
	uint8_t tmpbyte;
	
	// Generate start
	I2C_GenerateSTART(I2Cx, ENABLE); 
	// Wait for generating the start and take the bus
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_TIMEOUT;}
	
	// Send slave address and select master transmitter mode 
	I2C_Send7bitAddress(I2Cx, IMU_I2C_ADD, I2C_Direction_Transmitter);
	// Wait for slave to be acknowledged
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_DEVICE_NOT_CONNECTED;}
	
	// sending address of register to be read
	I2C_SendData(I2Cx, IMU_ACCEL_H);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_SEND_FAIL;}
	
	
	/**************** Read Accel ****************/
	// generate restart
	I2C_GenerateSTART(I2Cx, ENABLE); 
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_TIMEOUT;}
	
	// Send slave address and select master receiver mode 
	I2C_Send7bitAddress(I2Cx, IMU_I2C_ADD, I2C_Direction_Receiver);
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// ACCEL_XOUT_H
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Ax_Raw = (int16_t)(tmpbyte << 8);
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// ACCEL_XOUT_L
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Ax_Raw |= tmpbyte;
	imu_sensor_data.Ax = (float)imu_sensor_data.Ax_Raw * 2.0f / 32768.0f;
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// ACCEL_YOUT_H
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Ay_Raw = (int16_t)(tmpbyte << 8);
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// ACCEL_YOUT_L
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Ay_Raw |= tmpbyte;
	imu_sensor_data.Ay = (float)imu_sensor_data.Ay_Raw * 2.0f / 32768.0f;
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// ACCEL_ZOUT_H
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Az_Raw = (int16_t)(tmpbyte << 8);
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// ACCEL_ZOUT_L
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Az_Raw |= tmpbyte;
	imu_sensor_data.Az = (float)imu_sensor_data.Az_Raw * 2.0f / 32768.0f;
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	
	/************** Read Temperature ************/
	// TEMPRATURE_H
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Temprature_Raw = (int16_t)(tmpbyte << 8);
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// TEMPRATURE_L
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Temprature_Raw |= tmpbyte;
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	/***************** Read Gyro ****************/
	// GYRO_XOUT_H
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Gx_Raw = (int16_t)(tmpbyte << 8);
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// GYRO_XOUT_L
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Gx_Raw |= tmpbyte;
	imu_sensor_data.Gx = (float)imu_sensor_data.Gx_Raw * 250.0f / 32768.0f;
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// GYRO_YOUT_H
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Gy_Raw = (int16_t)(tmpbyte << 8);
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// GYRO_YOUT_L
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Gy_Raw |= tmpbyte;
	imu_sensor_data.Gy = (float)imu_sensor_data.Gy_Raw * 250.0f / 32768.0f;
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// disable ACK before receiving last byte
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	
	// GYRO_ZOUT_H
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Gz_Raw = (int16_t)(tmpbyte << 8);
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// GYRO_ZOUT_L
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Gz_Raw |= tmpbyte;
	imu_sensor_data.Gz = (float)imu_sensor_data.Gz_Raw * 250.0f / 32768.0f;
	
	I2C_GenerateSTOP(I2Cx, ENABLE);
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	
	/***************** Read Mag *****************/
	// Generate start
	I2C_GenerateSTART(I2Cx, ENABLE); 
	// Wait for generating the start and take the bus
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_TIMEOUT;}
	
	// Send slave address and select master transmitter mode 
	I2C_Send7bitAddress(I2Cx, IMU_I2C_ADD, I2C_Direction_Transmitter);
	// Wait for slave to be acknowledged
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_DEVICE_NOT_CONNECTED;}
	
	// sending address of register to be read
	I2C_SendData(I2Cx, IMU_GYRO_H);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_SEND_FAIL;}
	
	// generate restart
	I2C_GenerateSTART(I2Cx, ENABLE); 
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_TIMEOUT;}
	
	// Send slave address and select master receiver mode 
	I2C_Send7bitAddress(I2Cx, IMU_I2C_ADD, I2C_Direction_Receiver);
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// MAG_XOUT_H
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Mx_Raw = (int16_t)(tmpbyte << 8);
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// MAG_XOUT_L
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Mx_Raw |= tmpbyte;
	imu_sensor_data.Mx = (float)imu_sensor_data.Mx_Raw * 10.0f * 4912.0f / 32768.0f;
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// MAG_YOUT_H
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.My_Raw = (int16_t)(tmpbyte << 8);
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// MAG_YOUT_L
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.My_Raw |= tmpbyte;
	imu_sensor_data.My = (float)imu_sensor_data.My_Raw * 10.0f * 4912.0f / 32768.0f;
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// disable ACK before receiving last byte
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	
	// MAG_ZOUT_H
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Mz_Raw = (int16_t)(tmpbyte << 8);
	
	// wait for a byte to be recieved
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, MAX_WAIT) == TIMEOUT)
	{return ERROR_CODE_RECV_FAIL;}
	
	// MAG_ZOUT_L
	tmpbyte = I2C_ReceiveData(I2Cx);
	imu_sensor_data.Mz_Raw |= tmpbyte;
	imu_sensor_data.Mz = (float)imu_sensor_data.Mz_Raw * 10.0f * 4912.0f / 32768.0f;
	
	I2C_GenerateSTOP(I2Cx, ENABLE);
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	
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
