#include "controller.h"



//-- PRIVATE VARIABLES -----------------------
static gps_sensor_t gps_sensor_data;
static imu_sensor_t imu_sensor_data;
static tmp_sensor_t tmp_sensor_data;
static mgn_sensor_t mgn_sensor_data;
static time_t time_data;
static ref_vectors_t ref_vector_data;

void CTRL_Control_Init(void)
{
   
   
}

void CTRL_Control_Update(void)
{
   //-- update sensor values and status [Start] -----
   gps_sensor_data = GPS_Sensor_GetData();
   imu_sensor_data = IMU_Sensor_GetData();
   tmp_sensor_data = TMP_Sensor_GetData();
   mgn_sensor_data = MGN_Sensor_GetData();
   time_data = time_getTime();
   ref_vector_data = Ref_Vectors_GetData();
   //-- update sensor values and status [End] -------
   
   //-- update attitude determination [Start] -------
   WahbaRotMStruct WahbaRot;
   
   // insert sun reference vector
   WahbaRot.w_a[0] = ref_vector_data.sun_vec.sun_pos_ned.x / ref_vector_data.sun_vec.norm;
   WahbaRot.w_a[1] = ref_vector_data.sun_vec.sun_pos_ned.y / ref_vector_data.sun_vec.norm;
   WahbaRot.w_a[2] = ref_vector_data.sun_vec.sun_pos_ned.z / ref_vector_data.sun_vec.norm;
   
   // insert geomagentic reference vector
   WahbaRot.w_m[0] = ref_vector_data.geomag_vec.Xm / ref_vector_data.geomag_vec.norm;
   WahbaRot.w_m[1] = ref_vector_data.geomag_vec.Ym / ref_vector_data.geomag_vec.norm;
   WahbaRot.w_m[2] = ref_vector_data.geomag_vec.Zm / ref_vector_data.geomag_vec.norm;
   
   /* Check if the sun sensor is available */
   if (adcs_sensors.sun.sun_status == DEVICE_ENABLE)
   {
      WahbaRot.sun_sensor_gain = 1;
   } 
   else if (adcs_sensors.sun.sun_status == DEVICE_DISABLE) 
   {
      /* Propagate the sun sensor vector */
      WahbaRot.sun_sensor_gain = 0.1;
      mulMatrVec(adcs_sensors.sun.sun_xyz, WahbaRot.RotM, WahbaRot.w_a);
   } 
   else 
   {
      /* Error in sun sensor */
      adcs_sensors.sun.sun_xyz[0] = 0;
      adcs_sensors.sun.sun_xyz[1] = 0;
      adcs_sensors.sun.sun_xyz[2] = 0;
      WahbaRot.sun_sensor_gain = 0;
   }
   /* Check magneto-meter values according to norm of IGRF */
   if (adcs_sensors.mgn.rm_norm > MAX_IGRF_NORM
   || adcs_sensors.mgn.rm_norm < MIN_IGRF_NORM) 
   {
      adcs_sensors.mgn.rm_status = DEVICE_ERROR;
      error_status_value = ERROR_SENSOR;
   }
   if (adcs_sensors.imu.xm_norm > MAX_IGRF_NORM
   || adcs_sensors.imu.xm_norm < MIN_IGRF_NORM) 
   {
      adcs_sensors.mgn.rm_status = DEVICE_ERROR;
      error_status_value = ERROR_SENSOR;
   }
   /* Check the magneto-meter sensor to run attitude algorithm */
   if ((adcs_sensors.mgn.rm_status == DEVICE_NORMAL
   && adcs_sensors.imu.xm_status == DEVICE_NORMAL)
   || adcs_sensors.mgn.rm_status == DEVICE_NORMAL) 
   {
      WahbaRotM(adcs_sensors.sun.sun_xyz, gyroscope,
             adcs_sensors.mgn.rm_f, &WahbaRot);
      WahbaRot.run_flag = true;
   } 
   else if (adcs_sensors.imu.xm_status == DEVICE_NORMAL) 
   {
      WahbaRotM(adcs_sensors.sun.sun_xyz, gyroscope,
             adcs_sensors.imu.xm_f, &WahbaRot);
      WahbaRot.run_flag = true;
   }
    
   // update attitude control
   
   
   
}
