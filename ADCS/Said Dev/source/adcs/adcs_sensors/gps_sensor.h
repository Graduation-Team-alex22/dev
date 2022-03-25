/*
  This module provides an interface to Neo-6M GPS module

@Hardware:
  - 1 UART peripheral chosen by Application
  - 1 DMA stream channel chosen by Application
	



*/

#ifndef GPS_H__
#define GPS_H__

#include <stdint.h>
#include "../hsi_library/stm32f4xx_usart.h"


#define GPS_BAUDRATE		9600
#define GPS_RX_BUFFER_SIZE ((uint16_t)512)


uint8_t GPS_Sensor_Init(USART_TypeDef * uartx, DMA_Stream_TypeDef * DMA_streamx, uint32_t DMA_channelx);
uint16_t GPS_Sensor_Update(void);
uint8_t GPS_Sensor_GetData(char * buf);


/*
//#include "../services_utilities/time.h"
//#include "../services_utilities/services.h"
#define NMEA_MAX_FIELD_SIZE 15
#define NMEA_MAX_FIELDS     22
#define NMEA_HEADER         0
#define NMEA_HEADER_SIZE    6
#define NMEA_MAX_LEN        80
#define NMEA_NUM_SENTENCES  8
#define NMEA_GSA_3DFIX      2
#define NMEA_GPS_TIME       1
#define NMEA_GPS_WEEK       2
#define NMEA_LSP_X_WGS      3
#define NMEA_LSP_Y_WGS      4
#define NMEA_LSP_Z_WGS      5
#define NMEA_LSV_VX_WGS     3
#define NMEA_LSV_VY_WGS     4
#define NMEA_LSV_VZ_WGS     5
#define NMEA_GGA_TIME       1
#define NMEA_GGA_NUM_SAT    7
typedef struct {
    double x;
    double y;
    double z;
} xyz_t;
typedef enum {
    GPS_ERROR_FLASH = 0,
    GPS_OFF,
    GPS_UNLOCK,
    GPS_RESET
} _gps_status;
typedef struct {
    uint8_t d3fix;
    xyz_t p_gps_ecef; // in km
    xyz_t v_gps_ecef; // in km/s
    float utc_time;
    double sec;
    uint16_t week;
    uint8_t num_sat;
    _gps_status status;
    uint8_t reset_flag;
} gps_sensor_t;
//SAT_returnState gps_parse_fields(uint8_t *buf, const uint8_t size, uint8_t (*res)[NMEA_MAX_FIELDS]);
//SAT_returnState gps_parse_logic(const uint8_t (*res)[NMEA_MAX_FIELDS], gps_sensor_t *pGpsSensor);
void init_gps_uart();
void get_gps_sentences();
//void init_gps(time_utc_t gps_utc, gps_sensor_t *pGps_sensor);
//void update_gps_alarm_from_flash(time_utc_t gps_utc, gps_sensor_t *gps_state_value);
void update_gps(gps_sensor_t *gps_state_value);
void HAL_GPS_Alarm_Handler(gps_sensor_t *pGpsSensor);
//_gps_status HAL_SetAlarm_GPS_ON(time_utc_t gps_utc); // + HAL_StatusTypeDef HAL_SetAlarm_GPS(uint8_t gps_alarm_hour, uint8_t gps_alarm_min, uint8_t gps_alarm_sec);
//_gps_status HAL_SetAlarm_GPS_LOCK(time_utc_t gps_utc, uint8_t gps_alarm_ulock_min);
void HAL_GPS_UART_IRQHandler(USART_TypeDef *huart);
void UART_GPS_Receive_IT(USART_TypeDef *huart);
*/
#endif
