/*
 * adcs_frame.c
 *
 *  Created on: Jun 13, 2016
 *      Author: azisi
 */

#include "adcs_frame.h"
#include "adcs_common.h"

#include <math.h>

#define C1      (1.72027916940703639E-2)
#define C1P2P   (C1 + TWOPI)
#define THGR70  (1.7321343856509374)
#define FK5R    (5.07551419432269442E-15)

static double gha_aries(double jd);

void ECI2ECEF(double jd, xyz_t from, xyz_t *to) {

    double Cgst, Sgst = 0;
    double gst = gha_aries(jd);
    Cgst = cos(gst);
    Sgst = sin(gst);
    to->x = from.x * Cgst + from.y * Sgst;
    to->y = -from.x * Sgst + from.y * Cgst;
    to->z = from.z;

}

void ECEF2ECI(double jd, xyz_t from, xyz_t *to) {

    double Cgst, Sgst = 0;
    double gst = - gha_aries(jd); // R^-1(a) = R(a) (z-axis)
    Cgst = cos(gst);
    Sgst = sin(gst);
    to->x = from.x * Cgst + from.y * Sgst;
    to->y = -from.x * Sgst + from.y * Cgst;
    to->z = from.z;

}

void ECI2NED(xyz_t from, xyz_t *to, double w, double i, double u) {

    double R[3][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };

    R[0][0] = -sin(u) * sin(w) * cos(i) + cos(u) * cos(w);
    R[0][1] = sin(u) * cos(i) * cos(w) + sin(w) * cos(u);
    R[0][2] = sin(i) * sin(u);

    R[1][0] = -sin(u) * cos(w) - sin(w) * cos(i) * cos(u);
    R[1][1] = -sin(u) * sin(w) + cos(i) * cos(u) * cos(w);
    R[1][2] = sin(i) * cos(u);

    R[2][0] = sin(i) * sin(w);
    R[2][1] = -sin(i) * cos(w);
    R[2][2] = cos(i);

    to->x = R[1][0] * from.x + R[1][1] * from.y + R[1][2] * from.z; // Y
    to->y = -(R[2][0] * from.x + R[2][1] * from.y + R[2][2] * from.z); // -Z
    to->z = -(R[0][0] * from.x + R[0][1] * from.y + R[0][2] * from.z); // -X

}

void cart2spher(xyz_t from, llh_t *to) {

    /* Cartesian to Spherical in deg and km from earth center */
    /* Altitude */
    to->alt = sqrt(from.x * from.x + from.y * from.y + from.z * from.z);
    /* Latitude [-90, 90] */
    to->lat = DEG(asin(from.z / to->alt));
    /* Longitude [-180, 180] */
    to->lon = DEG(atan2(from.y, from.x));

}

void spher2cart(llh_t from, xyz_t *to) {

    // phi -> lon, th -> lat
    to->x = from.alt * sin(RAD(from.lat)) * cos(RAD(from.lon));
    to->y = from.alt * sin(RAD(from.lat)) * sin(RAD(from.lon));
    to->z = from.alt * cos(RAD(from.lat));

}

double norm(double x1, double x2, double x3) {
    return sqrt(x1 * x1 + x2 * x2 + x3 * x3);
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
static double gha_aries(double jd) {

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
