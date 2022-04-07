#ifndef CONTROLLER_H__
#define CONTROLLER_H__

#include "stdint.h"

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

void CTRL_Control_Determination_Update(void);
void CTRL_Control_Attitude_Update(void);


#endif
