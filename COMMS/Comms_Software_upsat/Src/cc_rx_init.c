/*
 * upsat-comms-software: Communication Subsystem Software for UPSat satellite
 *
 *  Copyright (C) 2016, Libre Space Foundation <http://librespacefoundation.org/>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Rf settings for CC1120
 */
#include <cc_tx_init.h>
#include "cc1120_config.h"


// Whitening = false
// Packet length = 255
// Bit rate = 9.6
// Performance mode = High Performance
// Modulation format = 2-FSK
// RX filter BW = 25.000000
// Deviation = 3.997803
// Packet length mode = Variable
// TX power = 2
// Packet bit length = 0
// Device address = 0
// PA ramping = false
// Carrier frequency = 145.835002
// Symbol rate = 9.6
// Manchester enable = false
// Address config = No address check

static const registerSetting_t RX_preferredSettings[]=
{
  {IOCFG3,            0xB0}, /* This GPIO Pin DOES NOT WORK ON OUR BOARD!!!!! */
  {IOCFG2,            0x06}, /* PKT_SYNC_RXTX, on rising-falling edge*/
  {IOCFG0,            0x00}, /* RXFIFO_THR, on rising edge */
  {SYNC1,             0x7A},
  {SYNC0,             0x0E},
  {SYNC_CFG1,         0x0B},
  {SYNC_CFG0,         0x0B},
  {DCFILT_CFG,        0x1C},
  {PREAMBLE_CFG1,     0x2A},
  {IQIC,              0xC6},
  {CHAN_BW,           0x08},
  {MDMCFG0,           0x05},
  {SYMBOL_RATE2,      0x43},
  {SYMBOL_RATE1,      0xA9},
  {SYMBOL_RATE0,      0x2A},
  {AGC_REF,           0x20},
  {AGC_CS_THR,        0x19},
  {AGC_CFG1,          0xA9},
  {AGC_CFG0,          0xCF},
  {FIFO_CFG,          CC1120_RXFIFO_THR},
  {FS_CFG,            0x1B},
  {PKT_CFG1,          0x00},
  {PKT_CFG0,          0x20},
  {PA_CFG2,           0x5D},
  {PA_CFG0,           0x7D},
  {PKT_LEN,           0xFF},
  {IF_MIX_CFG,        0x00},
  {FREQOFF_CFG,       0x22},
  {FREQ2,             0x6D},
  {FREQ1,             0x60},
  {FREQ0,             0x52},
  {FS_DIG1,           0x00},
  {FS_DIG0,           0x5F},
  {FS_CAL1,           0x40},
  {FS_CAL0,           0x0E},
  {FS_DIVTWO,         0x03},
  {FS_DSM0,           0x33},
  {FS_DVC0,           0x17},
  {FS_PFD,            0x50},
  {FS_PRE,            0x6E},
  {FS_REG_DIV_CML,    0x14},
  {FS_SPARE,          0xAC},
  {FS_VCO0,           0xB4},
  {XOSC5,             0x0E},
  {XOSC1,             0x03},
  {PARTNUMBER,        0x48},
  {PARTVERSION,       0x21},
  {MODEM_STATUS1,     0x10},
};

static const registerSetting_t rx_temp_sensor_setup[] =
{
    {DCFILT_CFG,        0x40}, //Tempsens settings, bit 6 high
    {CHAN_BW,           0x88},
    {FREQ_IF_CFG,       0x00},
    {MDMCFG1,           0x47}, //Tempsens settings, single ADC, I channel
    {ATEST,             0x2A}, //Tempsens settings
    {ATEST_MODE,        0x07}, //Tempsens settings
    {GBIAS1,            0x07}, //Tempsens settings
    {PA_IFAMP_TEST,     0x01}, //Tempsens settings
};




/**
 * Configures the RX CC1120 registers for normal operation
 */
void
rx_register_config ()
{
  uint8_t writeByte;
  uint32_t i;
  // Reset radio
  cc_rx_cmd (SRES);

  // Write registers to radio
  for (i = 0; i < (sizeof(RX_preferredSettings) / sizeof(registerSetting_t));
      i++) {
    writeByte = RX_preferredSettings[i].dat;
    cc_rx_wr_reg (RX_preferredSettings[i].addr, writeByte);
  }
}

/**
 * Configures the RX CC1120 registers for digital temperature readings
 */
void
rx_temp_sensor_register_config ()
{
  uint8_t writeByte;
  uint32_t i;
  // Reset radio
  cc_rx_cmd (SRES);

  // Write registers to radio
  for (i = 0; i < (sizeof(rx_temp_sensor_setup) / sizeof(registerSetting_t));
      i++) {
    writeByte = rx_temp_sensor_setup[i].dat;
    cc_rx_wr_reg (rx_temp_sensor_setup[i].addr, writeByte);
  }
}

/**
 * Performs the manual calibration of the RX CC1120 chip
 */
void
rx_manual_calibration ()
{

  uint8_t original_fs_cal2;
  uint8_t calResults_for_vcdac_start_high[3];
  uint8_t calResults_for_vcdac_start_mid[3];
  uint8_t marcstate;
  uint8_t writeByte;

  // 1) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
  writeByte = 0x00;
  cc_rx_wr_reg (FS_VCO2, writeByte);

  // 2) Start with high VCDAC (original VCDAC_START + 2):
  cc_rx_rd_reg (FS_CAL2, &original_fs_cal2);
  writeByte = original_fs_cal2 + VCDAC_START_OFFSET;
  cc_rx_wr_reg (FS_CAL2, writeByte);

  // 3) Calibrate and wait for calibration to be done
  //   (radio back in IDLE state)
  cc_rx_cmd (SCAL);

  do {
    cc_rx_rd_reg (MARCSTATE, &marcstate);
    marcstate=0x41; // for debugging
  }
  while (marcstate != 0x41);

  // 4) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with
  //    high VCDAC_START value
  cc_rx_rd_reg (FS_VCO2, &calResults_for_vcdac_start_high[FS_VCO2_INDEX]);
  cc_rx_rd_reg (FS_VCO4, &calResults_for_vcdac_start_high[FS_VCO4_INDEX]);
  cc_rx_rd_reg (FS_CHP, &calResults_for_vcdac_start_high[FS_CHP_INDEX]);

  // 5) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
  writeByte = 0x00;
  cc_rx_wr_reg (FS_VCO2, writeByte);

  // 6) Continue with mid VCDAC (original VCDAC_START):
  writeByte = original_fs_cal2;
  cc_rx_wr_reg (FS_CAL2, writeByte);

  // 7) Calibrate and wait for calibration to be done
  //   (radio back in IDLE state)
  cc_rx_cmd (SCAL);

  do {
    cc_rx_rd_reg (MARCSTATE, &marcstate);
    marcstate=0x41; // for debugging
  }
  while (marcstate != 0x41);

  // 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained
  //    with mid VCDAC_START value
  cc_rx_rd_reg (FS_VCO2, &calResults_for_vcdac_start_mid[FS_VCO2_INDEX]);
  cc_rx_rd_reg (FS_VCO4, &calResults_for_vcdac_start_mid[FS_VCO4_INDEX]);
  cc_rx_rd_reg (FS_CHP, &calResults_for_vcdac_start_mid[FS_CHP_INDEX]);

  // 9) Write back highest FS_VCO2 and corresponding FS_VCO
  //    and FS_CHP result
  if (calResults_for_vcdac_start_high[FS_VCO2_INDEX]
      > calResults_for_vcdac_start_mid[FS_VCO2_INDEX]) {
    writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
    cc_rx_wr_reg (FS_VCO2, writeByte);
    writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
    cc_rx_wr_reg (FS_VCO4, writeByte);
    writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
    cc_rx_wr_reg (FS_CHP, writeByte);
  }
  else {
    writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
    cc_rx_wr_reg (FS_VCO2, writeByte);
    writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
    cc_rx_wr_reg (FS_VCO4, writeByte);
    writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
    cc_rx_wr_reg (FS_CHP, writeByte);
  }
}


