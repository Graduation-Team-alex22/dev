#ifndef SPIN_DRIVER_H__
#define	SPIN_DRIVER_H__

#include "adcs_actuators.h"

/* Spin Torquer */
#define SPIN_ID         0x03
#define SPIN_TIMEOUT    10000
#define MAX_RPM         15000
#define CNT2RPM(x)      48000000*15/(x)
#define RPM2CNT(x)      CNT2RPM(x)

typedef enum {
    MOTOR_ERROR = 0,
    MOTOR_INSYNC,				// In sync to bridge pulses, but not locked  yet to reference RPM 
    MOTOR_LOCKED,				// In Synchronous mode to  reference RPM
    MOTOR_STALL,				// MOTOR stalled (that is bad, unless RPMr <100, MOTOR_STALL)
    MOTOR_STOP
} spin_status_e;

typedef struct {
    int32_t RPM;
    uint32_t rampTime;
    uint32_t crc;
    int32_t m_RPM;
    spin_status_e status;
} spin_handle_t;

spin_status_e spin_torquer_init(spin_handle_t *pSpin_handle);
spin_status_e spin_state_get(spin_handle_t *pSpin_handle);
spin_status_e spin_torquer_update(spin_handle_t *pSpin_handle);

#endif
