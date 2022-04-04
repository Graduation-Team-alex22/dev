#ifndef CONTROLLER_H__
#define CONTROLLER_H__

#include "stdint.h"

// controller modules
#include "frame.h"
#include "ref_vectors.h"
#include "sgp4.h"
#include "wahbaRotM.h"

// sensor modules
#include "../adcs_sensors/gps_sensor.h"
#include "../adcs_sensors/imu_sensor.h"
#include "../adcs_sensors/mgn_sensor.h"
#include "../adcs_sensors/tmp_sensor.h"
#include "../adcs_sensors/sun_sensor.h"

#include "../services_utilities/time.h"
#include "../services_utilities/common.h"

// actuators modules
#include "../adcs_actuators/mgn_torquer_driver.h"
#include "../adcs_actuators/spin_motor_driver.h"




typedef struct {
   /* B-dot Controller */
   float b_dot[3];
   float b_dot_prev[3];
   uint16_t k_bdot;
   /* Pointing controller */
   uint16_t k_pointing[2];
   int16_t sp_yaw;
   int16_t sp_pitch;
   int16_t sp_roll;
   /* Control output for magneto torquers */
   float Ix, Iy, Iz;
   /* Spin torquer control */
   uint16_t k_spin;
   float sp_rpm;
   int32_t const_rmp;
} adcs_control_t;

void CTRL_Control_Init(void);
void CTRL_Control_Determination_Update(void);
void CTRL_Control_Attitude_Update(void);

void spin_torquer_controller(float w, adcs_control_t *control_struct); 
void b_dot(float b[3], float b_prev[3], float b_norm, adcs_control_t *control_struct);
void pointing_controller(float b[3], float b_norm, WahbaRotMStruct *WStruct, adcs_control_t *control_struct);


#endif
