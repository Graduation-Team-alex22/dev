/*
This parser supports only two types of nmea messages
but it is scalable so you can easily add more

*/

#ifndef NMEA_PARSING_H__
#define NMEA_PARSING_H__

#include "stdint.h"

/******************************************
GP-GGA (GPS fix data)
	$--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
	 1) Time (UTC)
	 2) Latitude
	 3) N or S (North or South)
	 4) Longitude
	 5) E or W (East or West)
	 6) GPS Quality Indicator,
	 0 - fix not available,
	 1 - GPS fix,
	 2 - Differential GPS fix
	 7) Number of satellites in view, 00 - 12
	 8) Horizontal Dilution of precision
	 9) Antenna Altitude above/below mean-sea-level (geoid)
	10) Units of antenna altitude, meters
	11) Geoidal separation, the difference between the WGS-84 earth
	 ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid
	12) Units of geoidal separation, meters
	13) Age of differential GPS data, time in seconds since last SC104
	 type 1 or 9 update, null field when DGPS is not used
	14) Differential reference station ID, 0000-1023
	15) Checksum


GP-GSA (GPS DOP and active satellites)
	$--GSA,a,a,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x.x,x.x,x.x*hh
	 1) Selection mode
	 2) Mode
	 3) ID of 1st satellite used for fix
	 4) ID of 2nd satellite used for fix
	 ...
	14) ID of 12th satellite used for fix
	15) PDOP in meters
	16) HDOP in meters
	17) VDOP in meters
	18) Checksum

******************************************/

// struct for parser output
typedef struct {
	// GPGGA
	float time;                // UTC time
	double lat;                // Latitude
	double lon;                // Longitude
	uint8_t fix;               // GPS Fix
	uint8_t num_sat;           // visible satellites number
	float hdop;                // Horizontal Delusion of Precision
	double alt;                // Altitude - from sea level
	double Geoidal_sep;
	double diff_fix_age;
	uint16_t diff_ref_stationID;
	
	// GPGSA
	uint8_t sel_mode;
	uint8_t mode;
	uint16_t fix_sat_ID[12];
	float DOP[3];              // Positional, Horizontal, Vertical Delusion of Precision
	
} parser_output_t;

//-- PUPLIC FUNCTIONS -------------------
/*
   nmea_parser_init

   Initializes parser structures and mechanisms

   @param None

   @return     Error_code, or zero if no error
*/
uint8_t nmea_parser_init(void);

/*
   nmea_parser_init

   Initializes parser structures and mechanisms

   @param nmea_sen_str     a pointer to the nmea string to be parsed.  
   @return Error_code      error_code, or zero if no error
*/
uint8_t nmea_sentence_parsing(char* nmea_sen_str);

/*
   nmea_sentence_getOutput

   returns the parser output of the latest parsing proccess

   @param  None.  
   @return parser_output_t an instance of the parser output structure loaded
                           with parser results.
*/
parser_output_t nmea_sentence_getOutput(void);

#endif
