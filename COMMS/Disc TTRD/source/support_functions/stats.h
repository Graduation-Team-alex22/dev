#ifndef INC_STATS_H_
#define INC_STATS_H_

// Project header
#include "../main/main.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_adc_task.h"
#include <math.h>
#include "wod_handling.h"

typedef struct 
{
  uint16_t rx_failed_cnt;
  uint16_t rx_crc_failed_cnt;
  uint16_t tx_failed_cnt;
  uint16_t tx_frames_cnt;
  uint16_t rx_frames_cnt;
  uint32_t uptime_h;
  uint32_t uptime_m;
  uint32_t uptime_s;
  uint32_t last_tick;
  float comms_temperature;
  float eps_temperature;
  float bat_temperature;
  int16_t last_tx_error_code;
  int16_t last_rx_error_code;
  uint32_t battery_mV;
  int32_t battery_mA;
  uint32_t bus_3300_mA;
  uint32_t bus_5000_mA;
  uint16_t invalid_dest_frames_cnt;
  uint32_t last_utc;
  uint8_t rst_src;
  uint16_t sw_on_cmd_cnt;
  uint16_t sw_off_cmd_cnt;
  uint16_t sw_cmd_failed_cnt;
} comms_rf_stat_t;

//Init
void comms_rf_stats_init(comms_rf_stat_t *h);

//Update
uint32_t comms_rf_stats_update(void);

//Setters
void comms_rf_stats_set_reset_src(comms_rf_stat_t *h);

void comms_rf_stats_frame_received(comms_rf_stat_t *h, uint8_t succesfull,
			      int16_t error);

void comms_rf_stats_frame_transmitted(comms_rf_stat_t *h, uint8_t succesfull,
				 int16_t error);

void comms_rf_stats_invalid_dest_frame(comms_rf_stat_t *h);

void comms_rf_stats_invalid_crc_frame(comms_rf_stat_t *h);

void comms_rf_stats_wod_received(comms_rf_stat_t *h, const uint8_t *obc_wod);

void comms_rf_stats_sw_cmd_received(comms_rf_stat_t *h, uint8_t on);

void comms_rf_stats_sw_cmd_failed(comms_rf_stat_t *h);


//Getters
float comms_rf_stats_get_temperature(comms_rf_stat_t *h);







#endif /* INC_STATS_H_ */
