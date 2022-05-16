#include "sun_sensor.h"

#include "../services_utilities/obc_comm.h"

static sun_sensor_t sun_sensor;


uint8_t SUN_Sensor_Init(void)
{
   
   return 0;
}

uint8_t SUN_Sensor_Update(void)
{
   // check for available solar panel readings
   if(OBC_Comm_GetFlags() & SLR_FLAG_BIT)
   {
      // get the new readings
      uint8_t tmp[SLR_NUM * 2];
      OBC_Comm_GetSLR(tmp);
      
      // parse the new data
      for(int i=0; i < SLR_NUM; i++)
      {
         sun_sensor.v_sun_raw[i] = ((uint16_t)tmp[2*i + 1] << 8) | tmp[2*i];
      }
      
      // translate raw reading into volts
      float volts_sum = 0;
      for(int i=0; i < SLR_NUM; i++)
      {
         sun_sensor.v_sun[i] = sun_sensor.v_sun_raw[i] * ADC2VOLTS_COEF;
         volts_sum += sun_sensor.v_sun[i];
      }
      
      // check for activation threshold
      if( volts_sum/SLR_NUM >= DAY_LIGHT_THRESHOLD )
      {
         xyz_t tmp = { .x = sun_sensor.v_sun[0] - sun_sensor.v_sun[1],   /* x */
                       .y = sun_sensor.v_sun[2] - sun_sensor.v_sun[3],   /* y */
                       .z = sun_sensor.v_sun[4] - sun_sensor.v_sun[5]    /* z */
                     };
         
         // translate volts to vector components
         // we assume here that the voltage is lineary proportional to
         // component intensity and that's a vulgar oversimplification
         // but we are running out of time so someone else may have to do it
         // x component
         sun_sensor.sun_xyz[0] = tmp.x;
         sun_sensor.sun_xyz[1] = tmp.y;
         sun_sensor.sun_xyz[2] = tmp.z;
      }
      // it's night time
      else
      {
         sun_sensor.sun_xyz[0] = 0;
         sun_sensor.sun_xyz[1] = 0;
         sun_sensor.sun_xyz[2] = 0;
      }
   }
   
   return NO_ERROR;
}

sun_sensor_t SUN_Sensor_GetData(void)
{
 return sun_sensor;  
}
