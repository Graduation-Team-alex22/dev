#ifndef __CC_TX_INIT_H
#define __CC_TX_INIT_H
#include "../main/main.h"
#include "cw.h"


// Was used in (ONLY INCLUDED IN) cc_rx_init.c, cc_tx_init.c, cc112x_spi.c, comms_manager.c, and (ONLY INCLUDED IN) main.c

#ifdef CC1101_UART

#elif CC1120

#define CC_EXT_ADD 0x2F00

//Setters 
uint8_t cc_tx_wr_reg (uint16_t addr, uint8_t data);
int32_t cc_tx_data_continuous (const uint8_t *data, size_t size, uint8_t *rec_data,
		       size_t timeout_ms);
int32_t cc_tx_cw(const cw_pulse_t *in, size_t len);

uint8_t cc_tx_cmd (uint8_t CMDStrobe);
void tx_manualCalibration ();
void tx_registerConfig ();
void tx_cw_registerConfig ();

//HAL_StatusTypeDef
//cc_tx_spi_write_fifo(const uint8_t *data, uint8_t *spi_rx_data, size_t len);

//Getters
uint8_t cc_tx_rd_reg (uint16_t addr, uint8_t *data);


int32_t rx_data (uint8_t *out, size_t len, size_t timeout_ms);
#elif CC1101_SPI


#endif

#endif
