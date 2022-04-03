#ifndef REF_VECTORS_H__
#define REF_VECTORS_H__

#include "frame.h"

typedef struct {
    xyz_t sun_pos_eci;  // Position of Sun in ECI AU
    xyz_t sun_pos_ned;  // Position of Sun in NED AU
    double norm;
    double rtasc;
    double decl;
} sun_vector_t;

typedef struct {
    double sdate;
    double latitude;
    double longitude;
    double alt;
    double Xm;
    double Ym;
    double Zm;
    double norm;
    double decl;
    double incl;
    double h;
    double f;
} geomag_vector_t;


void CTRL_Ref_Sun_Update(double julian_date);

//extern geomag_vector_t igrf_vector;
void geomag(geomag_vector_t *pGeomag_vector);


//extern sun_vector_t sun_vector;
/*
 *	Gets the position victor of the sun
 *	in ECI (Earth Centred Inertial) frame
 *	of reference based on the time in ( Julian
 *	day) format
 */
void update_sun(sun_vector_t *pSun_vector);



#endif
