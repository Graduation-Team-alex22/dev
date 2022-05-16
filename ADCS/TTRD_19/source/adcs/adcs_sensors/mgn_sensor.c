#include "mgn_sensor.h"
#include " ../../../main/project.h"
#include "../config.h"
#include "../support_functions/ttrd2-05a-t0401a-v001a_timeout_t3.h"

/************* LOCAL DEFINITIONS ***************/
// costants
#define 	I2C_EVENT_WAIT				50			// microseconds
#define 	MGN_ID_A_VALUE				0x48


// register addresses
#define 	MGN_REG_DATA_XL				0x00
#define 	MGN_REG_DATA_XH				0x01
#define 	MGN_REG_DATA_YL				0x02
#define 	MGN_REG_DATA_YH				0x03
#define 	MGN_REG_DATA_ZL				0x04
#define 	MGN_REG_DATA_ZH				0x05
#define 	MGN_REG_STATUS	   			0x06
#define 	MGN_REG_TMP_L					0x07
#define 	MGN_REG_TMP_H					0x08
#define 	MGN_REG_CONTROL1	   		0x09
#define 	MGN_REG_CONTROL2		   	0x0A
#define 	MGN_REG_SET_PERIOD	   	0x0B

/************* LOCAL VARIABLES ***************/
static I2C_TypeDef* mgn_I2Cx_g;
static mgn_sensor_t mgn_data_g;
static uint8_t mgn_range_g;
// activation
static activated_sensor_e activated_mgn = FIRST;

/************* PRIVATE FUNCTIONS ************/
uint8_t MGN_Configure(mgn_init_t* mgn_init);

// -- PUBLIC FUNCTIONS' IMPLEMENTATION ---------------------------
uint32_t MGN_Sensor_Init(I2C_TypeDef* I2Cx, mgn_init_t* mgn_init)
{
   uint8_t error_code;

   assert_param(IS_I2C_ALL_PERIPH(I2Cx));
   assert_param(MGN_IS_OVER_SAMPLING_X(mgn_init->over_sampling));
   assert_param(MGN_IS_OUTPUT_RATE_X(mgn_init->output_rate));
   assert_param(MGN_IS_RANGE_X(mgn_init->range));
   assert_param(MGN_IS_OP_MODE_X(mgn_init->op_mode));

   // update local variables
   mgn_I2Cx_g = I2Cx;
   // translate bit mask to range value in Gauss
   mgn_range_g = (mgn_init->range >> 4) * 6 + 2;

   // Set up timeout mechanism
   TIMEOUT_T3_USEC_Init();

   // check if the device is connected - no check for whoiam
   error_code = I2Cx_Is_Device_Connected(I2Cx, MGN_I2C_ADD, 0xFF, 0xFF, I2C_EVENT_WAIT);
   //if(error_code){ return ERROR_MGN_CONNECTED_BASE + error_code;}
   if(error_code)
   {
      mgn_data_g.status = DEVICE_BROKEN;
      return NO_ERROR;
   }
   
   // configure the device
   error_code = MGN_Configure(mgn_init);
   //if(error_code){ return ERROR_MGN_CONFIG_BASE + error_code;}
   if(error_code)
   {
      mgn_data_g.status = DEVICE_BROKEN;
      return NO_ERROR;
   }

   // wait for 6 ms
   //TIMEOUT_T3_USEC_Start();
   //while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(6000));

   // update device status
   mgn_data_g.status = DEVICE_OK;
   
   return NO_ERROR;
}

uint32_t MGN_Sensor_Update(void)
{
   if(mgn_data_g.status != DEVICE_OK)
   {
      return NO_ERROR;
   }
   
   uint8_t error_code, data[6] = {0};

   error_code =I2Cx_Recv_Bytes(mgn_I2Cx_g, MGN_I2C_ADD, MGN_REG_DATA_XL, data, 6, I2C_EVENT_WAIT);
   //if(error_code) {return ERROR_MGN_UPDATE_BASE + error_code;}
   if(error_code)
   {
      mgn_data_g.status = DEVICE_BROKEN;
      return NO_ERROR;
   }

   // extract raw data
   mgn_data_g.raw[0] = ((int16_t) data[1] << 8) | data[0] ;
   mgn_data_g.raw[1] = ((int16_t) data[3] << 8) | data[2] ;
   mgn_data_g.raw[2] = ((int16_t) data[5] << 8) | data[4] ;
   
   // store mag values before update
   mgn_data_g.prev_mag[0] = mgn_data_g.mag[0];
   mgn_data_g.prev_mag[1] = mgn_data_g.mag[1];
   mgn_data_g.prev_mag[2] = mgn_data_g.mag[2];   
   
   // calculate value in Gauss
   mgn_data_g.mag[0] = (float)mgn_data_g.raw[0] * mgn_range_g /32768;
   mgn_data_g.mag[1] = (float)mgn_data_g.raw[1] * mgn_range_g /32768;
   mgn_data_g.mag[2] = (float)mgn_data_g.raw[2] * mgn_range_g /32768;
   
   // filter mag readings
   mgn_data_g.mag_filtered[0] = A_MGN * mgn_data_g.mag[0] 
                              + (1 - A_MGN) * mgn_data_g.prev_mag[0];
   mgn_data_g.mag_filtered[1] = A_MGN * mgn_data_g.mag[1] 
                              + (1 - A_MGN) * mgn_data_g.prev_mag[1];
   mgn_data_g.mag_filtered[2] = A_MGN * mgn_data_g.mag[2] 
                              + (1 - A_MGN) * mgn_data_g.prev_mag[2];                           
   
   // calculate the magnitude of the vector
   mgn_data_g.rm_norm = vect_magnitude_arr(mgn_data_g.mag);
   if (mgn_data_g.rm_norm > MAX_IGRF_NORM
   || mgn_data_g.rm_norm  < MIN_IGRF_NORM) 
   {
      mgn_data_g.status = READING_ERROR;
   }
   
   return NO_ERROR;
}

mgn_sensor_t MGN_Sensor_GetData(void)
{
   return mgn_data_g;
}

// -- PRIVATE FUNCTIONS' IMPLEMENTATION ---------------------------
uint8_t MGN_Configure(mgn_init_t* mgn_init)
{
   uint8_t error_code, tmpreg;

   // configure set/reset period register (recommended by datasheet)
   tmpreg = 0x01;
   I2Cx_Send_Bytes(mgn_I2Cx_g, MGN_I2C_ADD, MGN_REG_SET_PERIOD, &tmpreg, 1, I2C_EVENT_WAIT);

   // Control register : over sampling, output rate, range , operation mode
   tmpreg = mgn_init->over_sampling | mgn_init->output_rate | mgn_init->range | mgn_init->op_mode;

   // write to Control register
   error_code = I2Cx_Send_Bytes(mgn_I2Cx_g, MGN_I2C_ADD, MGN_REG_CONTROL1, &tmpreg, 1, I2C_EVENT_WAIT);
   if(error_code) {return error_code ;}

   // enable rolling pointer in control register 2
   tmpreg = 0x01 << 6;
   error_code = I2Cx_Send_Bytes(mgn_I2Cx_g, MGN_I2C_ADD, MGN_REG_CONTROL2, &tmpreg, 1, I2C_EVENT_WAIT);

   return error_code;
}


void MGN_Sensor_SetStatus(device_status_e new_status)
{
   mgn_data_g.status = new_status;
}

void MGN_Change_Activated_Module(void)
{
   switch(activated_mgn)
   {
      case FIRST:
         activated_mgn = SECOND;
         GPIO_SetBits(SENSOR_ACTIVATION_PORT, MGN_ACTIVATION_PIN);
      break;
      
      case SECOND:
         activated_mgn = FIRST;
         GPIO_ResetBits(SENSOR_ACTIVATION_PORT, MGN_ACTIVATION_PIN);
      break;   
   }
}
