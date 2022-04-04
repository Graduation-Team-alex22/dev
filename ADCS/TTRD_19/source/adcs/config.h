#ifndef CONFIG_H__
#define CONFIG_H__



//---PUBLIC IDENTIFIERS--------------------------
#define MAX_IGRF_NORM   0.8    // in guass, to check if the magneto-meter values are valid
#define MIN_IGRF_NORM   0.1   // in guass, to check if the magneto-meter values are valid

/* LP Filter */
#define A_GYRO ((float)0.7)    // Coefficient < 1.0
#define A_MGN  ((float)0.7)    // Coefficient < 1.0
#define A_XM   ((float)0.7)    // Coefficient < 1.0

#define WX_THRES        RAD(0.3f) // Threshold to enable B-dot controller (in deg/s)
#define WY_THRES        RAD(0.3f) // Threshold to enable B-dot controller (in deg/s)
#define WZ_THRES        RAD(0.3f) // Threshold to enable B-dot controller (in deg/s)

#define BDOT_GAIN            10     // Gain of B-dot controller, *0.1Nms
#define POINTING_GAIN_W      1      // Point gain kp1
#define POINTING_GAIN_Q      20     // Point gain kp2
#define SPIN_TORQUER_GAIN    1      // Spin torquer gain, *0.001Nms
#define SPIN_TORQUER_REF_RPM 5000   // Reference RMP in rmp

#define A_COIL                1.55f     // in m^2 = nA, n: number of turns, A: area of one turn
#define BDOT_FILTER           0.5f      // is 0 < BDOT_FILTER < 1
#define I_SPIN_TORQUER        2.12E-07f // Fly wheel moment of inertia kg*m^2
#define SATURATION_RPM        2000     // Limit for RPM of spin torquer
#define SPIN_TORQUER_FILTER_P 0.99f     // Pole in spin torquer filter
#define SPIN_TORQUER_FILTER_Z 1        // Zero in spin torquer filter

#define LOOP_TIME           1       // in sec

#endif
