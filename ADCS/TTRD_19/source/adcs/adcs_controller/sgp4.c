/* > sgp4.c
 *
 * 1.00 around 1980 - Felix R. Hoots & Ronald L. Roehrich, from original
 *                    SDP4.FOR and SGP4.FOR
 *
 ************************************************************************
 *
 *     Made famous by the spacetrack report No.3:
 *     "Models for Propogation of NORAD Element Sets"
 *     Edited and subsequently distributed by Dr. T. S. Kelso.
 *
 ************************************************************************
 *
 *	This conversion by:
 *	(c) Paul Crawford & Andrew Brooks 1994-2010
 *	University of Dundee
 *	psc (at) sat.dundee.ac.uk
 *	arb (at) sat.dundee.ac.uk
 *
 *	Released under the terms of the GNU LGPL V3
 *	http://www.gnu.org/licenses/lgpl-3.0.html
 *	
 *	This software is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 ************************************************************************
 * This is a significantly modified version.
 * the older version was redigned and refactored.
 * However, we give credit where credit is due.
 * Huge thanks to the original authers.
 * - Mohamed Said & Ali Fakharany, Alexandria Uni, 2022-04-05
 *
 */

#include "stm32f4xx.h"
#include <stdlib.h>
#include <math.h>
#include "frame.h"
#include "../services_utilities/flash.h"

#include "sgp4.h"

/*
 * =============================== MACROS ============================
 *
 *
 *  Define macro for sign transfer, double to nearest (long) integer,
 *  to square an expression (not nested), and A "safe" square, uses test
 *  to force correct sequence of evaluation when the macro is nested.
 */

/*
 * These macros are safe since they make no assignments.
 */
#define SIGN(a, b)  ((b) >= 0 ? fabs(a) : -fabs(a))

/* ================ Single or Double precision options. ================= */
#define DEFAULT_TO_SNGL 0
//#define SGP4_SNGL

#if defined( SGP4_SNGL ) || (DEFAULT_TO_SNGL && !defined( SGP4_DBLE ))
/* Single precision option. */
typedef float real;
#ifndef SGP4_SNGL
#define SGP4_SNGL
#endif

#else
/* Double precision option. */
typedef double real;
#ifndef SGP4_DBLE
#define SGP4_DBLE
#endif

#endif  /* Single or double choice. */
/* Something silly ? */
#if defined( SGP4_SNGL ) && defined( SGP4_DBLE )
#error sgp4.h - Cannot have both single and double precision defined
#endif

/* Use INLINE keyword when declaring inline functions */
#define INLINE inline


/* =========== Do we have sincos() functions available or not ? ======= */
/*
 We can use the normal ANSI 'C' library functions in sincos() macros, but if
 we have sincos() functions they are much faster (25% under some tests).

 Note:
 If we have available sincos() function we not define MACRO_SINCOS. If we
 haven't available sincos() function we define MACROS_SINCOS.
 */
#define MACRO_SINCOS
#ifdef MACRO_SINCOS
#define sincos(x,s,c) {double sc__tmp=(x);\
		*(s)=sin(sc__tmp);\
		*(c)=cos(sc__tmp);}

#define SINCOS(x,s,c) {double sc__tmp=(double)(x);\
		*(s)=(real)sin(sc__tmp);\
		*(c)=(real)cos(sc__tmp);}

#else

#ifdef SGP4_SNGL
#define SINCOS sincosf
void sincosf(float, float *, float *);
#else
#define SINCOS sincos
void sincos(double, double *, double *);
#endif /* ! SGP4_SNGL */

#endif /* ! MACRO_SINCOS */


/* ================= Stack space problems ? ======================== */

/* Automatic variables, faster (?) but needs more stack space. */

#define LOCAL_REAL   real
#define LOCAL_DOUBLE double

/* ===== Macro fixes for float/double in math.h type functions. ===== */

#define SIN(x)      (real)sin((double)(x))
#define COS(x)      (real)cos((double)(x))
#define SQRT(x)     (real)sqrt((double)(x))
#define FABS(x)     (real)fabs((double)(x))
#define POW(x,y)    (real)pow((double)(x), (double)(y))
#define FMOD(x,y)   (real)fmod((double)(x), (double)(y))
#define ATAN2(x,y)  (real)atan2((double)(x), (double)(y))

#ifdef SGP4_SNGL
#define CUBE FCUBE
#define POW4 FPOW4
#else
#define CUBE DCUBE
#define POW4 DPOW4
#endif

/* ================ single / double precision fix-ups =============== */

#define ECC_ZERO        ((real)0.0)             /* Zero eccentricity case ? */
#define ECC_ALL         ((real)1.0e-4)          /* For all drag terms in GSFC case. */
#define ECC_EPS	        ((real)1.0e-6)          /* Too low for computing further drops. */
#define ECC_LIMIT_LOW   ((real)-1.0e-3)         /* Exit point for serious decaying of orbits. */
#define ECC_LIMIT_HIGH  ((real)(1.0 - ECC_EPS)) /* Too close to 1 */

#define EPS_COSIO   (1.5e-12)   /* Minimum divisor allowed for (...)/(1+cos(IO)) */

#define TOTHRD  (2.0/3.0)

#if defined( SGP4_SNGL ) || 0
#define NR_EPS  ((real)(1.0e-6))    /* Minimum ~1e-6 min for float. */
#else
#define NR_EPS  ((real)(1.0e-12))    /* Minimum ~1e-14 for double. */
#endif

#define Q0      ((real)120.0)
#define S0      ((real)78.0)
#define XJ2     ((real)1.082616e-3)
#define XJ3     ((real)-2.53881e-6)
#define XJ4     ((real)-1.65597e-6)
#define XKMPER  (6378.135f)            /* Km per earth radii */
#define XMNPDA  (1440.0f)              /* Minutes per day */
#define AE      (1.0f)                 /* Earth radius in "chosen units". */

#if 0
/* Original code constants. */
#define XKE     (0.743669161e-1)
#define CK2     ((real)5.413080e-4)   /* (0.5 * XJ2 * AE * AE) */
#define CK4     ((real)0.62098875e-6) /* (-0.375 * XJ4 * AE * AE * AE * AE) */
#define QOMS2T  ((real)1.88027916e-9) /* (pow((Q0 - S0)*AE/XKMPER, 4.0)) */
#define KS      ((real)1.01222928)    /* (AE * (1.0 + S0/XKMPER)) */
#else
/* GSFC improved coeficient resolution. */
#define XKE     ((real)7.43669161331734132e-2)
#define CK2     ((real)(0.5 * XJ2 * AE * AE))
#define CK4     ((real)(-0.375 * XJ4 * AE * AE * AE * AE))
#define QOMS2T  ((real)1.880279159015270643865e-9)          /* (pow((Q0 - S0)*AE/XKMPER, 4.0)) */
#define KS      ((real)(AE * (1.0 + S0/XKMPER)))
#endif


//--PRIVATE STURCTS-------------------------------------
typedef struct kep_s {
   double theta;           /* Angle "theta" from equatorial plane (rad) = U. */
   double ascn;            /* Right ascension (rad). */
   double eqinc;           /* Equatorial inclination (rad). */
   double radius;          /* Radius (km). */
   double rdotk;
   double rfdotk;
   /*
   * Following are without short-term perturbations but used to
   * speed searchs.
   */
   double argp;            /* Argument of perigee at 'tsince' (rad). */
   double smjaxs;          /* Semi-major axis at 'tsince' (km). */
   double ecc;             /* Eccentricity at 'tsince'. */
} kep_t;

//========== PRIVATE VARIABLES ========================
static tle_data_t tle_data;

static xyz_t p_eci;
static xyz_t v_eci;
static const real a3ovk2 = (real) (-XJ3 / CK2 * (AE * AE * AE));

static double xno; /* Mean motion (rad/min) */


static double SGP4_jd0; /* Julian Day for epoch (available to outside functions). */
static sgp4_status imode = SGP4_NOT_INIT;
static real sinIO, cosIO, sinXMO, cosXMO;
static real c1, c2, c3, c4, c5, d2, d3, d4;
static real omgcof, xmcof, xlcof, aycof;
static real t2cof, t3cof, t4cof, t5cof;
static real xnodcf, delmo, x7thm1, x3thm1, x1mth2;
static real aodp, eta, omgdot, xnodot;
static double xnodp, xmdot;

// static uint32_t Isat = 0; /* 16-bit compilers need 'long' integer for higher space catalogue numbers. */
double perigee, period, apogee;

// static uint32_t Icount = 0;
//static uint8_t MaxNR = 0;
static uint8_t Set_LS_zero = 0; /* Set to 1 to zero Lunar-Solar terms at epoch. */


//---PRIVATE PROTOTYPES--------------------------------
static sgp4_status init_sgp4(void);
static sgp4_status sgp4(double tsince, uint8_t withvel, kep_t *kep);
static void kep2xyz(kep_t *K, xyz_t *pos, xyz_t *vel);


//======= PUBLIC FUNCTIONS IMPLEMENTATION ===============
uint8_t CTRL_SGP4_Init(void)
{
   // get tle data
   tle_data = CTRL_TLE_GetData();
   return 0;
}   

uint8_t CTRL_SGP4_Update(void)
{
   // get tle data
   tle_data_t tle_tmp = CTRL_TLE_GetData();
   
   // check on data status
   if(tle_tmp.data_status == NEW_DATA)
   {
      // save old tle while we check on the new one
      tle_data_t tle_old = tle_data;
      
      // get tbe sgp4 operation mode based on new tle
      tle_data = tle_tmp;
      sgp4_status imode = init_sgp4();
      
      // check if we don't have the correct operation mode
      if(imode != SGP4_NEAR_SIMP && imode != SGP4_NEAR_NORM)
      {
        // revert to old data
         tle_data = tle_old;
      }
   }

   // calculate position and velocity
   sgp4_status satpos_status = satpos_xyz( time_getTime().Julian_Date, &p_eci, &v_eci);

   // check if the output is correct
   if (satpos_status == SGP4_ZERO_ECC || satpos_status == SGP4_NEAR_SIMP
            || satpos_status == SGP4_NEAR_NORM) {
      return 0;
   }
   
    return ERROR_CODE_SGP4_MODE;
}

        
//======= PRIVATE FUNCTIONS IMPLEMENTATION ===============
static INLINE double DCUBE(double a) {
    return (a * a * a);
}

/*
static INLINE float FCUBE(float a) {
    return (a * a * a);
}
*/

static INLINE double DPOW4(double a) {
    a *= a;
    return (a * a);
}
/*
static INLINE float FPOW4(float a) {
    a *= a;
    return (a * a);
}
*/

/* ==================================================================== */



/* ======================================================================
 Compute the satellite position and/or velocity for a given time (in the
 form of Julian day number.)
 Calling arguments are:

 jd   : Time as Julian day number.

 pos  : Pointer to position vector, km (NULL if not required).

 vel  : Pointer to velocity vector, km/sec (NULL if not required).

 ====================================================================== */

sgp4_status satpos_xyz(double jd, xyz_t *pos, xyz_t *vel) {
    kep_t K;
    uint8_t withvel;
    sgp4_status rv;
    double tsince;

    tsince = (jd - SGP4_jd0) * XMNPDA;

    if (vel != NULL)
        withvel = 1;
    else
        withvel = 0;

    rv = sgp4(tsince, withvel, &K);

    kep2xyz(&K, pos, vel);

    return rv;
}

/* =======================================================================
 The init_sgp4() function passes all of the required orbital elements to
 the sgp4() function together with the pre-calculated constants. There is
 some basic error traps and the determination of the orbital model is made.
 For near-earth satellites (xnodp < 225 minutes according to the NORAD
 classification) the SGP4 model is used, with truncated terms for low
 perigee heights when the drag terms are high. For deep-space satellites
 the SDP4 model is used and the deep-space terms initialised (a slow
 process). For orbits with an eccentricity of less than ECC_EPS the model
 reverts to a very basic circular model. This is not physically meaningfull
 but such a circluar orbit is not either! It is fast though.
 Callinr arguments:

 
 The return value indicates the orbital model used.
 ======================================================================= */

static sgp4_status init_sgp4(void) {
    LOCAL_REAL theta2, theta4, xhdot1, x1m5th;
    LOCAL_REAL s4, del1, del0;
    LOCAL_REAL betao, betao2, coef, coef1;
    LOCAL_REAL etasq, eeta, qoms24;
    LOCAL_REAL pinvsq, tsi, psisq, c1sq;
    LOCAL_DOUBLE a0, a1;      // epoch
    real temp0, temp1, temp2, temp3;
    uint32_t iday, iyear;

    /* Copy over elements. */
    /* Convert year to Gregorian with century as 1994 or 94 type ? */

    iyear = (uint32_t) tle_data.orbit_data.ep_year;

    if (iyear < 1960) {
        /* Assume 0 and 100 both refer to 2000AD */
        iyear += (iyear < 60 ? 2000 : 1900);
    }

    if (iyear < 1901 || iyear > 2099) {
        imode = SGP4_ERROR;
        return imode;
    }

    // Isat = tle_data.orbit_data.satno;

    /* Compute days from 1st Jan 1900 (works 1901 to 2099 only). */

    iday = ((iyear - 1901) * 1461L) / 4L + 364L + 1L;

    SGP4_jd0 = JD1900 + iday + (tle_data.orbit_data.ep_day - 1.0); /* Julian day number. */

    //epoch = (iyear - 1900) * 1.0e3 + tle_data.orbit_data.ep_day; /* YYDDD.DDDD as from 2-line. */

    xno = (double) tle_data.orbit_data.rev * TWOPI / XMNPDA; /* Radian / unit time. */


    /* A few simple error checks here. */

    if (tle_data.orbit_data.ecc < (real) 0.0 || tle_data.orbit_data.ecc > ECC_LIMIT_HIGH) {
        imode = SGP4_ERROR;
        return imode;
    }

    if (xno < 0.035 * TWOPI / XMNPDA || xno > 18.0 * TWOPI / XMNPDA) {
        imode = SGP4_ERROR;
        return imode;
    }

    if (tle_data.orbit_data.eqinc < (real) 0.0 || tle_data.orbit_data.eqinc > (real) PI) {
        imode = SGP4_ERROR;
        return imode;
    }

    /* Start the initialisation. */

    if (tle_data.orbit_data.ecc < ECC_ZERO)
        imode = SGP4_ZERO_ECC; /* Special mode for "ideal" circular orbit. */
    else
        imode = SGP4_NOT_INIT;

    /*
     Recover original mean motion (xnodp) and semimajor axis (aodp)
     from input elements.
     */

    SINCOS(tle_data.orbit_data.eqinc, &sinIO, &cosIO);

    theta2 = cosIO * cosIO;
    theta4 = theta2 * theta2;
    x3thm1 = (real) 3.0 * theta2 - (real) 1.0;
    x1mth2 = (real) 1.0 - theta2;
    x7thm1 = (real) 7.0 * theta2 - (real) 1.0;

    a1 = pow(XKE / xno, TOTHRD);
    betao2 = (real) 1.0 - tle_data.orbit_data.ecc * tle_data.orbit_data.ecc;
    betao = SQRT(betao2);
    temp0 = (real) (1.5 * CK2) * x3thm1 / (betao * betao2);
    del1 = temp0 / (a1 * a1);
    a0 = a1 * (1.0 - del1 * (1.0 / 3.0 + del1 * (1.0 + del1 * 134.0 / 81.0)));
    del0 = temp0 / (a0 * a0);
    xnodp = xno / (1.0 + del0);
    aodp = (real) (a0 / (1.0 - del0));
    perigee = (aodp * (1.0 - tle_data.orbit_data.ecc) - AE) * XKMPER;
    apogee = (aodp * (1.0 + tle_data.orbit_data.ecc) - AE) * XKMPER;
    period = (TWOPI * 1440.0 / XMNPDA) / xnodp;

    if (perigee <= 0.0) {
        ;
    }

    if (imode == SGP4_ZERO_ECC)
        return imode;

    if (period >= 225.0 && Set_LS_zero < 2) {
        imode = SGP4_DEEP_NORM; /* Deep-Space model(s). */
    } else if (perigee < 220.0) {
        /*
         For perigee less than 220 km the imode flag is set so the
         equations are truncated to linear variation in sqrt A and
         quadratic variation in mean anomaly. Also the c3 term, the
         delta omega term and the delta m term are dropped.
         */
        imode = SGP4_NEAR_SIMP; /* Near-space, simplified equations. */
    } else {
        imode = SGP4_NEAR_NORM; /* Near-space, normal equations. */
    }

    /* For perigee below 156 km the values of S and QOMS2T are altered */

    if (perigee < 156.0) {
        s4 = (real) (perigee - 78.0);

        if (s4 < (real) 20.0) {
            s4 = (real) 20.0;
        } else {
            ;
        }

        qoms24 = POW4((real) ((120.0 - s4) * (AE / XKMPER)));
        s4 = (real) (s4 / XKMPER + AE);
    } else {
        s4 = KS;
        qoms24 = QOMS2T;
    }

    pinvsq = (real) 1.0 / (aodp * aodp * betao2 * betao2);
    tsi = (real) 1.0 / (aodp - s4);
    eta = aodp * tle_data.orbit_data.ecc * tsi;
    etasq = eta * eta;
    eeta = tle_data.orbit_data.ecc * eta;
    psisq = FABS((real )1.0 - etasq);
    coef = qoms24 * POW4(tsi);
    coef1 = coef / POW(psisq, 3.5);

    c2 = coef1 * (real) xnodp * (aodp * ((real) 1.0 + (real) 1.5 * etasq + eeta * ((real) 4.0 + etasq))
               + (real) (0.75 * CK2) * tsi / psisq * x3thm1 * 
               ((real) 8.0 + (real) 3.0 * etasq * ((real) 8.0 + etasq)));

    c1 = tle_data.orbit_data.bstar * c2;

    c4 = (real) 2.0 * (real) xnodp * coef1 * aodp * betao2 * (eta * ((real) 2.0 + (real) 0.5 * etasq)
         + tle_data.orbit_data.ecc * ((real) 0.5 + (real) 2.0 * etasq) - (real) (2.0 * CK2) * tsi / (aodp * psisq)
         * ((real) -3.0 * x3thm1 * ((real) 1.0 - (real) 2.0 * eeta + etasq * ((real) 1.5 - 
         (real) 0.5 * eeta)) + (real) 0.75 * x1mth2 * ((real) 2.0 * etasq - eeta * ((real) 1.0 + etasq))
         * COS((real )2.0 * tle_data.orbit_data.argp)));

    c5 = c3 = omgcof = (real) 0.0;

    if (imode == SGP4_NEAR_NORM) {
        /* BSTAR drag terms for normal near-space 'normal' model only. */
        c5 = (real) 2.0 * coef1 * aodp * betao2
                * ((real) 1.0 + (real) 2.75 * (etasq + eeta) + eeta * etasq);

        if (tle_data.orbit_data.ecc > ECC_ALL) {
            c3 = coef * tsi * a3ovk2 * (real) xnodp * (real) AE * sinIO / tle_data.orbit_data.ecc;
        }

        omgcof = tle_data.orbit_data.bstar * c3 * COS(tle_data.orbit_data.argp);
    }

    temp1 = (real) (3.0 * CK2) * pinvsq * (real) xnodp;
    temp2 = temp1 * CK2 * pinvsq;
    temp3 = (real) (1.25 * CK4) * pinvsq * pinvsq * (real) xnodp;

    xmdot = xnodp + (0.5 * temp1 * betao * x3thm1 + (real) 0.0625 * temp2 * betao
            * ((real) 13.0 - (real) 78.0 * theta2 + (real) 137.0 * theta4));

    x1m5th = (real) 1.0 - (real) 5.0 * theta2;

    omgdot = (real) -0.5 * temp1 * x1m5th + (real) 0.0625 * temp2 * ((real) 7.0 - (real) 114.0 * theta2
            + (real) 395.0 * theta4) + temp3 * ((real) 3.0 - (real) 36.0 * theta2 + (real) 49.0 * theta4);

    xhdot1 = -temp1 * cosIO;
    xnodot = xhdot1 + ((real) 0.5 * temp2 * ((real) 4.0 - (real) 19.0 * theta2)
            + (real) 2.0 * temp3 * ((real) 3.0 - (real) 7.0 * theta2)) * cosIO;

    xmcof = (real) 0.0;
    if (tle_data.orbit_data.ecc > ECC_ALL) {
        xmcof = (real) (-TOTHRD * AE) * coef * tle_data.orbit_data.bstar / eeta;
    }

    xnodcf = (real) 3.5 * betao2 * xhdot1 * c1;
    t2cof = (real) 1.5 * c1;

    /* Check for possible divide-by-zero for X/(1+cosIO) when calculating xlcof */
    temp0 = (real) 1.0 + cosIO;

    if (fabs(temp0) < EPS_COSIO)
        temp0 = (real) SIGN(EPS_COSIO, temp0);

    xlcof = (real) 0.125 * a3ovk2 * sinIO * ((real) 3.0 + (real) 5.0 * cosIO) / temp0;

    aycof = (real) 0.25 * a3ovk2 * sinIO;

    SINCOS(tle_data.orbit_data.mnan, &sinXMO, &cosXMO);
    delmo = CUBE((real) 1.0 + eta * cosXMO);

    if (imode == SGP4_NEAR_NORM) {
        c1sq = c1 * c1;
        d2 = (real) 4.0 * aodp * tsi * c1sq;
        temp0 = d2 * tsi * c1 / (real) 3.0;
        d3 = ((real) 17.0 * aodp + s4) * temp0;
        d4 = (real) 0.5 * temp0 * aodp * tsi
                * ((real) 221.0 * aodp + (real) 31.0 * s4) * c1;
        t3cof = d2 + (real) 2.0 * c1sq;
        t4cof = (real) 0.25
                * ((real) 3.0 * d3
                        + c1 * ((real) 12.0 * d2 + (real) 10.0 * c1sq));
        t5cof = (real) 0.2
                * ((real) 3.0 * d4 + (real) 12.0 * c1 * d3
                        + (real) 6.0 * d2 * d2
                        + (real) 15.0 * c1sq * ((real) 2.0 * d2 + c1sq));
    } else if (imode == SGP4_DEEP_NORM) {
        ;
    }

    return imode;
}

/* =======================================================================
 The sgp4() function computes the Keplarian elements that describe the
 position and velocity of the satellite. Depending on the initialisation
 (and the compile options) the deep-space perturbations are also included
 allowing sensible predictions for most satellites. These output elements
 can be transformed to Earth Centered Inertial coordinates (X-Y-Z) and/or
 to sub-satellite latitude and longitude as required. The terms for the
 velocity solution are often not required so the 'withvel' flag can be used
 to by-pass that step as required. This function is normally called through
 another since the input 'tsince' is the time from epoch.
 Calling arguments:

 tsince   : Input, time from epoch (minutes).

 withvel  : Input, non-zero if velocity terms required.

 kep      : Output, the Keplarian position / velocity of the satellite.

 The return value indicates the orbital mode used.

 ======================================================================= */

static sgp4_status sgp4(double tsince, uint8_t withvel, kep_t *kep) {
    LOCAL_REAL rk, uk, xnodek, xinck, em, xinc;
    LOCAL_REAL xnode, delm, axn, ayn, omega;
    LOCAL_REAL capu, epw, elsq, invR, beta2, betal;
    LOCAL_REAL sinu, sin2u, cosu, cos2u;
    LOCAL_REAL a, e, r, u, pl;
    LOCAL_REAL sinEPW, cosEPW, sinOMG, cosOMG;
    LOCAL_DOUBLE xmp, xl, xlt;
    const uint8_t MAXI = 10;

    real esinE, ecosE, maxnr;
    real temp0, temp1, temp2, temp3;
    real tempa, tempe, templ;
    uint8_t ii;

#ifdef SGP4_SNGL
    real ts = (real) tsince;
#else
#define ts tsince
#endif /* ! SGP4_SNGL */

    /* Update for secular gravity and atmospheric drag. */

    em = tle_data.orbit_data.ecc;
    xinc = tle_data.orbit_data.eqinc;

    xmp = (double) tle_data.orbit_data.mnan + xmdot * tsince;
    xnode = tle_data.orbit_data.ascn + ts * (xnodot + ts * xnodcf);
    omega = tle_data.orbit_data.argp + omgdot * ts;

    switch (imode) {
    case SGP4_ZERO_ECC:
        /* Not a "real" orbit but OK for fast computation searches. */
        kep->smjaxs = kep->radius = (double) aodp * XKMPER / AE;
        kep->theta = fmod(PI + xnodp * tsince, TWOPI) - PI;
        kep->eqinc = (double) tle_data.orbit_data.eqinc;
        kep->ascn = tle_data.orbit_data.ascn;

        kep->argp = 0;
        kep->ecc = 0;

        kep->rfdotk = 0;
        if (withvel)
            kep->rfdotk = aodp * xnodp * (XKMPER / AE * XMNPDA / 86400.0f); /* For km/sec */
        else
            kep->rfdotk = 0;

        return imode;

    case SGP4_NEAR_SIMP:
        tempa = (real) 1.0 - ts * c1;
        tempe = tle_data.orbit_data.bstar * ts * c4;
        templ = ts * ts * t2cof;
        a = aodp * tempa * tempa;
        e = em - tempe;
        xl = xmp + omega + xnode + xnodp * templ;
        break;

    case SGP4_NEAR_NORM:
        delm = xmcof * (CUBE((real) 1.0 + eta * COS(xmp)) - delmo);
        temp0 = ts * omgcof + delm;
        xmp += (double) temp0;
        omega -= temp0;
        tempa = (real) 1.0 - (ts * (c1 + ts * (d2 + ts * (d3 + ts * d4))));
        tempe = tle_data.orbit_data.bstar * (c4 * ts + c5 * (SIN(xmp) - sinXMO));
        templ = ts * ts * (t2cof + ts * (t3cof + ts * (t4cof + ts * t5cof)));
        //xmp   += (double)temp0;
        a = aodp * tempa * tempa;
        e = em - tempe;
        xl = xmp + omega + xnode + xnodp * templ;
        break;

    default:
        return SGP4_ERROR;
    }

    if (a < (real) 1.0) {
        return SGP4_ERROR;
    }

    if (e < ECC_LIMIT_LOW) {
        return SGP4_ERROR;
    }

    if (e < ECC_EPS) {
        e = ECC_EPS;
    } else if (e > ECC_LIMIT_HIGH) {
        e = ECC_LIMIT_HIGH;
    }

    beta2 = (real) 1.0 - e * e;

    /* Long period periodics */
    SINCOS(omega, &sinOMG, &cosOMG);

    temp0 = (real) 1.0 / (a * beta2);
    axn = e * cosOMG;
    ayn = e * sinOMG + temp0 * aycof;
    xlt = xl + temp0 * xlcof * axn;

    elsq = axn * axn + ayn * ayn;
    if (elsq >= (real) 1.0) {
        return SGP4_ERROR;
    }

    /* Sensibility check for N-R correction. */
    kep->ecc = sqrt(elsq);

    /*
     * Solve Kepler's equation using Newton-Raphson root solving. Here 'capu' is
     * almost the "Mean anomaly", initialise the "Eccentric Anomaly" term 'epw'.
     * The fmod() saves reduction of angle to +/-2pi in SINCOS() and prevents
     * convergence problems.
     *
     * Later modified to support 2nd order NR method which saves roughly 1 iteration
     * for only a couple of arithmetic operations.
     */

    epw = capu = fmod(xlt - xnode, TWOPI);

    maxnr = kep->ecc;

    for (ii = 0; ii < MAXI; ii++) {
        double nr, f, df;
        SINCOS(epw, &sinEPW, &cosEPW);

        ecosE = axn * cosEPW + ayn * sinEPW;
        esinE = axn * sinEPW - ayn * cosEPW;

        f = capu - epw + esinE;
        if (fabs(f) < NR_EPS)
            break;

        df = 1.0 - ecosE;

        /* 1st order Newton-Raphson correction. */
        nr = f / df;

        if (ii == 0 && FABS(nr) > 1.25 * maxnr)
            nr = SIGN(maxnr, nr);
#if 1
        /* 2nd order Newton-Raphson correction. */
        else
            nr = f / (df + 0.5 * esinE * nr); /* f/(df - 0.5*d2f*f/df) */
#endif

        epw += nr; /* Newton-Raphson correction of -F/DF. */
        //if (fabs(nr) < NR_EPS) break;
    }

    /* Short period preliminary quantities */
    temp0 = (real) 1.0 - elsq;
    betal = SQRT(temp0);
    pl = a * temp0;
    r = a * ((real) 1.0 - ecosE);
    invR = (real) 1.0 / r;
    temp2 = a * invR;
    temp3 = (real) 1.0 / ((real) 1.0 + betal);
    cosu = temp2 * (cosEPW - axn + ayn * esinE * temp3);
    sinu = temp2 * (sinEPW - ayn - axn * esinE * temp3);
    u = ATAN2(sinu, cosu);
    sin2u = (real) 2.0 * sinu * cosu;
    cos2u = (real) 2.0 * cosu * cosu - (real) 1.0;
    temp0 = (real) 1.0 / pl;
    temp1 = CK2 * temp0;
    temp2 = temp1 * temp0;

    /* Update for short term periodics to position terms. */

    rk = r * ((real) 1.0 - (real) 1.5 * temp2 * betal * x3thm1)
            + (real) 0.5 * temp1 * x1mth2 * cos2u;
    uk = u - (real) 0.25 * temp2 * x7thm1 * sin2u;
    xnodek = xnode + (real) 1.5 * temp2 * cosIO * sin2u;
    xinck = xinc + (real) 1.5 * temp2 * cosIO * sinIO * cos2u;

    if (rk < (real) 1.0) {
        return SGP4_ERROR;
    }

    kep->radius = rk * XKMPER / AE; /* Into km */
    kep->theta = uk;
    kep->eqinc = xinck;
    kep->ascn = xnodek;
    kep->argp = omega;
    kep->smjaxs = a * XKMPER / AE;

    /* Short period velocity terms ?. */
    if (withvel) {
        /* xn = XKE / pow(a, 1.5); */
        temp0 = SQRT(a);
        temp2 = (real) XKE / (a * temp0);

        kep->rdotk = ((real) XKE * temp0 * esinE * invR
                - temp2 * temp1 * x1mth2 * sin2u)
                * (XKMPER / AE * XMNPDA / 86400.0f); /* Into km/sec */

        kep->rfdotk = ((real) XKE * SQRT(pl) * invR
                + temp2 * temp1 * (x1mth2 * cos2u + (real) 1.5 * x3thm1))
                * (XKMPER / AE * XMNPDA / 86400.0f);
    } else {
        kep->rdotk = kep->rfdotk = 0;
    }

#ifndef SGP4_SNGL
#undef ts
#endif

    return imode;
}

/* ====================================================================

 Transformation from "Kepler" type coordinates to cartesian XYZ form.
 Calling arguments:

 K    : Kepler structure as filled by sgp4();

 pos  : XYZ structure for position.

 vel  : same for velocity.

 ==================================================================== */

static void kep2xyz(kep_t *K, xyz_t *pos, xyz_t *vel) {
    real xmx, xmy;
    real ux, uy, uz, vx, vy, vz;
    real sinT, cosT, sinI, cosI, sinS, cosS;

    /* Orientation vectors for X-Y-Z format. */

    SINCOS((real) K->theta, &sinT, &cosT);
    SINCOS((real) K->eqinc, &sinI, &cosI);
    SINCOS((real) K->ascn, &sinS, &cosS);

    xmx = -sinS * cosI;
    xmy = cosS * cosI;

    ux = xmx * sinT + cosS * cosT;
    uy = xmy * sinT + sinS * cosT;
    uz = sinI * sinT;

    /* Position and velocity */

    if (pos != NULL) {
        pos->x = K->radius * ux;
        pos->y = K->radius * uy;
        pos->z = K->radius * uz;
    }

    if (vel != NULL) {
        vx = xmx * cosT - cosS * sinT;
        vy = xmy * cosT - sinS * sinT;
        vz = sinI * cosT;

        vel->x = K->rdotk * ux + K->rfdotk * vx;
        vel->y = K->rdotk * uy + K->rfdotk * vy;
        vel->z = K->rdotk * uz + K->rfdotk * vz;
    }

}

