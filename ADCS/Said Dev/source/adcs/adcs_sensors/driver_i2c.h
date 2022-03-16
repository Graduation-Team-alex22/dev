/**********************************************************
simple I2C driver build upon LLD layer.

Notes: None

Auther: Mohamed Said & Ali Fakharany
Date:		2022-03-15

**********************************************************/

#ifndef DRIVER_I2C_H__
#define DRIVER_I2C_H__

#include "stdint.h"
#include "../hsi_library/stm32f4xx_i2c.h"

/********** ERROR CODE DEFINITION **********/
#define NO_ERROR													0			// don't change
#define ERROR_CODE_TIMEOUT 								1
#define ERROR_CODE_DEVICE_NOT_CONNECTED		2
#define	ERROR_CODE_START_FAIL							4
#define	ERROR_CODE_SEND_FAIL							5
#define	ERROR_CODE_RECV_FAIL							6
#define ERROR_CODE_COMM_FAIL							7


/************ Public Interfaces ************/
uint8_t I2Cx_Send_Bytes(I2C_TypeDef* I2Cx, uint8_t device_add, uint8_t reg_add, uint8_t* pData, uint8_t size, uint16_t timeout);
uint8_t I2Cx_Recv_Bytes(I2C_TypeDef* I2Cx, uint8_t device_add, uint8_t reg_add, uint8_t* pData, uint8_t size, uint16_t timeout);
uint8_t wait_for_event(I2C_TypeDef* I2Cx, uint32_t event, uint32_t timeout );

#endif
