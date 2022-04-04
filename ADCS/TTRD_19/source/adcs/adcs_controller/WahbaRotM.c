/*
 * WahbaRotM.c//
 *
 *  Created on: Jul 20, 2014
 *      Author: mobintu
 */
#include "WahbaRotM.h"
#include "stdio.h"

#define SATUR(x,Lim) (x>Lim)?(Lim):((x<-Lim)?(-Lim):x)
#define SATUR2(x,Low,High) (x>High)?(High):((x<Low)?(Low):x)

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
    static float a, g, ga, gm, m, A[3][3], dt;
    real (*RotM)[3];
    a = WStruct->sun_sensor_gain;
    m = 1;
    g = 0.0;
    ga = 1 - a;
    gm = 1 - m;

    static float AccErr, *w_a, *w_g, *w_m, *g_va, *g_vm; //,*g_vg;
    w_a = WStruct->w_a;
    w_g = WStruct->w_g;
    w_m = WStruct->w_m;
    RotM = WStruct->RotM;
    g_va = WStruct->g_va;
    //g_vg=WStruct->g_vg;
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
//	for(int i=0;i<3;i++){
//		A[i][0]=a*accn[0]*w_a[i] + m*magn[0]*w_m[i];
//		A[i][1]=a*accn[1]*w_a[i] + m*magn[1]*w_m[i];
//		A[i][2]=a*accn[2]*w_a[i] + m*magn[2]*w_m[i];
//	}
    //	real A1[3][3]={{1.21,3.214,0.1},{0,0,0},{2.12,0.120,0.43}};
    //real A1[3][3]={{1.001,2,3},{0,0,0},{2,4,6}}; //check svd

    WahbaJacobiSVDRotM(A, RotM);
    dRpropagat(Rbe, RotM, gyr, dt);

//	mulMatrTrVec(w_m,Rbe,magn);
//	w_m[0]=sqrtv(w_m[0]*w_m[0]+w_m[1]*w_m[1]);
//	w_m[1]=0;

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

    tr = ((WStruct->dR[0][0] + WStruct->dR[1][1] + WStruct->dR[2][2]) - 1.0) / 2.0;
    tr = SATUR(tr, 1);
    tmp = acosv(tr);
    WStruct->dth = tmp;
    if (fabsv(tmp) < TOL) {
        tmp = 0.5 / dt;  // x/sin(x)|x->0
    } else {
        tmp = 0.5 / dt * tmp / sinv(tmp);
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

