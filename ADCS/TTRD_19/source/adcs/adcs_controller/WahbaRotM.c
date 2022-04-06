/*
 * WahbaRotM.c//
 *
 *  Created on: Jul 20, 2014
 *      Author: mobintu
 *
 *-------------------------------
 *  Created on: Apr 6, 2022
 *      Author: Mohamed Said & Ali Fakharany
 *
 */
#include "WahbaRotM.h"

#define SATUR(x,Lim) (x>Lim)?(Lim):((x<-Lim)?(-Lim):x)
#define SATUR2(x,Low,High) (x>High)?(High):((x<Low)?(Low):x)

//static inline void dRpropagat(real (*R)[3], real (*dR)[3], real w[3], real dt);
//static inline void mulMatrTrMatr(real (*A)[3], real (*B)[3], real (*C)[3]);
static inline void mulMatrVec(real *v, real (*A)[3], real *x);
//static inline void mulMatrTrVec(real *v, real (*A)[3], real *x);


/**
 * @brief  Matrix multiplication A=BxC
 * @param  A,B,C real 3x3 matrices, A will be overwritten with the result BxC
 * @retval None
 */
void mulMatr(real (*A)[3], real (*B)[3], real (*C)[3]) {
    int i;
    for (i = 0; i < 3; i++) {
        A[0][i] = B[0][0] * C[0][i] + B[0][1] * C[1][i] + B[0][2] * C[2][i];
        A[1][i] = B[1][0] * C[0][i] + B[1][1] * C[1][i] + B[1][2] * C[2][i];
        A[2][i] = B[2][0] * C[0][i] + B[2][1] * C[1][i] + B[2][2] * C[2][i];
    }
}

/**
 * @brief  Matrix multiplication A=BxC^T
 * @param  A,B,C real 3x3 matrices, A will be overwritten with the result BxC^T
 * @retval None
 */
void mulMatrMatrTr(real (*A)[3], real (*B)[3], real (*C)[3]) {
    int i;
    for (i = 0; i < 3; i++) {
        A[0][i] = B[0][0] * C[i][0] + B[0][1] * C[i][1] + B[0][2] * C[i][2];
        A[1][i] = B[1][0] * C[i][0] + B[1][1] * C[i][1] + B[1][2] * C[i][2];
        A[2][i] = B[2][0] * C[i][0] + B[2][1] * C[i][1] + B[2][2] * C[i][2];
    }
}

/**
 * @brief  Matrix multiplication A=B^TxC
 * @param  A,B,C real 3x3 matrices, A will be overwritten with the result BxC^T
 * @retval None
 */
/*
static inline void mulMatrTrMatr(real (*A)[3], real (*B)[3], real (*C)[3]) {
    int i;
    for (i = 0; i < 3; i++) {
        A[i][0] = B[0][0] * C[i][0] + B[0][1] * C[i][1] + B[0][2] * C[i][2];
        A[i][1] = B[1][0] * C[i][0] + B[1][1] * C[i][1] + B[1][2] * C[i][2];
        A[i][2] = B[2][0] * C[i][0] + B[2][1] * C[i][1] + B[2][2] * C[i][2];
    }
}
*/
/**
 * @brief  Matrix - Vector multiplication v=Ax
 * @param  v,x vectors of length 3, A real 3x3 matrix
 * @retval None
 */
static inline void mulMatrVec(real *v, real (*A)[3], real *x) {

    v[0] = A[0][0] * x[0] + A[0][1] * x[1] + A[0][2] * x[2];
    v[1] = A[1][0] * x[0] + A[1][1] * x[1] + A[1][2] * x[2];
    v[2] = A[2][0] * x[0] + A[2][1] * x[1] + A[2][2] * x[2];
}

/**
 * @brief  Matrix Transpose - Vector multiplication v=A'x
 * @param  v,x vectors of length 3, A real 3x3 matrix
 * @retval None
 */
static inline void mulMatrTrVec(real *v, real (*A)[3], real *x) {

    v[0] = A[0][0] * x[0] + A[1][0] * x[1] + A[2][0] * x[2];
    v[1] = A[0][1] * x[0] + A[1][1] * x[1] + A[2][1] * x[2];
    v[2] = A[0][2] * x[0] + A[1][2] * x[1] + A[2][2] * x[2];
}

/**
 * Calculate rotation matrix from euler angles, rotation order ZYX
 * @param rotm
 * @param euler
 */
void euler2rotm(float *rotm, const float *euler) {

    rotm[0] = cosv(euler[0]) * cosv(euler[1]);
    rotm[1] = cosv(euler[0]) * sinv(euler[1]) * sinv(euler[2]) - cosv(euler[2])
            * sinv(euler[0]);
    rotm[2] = sinv(euler[0]) * sinv(euler[2]) + cosv(euler[0]) * cosv(euler[2])
            * sinv(euler[1]);

    rotm[3] = cosv(euler[1]) * sinv(euler[0]);
    rotm[4] = cosv(euler[0]) * cosv(euler[2]) + sinv(euler[0]) * sinv(euler[1])
            * sinv(euler[2]);
    rotm[5] = cosv(euler[2]) * sinv(euler[0]) * sinv(euler[1]) - cosv(euler[0])
            * sinv(euler[2]);

    rotm[6] = - sinv(euler[1]);
    rotm[7] = cosv(euler[1]) * sinv(euler[2]);
    rotm[8] = cosv(euler[1]) * cosv(euler[2]);

}

/**
 * Calculate euler angles from rotation matrix, rotation order ZYX
 * @param rotmtx
 * @param euler
 */
static inline void rotmtx2euler(const float *rotmtx, float *euler) {

    const float tol = 1E-6;
    const float test = sqrt(rotmtx[0] * rotmtx[0] + rotmtx[3] * rotmtx[3]);

        if (test >= +tol) {
            euler[0] = atan2(rotmtx[3], rotmtx[0]);
            euler[1] = atan2(-rotmtx[6], test);
            euler[2] = atan2f(rotmtx[7], rotmtx[8]);
        } else {
            euler[0] = 0;
            euler[1] = atan2f(-rotmtx[6], test);
            euler[2] = atan2f(-rotmtx[5], rotmtx[4]);
        }
}

/**
 * @brief  Matrix derivative
 * @param  A real 3x3 matrix
 * @retval real det(A)
 */
real detMatr(real (*A)[3]) {
    return A[0][0] * A[1][1] * A[2][2] + A[0][1] * A[1][2] * A[2][0]
            + A[0][2] * A[1][0] * A[2][1]
            - (A[0][2] * A[1][1] * A[2][0] + A[0][1] * A[1][0] * A[2][2]
            + A[0][0] * A[1][2] * A[2][1]);
}

/**
 * @brief  Vector Norm
 * @param  V 3x1 real vector
 * @retval real ||V||
 */
static inline real normV(real V[3]) {
    return sqrtv(V[0] * V[0] + V[1] * V[1] + V[2] * V[2]);
}

/**
 * @brief  Vector Norm difference
 * @param  A,V 3x1 real vector
 * @retval real ||A-V||
 */
/*
static inline real normVd(real A[3], real B[3]) {
    real V[3];
    V[0] = A[0] - B[0];
    V[1] = A[1] - B[1];
    V[2] = A[2] - B[2];
    return sqrtv(V[0] * V[0] + V[1] * V[1] + V[2] * V[2]);
}*/

/**
 * @brief  Vector Dot product
 * @param  A,B 3x1 real vector
 * @retval real A*B
 */
/*static inline real dot(real A[3], real B[3]) {
    return sqrtv(A[0] * B[0] + A[1] * B[1] + A[2] * B[2]);
}*/

static inline void dRpropagat(real (*Rbe)[3], real (*R)[3], real *wb, real dt) {
    real norm, th, c, s, u;
    real w[3];
    real dR[3][3];

    mulMatrTrVec(w, R, wb); // w body to earth;

    norm = normV(w);
    if (norm < (real)1e-6) {
        for (int i = 0; i < 3; i++) {
            dR[0][i] = 0;
            dR[1][i] = 0;
            dR[2][i] = 0;
        }
        dR[0][0] = 1;
        dR[1][1] = 1;
        dR[2][2] = 1;
    } else {
        w[0] = w[0] / norm;
        w[1] = w[1] / norm;
        w[2] = w[2] / norm;

        th = norm * dt;
        c = cosv(th);
        s = sinv(th);
        u = 1 - c;

        dR[0][0] = w[0] * w[0] * u + c;
        dR[0][1] = w[0] * w[1] * u - w[2] * s;
        dR[0][2] = w[0] * w[2] * u + w[1] * s;
        dR[1][0] = w[1] * w[0] * u + w[2] * s;
        dR[1][1] = w[1] * w[1] * u + c;
        dR[1][2] = w[1] * w[2] * u - w[0] * s;
        dR[2][0] = w[2] * w[0] * u - w[1] * s;
        dR[2][1] = w[2] * w[1] * u + w[0] * s;
        dR[2][2] = w[2] * w[2] * u + c;

        mulMatrMatrTr(Rbe, R, dR);
    }
}

/*static inline void dRpropagatInfinite(real (*Rbe)[3], real (*R)[3], real *wb, real dt) {
    for (int i = 0; i < 3; i++) {
        Rbe[0][i] = 0 * R[0][i] + wb[2] * R[1][i] - wb[1] * R[2][i];
        Rbe[1][i] = -wb[2] * R[0][i] + 0 * R[1][i] + wb[0] * R[2][i];
        Rbe[2][i] = wb[1] * R[0][i] - wb[0] * R[1][i] + 0 * R[2][i];
    }
}
*/

/**
 * Calculate quaternions from rotation matrix, [x y z w].
 * @param rotmtx
 * @param q
 */
void rotmtx2quat(const float *rotmtx, Quat4 *q) {
    q->z = 0.0f;
    float sqtrp1;
    float sqtrp1x2;
    float d[3];
    float sqdip1;

    const float tr = rotmtx[0] + rotmtx[4] + rotmtx[8];
    if (tr > 1e-6f) {
        sqtrp1 = sqrtv(tr + 1.0f);
        sqtrp1x2 = (float)2.0 * sqtrp1;

        q->w = 0.5f * sqtrp1;
        q->x = (rotmtx[7] - rotmtx[5]) / sqtrp1x2;
        q->y = (rotmtx[2] - rotmtx[6]) / sqtrp1x2;
        q->z = (rotmtx[3] - rotmtx[1]) / sqtrp1x2;
    } else {
        d[0] = rotmtx[0];
        d[1] = rotmtx[4];
        d[2] = rotmtx[8];

        if ((d[1] > d[0]) && (d[1] > d[2])) {
            sqdip1 = sqrtv(d[1] - d[0] - d[2] + 1.0f);
            q->y = 0.5f * sqdip1;

            if (fabsf(sqdip1) > 1e-6f)
                sqdip1 = 0.5f / sqdip1;

            q->w = (rotmtx[2] - rotmtx[6]) * sqdip1;
            q->x = (rotmtx[3] + rotmtx[1]) * sqdip1;
            q->z = (rotmtx[7] + rotmtx[5]) * sqdip1;
        } else if (d[2] > d[0]) {
            //max value at R(3,3)
            sqdip1 = sqrtv(d[2] - d[0] - d[1] + 1.0f);

            q->z = 0.5f * sqdip1;

            if (fabsf(sqdip1) > 1e-6f)
                sqdip1 = 0.5f / sqdip1;

            q->w = (rotmtx[3] - rotmtx[1]) * sqdip1;
            q->x = (rotmtx[2] + rotmtx[6]) * sqdip1;
            q->y = (rotmtx[7] + rotmtx[5]) * sqdip1;
        } else {
            // max value at R(1,1)
            sqdip1 = sqrtv(d[0] - d[1] - d[2] + 1.0f);

            q->x = 0.5f * sqdip1;

            if (fabsf(sqdip1) > 1e-6f)
                sqdip1 = 0.5f / sqdip1;

            q->w = (rotmtx[7] - rotmtx[5]) * sqdip1;
            q->y = (rotmtx[3] + rotmtx[1]) * sqdip1;
            q->z = (rotmtx[2] + rotmtx[6]) * sqdip1;
        }
    }

}


WahbaRotMStruct WahbaRot;

//int dsvd(float (*a)[3], int m, int n, float w[], float (*v)[3]);

void initWahbaStruct(WahbaRotMStruct *WStruct, real dt) {

    WStruct->w_m[0] = 0;
    WStruct->w_m[1] = 0;
    WStruct->w_m[2] = 0;

    WStruct->w_a[0] = 0;
    WStruct->w_a[1] = 0;
    WStruct->w_a[2] = 0;

    WStruct->w_g[0] = -0.707;
    WStruct->w_g[1] = 0.0;
    WStruct->w_g[2] = 0.707;

    WStruct->dt = dt;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            WStruct->RotM[i][j] = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            WStruct->RotM_prev[i][j] = 0;
    for (int i = 0; i < 3; i++)
        WStruct->RotM[i][i] = 1;
    for (int i = 0; i < 3; i++)
        WStruct->RotM_prev[i][i] = 1;

    for (int i = 0; i < 3; i++) {
        WStruct->g_va[i] = 0;
        WStruct->g_vg[i] = 0;
        WStruct->g_vm[i] = 0;
    }
}

volatile real RtM[3][3] = { { 0.99756, 0.0697, 0 }, { -0.0697, 0.99756, 0 }, {
        0, 0, 1.0 } };
volatile real Rr[3][3];

void WahbaRotM(float acc[3], float gyr[3], float mag[3], WahbaRotMStruct *WStruct) {
    static float a, ga, gm, m, A[3][3], dt; // g,
    real (*RotM)[3];
    a = WStruct->sun_sensor_gain;
    m = 1;
    // g = 0.0;
    ga = 1 - a;
    gm = 1 - m;

    static float *w_a, *w_m, *g_va, *g_vm; //,*g_vg,AccErr;*w_g,
    w_a = WStruct->w_a;
    // w_g = WStruct->w_g;
    w_m = WStruct->w_m;
    RotM = WStruct->RotM;
    g_va = WStruct->g_va;
    // g_vg=WStruct->g_vg;
    g_vm = WStruct->g_vm;

    dt = WStruct->dt;

    static real Rbe[3][3];

    real accn[3], magn[3], An, Mn;
    An = normV(acc);
    if (An == 0) { An = 1; }
    Mn = normV(mag);
    if (Mn == 0) { Mn = 1; }
    for (int i = 0; i < 3; i++) {
        accn[i] = acc[i] / An;
        magn[i] = mag[i] / Mn;
    }

    for (int i = 0; i < 3; i++) {
        A[i][0] = a * accn[0] * w_a[i] + m * magn[0] * w_m[i]
                + ga * g_va[0] * w_a[i] + gm * g_vm[0] * w_m[i];
        A[i][1] = a * accn[1] * w_a[i] + m * magn[1] * w_m[i]
                + ga * g_va[1] * w_a[i] + gm * g_vm[1] * w_m[i];
        A[i][2] = a * accn[2] * w_a[i] + m * magn[2] * w_m[i]
                + ga * g_va[2] * w_a[i] + gm * g_vm[2] * w_m[i];
    }

    WahbaJacobiSVDRotM(A, RotM);
    dRpropagat(Rbe, RotM, gyr, dt);

    mulMatrVec(g_va, Rbe, w_a);
    mulMatrVec(g_vm, Rbe, w_m);

    //mulMatr(Rr,RtM,Rbe);
    //Conversion to euler takes 7-8us...
    rotmtx2euler((const float *) RotM, (float *) WStruct->Euler);
    rotmtx2quat((const float *) RotM, (Quat4 *) WStruct->q);

    // Calculate W from dR
    static real tmp;
    static real tr;

    mulMatrMatrTr(WStruct->dR, RotM, WStruct->RotM_prev);

    tr = ((WStruct->dR[0][0] + WStruct->dR[1][1] + WStruct->dR[2][2]) - 1.0f) / 2.0f;
    tr = SATUR(tr, 1);
    tmp = acosv(tr);
    WStruct->dth = tmp;
    if (fabsv(tmp) < TOL) {
        tmp = 0.5f / dt;  // x/sin(x)|x->0
    } else {
        tmp = 0.5f / dt * tmp / sinv(tmp);
    }
    WStruct->W[0] = tmp * (WStruct->dR[2][1] - WStruct->dR[1][2]);
    WStruct->W[1] = tmp * (WStruct->dR[0][2] - WStruct->dR[2][0]);
    WStruct->W[2] = tmp * (WStruct->dR[1][0] - WStruct->dR[0][1]);

    for (int i = 0; i < 3; i++) {
        WStruct->RotM_prev[0][i] = RotM[0][i];
        WStruct->RotM_prev[1][i] = RotM[1][i];
        WStruct->RotM_prev[2][i] = RotM[2][i];
    }
}

