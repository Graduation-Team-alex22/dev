/**********************************************************
simple I2C driver build upon LLD layer.

Notes:  I2C peripheral to be used must be already initialized
        before using any available functions.

Auther: Mohamed Said & Ali Fakharany
Date:		2022-03-15

**********************************************************/

#ifndef DRIVER_I2C_H__
#define DRIVER_I2C_H__

#include "stdint.h"
#include "../hsi_library/stm32f4xx_i2c.h"
#include "../support_functions/ttrd2-05a-t0401a-v001a_timeout_t3.h"

/********** ERROR CODE DEFINITION **********/
#define NO_ERROR                                0			// don't change
#define ERROR_CODE_TIMEOUT                      1
#define ERROR_CODE_DEVICE_NOT_CONNECTED		   2
#define ERROR_CODE_BAD_WHOIAM							3
#define ERROR_CODE_START_FAIL					   	4
#define ERROR_CODE_SEND_FAIL							5
#define ERROR_CODE_RECV_FAIL							6
#define ERROR_CODE_COMM_FAIL							7

/************* PUBLIC MACROS ***************/
#define NO_WHOIAM                               0xFF

/************ Public Interfaces ************/
/*
  I2Cx_Send_Bytes
  
  Send a number of bytes over a specified I2C peripheral
	
	@note    The I2C peripheral must be already iniialized before using this funcion.
	
	@param I2Cx,         I2C Peripheral to be used.
	@param device_add,   The I2C address of the slave device.
	@param reg_add,      The address of slave register to write at.
	@param pData,        Pointer to data to be sent.
	@param size,         Number of data bytes to be sent.
	@param timeout,      Max waiting time for I2C events in microseconds.

  @return error_code,  An error code, Or Zero if no Error.

*/
uint8_t I2Cx_Send_Bytes(I2C_TypeDef* I2Cx, uint8_t device_add, uint8_t reg_add, uint8_t* pData, uint8_t size, uint16_t timeout);

/*
  I2Cx_Recv_Bytes
  
  Receive a number of bytes over a specified I2C peripheral
	
	@note    The I2C peripheral must be already iniialized before using this funcion.
	
	@param I2Cx,         I2C Peripheral to be used.
	@param device_add,   The I2C address of the slave device.
	@param reg_add,      The address of slave register to read from.
	@param pData,        Pointer to data to be received.
	@param size,         Number of data bytes to be received.
	@param timeout,      Max waiting time for I2C events in microseconds.

  @return error_code,  An error code, Or Zero if no Error.

*/
uint8_t I2Cx_Recv_Bytes(I2C_TypeDef* I2Cx, uint8_t device_add, uint8_t reg_add, uint8_t* pData, uint8_t size, uint16_t timeout);

/*
  I2Cx_Is_Device_Connected
  
  Checks if a slave device is connected over a specified I2C peripheral
	
	@note    The I2C peripheral must be already iniialized before using this funcion.
	
	@param I2Cx,           I2C Peripheral to be used.
	@param device_add,     The I2C address of the slave device.
	@param whoiam_reg_add, The address of slave identifer register to read from.
	                       @note  pass NO_WHOIAM if the device has no Identity registers.
	@param whoiam_reg_val, The identifacation value we expect.
	@param timeout,        Max waiting time for I2C events in microseconds.

  @return error_code,  An error code, Or Zero if no Error.

*/
uint8_t I2Cx_Is_Device_Connected(I2C_TypeDef* I2Cx, uint8_t device_add, uint8_t whoiam_reg_add, uint8_t whoiam_reg_val, uint32_t timeout);

/*
  wait_for_event
  
  waits for a specific I2C event over a specified I2C peripheral
	
	@note    The I2C peripheral must be already iniialized before using this funcion.
	
	@param I2Cx,         I2C Peripheral to be used.
	@param event,        The I2C event to wait for.
	                     I2C events are provided in LLD I2C driver.
											 This parameter can be one of the following values:
                       @arg I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED: EV1
                       @arg I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED: EV1
                       @arg I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED: EV1
                       @arg I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED: EV1
                       @arg I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED: EV1
                       @arg I2C_EVENT_SLAVE_BYTE_RECEIVED: EV2
                       @arg (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_DUALF): EV2
                       @arg (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_GENCALL): EV2
                       @arg I2C_EVENT_SLAVE_BYTE_TRANSMITTED: EV3
                       @arg (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_DUALF): EV3
                       @arg (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_GENCALL): EV3
                       @arg I2C_EVENT_SLAVE_ACK_FAILURE: EV3_2
                       @arg I2C_EVENT_SLAVE_STOP_DETECTED: EV4
                       @arg I2C_EVENT_MASTER_MODE_SELECT: EV5
                       @arg I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED: EV6     
                       @arg I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED: EV6
                       @arg I2C_EVENT_MASTER_BYTE_RECEIVED: EV7
                       @arg I2C_EVENT_MASTER_BYTE_TRANSMITTING: EV8
                       @arg I2C_EVENT_MASTER_BYTE_TRANSMITTED: EV8_2
                       @arg I2C_EVENT_MASTER_MODE_ADDRESS10: EV9
	@param timeout,      Max waiting time for I2C events in microseconds.

  @return error_code,  An error code, Or Zero if no Error.

*/
uint8_t wait_for_event(I2C_TypeDef* I2Cx, uint32_t event, uint32_t timeout );


#endif
