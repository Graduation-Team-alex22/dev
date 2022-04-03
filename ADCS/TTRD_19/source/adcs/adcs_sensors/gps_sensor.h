/**********************************************************
  This module provides an interface to Neo-6M GPS module

@Hardware:
  - 1 UART peripheral chosen by Application
  - 1 DMA stream channel chosen by Application

Auther: Mohamed Said & Ali Fakharany
Date:		2022-03-15

**********************************************************/	

#ifndef GPS_H__
#define GPS_H__

#include "stdint.h"
#include "../hsi_library/stm32f4xx_usart.h"
#include "../services_utilities/nmea_parsing.h"

/************* PUBLIC MACROS ***************/
#define  GPS_BAUDRATE         9600
#define  GPS_RX_BUFFER_SIZE   ((uint16_t)512)

#define  ERROR_CODE_NMEA_BUFFER_FULL      1

/************ Public structures ************/
typedef struct {
   double x;
   double y;
   double z;
} xyz_t;

typedef struct {
   uint8_t d3fix;                // 0 no fix , 1 fix , 2 differential fix
   xyz_t p_gps_ecef;             // in km
   double p_gps_lla[3];          // altitude is in meters from sea level
   xyz_t v_gps_ecef;             // in km/s
   float utc_time;               // HHMMSS.xx
   double sec;
   uint16_t week;
   uint8_t num_sat;              // number of GPS satellites in vision
   uint8_t reset_flag;
   float DOP[3];                 // Delusion of precision (lower is better) - order: P H V
} gps_sensor_t;


/************ Public Interfaces ************/
/*
   GPS_Sensor_Init

   Initialize GPS Sensor 

   @note    The uart peripheral must be already iniialized by the task before using this funcion.
   @note    The DMA stream must be already iniialized by the task before using this funcion.
   @note    The DMA channel must be selected by the task before using this funcion.

   @param DMA_streamx,       DMA stream to be used.

   @return error_code,  An error code, Or Zero if no Error.

*/
uint8_t GPS_Sensor_Init(DMA_Stream_TypeDef * DMA_streamx);

/*
   GPS_Sensor_Update

   Receive updated GPS data from the sensor over a specified uart peripheral

   @note    None

   @param   None

   @return Zero if no Error.

*/
uint16_t GPS_Sensor_Update(void);

char* GPS_Sensor_get_pBuffer(void);

gps_sensor_t GPS_Sensor_GetData(void);

#endif
