#ifndef TLE_H__
#define TLE_H__

#include "../services_utilities/flash.h"

#define TLE_SIZE 145
#define TLE_LINE2_OFFSET 69+1
#define TLE_INIT_STRING "1 25544U 98067A   16229.19636472  .00005500  00000-0  87400-4 0  9991\n2 25544  51.6439 118.5889 0001926 134.0246   3.7037 15.55029964 14324"

// ERROR CODES
#define ERROR_CODE_TLE_FLASH     1

typedef struct orbit_s {
   /* Add the epoch time if required. */
   uint16_t ep_year; /* Year of epoch, e.g. 94 for 1994, 100 for 2000AD */
   double ep_day; /* Day of epoch from 00:00 Jan 1st ( = 1.0 ) */
   double rev; /* Mean motion, revolutions per day */
   double bstar; /* Drag term .*/
   double eqinc; /* Equatorial inclination, radians */
   double ecc; /* Eccentricity */
   double mnan; /* Mean anomaly at epoch from elements, radians */
   double argp; /* Argument of perigee, radians */
   double ascn; /* Right ascension (ascending node), radians */
   double smjaxs; /* Semi-major axis, km */
   uint32_t norb; /* Orbit number, for elements */
   uint16_t satno; /* Satellite number. */
} orbit_t;

typedef struct{
   orbit_t orbit_data;
   enum { INIT_DATA, NEW_DATA, OLD_DATA} data_status;
}tle_data_t;

uint8_t CTRL_TLE_Init(void);
uint8_t CTRL_TLE_Update(void);
tle_data_t CTRL_TLE_GetData(void);

uint8_t init_tle(void);
uint8_t update_tle(orbit_t *tle, orbit_t new_tle);

orbit_t read_tle(const char *tle_string);
flash_status_e flash_write_tle(orbit_t *flash_tle);
flash_status_e flash_read_tle(orbit_t *flash_tle);

double sgn(double x);
double mag(double x[3]);
void cross(double vec1[3], double vec2[3], double outvec[3]);
double dot(double x[3], double y[3]);
double angle(double vec1[3], double vec2[3]);
static void newtonnu(double ecc, double nu, double *e0, double *m);
double sgp4_asinh(double xval);

#endif
