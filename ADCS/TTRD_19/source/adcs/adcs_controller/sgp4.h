/* > sgp4.h
 *
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
 *
 * 2.00 psc Sun May 28 1995 - Modifed for non-Dundee use.
 *
 */

#ifndef _SGP4_H
#define _SGP4_H

#include "tle.h"
#include "../services_utilities/common.h"
#include "../services_utilities/time.h"

// ERROR CODES
#define ERROR_CODE_SGP4_MODE     1

/* SGP4 function return values. */
typedef enum {
   SGP4_ERROR = -1,
   SGP4_NOT_INIT,
   SGP4_ZERO_ECC,
   SGP4_NEAR_SIMP,
   SGP4_NEAR_NORM,
   SGP4_DEEP_NORM,
   SGP4_DEEP_RESN,
   SGP4_DEEP_SYNC,
} sgp4_status;


uint8_t CTRL_SGP4_Init(void);
uint8_t CTRL_SGP4_Update(void);

void init_satpos_xyz(void);
sgp4_status satpos_xyz(double jd, xyz_t *pos, xyz_t *vel);

orbit_t calculate_tle(xyz_t position, xyz_t velocity, tle_epoch_t updt_tle_epoch);


#endif
