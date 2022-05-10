#include "stats.h"









//Private functions
static inline void update_temperature(comms_rf_stat_t *h)// this gives the temp in mv
{
	h->comms_temperature = ADC1_Get_Temperature_mv();					// this gives the temp in mv
	//h->comms_temperature = ADC1_Get_Temperature_Celsius();	// this gives the temp in Celsius
}
static inline void update_uptime(comms_rf_stat_t *h)
{
  uint32_t now = getCurrentTick();
  uint32_t diff = now - h->last_tick;

  h->uptime_s += (diff/1000);
  h->uptime_m += h->uptime_s / 60;
  h->uptime_s %= 60;
  h->uptime_h += h->uptime_m / 60;
  h->uptime_m %= 60;
  h->last_tick = now;
}
static uint32_t get_wod_utc_time(const uint8_t *obc_wod)
{
  uint32_t ret = 0;
  if(obc_wod == NULL)
	{
    return ret;
  }
  memcpy(&ret, obc_wod, sizeof(uint32_t));
  return ret;
}
static uint32_t get_wod_bat_voltage(const uint8_t *obc_wod)
{
  uint32_t ret = 0;
  uint8_t val;
  if(obc_wod == NULL)
	{
    return ret;
  }

  val = obc_wod[sizeof(uint32_t)];
  if(bat_voltage_valid(val)) {
    ret = 3000 + val * 50;
  }
  return ret;
}
static int32_t get_wod_bat_current(const uint8_t *obc_wod)
{
  int32_t ret = -2000;
  uint8_t val;
  if(obc_wod == NULL)
	{
    return ret;
  }

  val = obc_wod[sizeof(uint32_t) + 1];
  if(bat_current_valid(val)) {
    ret = -1000 + val * 8;
  }
  return ret;
}
static uint32_t get_wod_bus_3300_current(const uint8_t *obc_wod)
{
  int32_t ret = 0xFFFFFFFF;
  uint8_t val;
  if(obc_wod == NULL)
	{
    return ret;
  }

  val = obc_wod[sizeof(uint32_t) + 2];
  if(bus_3300mV_current_valid(val)) 
	{
    ret = val * 25;
  }
  return ret;
}
static uint32_t get_wod_bus_5000_current(const uint8_t *obc_wod)
{
  int32_t ret = 0xFFFFFFFF;
  uint8_t val;
  if(obc_wod == NULL)
	{
    return ret;
  }

  val = obc_wod[sizeof(uint32_t) + 3];
  if(bus_5000mV_current_valid(val)) 
	{
    ret = val * 25;
  }
  return ret;
}
static float get_wod_eps_temp(const uint8_t *obc_wod)
{
  float ret = nanf("NaN");
  uint8_t val;
  if(obc_wod == NULL)
	{
    return ret;
  }

  val = obc_wod[sizeof(uint32_t) + 5];
  if(eps_temp_valid(val)) 
	{
    ret = -15.0 + val * 0.25;
  }
  return ret;
}
static float get_wod_bat_temp(const uint8_t *obc_wod)
{
  float ret = nanf("NaN");
  uint8_t val;
  if(obc_wod == NULL)
	{
    return ret;
  }

  val = obc_wod[sizeof(uint32_t) + 6];
  if(eps_temp_valid(val)) 
	{
    ret = -15.0 + val * 0.25;
  }
  return ret;
}

//Init
void comms_rf_stats_init(comms_rf_stat_t *h)
{
  if(h == NULL)
	{
    return;
  }

  memset(h, 0, sizeof(comms_rf_stat_t));
  h->last_tick = getCurrentTick();
}

//Update
uint32_t comms_rf_stats_update(void)
{
	uint32_t now;
	static uint32_t stats_tick;
	comms_rf_stat_t comms_stats;
	if (now - stats_tick > COMMS_STATS_PERIOD_MS)
	{
		update_uptime(&comms_stats);
		update_temperature(&comms_stats);
		if (comms_stats.rx_failed_cnt < 10 && comms_stats.tx_failed_cnt < 5) 
		{
			UART2_BUF_O_Write_String_To_Buffer("HAL_IWDG_Refresh");// still need make reset mechanism //<-------------
			comms_rf_stats_init(&comms_stats);
		}
		
		large_data_IDLE ();
		pkt_pool_IDLE(now);
	}	
	return RETURN_NORMAL_STATE;
}

//Setters
void comms_rf_stats_set_reset_src(comms_rf_stat_t *h)
{
	uint8_t src;
	HSI_reset_source(&src);
  if (h == NULL) 
	{
    return;
  }
  h->rst_src = src;
}

void comms_rf_stats_frame_received(comms_rf_stat_t *h, uint8_t succesfull, int16_t error)
{
  if(h == NULL)
	{
    return;
  }

  if(succesfull)
	{
    h->rx_failed_cnt = 0;
  }
  else 
	{
    h->rx_failed_cnt++;
    h->last_rx_error_code = error;
  }
  h->rx_frames_cnt++;
}

void comms_rf_stats_frame_transmitted(comms_rf_stat_t *h, uint8_t succesfull, int16_t error)
{
  if(h == NULL)
	{
    return;
  }

  if(succesfull)
	{
    h->tx_failed_cnt = 0;
  }
  else 
	{
    h->tx_failed_cnt++;
    h->last_tx_error_code = error;
  }
  h->tx_frames_cnt++;
}
/**
 * The function increases the counter of the frames received,
 * containing however invalid destination information (callsign or SSID)
 * @param h pointer to a valid handle
 */
void comms_rf_stats_invalid_dest_frame(comms_rf_stat_t *h)
{
  if(h == NULL)
	{
    return;
  }
  h->invalid_dest_frames_cnt++;
}
/**
 * The function increases the counter of the frames received with wrong CRC.
 * @param h pointer to a valid handle
 */
void comms_rf_stats_invalid_crc_frame(comms_rf_stat_t *h)
{
  if(h == NULL)
	{
    return;
  }
  h->rx_crc_failed_cnt++;
}
/**
 * Updates the statistics that COMMS tracks based on the WOD that received
 * from the OBC
 * @param h h pointer to a valid handle
 * @param obc_wod the WOD at it was received from the OBC
 */
void comms_rf_stats_wod_received(comms_rf_stat_t *h, const uint8_t *obc_wod)
{
  if(h == NULL)
	{
    return;
  }
  h->last_utc = get_wod_utc_time(obc_wod);
  h->bat_temperature = get_wod_bat_temp(obc_wod);
  h->eps_temperature = get_wod_eps_temp(obc_wod);
  h->battery_mA = get_wod_bat_current(obc_wod);
  h->battery_mV = get_wod_bat_voltage(obc_wod);
  h->bus_3300_mA = get_wod_bus_3300_current(obc_wod);
  h->bus_5000_mA = get_wod_bus_5000_current(obc_wod);
}
/**
 * Updates the internal statistic counters about the COMMS ON/OFF switch
 * command.
 *
 * @param h h pointer to a valid handle
 * @param on 1 for switch ON, 0 for switch OFF
 */
void comms_rf_stats_sw_cmd_received(comms_rf_stat_t *h, uint8_t on)
{
  if(h == NULL)
	{
    return;
  }
  if(on)
	{
    h->sw_on_cmd_cnt++;
  }
  else
	{
    h->sw_off_cmd_cnt++;
  }
}
/**
 * This function increments the internal counter that tracks the invalid
 * ON/OFF switch commands
 * @param h h pointer to a valid handle
 */
void comms_rf_stats_sw_cmd_failed(comms_rf_stat_t *h)
{
  if(h == NULL)
	{
    return;
  }
  h->sw_cmd_failed_cnt++;
}

//Getters
float comms_rf_stats_get_temperature(comms_rf_stat_t *h)
{
  if(h == NULL)
	{
    return nanf("NaN");
  }
  return h->comms_temperature;
}



//////////////////////////////////////////////////////////////////////////////////






