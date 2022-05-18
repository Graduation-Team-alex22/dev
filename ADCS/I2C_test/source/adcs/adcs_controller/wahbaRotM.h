#ifndef WAHBAROTM_H__
#define	WAHBAROTM_H__

#define ARM_MATH_CM4
#include "stm32f4xx.h"
#include "arm_math.h"
#include "../services_utilities/common.h"

#define TOL 1e-6f

static inline float arm_sqrt(float in) {
    float out;
    arm_sqrt_f32(in, &out);
    return out;
}

#define SIGN(x) (((x) > 0) - ((x) < 0))

//#define USE_DOUBLE
#ifdef USE_DOUBLE

typedef double real;
#define sqrtv sqrt
#define sinv sin
#define asinv asin
#define cosv cos
#define acosv acos
#define atan2v atan2
#define fabsv fabs
#else

typedef float real;
#define sqrtv(x) arm_sqrt(x)
#define sinv    sinf
#define asinv   asinf
#define cosv    cosf
#define acosv   acosf
#define atan2v  atan2f
#define fabsv   fabsf
#define powv    powf
#endif

typedef struct {
    real x;
    real y;
    real z;
    real w;
} Quat4; //17bytes

typedef struct {
    real w_a[3];
    real w_g[3];
    real w_m[3];
    real g_vg[3];
    real g_va[3];
    real g_vm[3];
    real q[4];
    real sun_sensor_gain;
    float RotM[3][3];
    real RotM_prev[3][3];
    real dR[3][3];
    real dth;
    real W[3];
    float Euler[3];
    float AccErr;
    float dt;
    uint8_t run_flag;
} WahbaRotMStruct;

extern WahbaRotMStruct WahbaRot;

void initWahbaStruct(WahbaRotMStruct *WStruct, real dt);
void WahbaRotM(float acc[], float gyr[], float mag[], WahbaRotMStruct *WStruct);
void WahbaJacobiSVDRotM(real (*A)[3], real (*RotM)[3]);

void jacobi2Sided(real (*A)[3], real (*U)[3], real (*V)[3]);
void apply_jacobi_R(real (*A)[3], int p, int q, real c, real s, real k);
void apply_jacobi_L(real (*A)[3], int p, int q, real c, real s, real k);

real detMatr(real (*A)[3]);
void mulMatrMatrTr(real (*A)[3], real (*B)[3], real (*C)[3]);
void euler2rotm(float *rotm, const float *euler);
void mulMatr(real (*A)[3], real (*B)[3], real (*C)[3]);
void rotmtx2quat(const float *rotmtx, Quat4 *q);


#endif
