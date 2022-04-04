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

#include "../services_utilities/time.h"

void CTRL_Control_Init(void);
void CTRL_Control_Update(void);



#endif
