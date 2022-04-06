#include "stdint.h"
#include "string.h"

#include "../services_utilities/common.h"
#include "../services_utilities/time.h"

#include "tle.h"

#include <stdlib.h>
#include "math.h"

//-- PRIVATE VARIABLES -----------------------------
static tle_data_t tle_data;

//-- PRIVATE FUNCTIONS -----------------------------
static uint32_t i_read(const char *str, uint8_t start, uint8_t stop);
static double d_read(const char *str, uint8_t start, uint8_t stop);
static void rv2coe(double r[3], double v[3], double mu, double *p, double *a,
        double *ecc, double *incl, double *omega, double *argp, double *nu,
        double *m, double *arglat, double *truelon, double *lonper);
        
uint8_t CTRL_TLE_Init(void)
{
   // initialize the privete variable with hardcoded TLE
   tle_data.orbit_data = read_tle(TLE_INIT_STRING);
   tle_data.data_status = INIT_DATA;
   return 0;
}

uint8_t CTRL_TLE_Update(void)
{
   orbit_t tmp_orbit;
   // Try to fetch tle data from flash
   if (flash_read_tle(&tmp_orbit) == FLASH_ERROR)
   { 
      tle_data.data_status = OLD_DATA;
      return ERROR_CODE_TLE_FLASH;
   }
   
   tle_data.orbit_data = tmp_orbit;
   tle_data.data_status = NEW_DATA;
   
   return 0;
}

tle_data_t CTRL_TLE_GetData(void)
{
   return tle_data;
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
orbit_t read_tle(const char *tle_string) {

    orbit_t tmp_tle;
    double bm, bx;

    tmp_tle.ep_year = (int) i_read(tle_string, 19, 20);

    if (tmp_tle.ep_year < 57) {
        tmp_tle.ep_year += 2000;
    } else {
        tmp_tle.ep_year += 1900;
    }
    tmp_tle.ep_day = d_read(tle_string, 21, 32);

    bm = d_read(tle_string, 54, 59) * 1.0e-5;
    bx = d_read(tle_string, 60, 61);
    tmp_tle.bstar = bm * pow(10.0, bx);

    tmp_tle.eqinc = RAD(d_read(tle_string, 9+TLE_LINE2_OFFSET, 16+TLE_LINE2_OFFSET));
    tmp_tle.ascn = RAD(d_read(tle_string, 18+TLE_LINE2_OFFSET, 25+TLE_LINE2_OFFSET));
    tmp_tle.ecc = d_read(tle_string, 27 + TLE_LINE2_OFFSET, 33 + TLE_LINE2_OFFSET)
            * 1.0e-7;
    tmp_tle.argp = RAD(d_read(tle_string, 35+TLE_LINE2_OFFSET, 42+TLE_LINE2_OFFSET));
    tmp_tle.mnan = RAD(d_read(tle_string, 44+TLE_LINE2_OFFSET, 51+TLE_LINE2_OFFSET));
    tmp_tle.rev = d_read(tle_string, 53 + TLE_LINE2_OFFSET, 63 + TLE_LINE2_OFFSET);
    tmp_tle.norb = i_read(tle_string, 64 + TLE_LINE2_OFFSET, 68 + TLE_LINE2_OFFSET);

    tmp_tle.satno = 13;

    return tmp_tle;
}


/* ==================================================================
 Mimick the FORTRAN formatted read (assumes array starts at 1), copy
 characters to buffer then convert.
 ================================================================== */
static uint32_t i_read(const char *str, uint8_t start, uint8_t stop)
{
   uint32_t itmp = 0;
   uint8_t ii = 0;
   char tmp[TLE_SIZE];

   start--; /* 'C' arrays start at 0 */
   stop--;

   for (ii = start; ii <= stop; ii++)
   {
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

static double d_read(const char *str, uint8_t start, uint8_t stop)
{
   double dtmp = 0;
   uint8_t ii = 0;
   char tmp[TLE_SIZE];

   start--;
   stop--;

   for (ii = start; ii <= stop; ii++)
   {
      *(tmp + ii - start) = str[ii]; /* Copy the characters. */
   }
   *(tmp + ii - start) = '\0'; /* NUL terminate */

   dtmp = atof(tmp); /* Convert to long integer. */

   return dtmp;
}

/**
 * @brief Calculate TLE, with given position, velocity and TLE epoch
 * @param position in km
 * @param velocity in km/s
 * @param updt_tle_epoch
 * @return new TLE
 */
orbit_t calculate_tle(xyz_t position, xyz_t velocity, tle_epoch_t updt_tle_epoch) {

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
   magr = vect_magnitude_arr(r);
   magv = vect_magnitude_arr(v);

   // ------------------  find h n and e vectors   ----------------
   cross(r, v, hbar);
   magh = vect_magnitude_arr(hbar);
   if (magh > small) {
     nbar[0] = -hbar[1];
     nbar[1] = hbar[0];
     nbar[2] = 0.0;
     magn = vect_magnitude_arr(nbar);
     c1 = magv * magv - mu / magr;
     rdotv = dot(r, v);
     for (i = 0; i <= 2; i++)
         ebar[i] = (c1 * r[i] - rdotv * v[i]) / mu;
     *ecc = vect_magnitude_arr(ebar);

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


flash_status_e flash_write_tle(orbit_t *flash_tle) {

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

   if (flash_erase_block4K(TLE_BASE_ADDRESS) == FLASH_NORMAL) 
   {
     flash_write_page(tle_data, sizeof(tle_data), TLE_BASE_ADDRESS);
   } 
   else 
   { 
      return FLASH_ERROR; 
   }

   return FLASH_NORMAL;

}

flash_status_e flash_read_tle(orbit_t *flash_tle) 
{
   uint8_t tle_data[8] = { 0 };
   uint8_t i = 0;
   uint32_t flash_read_address = TLE_BASE_ADDRESS;

   for (i = 0; i < 8; i++) 
   {
      if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) 
      {
         flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
      } 
      else 
      { 
         return FLASH_ERROR; 
      }
   }
   cnv8_D(tle_data, &flash_tle->argp);

   for (i = 0; i < 8; i++) 
   {
      if (flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) 
      {
         flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
      } 
      else 
      { 
         return FLASH_ERROR; 
      }
   }
   cnv8_D(tle_data, &flash_tle->ascn);

   for (i = 0; i < 8; i++) 
   {
      if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) 
      {
         flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
      } 
      else 
      { 
         return FLASH_ERROR; 
      }
   }
   cnv8_D(tle_data, &flash_tle->bstar);

   for (i = 0; i < 8; i++) 
   {
      if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) 
      {
         flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
      } 
      else 
      { 
         return FLASH_ERROR; 
      }
   }
   cnv8_D(tle_data, &flash_tle->ecc);

   for (i = 0; i < 8; i++) 
   {
      if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) 
      {
         flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
      } 
      else 
      { 
         return FLASH_ERROR; 
      }
   }
   cnv8_D(tle_data, &flash_tle->ep_day);

   for (i = 0; i < 8; i++) 
   {
      if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL)
      {
         flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
      } 
      else 
      { 
         return FLASH_ERROR; 
      }
   }
   cnv8_D(tle_data, &flash_tle->eqinc);

   for (i = 0; i < 8; i++) 
   {
      if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) 
      {
         flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
      } 
      else 
      { 
         return FLASH_ERROR;
      }
   }
   cnv8_D(tle_data, &flash_tle->mnan);

   for (i = 0; i < 8; i++) 
   {
      if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL) 
      {
         flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
      } 
      else 
      { 
         return FLASH_ERROR; 
      }
   }
   cnv8_D(tle_data, &flash_tle->rev);

   for (i = 0; i < 2; i++) 
   {
      if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL)
      {
         flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
      } 
      else 
      { 
         return FLASH_ERROR; 
      }
   }
   cnv8_16LE(tle_data, &flash_tle->satno);

   for (i = 0; i < 2; i++) 
   {
      if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL)
      {
         flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
      }
      else 
      { 
         return FLASH_ERROR; 
      }
   }
   cnv8_16LE(tle_data, &flash_tle->ep_year);

   for (i = 0; i < 4; i++) 
   {
      if(flash_read_byte(&tle_data[i], flash_read_address) == FLASH_NORMAL)
      {
         flash_read_address = flash_read_address + TLE_ADDRESS_OFFSET;
      }
      else 
      { 
         return FLASH_ERROR; 
      }
   }
   cnv8_32(tle_data, &flash_tle->norb);

   return FLASH_NORMAL;
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

double sgn(double x) {
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

double mag(double x[3]) {
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

void cross(double vec1[3], double vec2[3], double outvec[3]) {
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

double dot(double x[3], double y[3]) {
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

double angle(double vec1[3], double vec2[3]) {
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
 *                           function newtonnu
 *
 *  this function solves keplers equation when the true anomaly is known.
 *    the mean and eccentric, parabolic, or hyperbolic anomaly is also found.
 *    the parabolic limit at 168? is arbitrary. the hyperbolic anomaly is also
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
 *    e0          - eccentric anomaly              0.0  to 2pi rad       153.02 ?
 *    m           - mean anomaly                   0.0  to 2pi rad       151.7425 ?
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

void newtonnu(double ecc, double nu, double *e0, double *m) {
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
double sgp4_asinh(double xval) {
    return log(xval + sqrt(xval * xval + 1.0));
}  // end sgp4_asinh
