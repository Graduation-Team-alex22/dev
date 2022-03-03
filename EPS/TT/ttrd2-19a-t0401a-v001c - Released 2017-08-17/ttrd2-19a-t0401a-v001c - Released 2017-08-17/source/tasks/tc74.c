#include "tc74.h"
#include "stm32f4xx_rcc.h"

/**
  * @brief Reads the word in temperature measurement register of the device and returns the current status of the specified address.
  * @param  h_i2c: pointer to the i2c bus peripheral handle that the temperature sensor ics are connected.
  * @param  device_i2c_address: i2c adress of the sensor to request measurement.
  * @param  receive_word: pointer to the word where the measurement register will be returned.
  * @retval TC_74_STATUS.
  */
 
void TC74_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
   I2C_InitTypeDef I2C_InitStructure;
	//	Enable peripheral clock using RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2Cx, ENABLE)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	 // GPIOB clock enable 
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	 //RCC_I2CCLKConfig(RCC_PCLKConfig());
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);

 

   

   // GPIO config
   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10|GPIO_Pin_11; 
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
	//RCC_I2CCLKConfig(RCC_PCLKConfig());
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);
   GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_I2C2);
	 I2C_DeInit(I2C1);


	/*---------------- Reset I2C init structure parameters values ----------------*/
  /* initialize the I2C_ClockSpeed member */
  I2C_InitStructure.I2C_ClockSpeed = 100000;
  /* Initialize the I2C_Mode member */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  /* Initialize the I2C_DutyCycle member */
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  /* Initialize the I2C_OwnAddress1 member */
  I2C_InitStructure.I2C_OwnAddress1 = 0;
	
  /* Initialize the I2C_Ack member */
  I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
  /* Initialize the I2C_AcknowledgedAddress member */
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

I2C_Init(I2C2,&I2C_InitStructure);

}
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