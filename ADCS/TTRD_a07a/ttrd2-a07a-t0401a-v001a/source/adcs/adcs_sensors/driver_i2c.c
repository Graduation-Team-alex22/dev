#include "driver_i2c.h"
#include "../support_functions/ttrd2-05a-t0401a-v001a_timeout_t3.h"


#define STOP_GENERATION_WAIT			50		//  in microsecond

#define	TIMEOUT			1
#define OK 					0		// don't change


uint8_t wait_for_event(I2C_TypeDef* I2Cx, uint32_t event, uint32_t timeout )
{
	uint32_t T3;
	
	TIMEOUT_T3_USEC_Start();
	while (!I2C_CheckEvent(I2Cx, event) && 
		     (COUNTING == (T3 = TIMEOUT_T3_USEC_Get_Timer_State(timeout))));
	if (T3 == TIMED_OUT) return TIMEOUT;
	return OK;
}


uint8_t I2Cx_Send_Bytes(I2C_TypeDef* I2Cx, uint8_t device_add, uint8_t reg_add, uint8_t* pData, uint8_t size, uint16_t timeout)
{
	
	// generate start
	I2C_GenerateSTART(I2Cx, ENABLE); 
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, timeout) == TIMEOUT)
	{return ERROR_CODE_TIMEOUT;}
	
	// Send slave address and select master transmitter mode 
	I2C_Send7bitAddress(I2Cx, device_add, I2C_Direction_Transmitter);
	// Wait for slave to be acknowledged
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, timeout) == TIMEOUT)
	{return ERROR_CODE_DEVICE_NOT_CONNECTED;}
	
	// sending register address
	I2C_SendData(I2Cx, reg_add);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, timeout) == TIMEOUT)
	{return ERROR_CODE_SEND_FAIL;}
	
	for(int i = 0; i < size; i++)
	{	
		I2C_SendData(I2Cx, pData[i]);
		if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, timeout) == TIMEOUT)
		{return ERROR_CODE_SEND_FAIL;}
	}
	
	I2C_GenerateSTOP(I2Cx, ENABLE);
	TIMEOUT_T3_USEC_Start();
	while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(STOP_GENERATION_WAIT));
	
	
	return NO_ERROR;
}


uint8_t I2Cx_Recv_Bytes(I2C_TypeDef* I2Cx, uint8_t device_add, uint8_t reg_add, uint8_t* pData, uint8_t size, uint16_t timeout)
{
	if(size == 0) return 0;
	
	// Generate start
	I2C_GenerateSTART(I2Cx, ENABLE); 
	// Wait for generating the start and take the bus
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, timeout) == TIMEOUT)
	{
	  return ERROR_CODE_TIMEOUT;
	}
	
	// Send slave address and select master transmitter mode 
	I2C_Send7bitAddress(I2Cx, device_add, I2C_Direction_Transmitter);
	// Wait for slave to be acknowledged
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, timeout) == TIMEOUT)
	{
	  return ERROR_CODE_DEVICE_NOT_CONNECTED;
	}
	
	// send register address
	I2C_SendData(I2Cx, reg_add);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, timeout) == TIMEOUT)
	{
		return ERROR_CODE_SEND_FAIL;
	}
	
	// generate restart
	I2C_GenerateSTART(I2Cx, ENABLE);
	if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, timeout) == TIMEOUT)
	{
		return ERROR_CODE_TIMEOUT;
	}
	
	if(size == 1)
	{
		// disable ack to receive one byte
		I2C_AcknowledgeConfig(I2Cx, DISABLE);
		// Send slave address and select master receiver mode 
		I2C_Send7bitAddress(I2Cx, device_add, I2C_Direction_Receiver);
		
		// wait for a byte to be recieved
		if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, timeout) == TIMEOUT)
		{return ERROR_CODE_RECV_FAIL;}
		
		pData[0] = I2C_ReceiveData(I2Cx);
	}else{
		// Send slave address and select master receiver mode 
		I2C_Send7bitAddress(I2Cx, device_add, I2C_Direction_Receiver);
		
		for(int i = 0; i< size; i++)
		{
			// wait for a byte to be recieved
			if(wait_for_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED, timeout) == TIMEOUT)
			{return ERROR_CODE_RECV_FAIL;}
			
			if(i == size-2) I2C_AcknowledgeConfig(I2Cx, DISABLE);
			
			pData[i] = I2C_ReceiveData(I2Cx);
		}
	}
	
	// generate stop after receiving one byte
	I2C_GenerateSTOP(I2Cx, ENABLE);
	// re-enable acknoledge
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	
	return NO_ERROR;
}

uint8_t I2Cx_Is_Device_Connected(I2C_TypeDef* I2Cx, uint8_t device_add, uint8_t whoiam_reg_add, uint8_t whoiam_reg_val, uint32_t timeout)
{
	uint8_t error_code;
	uint8_t data[1];
	
	/************ check if the device is connected ************/
	error_code = I2Cx_Recv_Bytes(I2Cx, device_add, whoiam_reg_add, data, 1, timeout);
	if(error_code){ return error_code;}
	
	// check for right WHO_I_AM value
	if( whoiam_reg_add != 0xFF && data[0] != whoiam_reg_val)
	{
		return ERROR_CODE_BAD_WHOIAM;
	}
	
	return NO_ERROR;
}

uint8_t I2Cx_Slave_Scanner(I2C_TypeDef* I2Cx, uint8_t *pDevices, uint8_t max_devices, uint32_t timeout)
{
	uint8_t index = 0, i = 1;
	
	while(i++ < 128 && index <= max_devices	)
	{
		// generate start
		I2C_GenerateSTART(I2Cx, ENABLE); 
		if(wait_for_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, timeout) == TIMEOUT)
		{continue;}
		
		// Send slave address and select master transmitter mode 
		I2C_Send7bitAddress(I2Cx, (i << 1), I2C_Direction_Transmitter);
		// Wait for slave to be acknowledged
		if(wait_for_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, timeout) == TIMEOUT)
		{continue;}
		
		pDevices[index] = (i << 1);
		index++;
	}
	
	return 0;
}
