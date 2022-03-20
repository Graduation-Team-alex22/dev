#include "imu_sensor.h"
#include "../support_functions/ttrd2-05a-t0401a-v001a_timeout_t3.h"

/************ PRIVATE DEFINITIONS ************/
// Error Codes


// Timeout Periods
#define 	I2C_EVENT_WAIT					50			// microseconds
#define 	IMU_MAG_PD_WAIT					200			// magnometer mode switch time in microseconds

// Device I2C Addresses
#define		IMU_I2C_ADD							((uint8_t)0xD0)
#define		IMU_AKM_ADD							((uint8_t)0x18)		// magnometer address

// Register I2C Addresses
#define 	IMU_REG_WHOIAM					((uint8_t)0x75)
#define		IMU_REG_PWR_MGMT_1			((uint8_t)0x6B)
#define		IMU_REG_PWR_MGMT_2			((uint8_t)0x6C)
#define		IMU_REG_MAG_CTRL				((uint8_t)0x0A)
#define		IMU_REG_MAG_ASAX				((uint8_t)0x10)		// X-axis sensitivity adjustment
#define		IMU_REG_ACCEL_DATA			((uint8_t)0x3B)
#define		IMU_REG_GYRO_DATA				((uint8_t)0x43)
#define		IMU_REG_MAG_DATA				((uint8_t)0x03)
#define		IMU_REG_MAG_ST1					((uint8_t)0x02)
#define		IMU_REG_INT_PIN_CFG			((uint8_t)0x37)
#define		IMU_REG_CONFIG					((uint8_t)0x1A)

// Constants
#define 	IMU_WHOIAM_VAL					((uint8_t)0x71)

// Bit Flags / Masks
#define		IMU_MAG_MODE_CONT1			((uint8_t)0x02)		// magnometer continuous mode 1 (8 Hz)
#define		IMU_MAG_MODE_CONT2			((uint8_t)0x06)		// magnometer continuous mode 2 (100 Hz)
#define		IMU_MAG_RES_16BIT				((uint8_t)0x10) 	// magnometer 16-bit resolution
#define		IMU_RESET								((uint8_t)0x80)
#define		IMU_SLEEP								((uint8_t)0x40)
#define		IMU_CLK_AUTO						((uint8_t)0x01)
#define		IMU_I2C_BYPAsS_EN				((uint8_t)0x02)


/**************** PRIVATE STRUCT *******************/
static struct	{
	int t;
	
} measure_param;


/***************** PRIVATE VAR *********************/
static imu_sensor_t imu_sensor_data;

/***************** PRIVATE FUNC *********************/
uint8_t	IMU_Wakeup(I2C_TypeDef* I2Cx);
uint8_t	IMU_Configure(I2C_TypeDef* I2Cx);
uint8_t IMU_Mag_Calib(I2C_TypeDef* I2Cx, float* calib);


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
	error_code = I2Cx_Is_Device_Connected(I2Cx, IMU_I2C_ADD, IMU_REG_WHOIAM, IMU_WHOIAM_VAL, I2C_EVENT_WAIT);
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
	
	return 0;
}



uint8_t	IMU_Wakeup(I2C_TypeDef* I2Cx)
{
	uint8_t error_code;
	uint8_t data;
	
	/************ reset the device ************/
	data = IMU_RESET;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_I2C_ADD, IMU_REG_PWR_MGMT_1, &data, 1, I2C_EVENT_WAIT);
	if(error_code){return error_code;}

	/************ clear sleep bit ************/
	data = 0x00;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_I2C_ADD, IMU_REG_PWR_MGMT_1, &data, 1, I2C_EVENT_WAIT);
	if(error_code){return error_code;}
	
	/************ choose clock source ************/
	data = IMU_CLK_AUTO;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_I2C_ADD, IMU_REG_PWR_MGMT_1, &data, 1, I2C_EVENT_WAIT);
	if(error_code){return error_code;}
	
	return 0;
}


uint8_t	IMU_Configure(I2C_TypeDef* I2Cx)
{
	uint8_t error_code;
	uint8_t data;
	
	/******************* MPU9255 Config ******************/
	// enable I2C bypass
	data = IMU_I2C_BYPAsS_EN;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_I2C_ADD, IMU_REG_INT_PIN_CFG, &data, 1, I2C_EVENT_WAIT);
	if(error_code){return error_code;}
	
	return 0;
}

uint8_t IMU_Mag_Calib(I2C_TypeDef* I2Cx, float* calib)
{
	uint8_t error_code;
	uint8_t data[3];
	
	/******** put magnometer in power down mode **********/
	data[0] = 0x00;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_AKM_ADD, IMU_REG_MAG_CTRL, data, 1, I2C_EVENT_WAIT);
	if(error_code){return error_code;}
	
	/******** wait for IMU_MAG_PD_WAIT **********/
	TIMEOUT_T3_USEC_Start();
	while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(IMU_MAG_PD_WAIT));
	
	/******** put magnometer in FUSE ROM Access **********/
	data[0] = 0x0F;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_AKM_ADD, IMU_REG_MAG_CTRL, data, 1, I2C_EVENT_WAIT);
	if(error_code){return error_code;}
	
	/******** wait for IMU_MAG_PD_WAIT **********/
	TIMEOUT_T3_USEC_Start();
	while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(IMU_MAG_PD_WAIT));
	
	/******** read calibration values **********/
	error_code = I2Cx_Recv_Bytes(I2Cx, IMU_AKM_ADD, IMU_REG_MAG_ASAX, data, 3, I2C_EVENT_WAIT);
	if(error_code){ return error_code;}
	
	calib[0] = (float)(data[0] - 128)/256.0 + 1.0;
	calib[1] = (float)(data[1] - 128)/256.0 + 1.0;
	calib[2] = (float)(data[2] - 128)/256.0 + 1.0;
	
	/******** put magnometer in power down mode **********/
	data[0] = 0x00;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_AKM_ADD, IMU_REG_MAG_CTRL, data, 1, I2C_EVENT_WAIT);
	if(error_code){return error_code;}
	
	/******** wait for IMU_MAG_PD_WAIT **********/
	TIMEOUT_T3_USEC_Start();
	while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(IMU_MAG_PD_WAIT));
	
	/******** put magnometer in Continuous mode **********/
	data[0] = IMU_MAG_MODE_CONT1 | IMU_MAG_RES_16BIT;
	error_code = I2Cx_Send_Bytes(I2Cx, IMU_AKM_ADD, IMU_REG_MAG_CTRL, data, 1, I2C_EVENT_WAIT);
	if(error_code){return error_code;}
	
	/******** wait for IMU_MAG_PD_WAIT **********/
	TIMEOUT_T3_USEC_Start();
	while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(IMU_MAG_PD_WAIT));
	
	return 0;
}

static enum {STAGE_1, STAGE_2, STAGE_3} update_stage_counter = STAGE_1;

uint8_t IMU_Update(I2C_TypeDef* I2Cx)
{
	uint8_t error_code;
	uint8_t data[8];
	
	switch(update_stage_counter)
	{
		case STAGE_1:
			update_stage_counter = STAGE_2 ;
			/**************** Read Accel ****************/
			error_code = I2Cx_Recv_Bytes(I2Cx, IMU_I2C_ADD, IMU_REG_ACCEL_DATA, data, 6, I2C_EVENT_WAIT);
			if(error_code){ return error_code;}
			
			imu_sensor_data.Ax_Raw = ((int16_t) data[0] << 8) | data[1];
			imu_sensor_data.Ax = (float)imu_sensor_data.Ax_Raw * imu_sensor_data.AMult;
			imu_sensor_data.Ay_Raw = ((int16_t) data[2] << 8) | data[3];
			imu_sensor_data.Ay = (float)imu_sensor_data.Ay_Raw * imu_sensor_data.AMult;
			imu_sensor_data.Az_Raw = ((int16_t) data[4] << 8) | data[5];
			imu_sensor_data.Az = (float)imu_sensor_data.Az_Raw * imu_sensor_data.AMult;
		break;
			
		case STAGE_2:
			update_stage_counter = STAGE_3 ;
		
			/***************** Read Gyro ****************/
			error_code = I2Cx_Recv_Bytes(I2Cx, IMU_I2C_ADD, IMU_REG_GYRO_DATA, data, 6, I2C_EVENT_WAIT);
			if(error_code){ return error_code;}
			
			imu_sensor_data.Gx_Raw = ((int16_t) data[0] << 8) | data[1];
			imu_sensor_data.Gx = (float)imu_sensor_data.Gx_Raw * imu_sensor_data.GMult;
			imu_sensor_data.Gy_Raw = ((int16_t) data[2] << 8) | data[3];
			imu_sensor_data.Gy = (float)imu_sensor_data.Gy_Raw * imu_sensor_data.GMult;
			imu_sensor_data.Gz_Raw = ((int16_t) data[4] << 8) | data[5];
			imu_sensor_data.Gz = (float)imu_sensor_data.Gz_Raw * imu_sensor_data.GMult;
		break;
			
		case STAGE_3:
			update_stage_counter = STAGE_1 ;
					
			/***************** Read Mag *****************/
			imu_sensor_data.Mx =0;
			imu_sensor_data.My =0;
			imu_sensor_data.Mz =0;

			error_code = I2Cx_Recv_Bytes(I2Cx, IMU_AKM_ADD, IMU_REG_MAG_ST1, data, 8, I2C_EVENT_WAIT);
			if(error_code){ return error_code;}
			
			// check on data ready flag and check for magnetic overflow
			if((data[0] & 0x01) && !(data[7] & 0x08) )
			{
				imu_sensor_data.Mx_Raw = ((int16_t) data[2] << 8) | data[1];
				imu_sensor_data.Mx = (float)imu_sensor_data.Mx_Raw * imu_sensor_data.MMult * imu_sensor_data.M_Calib[0];
				imu_sensor_data.My_Raw = ((int16_t) data[4] << 8) | data[3];
				imu_sensor_data.My = (float)imu_sensor_data.My_Raw * imu_sensor_data.MMult * imu_sensor_data.M_Calib[1];
				imu_sensor_data.Mz_Raw = ((int16_t) data[6] << 8) | data[5];
				imu_sensor_data.Mz = (float)imu_sensor_data.Mz_Raw * imu_sensor_data.MMult * imu_sensor_data.M_Calib[2];
			}
		break;
			
		default:
			// We shouldn't be here
			return 255;
	}
	
	return 0;
}


imu_sensor_t IMU_Get_Data(void){
	return imu_sensor_data;
}



