/**********************************************************
This module provides conversions between different frames of reference.
It supports the following frames:
   - ECEF: Earth-Centeric Earth-Fixed      @ https://en.wikipedia.org/wiki/Earth-centered,_Earth-fixed_coordinate_system
   - ECI: Earth-Centered Inertial          @ https://en.wikipedia.org/wiki/Earth-centered_inertial
   - LLH: Latitude, Longitude and Height   @ https://en.wikipedia.org/wiki/Geographic_coordinate_system
   - NED: North, East and Down             @ https://en.wikipedia.org/wiki/Local_tangent_plane_coordinates

Available conversions are:
   - LLA  ---> ECEF
   
Auther: Mohamed Said & Ali Fakharany
Date:		2022-04-02

**********************************************************/	

#ifndef FRAME_H__
#define FRAME_H__

typedef struct {
    double x;
    double y;
    double z;
} xyz_t;

typedef struct {
    double lat;
    double lon;
    double alt;
} llh_t;

// unit of length will follow the height units as is
xyz_t CTRL_LLA2ECEF(const llh_t *source);

/*
void ECEF2ECI(double jd, xyz_t from, xyz_t *to);
void ECI2ECEF(double jd, xyz_t from, xyz_t *to);
void ECI2NED(xyz_t from, xyz_t *to, double w, double i, double u);
void cart2spher(xyz_t from, lla_t *to);
void spher2cart(lla_t from, xyz_t *to);

double norm(double x1, double x2, double x3);
*/

#endif
