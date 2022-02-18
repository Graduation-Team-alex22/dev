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
 *
 * 1.07 arb     Oct    1994 - Transcribed by arb Oct 1994 into 'C', then
 *                            modified to fit Dundee systems by psc.
 *
 * 1.08 psc Mon Nov  7 1994 - replaced original satpos.c with SGP4 model.
 *
 * 1.09 psc Wed Nov  9 1994 - Corrected a few minor translation errors after
 *                            testing with example two-line elements.
 *
 * 1.10 psc Mon Nov 21 1994 - A few optimising tweeks.
 *
 * 1.11 psc Wed Nov 30 1994 - No longer uses eloset() and minor error in the
 *                            SGP4 code corrected.
 *
 * 2.00 psc Tue Dec 13 1994 - arb discovered the archive.afit.af.mil FTP site
 *                            with the original FORTRAN code in machine form.
 *                            Tidied up and added support for the SDP4 model.
 *
 * 2.01 psc Fri Dec 23 1994 - Tested out the combined SGP4/SDP4 code against
 *                            the original FORTRAN versions.
 *
 * 2.02 psc Mon Jan 02 1995 - Few more tweeks and tidied up the
 *                            doccumentation for more general use.
 *
 * 3.00 psc Mon May 29 1995 - Cleaned up for general use & distrabution (to
 *                            remove Dundee specific features).
 *
 * 3.01 psc Mon Jan 12 2004 - Minor bug fix for day calculation.
 *
 * 3.02 psc Mon Jul 10 2006 - Added if(rk < (real)1.0) test for sub-orbital decay.
 *
 * 3.03 psc Sat Aug 05 2006 - Added trap for divide-by-zero when calculating xlcof.
 *
 */

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sgp4.h"
#include "adcs_common.h"
#include "service_utilities.h"

/* Global Variables */
orbit_t upsat_tle = { .argp = 0, .ascn = 0, .bstar = 0, .ecc = 0, .ep_day = 0,
        .ep_year = 0, .eqinc = 0, .mnan = 0, .norb = 0, .rev = 0, .satno = 0,
        .smjaxs = 0 };

orbit_t temp_tle = { .argp = 0, .ascn = 0, .bstar = 0, .ecc = 0, .ep_day = 0,
        .ep_year = 0, .eqinc = 0, .mnan = 0, .norb = 0, .rev = 0, .satno = 0,
        .smjaxs = 0 };
xyz_t p_eci = {.x = 0, .y = 0, .z = 0};
xyz_t v_eci = {.x = 0, .y = 0, .z = 0};

/********************/

typedef struct kep_s {
    double theta; /* Angle "theta" from equatorial plane (rad) = U. */
    double ascn; /* Right ascension (rad). */
    double eqinc; /* Equatorial inclination (rad). */
    double radius; /* Radius (km). */
    double rdotk;
    double rfdotk;
    /*
     * Following are without short-term perturbations but used to
     * speed searchs.
     */
    double argp; /* Argument of perigee at 'tsince' (rad). */
    double smjaxs; /* Semi-major axis at 'tsince' (km). */
    double ecc; /* Eccentricity at 'tsince'. */
} kep_t;

static sgp4_status init_sgp4(orbit_t *orb);
static sgp4_status sgp4(double tsince, uint8_t withvel, kep_t *kep);
static void kep2xyz(kep_t *K, xyz_t *pos, xyz_t *vel);

/* Functions for TLE calculation */
static uint32_t i_read(uint8_t *str, uint8_t start, uint8_t stop);
static double d_read(uint8_t *str, uint8_t start, uint8_t stop);
static void rv2coe(double r[3], double v[3], double mu, double *p, double *a,
        double *ecc, double *incl, double *omega, double *argp, double *nu,
        double *m, double *arglat, double *truelon, double *lonper);
static double sgn(double x);
static double mag(double x[3]);
static void cross(double vec1[3], double vec2[3], double outvec[3]);
static double dot(double x[3], double y[3]);
static double angle(double vec1[3], double vec2[3]);
static double sgp4_asinh(double xval);
static void newtonnu(double ecc, double nu, double *e0, double *m);

/* ================ Single or Double precision options. ================= */

#define DEFAULT_TO_SNGL 1
#define SGP4_SNGL

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

/*
 * All other compilers can have static inline functions.
 * (SQR is used badly here: do_cal.c, glat2lat.c, satpos.c, vmath.h).
 */

static INLINE double DCUBE(double a) {
    return (a * a * a);
}
static INLINE float FCUBE(float a) {
    return (a * a * a);
}
static INLINE double DPOW4(double a) {
    a *= a;
    return (a * a);
}
static INLINE float FPOW4(float a) {
    a *= a;
    return (a * a);
}

/* =========== Do we have sincos() functions available or not ? ======= */
/*
 We can use the normal ANSI 'C' library functions in sincos() macros, but if
 we have sincos() functions they are much faster (25% under some tests).

 Note:
 If we have available sincos() function we not define MACRO_SINCOS. If we
 haven't available sincos() function we define MACROS_SINCOS.
 */
//#define MACRO_SINCOS
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
#define XKMPER  (6378.135)            /* Km per earth radii */
#define XMNPDA  (1440.0)              /* Minutes per day */
#define AE      (1.0)                 /* Earth radius in "chosen units". */

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
static const real a3ovk2 = (real) (-XJ3 / CK2 * (AE * AE * AE));

/* ================= Copy of the orbital elements ==================== */

static double xno; /* Mean motion (rad/min) */
static real xmo; /* Mean "mean anomaly" at epoch (rad). */
static real eo; /* Eccentricity. */
static real xincl; /* Equatorial inclination (rad). */
static real omegao; /* Mean argument of perigee at epoch (rad). */
static real xnodeo; /* Mean longitude of ascending node (rad, east). */
static real bstar; /* Drag term. */

/* ================== Local "global" variables for SGP4 ================= */

static double SGP4_jd0; /* Julian Day for epoch (available to outside functions). */
static sgp4_status imode = SGP4_NOT_INIT;
static real sinIO, cosIO, sinXMO, cosXMO;
static real c1, c2, c3, c4, c5, d2, d3, d4;
static real omgcof, xmcof, xlcof, aycof;
static real t2cof, t3cof, t4cof, t5cof;
static real xnodcf, delmo, x7thm1, x3thm1, x1mth2;
static real aodp, eta, omgdot, xnodot;
static double xnodp, xmdot;

static uint32_t Isat = 0; /* 16-bit compilers need 'long' integer for higher space catalogue numbers. */
double perigee, period, apogee;

static uint32_t Icount = 0;
static uint8_t MaxNR = 0;
static uint8_t Set_LS_zero = 0; /* Set to 1 to zero Lunar-Solar terms at epoch. */

/* ==================================================================== */

/**
 * @brief Calculate TLE, with given position, velocity and TLE epoch
 * @param position in km
 * @param velocity in km/s
 * @param updt_tle_epoch
 * @return new TLE
 */
orbit_t calculate_tle(xyz_t position, xyz_t velocity, _tle_epoch updt_tle_epoch) {

    double p[3], v[3];
    double semil_rect, nu, arglat, truelon, lonper;
    orbit_t tmp_tle;

    /* Take position-velocity in ECI in km-km/s */
    p[0] = position.x;
    p[1] = position.y;
    p[2] = position.z;
    v[0] = velocity.x;
    v[1] = velocity.y;
    v[2] = velocity.z;
    /* Init TLE */
    semil_rect = 0; /* semilatus rectum km */
    tmp_tle.smjaxs = 0; /* semimajor axis km */
    tmp_tle.ecc = 0; /* eccentricity */
    tmp_tle.eqinc = 0; /* inclination 0.0  to pi rad */
    tmp_tle.ascn = 0; /* longitude of ascending node 0.0  to 2pi rad */
    tmp_tle.argp = 0; /* argument of perigee 0.0  to 2pi rad */
    nu = 0; /* true anomaly 0.0  to 2pi rad */
    tmp_tle.mnan = 0; /* mean anomaly 0.0  to 2pi rad */
    arglat = 0; /* argument of latitude (ci) 0.0  to 2pi rad */
    truelon = 0; /* true longitude (ce) 0.0  to 2pi rad */
    lonper = 0; /* longitude of periapsis (ee) 0.0  to 2pi rad */
    tmp_tle.rev = 0;
    tmp_tle.bstar = 0;

    rv2coe(p, v, MU, &semil_rect, &(tmp_tle.smjaxs), &(tmp_tle.ecc),
            &(tmp_tle.eqinc), &(tmp_tle.ascn), &(tmp_tle.argp), &nu,
            &(tmp_tle.mnan), &arglat, &truelon, &lonper);

    tmp_tle.rev = sqrt(MU / (tmp_tle.smjaxs * tmp_tle.smjaxs * tmp_tle.smjaxs))
            * SOLAR_DAY_SEC / (2 * PI); /* Mean motion, revolutions per day */

    tmp_tle.bstar = EARTH_RADII * CD * RHO * AREA * 0.5 / MASS; /* Drag term .*/

    tmp_tle.ep_year = updt_tle_epoch.ep_year; /* Year of epoch, e.g. 94 for 1994, 100 for 2000AD */
    tmp_tle.ep_day = updt_tle_epoch.ep_day; /* Day of epoch from 00:00 Jan 1st ( = 1.0 ) */

    tmp_tle.norb = 13.0; /* Orbit number, for elements */
    tmp_tle.satno = 13.0; /* Satellite number. */

    return tmp_tle;
}

/* ====================================================================
 Read orbit parameters for "satno" in file "filename", return -1 if
 failed to find the corresponding data. Call with satno = 0 to get the
 next elements of whatever sort.

 ISS (ZARYA)
 1 25544U 98067A   16137.55001157  .00005721  00000-0  91983-4 0  9991
 2 25544  51.6440 215.8562 0001995 108.3968  92.4187 15.54614828    61
 OK

 adcs_state.orb_tle.ep_year = 16;
 adcs_state.orb_tle.ep_day = 137.55001157;
 adcs_state.orb_tle.rev = 15.54614828;
 adcs_state.orb_tle.bstar = 0.00175395 * powf(10.0, -5);
 adcs_state.orb_tle.eqinc = RAD(51.6440);
 adcs_state.orb_tle.ecc = 0.0001995;
 adcs_state.orb_tle.mnan = RAD(92.4187);
 adcs_state.orb_tle.argp = RAD(108.3968);
 adcs_state.orb_tle.ascn = RAD(215.8562);
 adcs_state.orb_tle.norb = 0;
 adcs_state.orb_tle.satno = 13;
 ==================================================================== */

orbit_t read_tle(uint8_t *tle) {

    orbit_t tmp_tle;
    double bm, bx;

    tmp_tle.ep_year = (int) i_read(tle, 19, 20);

    if (tmp_tle.ep_year < 57) {
        tmp_tle.ep_year += 2000;
    } else {
        tmp_tle.ep_year += 1900;
    }
    tmp_tle.ep_day = d_read(tle, 21, 32);

    bm = d_read(tle, 54, 59) * 1.0e-5;
    bx = d_read(tle, 60, 61);
    tmp_tle.bstar = bm * pow(10.0, bx);

    tmp_tle.eqinc = RAD(d_read(tle, 9+TLE_LINE2_OFFSET, 16+TLE_LINE2_OFFSET));
    tmp_tle.ascn = RAD(d_read(tle, 18+TLE_LINE2_OFFSET, 25+TLE_LINE2_OFFSET));
    tmp_tle.ecc = d_read(tle, 27 + TLE_LINE2_OFFSET, 33 + TLE_LINE2_OFFSET)
            * 1.0e-7;
    tmp_tle.argp = RAD(d_read(tle, 35+TLE_LINE2_OFFSET, 42+TLE_LINE2_OFFSET));
    tmp_tle.mnan = RAD(d_read(tle, 44+TLE_LINE2_OFFSET, 51+TLE_LINE2_OFFSET));
    tmp_tle.rev = d_read(tle, 53 + TLE_LINE2_OFFSET, 63 + TLE_LINE2_OFFSET);
    tmp_tle.norb = i_read(tle, 64 + TLE_LINE2_OFFSET, 68 + TLE_LINE2_OFFSET);

    tmp_tle.satno = 13;

    return tmp_tle;
}

/* ==================================================================
 Mimick the FORTRAN formatted read (assumes array starts at 1), copy
 characters to buffer then convert.
 ================================================================== */
static uint32_t i_read(uint8_t *str, uint8_t start, uint8_t stop) {
    uint32_t itmp = 0;
    uint8_t ii = 0;
    uint8_t tmp[TLE_SIZE];

    start--; /* 'C' arrays start at 0 */
    stop--;

    for (ii = start; ii <= stop; ii++) {
        *(tmp + ii - start) = str[ii]; /* Copy the characters. */
    }
    *(tmp + ii - start) = '\0'; /* NUL terminate */

    itmp = atol(tmp); /* Convert to long integer. */

    return itmp;
}

/* ==================================================================
 Mimick the FORTRAN formatted read (assumes array starts at 1), copy
 characters to buffer then convert.
 ================================================================== */

static double d_read(uint8_t *str, uint8_t start, uint8_t stop) {
    double dtmp = 0;
    uint8_t ii = 0;
    uint8_t tmp[TLE_SIZE];

    start--;
    stop--;

    for (ii = start; ii <= stop; ii++) {
        *(tmp + ii - start) = str[ii]; /* Copy the characters. */
    }
    *(tmp + ii - start) = '\0'; /* NUL terminate */

    dtmp = atof(tmp); /* Convert to long integer. */

    return dtmp;
}

tle_status update_tle(orbit_t *tle, orbit_t new_tle) {
    sgp4_status imode = SGP4_ERROR;
    tle_status tle_status_value = TLE_ERROR;

    imode = init_sgp4(&new_tle); // Check-Update TLE
    switch (imode) {
    case SGP4_ERROR:
        tle_status_value = TLE_ERROR; // SGDP error
        break;
    case SGP4_NOT_INIT:
        tle_status_value = TLE_ERROR; // SGDP not init
        break;
    case SGP4_ZERO_ECC:
        tle_status_value = TLE_NORMAL; // SGDP zero ecc
        break;
    case SGP4_NEAR_SIMP:
        tle_status_value = TLE_NORMAL; // SGP4 simple
        break;
    case SGP4_NEAR_NORM:
        tle_status_value = TLE_NORMAL; // SGP4 normal
        break;
    case SGP4_DEEP_NORM:
        tle_status_value = TLE_ERROR; // SDP4 normal
        break;
    case SGP4_DEEP_RESN:
        tle_status_value = TLE_ERROR; // SDP4 resonant
        break;
    case SGP4_DEEP_SYNC:
        tle_status_value = TLE_ERROR; // SDP4 synchronous
        break;
    default:
        tle_status_value = TLE_ERROR; // SGDP mode not recognised!
        break;
    }

    switch (tle_status_value) {
    case TLE_ERROR:
        // Not update the TLE and init again sgp4 with the previous TLE
        init_sgp4(tle);
        break;
    case TLE_NORMAL:
        tle->ep_year = new_tle.ep_year;
        tle->ep_day = new_tle.ep_day;
        tle->rev = new_tle.rev;
        tle->bstar = new_tle.bstar;
        tle->eqinc = new_tle.eqinc;
        tle->ecc = new_tle.ecc;
        tle->mnan = new_tle.mnan;
        tle->argp = new_tle.argp;
        tle->ascn = new_tle.ascn;
        tle->norb = new_tle.norb;
        tle->satno = new_tle.satno;
        break;
    }

    return tle_status_value;
}

flash_status flash_write_tle(orbit_t *flash_tle) {

    uint8_t tle_data[73] = { 0 };
    uint8_t tle_cnt = 0;

    cnvD_8(flash_tle->argp, &tle_data[tle_cnt]);
    tle_cnt += 8;
    cnvD_8(flash_tle->ascn, &tle_data[tle_cnt]);
    tle_cnt += 8;
    cnvD_8(flash_tle->bstar, &tle_data[tle_cnt]);
    tle_cnt += 8;
    cnvD_8(flash_tle->ecc, &tle_data[tle_cnt]);
    tle_cnt += 8;
    cnvD_8(flash_tle->ep_day, &tle_data[tle_cnt]);
    tle_cnt += 8;
    cnvD_8(flash_tle->eqinc, &tle_data[tle_cnt]);
    tle_cnt += 8;
    cnvD_8(flash_tle->mnan, &tle_data[tle_cnt]);
    tle_cnt += 8;
    cnvD_8(flash_tle->rev, &tle_data[tle_cnt]);
    tle_cnt += 8;
    cnv16_8(flash_tle->satno, &tle_data[tle_cnt]);
    tle_cnt += 2;
    cnv16_8(flash_tle->ep_year, &tle_data[tle_cnt]);
    tle_cnt += 2;
    cnv32_8(flash_tle->norb, &tle_data[tle_cnt]);

    if (flash_erase_block4K(TLE_BASE_ADDRESS) == FLASH_NORMAL) {
        flash_write_page(tle_data, sizeof(tle_data), TLE_BASE_ADDRESS);
    } else { return FLASH_ERROR; }

    return FLASH_NORMAL;

}

flash_status flash_read_tle(orbit_t *flash_tle) {

    uint8_t tle_data[8] = { 0 };
    uint8_t i = 0;
    uint32_t flash_read_address = TLE_BASE_ADDRESS;

    for (i = 0; i < 8; i++) {
        if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) {
            flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
        } else { return FLASH_ERROR; }
    }
    cnv8_D(tle_data, &flash_tle->argp);

    for (i = 0; i < 8; i++) {
        if (flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) {
            flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
        } else { return FLASH_ERROR; }
    }
    cnv8_D(tle_data, &flash_tle->ascn);

    for (i = 0; i < 8; i++) {
        if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) {
            flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
        } else { return FLASH_ERROR; }
    }
    cnv8_D(tle_data, &flash_tle->bstar);

    for (i = 0; i < 8; i++) {
        if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) {
            flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
        } else { return FLASH_ERROR; }
    }
    cnv8_D(tle_data, &flash_tle->ecc);

    for (i = 0; i < 8; i++) {
        if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) {
            flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
        } else { return FLASH_ERROR; }
    }
    cnv8_D(tle_data, &flash_tle->ep_day);

    for (i = 0; i < 8; i++) {
        if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) {
            flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
        } else { return FLASH_ERROR; }
    }
    cnv8_D(tle_data, &flash_tle->eqinc);

    for (i = 0; i < 8; i++) {
        if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) {
            flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
        } else { return FLASH_ERROR; }
    }
    cnv8_D(tle_data, &flash_tle->mnan);

    for (i = 0; i < 8; i++) {
        if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) {
            flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
        } else { return FLASH_ERROR; }
    }
    cnv8_D(tle_data, &flash_tle->rev);

    for (i = 0; i < 2; i++) {
        if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) {
            flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
        } else { return FLASH_ERROR; }
    }
    cnv8_16LE(tle_data, &flash_tle->satno);

    for (i = 0; i < 2; i++) {
        if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) {
            flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
        } else { return FLASH_ERROR; }
    }
    cnv8_16LE(tle_data, &flash_tle->ep_year);

    for (i = 0; i < 4; i++) {
        if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) {
            flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
        } else { return FLASH_ERROR; }
    }
    cnv8_32(tle_data, &flash_tle->norb);

    return FLASH_NORMAL;
}

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
 some basic error traps and the detemination of the orbital model is made.
 For near-earth satellites (xnodp < 225 minutes according to the NORAD
 classification) the SGP4 model is used, with truncated terms for low
 perigee heights when the drag terms are high. For deep-space satellites
 the SDP4 model is used and the deep-space terms initialised (a slow
 process). For orbits with an eccentricity of less than ECC_EPS the model
 reverts to a very basic circular model. This is not physically meaningfull
 but such a circluar orbit is not either! It is fast though.
 Callinr arguments:

 orb      : Input, structure with the orbital elements from NORAD 2-line
 element data in radian form.

 The return value indicates the orbital model used.
 ======================================================================= */

static sgp4_status init_sgp4(orbit_t *orb) {
    LOCAL_REAL theta2, theta4, xhdot1, x1m5th;
    LOCAL_REAL s4, del1, del0;
    LOCAL_REAL betao, betao2, coef, coef1;
    LOCAL_REAL etasq, eeta, qoms24;
    LOCAL_REAL pinvsq, tsi, psisq, c1sq;
    LOCAL_DOUBLE a0, a1, epoch;
    real temp0, temp1, temp2, temp3;
    uint32_t iday, iyear;

    /* Copy over elements. */
    /* Convert year to Gregorian with century as 1994 or 94 type ? */

    iyear = (uint32_t) orb->ep_year;

    if (iyear < 1960) {
        /* Assume 0 and 100 both refer to 2000AD */
        iyear += (iyear < 60 ? 2000 : 1900);
    }

    if (iyear < 1901 || iyear > 2099) {
        imode = SGP4_ERROR;
        return imode;
    }

    Isat = orb->satno;

    /* Compute days from 1st Jan 1900 (works 1901 to 2099 only). */

    iday = ((iyear - 1901) * 1461L) / 4L + 364L + 1L;

    SGP4_jd0 = JD1900 + iday + (orb->ep_day - 1.0); /* Julian day number. */

    epoch = (iyear - 1900) * 1.0e3 + orb->ep_day; /* YYDDD.DDDD as from 2-line. */

    eo = (real) orb->ecc;
    xno = (double) orb->rev * TWOPI / XMNPDA; /* Radian / unit time. */
    xincl = (real) orb->eqinc;
    xnodeo = (real) orb->ascn;
    omegao = (real) orb->argp;
    xmo = (real) orb->mnan;
    bstar = (real) orb->bstar;

    /* A few simple error checks here. */

    if (eo < (real) 0.0 || eo > ECC_LIMIT_HIGH) {
        imode = SGP4_ERROR;
        return imode;
    }

    if (xno < 0.035 * TWOPI / XMNPDA || xno > 18.0 * TWOPI / XMNPDA) {
        imode = SGP4_ERROR;
        return imode;
    }

    if (xincl < (real) 0.0 || xincl > (real) PI) {
        imode = SGP4_ERROR;
        return imode;
    }

    /* Start the initialisation. */

    if (eo < ECC_ZERO)
        imode = SGP4_ZERO_ECC; /* Special mode for "ideal" circular orbit. */
    else
        imode = SGP4_NOT_INIT;

    /*
     Recover original mean motion (xnodp) and semimajor axis (aodp)
     from input elements.
     */

    SINCOS(xincl, &sinIO, &cosIO);

    theta2 = cosIO * cosIO;
    theta4 = theta2 * theta2;
    x3thm1 = (real) 3.0 * theta2 - (real) 1.0;
    x1mth2 = (real) 1.0 - theta2;
    x7thm1 = (real) 7.0 * theta2 - (real) 1.0;

    a1 = pow(XKE / xno, TOTHRD);
    betao2 = (real) 1.0 - eo * eo;
    betao = SQRT(betao2);
    temp0 = (real) (1.5 * CK2) * x3thm1 / (betao * betao2);
    del1 = temp0 / (a1 * a1);
    a0 = a1 * (1.0 - del1 * (1.0 / 3.0 + del1 * (1.0 + del1 * 134.0 / 81.0)));
    del0 = temp0 / (a0 * a0);
    xnodp = xno / (1.0 + del0);
    aodp = (real) (a0 / (1.0 - del0));
    perigee = (aodp * (1.0 - eo) - AE) * XKMPER;
    apogee = (aodp * (1.0 + eo) - AE) * XKMPER;
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
    eta = aodp * eo * tsi;
    etasq = eta * eta;
    eeta = eo * eta;
    psisq = FABS((real )1.0 - etasq);
    coef = qoms24 * POW4(tsi);
    coef1 = coef / POW(psisq, 3.5);

    c2 =
            coef1 * (real) xnodp
                    * (aodp
                            * ((real) 1.0 + (real) 1.5 * etasq
                                    + eeta * ((real) 4.0 + etasq))
                            + (real) (0.75 * CK2) * tsi / psisq * x3thm1
                                    * ((real) 8.0
                                            + (real) 3.0 * etasq
                                                    * ((real) 8.0 + etasq)));

    c1 = bstar * c2;

    c4 =
            (real) 2.0 * (real) xnodp * coef1 * aodp * betao2
                    * (eta * ((real) 2.0 + (real) 0.5 * etasq)
                            + eo * ((real) 0.5 + (real) 2.0 * etasq)
                            - (real) (2.0 * CK2) * tsi / (aodp * psisq)
                                    * ((real) -3.0 * x3thm1
                                            * ((real) 1.0 - (real) 2.0 * eeta
                                                    + etasq
                                                            * ((real) 1.5
                                                                    - (real) 0.5
                                                                            * eeta))
                                            + (real) 0.75 * x1mth2
                                                    * ((real) 2.0 * etasq
                                                            - eeta
                                                                    * ((real) 1.0
                                                                            + etasq))
                                                    * COS((real )2.0 * omegao)));

    c5 = c3 = omgcof = (real) 0.0;

    if (imode == SGP4_NEAR_NORM) {
        /* BSTAR drag terms for normal near-space 'normal' model only. */
        c5 = (real) 2.0 * coef1 * aodp * betao2
                * ((real) 1.0 + (real) 2.75 * (etasq + eeta) + eeta * etasq);

        if (eo > ECC_ALL) {
            c3 = coef * tsi * a3ovk2 * (real) xnodp * (real) AE * sinIO / eo;
        }

        omgcof = bstar * c3 * COS(omegao);
    }

    temp1 = (real) (3.0 * CK2) * pinvsq * (real) xnodp;
    temp2 = temp1 * CK2 * pinvsq;
    temp3 = (real) (1.25 * CK4) * pinvsq * pinvsq * (real) xnodp;

    xmdot = xnodp
            + (0.5 * temp1 * betao * x3thm1
                    + (real) 0.0625 * temp2 * betao
                            * ((real) 13.0 - (real) 78.0 * theta2
                                    + (real) 137.0 * theta4));

    x1m5th = (real) 1.0 - (real) 5.0 * theta2;

    omgdot =
            (real) -0.5 * temp1 * x1m5th
                    + (real) 0.0625 * temp2
                            * ((real) 7.0 - (real) 114.0 * theta2
                                    + (real) 395.0 * theta4)
                    + temp3
                            * ((real) 3.0 - (real) 36.0 * theta2
                                    + (real) 49.0 * theta4);

    xhdot1 = -temp1 * cosIO;
    xnodot = xhdot1
            + ((real) 0.5 * temp2 * ((real) 4.0 - (real) 19.0 * theta2)
                    + (real) 2.0 * temp3 * ((real) 3.0 - (real) 7.0 * theta2))
                    * cosIO;

    xmcof = (real) 0.0;
    if (eo > ECC_ALL) {
        xmcof = (real) (-TOTHRD * AE) * coef * bstar / eeta;
    }

    xnodcf = (real) 3.5 * betao2 * xhdot1 * c1;
    t2cof = (real) 1.5 * c1;

    /* Check for possible divide-by-zero for X/(1+cosIO) when calculating xlcof */
    temp0 = (real) 1.0 + cosIO;

    if (fabs(temp0) < EPS_COSIO)
        temp0 = (real) SIGN(EPS_COSIO, temp0);

    xlcof = (real) 0.125 * a3ovk2 * sinIO * ((real) 3.0 + (real) 5.0 * cosIO)
            / temp0;

    aycof = (real) 0.25 * a3ovk2 * sinIO;

    SINCOS(xmo, &sinXMO, &cosXMO);
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

    em = eo;
    xinc = xincl;

    xmp = (double) xmo + xmdot * tsince;
    xnode = xnodeo + ts * (xnodot + ts * xnodcf);
    omega = omegao + omgdot * ts;

    switch (imode) {
    case SGP4_ZERO_ECC:
        /* Not a "real" orbit but OK for fast computation searches. */
        kep->smjaxs = kep->radius = (double) aodp * XKMPER / AE;
        kep->theta = fmod(PI + xnodp * tsince, TWOPI) - PI;
        kep->eqinc = (double) xincl;
        kep->ascn = xnodeo;

        kep->argp = 0;
        kep->ecc = 0;

        kep->rfdotk = 0;
        if (withvel)
            kep->rfdotk = aodp * xnodp * (XKMPER / AE * XMNPDA / 86400.0); /* For km/sec */
        else
            kep->rfdotk = 0;

        return imode;

    case SGP4_NEAR_SIMP:
        tempa = (real) 1.0 - ts * c1;
        tempe = bstar * ts * c4;
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
        tempe = bstar * (c4 * ts + c5 * (SIN(xmp) - sinXMO));
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
                * (XKMPER / AE * XMNPDA / 86400.0); /* Into km/sec */

        kep->rfdotk = ((real) XKE * SQRT(pl) * invR
                + temp2 * temp1 * (x1mth2 * cos2u + (real) 1.5 * x3thm1))
                * (XKMPER / AE * XMNPDA / 86400.0);
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

/*     ----------------------------------------------------------------
 *
 *                               sgp4ext.cpp
 *
 *    this file contains extra routines needed for the main test program for sgp4.
 *    these routines are derived from the astro libraries.
 *
 *                            companion code for
 *               fundamentals of astrodynamics and applications
 *                                    2007
 *                              by david vallado
 *
 *       (w) 719-573-2600, email dvallado@agi.com
 *
 *    current :
 *               7 may 08  david vallado
 *                           fix sgn
 *    changes :
 *               2 apr 07  david vallado
 *                           fix jday floor and str lengths
 *                           updates for constants
 *              14 aug 06  david vallado
 *                           original baseline
 *       ----------------------------------------------------------------      */

static double sgn(double x) {
    if (x < 0.0) {
        return -1.0;
    } else {
        return 1.0;
    }

}  // end sgn

/* -----------------------------------------------------------------------------
 *
 *                           function mag
 *
 *  this procedure finds the magnitude of a vector.  the tolerance is set to
 *    0.000001, thus the 1.0e-12 for the squared test of underflows.
 *
 *  author        : david vallado                  719-573-2600    1 mar 2001
 *
 *  inputs          description                    range / units
 *    vec         - vector
 *
 *  outputs       :
 *    vec         - answer stored in fourth component
 *
 *  locals        :
 *    none.
 *
 *  coupling      :
 *    none.
 * --------------------------------------------------------------------------- */

static double mag(double x[3]) {
    return sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
}  // end mag

/* -----------------------------------------------------------------------------
 *
 *                           procedure cross
 *
 *  this procedure crosses two vectors.
 *
 *  author        : david vallado                  719-573-2600    1 mar 2001
 *
 *  inputs          description                    range / units
 *    vec1        - vector number 1
 *    vec2        - vector number 2
 *
 *  outputs       :
 *    outvec      - vector result of a x b
 *
 *  locals        :
 *    none.
 *
 *  coupling      :
 *    mag           magnitude of a vector
 ---------------------------------------------------------------------------- */

static void cross(double vec1[3], double vec2[3], double outvec[3]) {
    outvec[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
    outvec[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
    outvec[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
}  // end cross

/* -----------------------------------------------------------------------------
 *
 *                           function dot
 *
 *  this function finds the dot product of two vectors.
 *
 *  author        : david vallado                  719-573-2600    1 mar 2001
 *
 *  inputs          description                    range / units
 *    vec1        - vector number 1
 *    vec2        - vector number 2
 *
 *  outputs       :
 *    dot         - result
 *
 *  locals        :
 *    none.
 *
 *  coupling      :
 *    none.
 *
 * --------------------------------------------------------------------------- */

static double dot(double x[3], double y[3]) {
    return (x[0] * y[0] + x[1] * y[1] + x[2] * y[2]);
}  // end dot

/* -----------------------------------------------------------------------------
 *
 *                           procedure angle
 *
 *  this procedure calculates the angle between two vectors.  the output is
 *    set to 999999.1 to indicate an undefined value.  be sure to check for
 *    this at the output phase.
 *
 *  author        : david vallado                  719-573-2600    1 mar 2001
 *
 *  inputs          description                    range / units
 *    vec1        - vector number 1
 *    vec2        - vector number 2
 *
 *  outputs       :
 *    theta       - angle between the two vectors  -pi to pi
 *
 *  locals        :
 *    temp        - temporary real variable
 *
 *  coupling      :
 *    dot           dot product of two vectors
 * --------------------------------------------------------------------------- */

static double angle(double vec1[3], double vec2[3]) {
    double small, undefined, magv1, magv2, temp;
    small = 0.00000001;
    undefined = 999999.1;

    magv1 = mag(vec1);
    magv2 = mag(vec2);

    if (magv1 * magv2 > small * small) {
        temp = dot(vec1, vec2) / (magv1 * magv2);
        if (fabs(temp) > 1.0)
            temp = sgn(temp) * 1.0;
        return acos(temp);
    } else
        return undefined;
}  // end angle

/* -----------------------------------------------------------------------------
 *
 *                           function sgp4_asinh
 *
 *  this function evaluates the inverse hyperbolic sine function.
 *
 *  author        : david vallado                  719-573-2600    1 mar 2001
 *
 *  inputs          description                    range / units
 *    xval        - angle value                                  any real
 *
 *  outputs       :
 *    arcsinh     - result                                       any real
 *
 *  locals        :
 *    none.
 *
 *  coupling      :
 *    none.
 *
 * --------------------------------------------------------------------------- */

static double sgp4_asinh(double xval) {
    return log(xval + sqrt(xval * xval + 1.0));
}  // end sgp4_asinh

/* -----------------------------------------------------------------------------
 *
 *                           function newtonnu
 *
 *  this function solves keplers equation when the true anomaly is known.
 *    the mean and eccentric, parabolic, or hyperbolic anomaly is also found.
 *    the parabolic limit at 168� is arbitrary. the hyperbolic anomaly is also
 *    limited. the hyperbolic sine is used because it's not double valued.
 *
 *  author        : david vallado                  719-573-2600   27 may 2002
 *
 *  revisions
 *    vallado     - fix small                                     24 sep 2002
 *
 *  inputs          description                    range / units
 *    ecc         - eccentricity                   0.0  to
 *    nu          - true anomaly                   -2pi to 2pi rad
 *
 *  outputs       :
 *    e0          - eccentric anomaly              0.0  to 2pi rad       153.02 �
 *    m           - mean anomaly                   0.0  to 2pi rad       151.7425 �
 *
 *  locals        :
 *    e1          - eccentric anomaly, next value  rad
 *    sine        - sine of e
 *    cose        - cosine of e
 *    ktr         - index
 *
 *  coupling      :
 *    asinh       - arc hyperbolic sine
 *
 *  references    :
 *    vallado       2007, 85, alg 5
 * --------------------------------------------------------------------------- */

static void newtonnu(double ecc, double nu, double *e0, double *m) {
    double small, sine, cose;

    /* ---------------------  implementation   --------------------- */
    *e0 = 999999.9;
    *m = 999999.9;
    small = 0.00000001;

    /* --------------------------- circular ------------------------ */
    if (fabs(ecc) < small) {
        *m = nu;
        *e0 = nu;
    }
    /* ---------------------- elliptical ----------------------- */
    else if (ecc < 1.0 - small) {
        sine = (sqrt(1.0 - ecc * ecc) * sin(nu)) / (1.0 + ecc * cos(nu));
        cose = (ecc + cos(nu)) / (1.0 + ecc * cos(nu));
        *e0 = atan2(sine, cose);
        *m = *e0 - ecc * sin(*e0);
    }
    /* -------------------- hyperbolic  -------------------- */
    else if (ecc > 1.0 + small) {
        if ((ecc > 1.0) && (fabs(nu) + 0.00001 < PI - acos(1.0 / ecc))) {
            sine = (sqrt(ecc * ecc - 1.0) * sin(nu)) / (1.0 + ecc * cos(nu));
            *e0 = sgp4_asinh(sine);
            *m = ecc * sinh(*e0) - *e0;
        }
    }
    /* ----------------- parabolic --------------------- */
    else if (fabs(nu) < 168.0 * DEG2RAD) {
        *e0 = tan(nu * 0.5);
        *m = *e0 + (*e0 * *e0 * *e0) / 3.0;
    }

    if (ecc < 1.0) {
        *m = fmod(*m, 2.0 * PI);
        if (*m < 0.0)
            *m = *m + 2.0 * PI;
        *e0 = fmod(*e0, 2.0 * PI);
    }
} /* end newtonnu */

/* -----------------------------------------------------------------------------
 *
 *                           function rv2coe
 *
 *  this function finds the classical orbital elements given the geocentric
 *    equatorial position and velocity vectors.
 *
 *  author        : david vallado                  719-573-2600   21 jun 2002
 *
 *  revisions
 *    vallado     - fix special cases                              5 sep 2002
 *    vallado     - delete extra check in inclination code        16 oct 2002
 *    vallado     - add constant file use                         29 jun 2003
 *    vallado     - add mu                                         2 apr 2007
 *
 *  inputs          description                    range / units
 *    r           - ijk position vector            km
 *    v           - ijk velocity vector            km / s
 *    mu          - gravitational parameter        km3 / s2
 *
 *  outputs       :
 *    p           - semilatus rectum               km
 *    a           - semimajor axis                 km
 *    ecc         - eccentricity
 *    incl        - inclination                    0.0  to pi rad
 *    omega       - longitude of ascending node    0.0  to 2pi rad
 *    argp        - argument of perigee            0.0  to 2pi rad
 *    nu          - true anomaly                   0.0  to 2pi rad
 *    m           - mean anomaly                   0.0  to 2pi rad
 *    arglat      - argument of latitude      (ci) 0.0  to 2pi rad
 *    truelon     - true longitude            (ce) 0.0  to 2pi rad
 *    lonper      - longitude of periapsis    (ee) 0.0  to 2pi rad
 *
 *  locals        :
 *    hbar        - angular momentum h vector      km2 / s
 *    ebar        - eccentricity     e vector
 *    nbar        - line of nodes    n vector
 *    c1          - v**2 - u/r
 *    rdotv       - r dot v
 *    hk          - hk unit vector
 *    sme         - specfic mechanical energy      km2 / s2
 *    i           - index
 *    e           - eccentric, parabolic,
 *                  hyperbolic anomaly             rad
 *    temp        - temporary variable
 *    typeorbit   - type of orbit                  ee, ei, ce, ci
 *
 *  coupling      :
 *    mag         - magnitude of a vector
 *    cross       - cross product of two vectors
 *    angle       - find the angle between two vectors
 *    newtonnu    - find the mean anomaly
 *
 *  references    :
 *    vallado       2007, 126, alg 9, ex 2-5
 * --------------------------------------------------------------------------- */

static void rv2coe(double r[3], double v[3], double mu, double *p, double *a,
        double *ecc, double *incl, double *omega, double *argp, double *nu,
        double *m, double *arglat, double *truelon, double *lonper) {

    double undefined, small, hbar[3], nbar[3], magr, magv, magn, ebar[3], sme,
            rdotv, infinite, temp, c1, hk, magh, e;

    uint8_t i;
    char typeorbit[3];

    small = 0.00000001;
    undefined = 999999.1;
    infinite = 999999.9;

// -------------------------  implementation   -----------------
    magr = mag(r);
    magv = mag(v);

// ------------------  find h n and e vectors   ----------------
    cross(r, v, hbar);
    magh = mag(hbar);
    if (magh > small) {
        nbar[0] = -hbar[1];
        nbar[1] = hbar[0];
        nbar[2] = 0.0;
        magn = mag(nbar);
        c1 = magv * magv - mu / magr;
        rdotv = dot(r, v);
        for (i = 0; i <= 2; i++)
            ebar[i] = (c1 * r[i] - rdotv * v[i]) / mu;
        *ecc = mag(ebar);

        // ------------  find a e and semi-latus rectum   ----------
        sme = (magv * magv * 0.5) - (mu / magr);
        if (fabs(sme) > small)
            *a = -mu / (2.0 * sme);
        else
            *a = infinite;
        *p = magh * magh / mu;

        // -----------------  find inclination   -------------------
        hk = hbar[2] / magh;
        *incl = acos(hk);

        // --------  determine type of orbit for later use  --------
        // ------ elliptical, parabolic, hyperbolic inclined -------
        strcpy(typeorbit, "ei");
        if (*ecc < small) {
            // ----------------  circular equatorial ---------------
            if ((*incl < small) || (fabs(*incl - PI) < small))
                strcpy(typeorbit, "ce");
            else
                // --------------  circular inclined ---------------
                strcpy(typeorbit, "ci");
        } else {
            // - elliptical, parabolic, hyperbolic equatorial --
            if ((*incl < small) || (fabs(*incl - PI) < small))
                strcpy(typeorbit, "ee");
        }

        // ----------  find longitude of ascending node ------------
        if (magn > small) {
            temp = nbar[0] / magn;
            if (fabs(temp) > 1.0)
                temp = sgn(temp);
            *omega = acos(temp);
            if (nbar[1] < 0.0)
                *omega = TWOPI - *omega;
        } else
            *omega = undefined;

        // ---------------- find argument of perigee ---------------
        if (strcmp(typeorbit, "ei") == 0) {
            *argp = angle(nbar, ebar);
            if (ebar[2] < 0.0)
                *argp = TWOPI - *argp;
        } else
            *argp = undefined;

        // ------------  find true anomaly at epoch    -------------
        if (typeorbit[0] == 'e') {
            *nu = angle(ebar, r);
            if (rdotv < 0.0)
                *nu = TWOPI - *nu;
        } else
            *nu = undefined;

        // ----  find argument of latitude - circular inclined -----
        if (strcmp(typeorbit, "ci") == 0) {
            *arglat = angle(nbar, r);
            if (r[2] < 0.0)
                *arglat = TWOPI - *arglat;
            *m = *arglat;
        } else
            *arglat = undefined;

        // -- find longitude of perigee - elliptical equatorial ----
        if ((*ecc > small) && (strcmp(typeorbit, "ee") == 0)) {
            temp = ebar[0] / *ecc;
            if (fabs(temp) > 1.0)
                temp = sgn(temp);
            *lonper = acos(temp);
            if (ebar[1] < 0.0)
                *lonper = TWOPI - *lonper;
            if (*incl > HALFPI)
                *lonper = TWOPI - *lonper;
        } else
            *lonper = undefined;

        // -------- find true longitude - circular equatorial ------
        if ((magr > small) && (strcmp(typeorbit, "ce") == 0)) {
            temp = r[0] / magr;
            if (fabs(temp) > 1.0)
                temp = sgn(temp);
            *truelon = acos(temp);
            if (r[1] < 0.0)
                *truelon = TWOPI - *truelon;
            if (*incl > HALFPI)
                *truelon = TWOPI - *truelon;
            *m = *truelon;
        } else
            *truelon = undefined;

        // ------------ find mean anomaly for all orbits -----------
        if (typeorbit[0] == 'e')
            newtonnu(*ecc, *nu, &e, m);
    } else {
        *p = undefined;
        *a = undefined;
        *ecc = undefined;
        *incl = undefined;
        *omega = undefined;
        *argp = undefined;
        *nu = undefined;
        *m = undefined;
        *arglat = undefined;
        *truelon = undefined;
        *lonper = undefined;
    }
}  // end rv2coe
