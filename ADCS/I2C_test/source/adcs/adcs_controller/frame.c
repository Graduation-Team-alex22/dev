#include "math.h"

#include "frame.h"

//-- PRIVATE MACROS --------------
#define C1      (1.72027916940703639E-2)
#define C1P2P   (C1 + TWOPI)
#define THGR70  (1.7321343856509374)
#define FK5R    (5.07551419432269442E-15)


//-- PRIVATE FUNCTIONS -----------
static double gha_aries(double jd);

xyz_t spher2cart(const llh_t *source)
{
   xyz_t ecef;
   
   // we approximate to a perfect sphere
   ecef.x = source->alt * cos(RAD(source->lat)) * cos(RAD(source->lon));
   ecef.y = source->alt * cos(RAD(source->lat)) * sin(RAD(source->lon));
   ecef.z = source->alt * sin(RAD(source->lat));
   
   return ecef;
}

xyz_t ECI2ECEF(const double* jd, const xyz_t* source)
{
   xyz_t ecef;

   double Cgst, Sgst = 0;
   double gst = gha_aries(*jd);
   Cgst = cos(gst);
   Sgst = sin(gst);
   ecef.x = source->x * Cgst + source->y * Sgst;
   ecef.y = -source->x * Sgst + source->y * Cgst;
   ecef.z = source->z;
   
   return ecef;
}

llh_t cart2spher(const xyz_t* source)
{
   llh_t llh;
   
   /* Cartesian to Spherical in deg and km from earth center */
   /* Altitude */
   llh.alt = sqrt(source->x * source->x + source->y * source->y + source->z * source->z);
   /* Latitude [-90, 90] */
   llh.lat = DEG(asin(source->z / llh.alt));
   /* Longitude [-180, 180] */
   llh.lon = DEG(atan2(source->y, source->x));
   
   return llh;
}

/*
 *
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
 */
static double gha_aries(double jd)
{

   /* Method from SGP4SUB.F code. */
   double ts70, ds70, trfac, theta, days50;
   long ids70;

   days50 = jd - 2400000.5 - 33281.0;

   ts70 = days50 - 7305.0;
   ids70 = (long) (ts70 + 1.0e-8);
   ds70 = ids70;

   trfac = ts70 - ds70;

   /* CALCULATE GREENWICH LOCATION AT EPOCH */
   theta = THGR70 + C1 * ds70 + C1P2P * trfac + ts70 * ts70 * FK5R;
   theta = fmod(theta, TWOPI);
   if (theta < 0.0)
      theta += TWOPI;

   return theta;
}
