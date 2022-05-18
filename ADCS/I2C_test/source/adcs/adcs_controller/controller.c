#include "stdint.h"

// sensor modules
#include "../adcs_sensors/imu_sensor.h"
#include "../adcs_sensors/mgn_sensor.h"
#include "../adcs_sensors/sun_sensor.h"

#include "../services_utilities/common.h"

// actuators modules
#include "../adcs_actuators/mgn_torquer_driver.h"
#include "../adcs_actuators/spin_motor_driver.h"

// controller modules
#include "ref_vectors.h"
#include "sgp4.h"
#include "../config.h"
#include "wahbaRotM.h"

#include "controller.h"

//-- PRIVATE VARIABLES -----------------------
static imu_sensor_t imu_sensor_data;
static mgn_sensor_t mgn_sensor_data;
static sun_sensor_t sun_sensor_data;
static ref_vectors_t ref_vector_data;
static magneto_handle_t magneto_torquer_handle;

static WahbaRotMStruct control_WahbaRot;

static adcs_control_t control = { .b_dot[0] = 0, .b_dot[1] = 0, .b_dot[2] = 0,
                          .b_dot_prev[0] = 0, .b_dot_prev[1] = 0,
                          .b_dot_prev[2] = 0,.k_bdot = BDOT_GAIN,
                          .k_pointing[0] = POINTING_GAIN_W,
                          .k_pointing[1] = POINTING_GAIN_Q,
                          .sp_yaw = 0, .sp_pitch = 0, .sp_roll = 0,
                          .Ix = 0, .Iy = 0, .Iz = 0 ,
                          .k_spin = SPIN_TORQUER_GAIN,
                          .sp_rpm = 0, .const_rmp = SPIN_TORQUER_REF_RPM };

                          
static spin_handle_t spin_motor_handle;
        
//-- PRIVATE FUNCTIONS ------------------
static void spin_torquer_controller(float w, adcs_control_t *control_struct); 
static void b_dot(float b[3], float b_prev[3], float b_norm, adcs_control_t *control_struct);
static void pointing_controller(float b[3], float b_norm, WahbaRotMStruct *WStruct, adcs_control_t *control_struct);
                          

void CTRL_Control_Determination_Update(void)
{
   //-- update sensor values and status [Start] -----
   imu_sensor_data = IMU_Sensor_GetData();
   mgn_sensor_data = MGN_Sensor_GetData();
   sun_sensor_data = SUN_Sensor_GetData();
   ref_vector_data = CTRL_Ref_Vectors_GetData();
   //-- update sensor values and status [End] -------
   
   //-- update attitude determination [Start] -------
   
   // insert sun reference vector
   control_WahbaRot.w_a[0] = ref_vector_data.sun_vec.sun_pos_ned.x / ref_vector_data.sun_vec.norm;
   control_WahbaRot.w_a[1] = ref_vector_data.sun_vec.sun_pos_ned.y / ref_vector_data.sun_vec.norm;
   control_WahbaRot.w_a[2] = ref_vector_data.sun_vec.sun_pos_ned.z / ref_vector_data.sun_vec.norm;
   
   // insert geomagentic reference vector
   control_WahbaRot.w_m[0] = ref_vector_data.geomag_vec.Xm / ref_vector_data.geomag_vec.norm;
   control_WahbaRot.w_m[1] = ref_vector_data.geomag_vec.Ym / ref_vector_data.geomag_vec.norm;
   control_WahbaRot.w_m[2] = ref_vector_data.geomag_vec.Zm / ref_vector_data.geomag_vec.norm;
   
   /* Check if the sun sensor is available */
   if (sun_sensor_data.status == DEVICE_OK)
   {
      control_WahbaRot.sun_sensor_gain = 1;
   } 
   else 
   {
      /* Error in sun sensor */
      sun_sensor_data.sun_xyz[0] = 0;
      sun_sensor_data.sun_xyz[1] = 0;
      sun_sensor_data.sun_xyz[2] = 0;
      control_WahbaRot.sun_sensor_gain = 0;
   }
   
   float gyroscope[3] = { 0 };
   /* Check the magneto-meter sensor to run attitude algorithm */
   if (mgn_sensor_data.status == DEVICE_OK)
   {
      WahbaRotM( sun_sensor_data.sun_xyz, gyroscope,
             (float*)mgn_sensor_data.mag_filtered, &control_WahbaRot);
      control_WahbaRot.run_flag = 1;
   } 
   else if (imu_sensor_data.status == DEVICE_OK) 
   {
      WahbaRotM(sun_sensor_data.sun_xyz, gyroscope,
             imu_sensor_data.xm_filtered, &control_WahbaRot);
      control_WahbaRot.run_flag = 1;
   }
    
}

void CTRL_Control_Attitude_Update(void)
{
   float angular_velocities[3] = { 0 };
   /* Reset control signals */
   magneto_torquer_handle.current_z = 0;
   magneto_torquer_handle.current_y = 0;
   control.Iz = 0;
   control.Iy = 0;
   control.sp_rpm = 0;
   /* Choose the correct velocities */
   if (control_WahbaRot.run_flag == 0)
   {
      angular_velocities[0] = imu_sensor_data.gyro_filtered[0];
      angular_velocities[1] = imu_sensor_data.gyro_filtered[1];
      angular_velocities[2] = imu_sensor_data.gyro_filtered[2];
      control.sp_rpm = 0;
   }
   else 
   {
      angular_velocities[0] = control_WahbaRot.W[0];
      angular_velocities[1] = control_WahbaRot.W[1];
      angular_velocities[2] = control_WahbaRot.W[2];
      spin_torquer_controller(angular_velocities[1], &control);
   }
   /* Run B-dot and spin torquer controller if the angular velocities are bigger than thresholds */
   if (fabsf(angular_velocities[0]) > (float)WX_THRES
   || fabsf(angular_velocities[1]) > (float)WY_THRES
   || fabsf(angular_velocities[2]) > (float)WZ_THRES) 
   {
      /* Check the magneto-meter sensor */
      if (mgn_sensor_data.status == DEVICE_OK)
      {
         b_dot((float*)mgn_sensor_data.mag_filtered, (float*)mgn_sensor_data.prev_mag,
                 mgn_sensor_data.rm_norm, &control);
         /* Set the currents to magneto-torquers in mA*/
         magneto_torquer_handle.current_z = (int8_t) (control.Iz * 1000);
         magneto_torquer_handle.current_y = (int8_t) (control.Iy * 1000);
      } 
      else if (imu_sensor_data.status == DEVICE_OK)
      {
         b_dot(imu_sensor_data.xm_filtered, imu_sensor_data.xm_prev,
                 imu_sensor_data.xm_norm, &control);
         /* Set the currents to magneto-torquers in mA*/
         magneto_torquer_handle.current_z = (int8_t) (control.Iz * 1000);
         magneto_torquer_handle.current_y = (int8_t) (control.Iy * 1000);
      }
   /* If angular velocities are smaller than the thresholds then run pointing controller */
   }
   else
   {
      /* Run pointing controller if the sun sensor is available */
      if (sun_sensor_data.status == DEVICE_OK
      && control_WahbaRot.run_flag == 1
      && mgn_sensor_data.status == DEVICE_OK)
      {
         /* Run pointing controller when the sun sensor is available */
         pointing_controller((float*)mgn_sensor_data.mag_filtered, mgn_sensor_data.rm_norm, &control_WahbaRot, &control);
         /* Set the currents to magneto-torquers in mA*/
         magneto_torquer_handle.current_z = (int8_t) (control.Iz * 1000);
         magneto_torquer_handle.current_y = (int8_t) (control.Iy * 1000);
         b_dot((float*)mgn_sensor_data.mag_filtered, (float*)mgn_sensor_data.prev_mag, mgn_sensor_data.rm_norm, &control);
         /* Set the currents to magneto-torquers in mA*/
         magneto_torquer_handle.current_z += (int8_t) (control.Iz * 1000);
         magneto_torquer_handle.current_y += (int8_t) (control.Iy * 1000);
      }
   }
   /* Set spin torquer RPM */
   spin_motor_handle.RPM = control.const_rmp + control.sp_rpm;
   
   // reset wahba run flag
   control_WahbaRot.run_flag = 0;
   
   // Update actuators
   Magneto_Torquer_SetHandler(magneto_torquer_handle);
   Spin_Motor_SetHandler(spin_motor_handle);
}

static float rpm_in_prev = 0;
static float rpm_out_prev = 0;
static float rpm_sum = 0;

void spin_torquer_controller(float w, adcs_control_t *control_struct) 
 {
   rpm_in_prev = rpm_sum;
   rpm_out_prev = control_struct->sp_rpm;
   /* Integration of RPM */
   rpm_sum += (-(float) (control_struct->k_spin) * 0.001f)
         * (w * (float)RAD2RPM / (float)I_SPIN_TORQUER) * (float)LOOP_TIME;
   /* Check for saturation */
   if (rpm_sum > SATURATION_RPM)
   {
      rpm_sum = SATURATION_RPM;
   } 
   else if (rpm_sum < -SATURATION_RPM)
   {
      rpm_sum = -SATURATION_RPM;
   }
   /* Filter the output of RPM */
   control_struct->sp_rpm = rpm_sum - SPIN_TORQUER_FILTER_Z *
                        rpm_in_prev + SPIN_TORQUER_FILTER_P * rpm_out_prev;
   /* Check for saturation if output RPM */
   if (control_struct->sp_rpm > SATURATION_RPM)
   {
      control_struct->sp_rpm = SATURATION_RPM;
   }
   else if (control_struct->sp_rpm < -SATURATION_RPM)
   {
      control_struct->sp_rpm = -SATURATION_RPM;
   }
}

void b_dot(float b[3], float b_prev[3], float b_norm, adcs_control_t *control_struct)
{
   float b_dot_x, b_dot_y, b_dot_z = 0;

   control_struct->b_dot_prev[0] = control_struct->b_dot[0];
   control_struct->b_dot_prev[1] = control_struct->b_dot[1];
   control_struct->b_dot_prev[2] = control_struct->b_dot[2];
   /* Calculate B-dot */
   control_struct->b_dot[0] = (b[0] - b_prev[0]) / LOOP_TIME;
   control_struct->b_dot[1] = (b[1] - b_prev[1]) / LOOP_TIME;
   control_struct->b_dot[2] = (b[2] - b_prev[2]) / LOOP_TIME;
   /* Moving average for B-dot */
   b_dot_x = BDOT_FILTER * control_struct->b_dot[0]
      + (1 - BDOT_FILTER) * control_struct->b_dot_prev[0];
   b_dot_y = BDOT_FILTER * control_struct->b_dot[1]
      + (1 - BDOT_FILTER) * control_struct->b_dot_prev[1];
   b_dot_z = BDOT_FILTER * control_struct->b_dot[2]
      + (1 - BDOT_FILTER) * control_struct->b_dot_prev[2];
   /* Calculate the currents of coils in A */
   control_struct->Ix = -((float) (control_struct->k_bdot) * 0.1f / A_COIL) * b_dot_x
      * (1 / b_norm);
   control_struct->Iy = -((float) (control_struct->k_bdot) * 0.1f / A_COIL) * b_dot_y
      * (1 / b_norm);
   control_struct->Iz = -((float) (control_struct->k_bdot) * 0.1f / A_COIL) * b_dot_z
      * (1 / b_norm);

}
        
void pointing_controller(float b[3], float b_norm, WahbaRotMStruct *WStruct, adcs_control_t *control_struct)
{
   float m_w[3], m_q[3] = { 0 };
   Quat4 sp_quat;
   float sp_rotm[3][3], rotm[3][3] = { 0 };
   float sp_euler[3] = { 0 };

   /* Convert set points to quaternions */
   sp_euler[0] = 0;
   sp_euler[1] = (float) RAD(control_struct->sp_pitch);
   sp_euler[2] = 0;
   euler2rotm((float *) rotm, (const float *) sp_euler);
   mulMatr(sp_rotm, rotm, WStruct->RotM);
   rotmtx2quat((const float *)sp_rotm, &sp_quat);
   /* Calculate control signal from angular velocities */
   m_w[0] = -(float) control_struct->k_pointing[0] * 0.01f
         * (b[1] * WStruct->W[2] - b[2] * WStruct->W[1]);
   m_w[1] = -(float) control_struct->k_pointing[0] * 0.01f
         * (b[0] * WStruct->W[2] - b[2] * WStruct->W[0]);
   m_w[2] = -(float) control_struct->k_pointing[0] * 0.01f
         * (b[0] * WStruct->W[1] - b[1] * WStruct->W[0]);

   /* Calculate control signal from pitch set point */
   m_q[0] = -(float) control_struct->k_pointing[1] * 0.01f
         * (b[1] * sp_quat.z - b[2] * sp_quat.y);
   m_q[1] = -(float) control_struct->k_pointing[1] * 0.01f
         * (b[0] * sp_quat.z - b[2] * sp_quat.x);
   m_q[2] = -(float) control_struct->k_pointing[1] * 0.01f
         * (b[0] * sp_quat.y - b[1] * sp_quat.x);

   /* Convert to current in A */
   control_struct->Ix = (m_q[0] + m_w[0]) / b_norm;
   control_struct->Iy = (m_q[1] + m_w[1]) / b_norm;
   control_struct->Iz = (m_q[2] + m_w[2]) / b_norm;
}
