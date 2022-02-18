/*
 * WahbaRotM.h
 *
 *  Created on: Jul 20, 2014
 *      Author: mobintu
 */

#ifndef WAHBAROTM_H_
#define WAHBAROTM_H_


#include "arm_math.h"
//#include "math.h"

#define TOL 1e-6


static inline float arm_sqrt(float in)
{
	float out;
	arm_sqrt_f32(in,&out);
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
#define sinv 	sinf
#define asinv 	asinf
#define cosv 	cosf
#define acosv 	acosf
#define atan2v	atan2f
#define fabsv 	fabsf
#define powv	powf
#endif

typedef struct {
	real x;
	real y;
	real z;
	real w;
}Quat4; //17bytes

typedef struct {
	real w_a[3];
	real w_g[3];
	real w_m[3];
	real g_vg[3];
	real g_va[3];
	real g_vm[3];
	real q[4];
	float RotM[3][3];
	real RotM_prev[3][3];
	real dR[3][3];
	real dth;
	real W[3];
	float Euler[3];
	float AccErr;
	float dt;
}WahbaRotMStruct;

void initWahbaStruct(WahbaRotMStruct *WStruct,real dt);
void WahbaRotM(float acc[],float gyr[],float mag[],WahbaRotMStruct *WStruct);
void WahbaJacobiSVDRotM(real (*A)[3], real (*RotM)[3]);

void jacobi2Sided(real (*A)[3],real (*U)[3],real (*V)[3]);
void apply_jacobi_R (real (*A)[3],int p,int q,real c,real s,real k);
void apply_jacobi_L (real (*A)[3],int p,int q,real c,real s,real k);

/*
static inline void dRpropagat(real (*R)[3],real (*dR)[3],real w[3],real dt);
static inline void mulMatr(real (*A)[3],real (*B)[3],real (*C)[3]);
static inline void mulMatrMatrTr(real (*A)[3],real (*B)[3],real (*C)[3]);
static inline void mulMatrTrMatr(real (*A)[3],real (*B)[3],real (*C)[3]);
static inline void mulMatrVec(real *v,real (*A)[3],real *x);
static inline void mulMatrTrVec(real *v,real (*A)[3],real *x);
static inline real detMatr(real (*A)[3]);
*/

/**
 * @brief  Matrix multiplication A=BxC
 * @param  A,B,C real 3x3 matrices, A will be overwritten with the result BxC
 * @retval None
 */
static inline void mulMatr(real (*A)[3],real (*B)[3],real (*C)[3]){
	int i;
	for (i=0;i<3;i++){
		A[0][i]=B[0][0]*C[0][i]+B[0][1]*C[1][i]+B[0][2]*C[2][i];
		A[1][i]=B[1][0]*C[0][i]+B[1][1]*C[1][i]+B[1][2]*C[2][i];
		A[2][i]=B[2][0]*C[0][i]+B[2][1]*C[1][i]+B[2][2]*C[2][i];
	}
}

/**
 * @brief  Matrix multiplication A=BxC^T
 * @param  A,B,C real 3x3 matrices, A will be overwritten with the result BxC^T
 * @retval None
 */
static inline void mulMatrMatrTr(real (*A)[3],real (*B)[3],real (*C)[3]){
	int i;
	for (i=0;i<3;i++){
		A[0][i]=B[0][0]*C[i][0]+B[0][1]*C[i][1]+B[0][2]*C[i][2];
		A[1][i]=B[1][0]*C[i][0]+B[1][1]*C[i][1]+B[1][2]*C[i][2];
		A[2][i]=B[2][0]*C[i][0]+B[2][1]*C[i][1]+B[2][2]*C[i][2];
	}
}

/**
 * @brief  Matrix multiplication A=B^TxC
 * @param  A,B,C real 3x3 matrices, A will be overwritten with the result BxC^T
 * @retval None
 */
static inline void mulMatrTrMatr(real (*A)[3],real (*B)[3],real (*C)[3]){
	int i;
	for (i=0;i<3;i++){
		A[i][0]=B[0][0]*C[i][0]+B[0][1]*C[i][1]+B[0][2]*C[i][2];
		A[i][1]=B[1][0]*C[i][0]+B[1][1]*C[i][1]+B[1][2]*C[i][2];
		A[i][2]=B[2][0]*C[i][0]+B[2][1]*C[i][1]+B[2][2]*C[i][2];
	}
}

/**
 * @brief  Matrix - Vector multiplication v=Ax
 * @param  v,x vectors of length 3, A real 3x3 matrix
 * @retval None
 */
static inline void mulMatrVec(real *v,real (*A)[3],real *x){
	int i;
	v[0]=A[0][0]*x[0]+A[0][1]*x[1]+A[0][2]*x[2];
	v[1]=A[1][0]*x[0]+A[1][1]*x[1]+A[1][2]*x[2];
	v[2]=A[2][0]*x[0]+A[2][1]*x[1]+A[2][2]*x[2];
}

/**
 * @brief  Matrix Transpose - Vector multiplication v=A'x
 * @param  v,x vectors of length 3, A real 3x3 matrix
 * @retval None
 */
static inline void mulMatrTrVec(real *v,real (*A)[3],real *x){
	int i;
	v[0]=A[0][0]*x[0]+A[1][0]*x[1]+A[2][0]*x[2];
	v[1]=A[0][1]*x[0]+A[1][1]*x[1]+A[2][1]*x[2];
	v[2]=A[0][2]*x[0]+A[1][2]*x[1]+A[2][2]*x[2];
}


//Rot2Euler
static inline void rotmtx2euler(const float *rotmtx, float *euler)
{
	/*0 1 2  0 3 6
      3 4 5  1 4 7
	  6 7 8  2 5 8
	 */

	// Rbe --> euler
	const float tol = 0.99999f;
	const float test = -rotmtx[2];
	if (test > +tol)
	{
		euler[0] = atan2f(-rotmtx[7], rotmtx[4]);
		euler[1] = +0.5f * M_PI;
		euler[2] = 0.0f;
	}
	else if (test < -tol)
	{
		euler[0] = atan2f(-rotmtx[7], rotmtx[4]);
		euler[1] = -0.5f * M_PI;
		euler[2] = 0.0f;
	}
	else
	{
		euler[0] = atan2f (rotmtx[5], rotmtx[8]);
		euler[1] = asinf (-rotmtx[2]); //3-4us!!
		euler[2] = atan2f (rotmtx[1], rotmtx[0]);
	}
}


/**
 * @brief  Matrix derivative
 * @param  A real 3x3 matrix
 * @retval real det(A)
 */
static inline real detMatr(real (*A)[3]){
	return A[0][0]*A[1][1]*A[2][2]+A[0][1]*A[1][2]*A[2][0]+A[0][2]*A[1][0]*A[2][1]-(A[0][2]*A[1][1]*A[2][0]+A[0][1]*A[1][0]*A[2][2]+A[0][0]*A[1][2]*A[2][1]);
}

/**
 * @brief  Vector Norm
 * @param  V 3x1 real vector
 * @retval real ||V||
 */
static inline real normV(real V[3]){
	return sqrtv(V[0]*V[0]+V[1]*V[1]+V[2]*V[2]);
}

/**
 * @brief  Vector Norm difference
 * @param  A,V 3x1 real vector
 * @retval real ||A-V||
 */
static inline real normVd(real A[3],real B[3]){
	real V[3];
	V[0]=A[0]-B[0];
	V[1]=A[1]-B[1];
	V[2]=A[2]-B[2];
	return sqrtv(V[0]*V[0]+V[1]*V[1]+V[2]*V[2]);
}

/**
 * @brief  Vector Dot product
 * @param  A,B 3x1 real vector
 * @retval real A*B
 */
static inline real dot(real A[3],real B[3]){
	return sqrtv(A[0]*B[0]+A[1]*B[1]+A[2]*B[2]);
}

static inline void dRpropagat(real (*Rbe)[3],real (*R)[3],real *wb,real dt){
	real norm,th,c,s,u;
	real w[3];
	real dR[3][3];

	mulMatrTrVec(w,R,wb); // w body to earth;

	norm =normV(w);
	if (norm<1e-6){
		for(int i=0;i<3;i++){
			dR[0][i]=0;
			dR[1][i]=0;
			dR[2][i]=0;
		}
		dR[0][0]=1;
		dR[1][1]=1;
		dR[2][2]=1;
	}
	else{
		w[0] = w[0]/norm;
		w[1] = w[1]/norm;
		w[2] = w[2]/norm;

		th = norm*dt;
		c =cosv(th);
		s =sinv(th);
		u = 1 - c;

		dR[0][0] = w[0]*w[0]*u + c;
		dR[0][1] = w[0]*w[1]*u - w[2]*s;
		dR[0][2] = w[0]*w[2]*u + w[1]*s;
		dR[1][0] = w[1]*w[0]*u + w[2]*s;
		dR[1][1] = w[1]*w[1]*u + c;
		dR[1][2] = w[1]*w[2]*u - w[0]*s;
		dR[2][0] = w[2]*w[0]*u - w[1]*s;
		dR[2][1] = w[2]*w[1]*u + w[0]*s;
		dR[2][2] = w[2]*w[2]*u + c;

		mulMatrMatrTr(Rbe,R,dR);
	}
}

static inline void dRpropagatInfinite(real (*Rbe)[3],real (*R)[3],real *wb,real dt){
	for (int i=0;i<3;i++){
		Rbe[0][i]=      0*R[0][i] + wb[2]*R[1][i] - wb[1]*R[2][i];
		Rbe[1][i]= -wb[2]*R[0][i] +     0*R[1][i] + wb[0]*R[2][i];
		Rbe[2][i]=  wb[1]*R[0][i]  -wb[0]*R[1][i] +     0*R[2][i];
	}
}

//Rot2quat
static inline void rotmtx2quat (const float *rotmtx, Quat4 *q)
{
	q->z = 0.0f;
	float sqtrp1;
	float sqtrp1x2;
	float d[3];
	float sqdip1;


	const float tr = rotmtx[0] + rotmtx[4] + rotmtx[8];
	if (tr>1e-6f)
	{
		sqtrp1 = sqrtv(tr + 1.0f);
		sqtrp1x2 = 2.0*sqtrp1;

		q->w = 0.5f*sqtrp1;
		q->x = (rotmtx[7] - rotmtx[5])/sqtrp1x2;
		q->y = (rotmtx[2] - rotmtx[6])/sqtrp1x2;
		q->z = (rotmtx[3] - rotmtx[1])/sqtrp1x2;
	}
	else
	{
		d[0]=rotmtx[0];
		d[1]=rotmtx[4];
		d[2]=rotmtx[8];

		if ((d[1] > d[0]) && (d[1] > d[2]))
		{
			sqdip1 = sqrtv(d[1] - d[0] - d[2] + 1.0f );
			q->y = 0.5f*sqdip1;

			if ( fabsf(sqdip1) > 1e-6f)
				sqdip1 = 0.5f/sqdip1;

			q->w = (rotmtx[2] - rotmtx[6])*sqdip1;
			q->x = (rotmtx[3] + rotmtx[1])*sqdip1;
			q->z = (rotmtx[7] + rotmtx[5])*sqdip1;
		}
		else if (d[2] > d[0])
		{
			//max value at R(3,3)
			sqdip1 = sqrtv(d[2] - d[0] - d[1] + 1.0f);

			q->z = 0.5f*sqdip1;

			if ( fabsf(sqdip1) > 1e-6f )
				sqdip1 = 0.5f/sqdip1;

			q->w = (rotmtx[3] - rotmtx[1])*sqdip1;
			q->x = (rotmtx[2] + rotmtx[6])*sqdip1;
			q->y = (rotmtx[7] + rotmtx[5])*sqdip1;
		}
		else
		{
			// max value at R(1,1)
			sqdip1 = sqrtv(d[0] - d[1] - d[2] + 1.0f );

			q->x = 0.5f*sqdip1;

			if ( fabsf(sqdip1) > 1e-6f )
				sqdip1 = 0.5f/sqdip1;

			q->w = (rotmtx[7] - rotmtx[5])*sqdip1;
			q->y = (rotmtx[3] + rotmtx[1])*sqdip1;
			q->z = (rotmtx[2] + rotmtx[6])*sqdip1;
		}
	}

}

#endif /* WAHBAROTM_H_ */
