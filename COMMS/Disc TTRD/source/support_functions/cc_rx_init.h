#ifndef INC_CC_RX_INIT_H_
#define INC_CC_RX_INIT_H_

// #include "cc_definitions.h"
// #include "cc112x_spi.h"

// Was used in comms_manager.c, sensors.c, and (ONLY INCLUDED IN) main.c

#include "../main/main.h"


// Setters 
uint8_t cc_rx_wr_reg (uint16_t addr, uint8_t data);

uint8_t cc_rx_cmd (uint8_t CMDStrobe);

//HAL_StatusTypeDef
//cc_rx_spi_write_fifo(uint8_t *data, uint8_t *spi_rx_data, size_t len);
void rx_manual_calibration ();
void rx_register_config ();
void rx_temp_sensor_register_config ();

//Getters
uint8_t cc_rx_rd_reg (uint16_t addr, uint8_t *data);

int32_t cc_rx_data_packet (uint8_t *out, size_t len, size_t timeout_ms);

//HAL_StatusTypeDef
//cc_rx_spi_read_fifo(uint8_t *out, size_t len);

//TF
uint8_t cc_rx_check_fifo_status(); // Checks if the RX FIFO of the CC1120 encountered any error. If yes, the FIFO is flushed and the RX CC1120 is set again in normal RX mode.


#endif /* INC_CC_RX_INIT_H_ */
