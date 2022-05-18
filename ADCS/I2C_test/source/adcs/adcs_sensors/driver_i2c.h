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
#include "../../hsi_library/stm32f4xx_i2c.h"

/********** ERROR CODE DEFINITION **********/
#define ERROR_CODE_TIMEOUT                      1
#define ERROR_CODE_DEVICE_NOT_CONNECTED		   2
#define ERROR_CODE_BAD_WHOIAM							3
#define ERROR_CODE_START_FAIL					   	4
#define ERROR_CODE_SEND_FAIL							5
#define ERROR_CODE_RECV_FAIL							6
#define ERROR_CODE_COMM_FAIL							7


/** Public function prototypes ---------------------------------------------- */
void i2c_init(void);
void i2c_write_no_reg(uint8_t address, uint8_t data);
void i2c_write_with_reg(uint8_t address, uint8_t reg, uint8_t data);
void i2c_write_multi_no_reg(uint8_t address, uint8_t* data, uint8_t len);
void i2c_write_multi_with_reg(uint8_t address, uint8_t reg, uint8_t* data, uint8_t len);
void i2c_read_no_reg(uint8_t address, uint8_t* data);
void i2c_read_with_reg(uint8_t address, uint8_t reg, uint8_t* data);
void i2c_read_multi_no_reg(uint8_t address, uint8_t len, uint8_t* data);
void i2c_read_multi_with_reg(uint8_t address, uint8_t reg, uint8_t len, uint8_t* data);


#endif

/********************************* END OF FILE ********************************/
/******************************************************************************/

