#include "imu_sensor.h"

/************ PRIVATE DEFINITIONS ************/

// Timeout Periods
#define  I2C_EVENT_WAIT             500			// microseconds
#define  IMU_MAG_PD_WAIT            200			// magnometer mode switch time in microseconds

// Device I2C Addresses
#define  IMU_I2C_ADD                ((uint8_t)0xD0)
#define  IMU_AKM_ADD                ((uint8_t)0x18)		// magnometer address

// Register I2C Addresses
#define  IMU_REG_WHOIAM             ((uint8_t)0x75)
#define  IMU_REG_PWR_MGMT_1         ((uint8_t)0x6B)
#define  IMU_REG_PWR_MGMT_2         ((uint8_t)0x6C)
#define  IMU_REG_MAG_CTRL           ((uint8_t)0x0A)
#define  IMU_REG_MAG_ASAX           ((uint8_t)0x10)		// X-axis sensitivity adjustment
#define  IMU_REG_ACCEL_DATA         ((uint8_t)0x3B)
#define  IMU_REG_GYRO_DATA          ((uint8_t)0x43)
#define  IMU_REG_MAG_DATA           ((uint8_t)0x03)
#define  IMU_REG_MAG_ST1            ((uint8_t)0x02)
#define  IMU_REG_INT_PIN_CFG        ((uint8_t)0x37)
#define  IMU_REG_CONFIG             ((uint8_t)0x1A)

// Constants
#define 	IMU_WHOIAM_VAL             ((uint8_t)0x71)

// Bit Flags / Masks
#define  IMU_MAG_MODE_CONT1         ((uint8_t)0x02)      // magnometer continuous mode 1 (8 Hz)
#define  IMU_MAG_MODE_CONT2         ((uint8_t)0x06)      // magnometer continuous mode 2 (100 Hz)
#define  IMU_MAG_RES_16BIT          ((uint8_t)0x10)      // magnometer 16-bit resolution
#define  IMU_RESET                  ((uint8_t)0x80)
#define  IMU_SLEEP                  ((uint8_t)0x40)
#define  IMU_CLK_AUTO               ((uint8_t)0x01)
#define  IMU_I2C_BYPAsS_EN          ((uint8_t)0x02)

/***************** PRIVATE VARIABLES *********************/
static imu_sensor_t imu_sensor_data;

/***************** PRIVATE FUNCTIONS *********************/
uint8_t  IMU_Wakeup(I2C_TypeDef* I2Cx);
uint8_t  IMU_Configure(I2C_TypeDef* I2Cx);
uint8_t  IMU_Mag_Calib(I2C_TypeDef* I2Cx, float* calib);

// -- PUBLIC FUNCTIONS' IMPLEMENTATION ---------------------------
uint8_t IMU_Sensor_Init(I2C_TypeDef* I2Cx)
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
   if(error_code){ return error_code+108;}

   // configuration
   error_code = IMU_Configure(I2Cx);
   if(error_code){ return error_code+116;}

   // calibration and multipliers
   imu_sensor_data.AMult = 2.0f / 32768.0f;
   imu_sensor_data.GMult = 250.0f / 32768.0f;
   imu_sensor_data.MMult = 10.0f * 4912.0f / 32768.0f;
   error_code = IMU_Mag_Calib(I2Cx, imu_sensor_data.M_Calib);
   if(error_code){ return error_code+140;}

   // update device status
   imu_sensor_data.status = DEVICE_OK;
   
   return 0;
}

uint8_t IMU_Sensor_Update(I2C_TypeDef* I2Cx)
{
   imu_sensor_data.status = DEVICE_OK;
   
   uint8_t error_code;
   uint8_t data[8];
   
   /**************** Read Accel ****************/
   error_code = I2Cx_Recv_Bytes(I2Cx, IMU_I2C_ADD, IMU_REG_ACCEL_DATA, data, 6, I2C_EVENT_WAIT);
   if(error_code){ return error_code;}

   imu_sensor_data.Ax_Raw = ((int16_t) data[0] << 8) | data[1];
   imu_sensor_data.Ax = (float)imu_sensor_data.Ax_Raw * imu_sensor_data.AMult;
   imu_sensor_data.Ay_Raw = ((int16_t) data[2] << 8) | data[3];
   imu_sensor_data.Ay = (float)imu_sensor_data.Ay_Raw * imu_sensor_data.AMult;
   imu_sensor_data.Az_Raw = ((int16_t) data[4] << 8) | data[5];
   imu_sensor_data.Az = (float)imu_sensor_data.Az_Raw * imu_sensor_data.AMult;
  
   /***************** Read Gyro ****************/
   error_code = I2Cx_Recv_Bytes(I2Cx, IMU_I2C_ADD, IMU_REG_GYRO_DATA, data, 6, I2C_EVENT_WAIT);
   if(error_code){ return error_code;}

   // store previous gyro readings
   imu_sensor_data.gyro_prev[0] = imu_sensor_data.Gx;
   imu_sensor_data.gyro_prev[1] = imu_sensor_data.Gy;
   imu_sensor_data.gyro_prev[2] = imu_sensor_data.Gz;
   
   imu_sensor_data.Gx_Raw = ((int16_t) data[0] << 8) | data[1];
   imu_sensor_data.Gx = (float)imu_sensor_data.Gx_Raw * imu_sensor_data.GMult;
   imu_sensor_data.Gy_Raw = ((int16_t) data[2] << 8) | data[3];
   imu_sensor_data.Gy = (float)imu_sensor_data.Gy_Raw * imu_sensor_data.GMult;
   imu_sensor_data.Gz_Raw = ((int16_t) data[4] << 8) | data[5];
   imu_sensor_data.Gz = (float)imu_sensor_data.Gz_Raw * imu_sensor_data.GMult;
   
   // calculate filtered gyro values 
   imu_sensor_data.gyro_filtered[0] = A_GYRO * imu_sensor_data.Gx + (1 - A_GYRO) * imu_sensor_data.gyro_prev[0];
   imu_sensor_data.gyro_filtered[1] = A_GYRO * imu_sensor_data.Gy + (1 - A_GYRO) * imu_sensor_data.gyro_prev[1];
   imu_sensor_data.gyro_filtered[2] = A_GYRO * imu_sensor_data.Gz + (1 - A_GYRO) * imu_sensor_data.gyro_prev[2];
   
   /***************** Read Mag *****************/
   error_code = I2Cx_Recv_Bytes(I2Cx, IMU_AKM_ADD, IMU_REG_MAG_ST1, data, 8, I2C_EVENT_WAIT);
   if(error_code){ return error_code;}
   
   // store previous mgn readings
   imu_sensor_data.xm_prev[0] = imu_sensor_data.Mx;
   imu_sensor_data.xm_prev[1] = imu_sensor_data.My;
   imu_sensor_data.xm_prev[2] = imu_sensor_data.Mz;
   
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
   
   // calculate filtered mgn values 
   imu_sensor_data.xm_filtered[0] = A_MGN * imu_sensor_data.Mx + (1 - A_MGN) * imu_sensor_data.xm_prev[0];
   imu_sensor_data.xm_filtered[1] = A_MGN * imu_sensor_data.My + (1 - A_MGN) * imu_sensor_data.xm_prev[1];
   imu_sensor_data.xm_filtered[2] = A_MGN * imu_sensor_data.Mz + (1 - A_MGN) * imu_sensor_data.xm_prev[2];
   
   // calculate the magnitude of mag vector
   imu_sensor_data.xm_norm = vect_magnitude_arr((double*)&imu_sensor_data.Mx);
   
   if (imu_sensor_data.xm_norm > MAX_IGRF_NORM
   || imu_sensor_data.xm_norm < MIN_IGRF_NORM) 
   {
      imu_sensor_data.status = READING_ERROR;
   }
  
   return 0;
}


imu_sensor_t IMU_Sensor_GetData(void){
   return imu_sensor_data;
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

// -- PRIVATE FUNCTIONS' IMPLEMENTATION ---------------------------
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

   calib[0] = (float)(data[0] - 128)/256 + 1;
   calib[1] = (float)(data[1] - 128)/256 + 1;
   calib[2] = (float)(data[2] - 128)/256 + 1;

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
