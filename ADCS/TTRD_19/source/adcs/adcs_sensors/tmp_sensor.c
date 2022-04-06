#include "tmp_sensor.h"
#include " ../../../main/project.h"
#include "../support_functions/ttrd2-05a-t0401a-v001a_timeout_t3.h"

// -- PRIVATE MACROS ---------------------------------------------
// Device I2C Address
#define TMP_I2C_ADD                 (0x18<<1)
#define TMP_ID_REG                  0x07
// temperature value register
#define TEMP_REG_ADDRESS            (0x05)

#define I2C_EVENT_WAIT              50         // microseconds
#define TMP_ID_VALUE                0x04

// ------ Private VARIABLES --------------------------------------------------
static I2C_TypeDef* tmp_I2Cx_g;
static tmp_sensor_t tmp_sensor;
static uint8_t data[2];

// ------ Private FUNCTIONS --------------------------------------------------
static float TEMP_SENSOR_Calculate_Value(void);

// -- PUBLIC FUNCTIONS' IMPLEMENTATION ---------------------------
uint32_t TMP_Sensor_Init(I2C_TypeDef* I2Cx)
{
   uint8_t error_code;
   assert_param(IS_I2C_ALL_PERIPH(I2Cx));

   // update local variables
   tmp_I2Cx_g = I2Cx;

   // Set up timeout mechanism
   TIMEOUT_T3_USEC_Init();

   // check if the device is connected - no check for whoiam
   error_code = I2Cx_Is_Device_Connected(I2Cx, TMP_I2C_ADD, TMP_ID_REG, TMP_ID_VALUE, I2C_EVENT_WAIT);
   if(error_code){ return ERROR_TMP_CONNECTED_BASE + error_code;}

   // wait for 6 ms
   TIMEOUT_T3_USEC_Start();
   while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(6000));

   // update device status
   tmp_sensor.status = DEVICE_OK;
   
   return 0;
}


uint32_t TMP_Sensor_update(void)
{
   uint8_t data[2];
   uint8_t error_code;

   error_code = I2Cx_Recv_Bytes(tmp_I2Cx_g, TMP_I2C_ADD, TEMP_REG_ADDRESS, data, 2, I2C_EVENT_WAIT);
   if(error_code) { return ERROR_TMP_UPDATE_BASE + error_code; }

   // Calculate temperature based on updated data
   tmp_sensor.temprature = TEMP_SENSOR_Calculate_Value();
   
   return 0;
}


tmp_sensor_t TMP_Sensor_GetData(void)
{
   return tmp_sensor;
} 

// -- PRIVATE FUNCTIONS' IMPLEMENTATION ---------------------------
static float TEMP_SENSOR_Calculate_Value(void)
{
   float ret = 0;
      
   data[0] = data[0] & 0x1F; //Clear flag bits

   if ((data[0] & 0x10) == 0x10) //TA < 0�C
   {
      data[0] = data[0] & 0x0F; //Clear SIGN
      ret = 256 - (data[0] * 16 + (float)data[1] / 16);
   }
   else //TA � 0�C
   {
      ret = (data[0] * 16 + (float)data[1] / 16); //Temperature = Ambient Temperature (�C)
   }

   return ret;
}