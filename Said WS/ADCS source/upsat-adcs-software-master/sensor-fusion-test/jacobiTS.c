/*
 * jacobiTS.c
 *
 *  Created on: Oct 30, 2014
 *      Author: mobintu
 */

#include "WahbaRotM.h"


void WahbaJacobiSVDRotM(real (*A)[3], real (*RotM)[3]){
	real U[3][3],detU;
	real V[3][3],detV;
	int i;
	for (i=0;i<3;i++){
		U[i][0]=0;
		U[i][1]=0;
		U[i][2]=0;
		V[i][0]=0;
		V[i][1]=0;
		V[i][2]=0;
		U[i][i]=1;
		V[i][i]=1;
	}

	jacobi2Sided(A,U,V);
	detU=detMatr(U);
	detV=detMatr(V);

	if(detU*detV<0){
		U[0][2]=-U[0][2];
		U[1][2]=-U[1][2];
		U[2][2]=-U[2][2];
	}

	mulMatrMatrTr(RotM,V,U);

	/*if(detU*detV<0){
		V[0][2]=-V[0][2];
		V[1][2]=-V[1][2];
		V[2][2]=-V[2][2];
	}
	 * for (i=0;i<3;i++){
		RotM[i][0]=U[0][0]*V[i][0]+U[0][1]*V[i][1]+U[0][2]*V[i][2];
		RotM[i][1]=U[1][0]*V[i][0]+U[1][1]*V[i][1]+U[1][2]*V[i][2];
		RotM[i][2]=U[2][0]*V[i][0]+U[2][1]*V[i][1]+U[2][2]*V[i][2];
	}*/
}

void jacobi2Sided(real (*A)[3],real (*U)[3],real (*V)[3]){
	int p,q;
	real w,x,y,z,m1,m2,c,s,r,c1,c2,s1,s2,r2,t2,d1,d2,k,tmp;
	int flg;
	int cnt=3;

	while(cnt--){
		for(p=1;p<3;p++){
			for(q=0;q<p;q++){
				w=A[p][p];
				x=A[p][q];
				y=A[q][p];
				z=A[q][q];
				flg=0;
				if(y==0 && z==0){
					y=x;
					x=0;
					flg=1;
				}

				m1=w+z;
				m2=x-y;
				if (fabsv(m2) <=TOL*fabsv(m1)){
					c=1;
					s=0;
				}
				else{
					r=m1/m2;
					s=SIGN(r)/sqrtv(1+r*r);
					c=s*r;
				}

				m1=s*(x+y)+c*(z-w);
				m2=2*(c*x-s*z);
				if (fabsv(m2) <=TOL*fabsv(m1)){
					c2=1;
					s2=0;
				}
				else{
					r2=m1/m2;
					t2=SIGN(r2)/(fabsv(r2)+sqrtv(1+r2*r2)); //%use hypot..?
					c2=1/sqrtv(1+t2*t2);
					s2=c2*t2;
				}
				c1=c2*c-s2*s;
				s1=s2*c+c2*s;

				if (flg==1){
					c2=c1;
					s2=s1;
					c1=1;
					s1=0;
				}
				d1=c1*(w*c2-x*s2)-s1*(y*c2-z*s2);
				d2=s1*(w*s2+x*c2)+c1*(y*s2+z*c2);

				if (fabsv(d1)>fabsv(d2)){
					tmp=c1; c1=-s1; s1=tmp;
					tmp=c2; c2=-s2; s2=tmp;
					tmp=d2; d2=d1; d1=tmp;
				}

				k=1;
				if (d1<0){
					c1=-c1;
					s1=-s1;
					k=-k;
				}
				if (d2<0){
					k=-k;
				}
				apply_jacobi_L (A, p, q, c1, s1,k);
				apply_jacobi_R (A, p, q, c2, s2,1);
				apply_jacobi_R (U, p, q, c1, s1,k);
				apply_jacobi_R (V, p, q, c2, s2,1);
			}
		}
	}
}


void apply_jacobi_R (real (*A)[3],int p,int q,real c,real s,real k){
	int i;
	real Aip,Aiq;
	//%    /* Apply rotation to matrix A,  A' = A J */
	for (i =0;i<3;i++){
		Aip =A[i][p];
		Aiq =A[i][q];
		A[i][p]= Aip * c - Aiq * s;
		A[i][q]=(Aip * s + Aiq * c)*k;
	}
}
void apply_jacobi_L (real (*A)[3],int p,int q,real c,real s,real k){
	int j;
	real Apj,Aqj;
	//%    /* Apply rotation to matrix A,  A' = A J */
	for (j =0;j<3;j++){
		Apj =A[p][j];
		Aqj =A[q][j];
		A[p][j]= Apj * c - Aqj * s;
		A[q][j]=(Apj * s + Aqj * c)*k;
	}
}



