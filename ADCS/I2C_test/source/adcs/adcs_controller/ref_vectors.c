#include "../services_utilities/time.h"
#include "../services_utilities/common.h"
#include "math.h"
#include "frame.h"
#include "sgp4.h"
#include "geomag.h"

#include "ref_vectors.h"

//-- PRIVATE VARIABLES ----------------------
static sun_vector_t sun_vector;
static geomag_vector_t igrf_vector;

/*

% Given Julian Date, calculate Earth-centered
% inertial (ECI) coordinates (in meters) of the vector(s) pointing from
% Earth's center to the center of the Sun.
% Encapsulated by Seth B. Wagenman, from stargazing.net/kepler/sun.html as
% corrected by https://astronomy.stackexchange.com/a/37199/34646 along with
% other minor corrections based on recent revisions to the ecliptic plane.

Cite:
Seth Wagenman (2022). approxECISunPosition
(https://www.mathworks.com/matlabcentral/fileexchange/78766-approxecisunposition),
MATLAB Central File Exchange. Retrieved April 2, 2022.
*/

void CTRL_Ref_Sun_Update(void)
{
   double julian_date = time_getTime().Julian_Date;
   // Set time(s) relative to the J2000.0 epoch 
   double fractionalDay = julian_date - 2451545.0;
   
   // calculate the mean longitude of the sun in degrees
   double meanLongitudeSunDegrees = 
      280.4606184 + ((36000.77005361 / 36525) * fractionalDay);
   
   // Calculate mean anomaly of the sun in degrees
   double meanAnomalySunDegrees = 
      357.5277233 + ((35999.05034 / 36525) * fractionalDay);
   double meanAnomalySunRadians = RAD(meanAnomalySunDegrees);
   
   // calculate ecliptic longitude of the Sun in degrees
   double eclipticLongitudeSunDegrees = meanLongitudeSunDegrees +
     (1.914666471 * sin(meanAnomalySunRadians)) +
     (0.918994643 * sin(2 * meanAnomalySunRadians));
   double eclipticLongitudeSunRadians = RAD(eclipticLongitudeSunDegrees);
   
   // calculate Obliquity of the ecliptic plane
   double epsilonDegrees = 23.43929 - ((46.8093 / 3600) * fractionalDay / 36525);
   double epsilonRadians = RAD(epsilonDegrees);
   
   /***** Calculations for unit vectors direction to the Sun are in ECI ****/
   //calculate the Earth to Sun unit vector(s) USING RADIANS, NOT DEGREES
   double unitEarthSunVector[3] = 
   {
      cos(eclipticLongitudeSunRadians),
      cos(epsilonRadians) * sin(eclipticLongitudeSunRadians),
      sin(epsilonRadians) * sin(eclipticLongitudeSunRadians)
   };
   
   // Scale up from unit vector(s); distances vary--Earth's orbit is elliptical
   double distEarthToSunInAstronomicalUnits =
      1.000140612 - (0.016708617 * cos(meanAnomalySunRadians)) -
      (0.000139589 * cos(2 * meanAnomalySunRadians));
   
   double distEarthToSunInMeters =
      149597870700 * distEarthToSunInAstronomicalUnits;

   sun_vector.sun_pos_eci.x = distEarthToSunInMeters * unitEarthSunVector[0];
   sun_vector.sun_pos_eci.y = distEarthToSunInMeters * unitEarthSunVector[1];
   sun_vector.sun_pos_eci.z = distEarthToSunInMeters * unitEarthSunVector[2];
   
}

void CTRL_Ref_Geomag_Update(void)
{
   xyz_t p_eci = CTRL_SGP4_GetPECI();
   xyz_t p_ecef = { .x = 0, .y = 0, .z = 0 };
   llh_t p_ecef_llh = { .lat = 0, .lon = 0, .alt = 0 };
   
   time_t time = time_getTime();
   
   p_ecef = ECI2ECEF(&time.Julian_Date, &p_eci);
   p_ecef_llh = cart2spher(&p_ecef);
   igrf_vector.sdate = time.decyear;
   igrf_vector.latitude = p_ecef_llh.lat;
   igrf_vector.longitude = p_ecef_llh.lon;
   igrf_vector.alt = p_ecef_llh.alt;
   geomag(&igrf_vector); // Xm,Ym,Zm in NED
   igrf_vector.norm = norm(igrf_vector.Xm, igrf_vector.Ym, igrf_vector.Zm);
   
}

ref_vectors_t CTRL_Ref_Vectors_GetData(void)
{
   ref_vectors_t r = {.sun_vec = sun_vector, .geomag_vec = igrf_vector};
   return r;
}
