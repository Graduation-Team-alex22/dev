#include "tc74.h"
/**
  * @brief Reads the word in temperature measurement register of the device and returns the current status of the specified address.
  * @param  h_i2c: pointer to the i2c bus peripheral handle that the temperature sensor ics are connected.
  * @param  device_i2c_address: i2c adress of the sensor to request measurement.
  * @param  receive_word: pointer to the word where the measurement register will be returned.
  * @retval TC_74_STATUS.
  */
 
void I2c2_init(void){};
 TC_74_STATUS TC74_read_device_temperature(uint8_t *h_i2c, uint8_t device_i2c_address, int8_t *receive_word ) {

	 


uint16_t device_status;
	 return device_status;
 }

 /**
   * @brief Wake up request to the sensor on the specified address.
   * @param  h_i2c: pointer to the i2c bus peripheral handle that the temperature sensor ics are connected.
   * @param  device_i2c_address: i2c address of the sensor to request measurement.
   * @retval TC_74_STATUS.
   */
 TC_74_STATUS TC74_device_wake_up(uint8_t *h_i2c, uint8_t device_i2c_address) {


 	 
 		uint8_t device_status = TC74_read_device_status(h_i2c, device_i2c_address);
 	 

	 return device_status;
 }

 /**
   * @brief Sleep device request to the sensor on the specified address.
   * @param  h_i2c: pointer to the i2c bus peripheral handle that the temperature sensor ics are connected.
   * @param  device_i2c_address: i2c address of the sensor to request measurement.
   * @retval TC_74_STATUS.
   */
 TC_74_STATUS TC74_device_sleep(uint8_t *h_i2c, uint8_t device_i2c_address) {

	 TC_74_STATUS device_status;
	 /*put device to sleep*/
	 
		 //soft error handle
		 device_status = DEVICE_ERROR;

	 return device_status;
 }

 /**
   * @brief Reads the word in device status register of the device and returns the current status of the specified address.
   * @param  h_i2c: pointer to the i2c bus peripheral handle that the temperature sensor ics are connected.
   * @param  device_i2c_address: i2c adress of the sensor to request measurement.
   * @param  receive_word: pointer to the word where the measurement register will be returned.
   * @retval TC_74_STATUS.
   */
 TC_74_STATUS TC74_read_device_status(uint8_t *h_i2c, uint8_t device_i2c_address ) {

	 TC_74_STATUS device_status = DEVICE_STATUS_LAST_VALUE;
	 uint8_t res;
	/*Read control regiter*/
	/*master sends the slave's temperature register adrress to read back*/
	
		//soft error handle
		device_status = DEVICE_ERROR;
	 return device_status;
	}

	
