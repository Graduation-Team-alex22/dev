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

static const registerSetting_t TX_preferredSettings[] =
  {
    { IOCFG3, 0x06 },
    { IOCFG2, 0x02 },
    { IOCFG1, 0x40 },
    { IOCFG0, 0x40 },
    { SYNC3, 0x00 },
    { SYNC2, 0x00 },
    { SYNC1, 0x7A },
    { SYNC0, 0x0E },
    { SYNC_CFG1, 0x0B },
    { SYNC_CFG0, 0x03 }, /* No SYNC word. It is generated on the MPU and handled manually */
    { DCFILT_CFG, 0x1C },
    { PREAMBLE_CFG1, 0x00 }, /* No preamble. It is generated on the MPU and handled manually */
    { PREAMBLE_CFG0, 0x0A }, /* No preamble. It is generated on the MPU and handled manually */
    { IQIC, 0xC6 },
    { CHAN_BW, 0x08 },
    { MDMCFG0, 0x05 },
    { SYMBOL_RATE2, 0x73 },
    { AGC_REF, 0x20 },
    { AGC_CS_THR, 0x19 },
    { AGC_CFG1, 0xA9 },
    { FIFO_CFG, CC1120_TXFIFO_THR },
    { SETTLING_CFG, 0x03 },
    { FS_CFG, 0x14 },
    { PKT_CFG1, 0x00 },
    { PKT_CFG0, CC1120_FIXED_PKT_LEN },
    { PA_CFG2, 0x2F },  //4-PACFG2 0X26, 6dBm 0x2B 2dbm 0x22,8 DBM 2F
	{ PA_CFG0, 0x7D },
	{ PKT_LEN, 0xFF },
	{ IF_MIX_CFG, 0x00 },
	{ FREQOFF_CFG, 0x22 },
	{ FREQ2, 0x6C },
	{ FREQ1, 0xF1 },
	{ FREQ0, 0x2F },
	{ FS_DIG1, 0x00 },
	{ FS_DIG0, 0x5F },
	{ FS_CAL1, 0x40 },
	{ FS_CAL0, 0x0E },
	{ FS_DIVTWO, 0x03 },
	{ FS_DSM0, 0x33 },
	{ FS_DVC0, 0x17 },
	{ FS_PFD, 0x50 },
	{ FS_PRE, 0x6E },
	{ FS_REG_DIV_CML, 0x14 },
	{ FS_SPARE, 0xAC },
	{ FS_VCO4, 0x13 },
	{ FS_VCO1, 0xAC },
	{ FS_VCO0, 0xB4 },
	{ XOSC5, 0x0E },
	{ XOSC1, 0x03 },
	{ DCFILTOFFSET_I1, 0xF8 },
	{ DCFILTOFFSET_I0, 0x39 },
	{ DCFILTOFFSET_Q1, 0x0E },
	{ DCFILTOFFSET_Q0, 0x9B },
	{ CFM_DATA_CFG, 0x00 },
	{ IQIE_I1, 0xEF },
	{ IQIE_I0, 0xDE },
	{ IQIE_Q1, 0x02 },
	{ IQIE_Q0, 0x2F },
	{ AGC_GAIN1, 0x13 },
	{ SERIAL_STATUS, 0x10 },
	{ MODCFG_DEV_E, 0x0B } };

const registerSetting_t CW_preferredSettings[] =
  {
    { IOCFG3, 0xB0 },
    { IOCFG2, 0x08 },
    { IOCFG1, 0xB0 },
    { IOCFG0, 0x09 },
    { SYNC_CFG1, 0x0B },
    { DEVIATION_M, 0x26 },
    { MODCFG_DEV_E, 0x05 },
    { DCFILT_CFG, 0x13 },
    { PREAMBLE_CFG1, 0x00 },
    { PREAMBLE_CFG0, 0x33 },
    { IQIC, 0x00 },
    { CHAN_BW, 0x03 },
    { MDMCFG0, 0x04 },
    { AGC_REF, 0x30 },
    { AGC_CS_THR, 0xEC },
    { AGC_CFG3, 0xD1 },
    { AGC_CFG2, 0x3F },
    { AGC_CFG1, 0x32 },
    { AGC_CFG0, 0x9F },
    { FIFO_CFG, 0x00 },
    { FS_CFG, 0x14 },
    { PKT_CFG2, 0x06 },
    { PKT_CFG1, 0x00 },
    { PKT_CFG0, 0x40 },
    { PA_CFG2, 0x66 },
    { PA_CFG0, 0x56 },
    { IF_MIX_CFG, 0x00 },
    { FREQOFF_CFG, 0x00 },
    { TOC_CFG, 0x0A },
    { CFM_DATA_CFG, 0x01 },
    { FREQ2, 0x6C },
    { FREQ1, 0xF1 },
    { FREQ0, 0x2F },
    { FS_DIG1, 0x00 },
    { FS_DIG0, 0x5F },
    { FS_CAL1, 0x40 },
    { FS_CAL0, 0x0E },
    { FS_DIVTWO, 0x03 },
    { FS_DSM0, 0x33 },
    { FS_DVC0, 0x17 },
    { FS_PFD, 0x50 },
    { FS_PRE, 0x6E },
    { FS_REG_DIV_CML, 0x14 },
    { FS_SPARE, 0xAC },
    { FS_VCO0, 0xB4 },
    { XOSC5, 0x0E },
    { XOSC1, 0x03 },
};

/**
 * Performs the setup of the TX CC1120 registers
 */
void
tx_registerConfig ()
{
  unsigned char writeByte;
  unsigned i;
  // Reset radio
  cc_tx_cmd (SRES);

  // Write registers to radio
  for (i = 0; i < (sizeof(TX_preferredSettings) / sizeof(registerSetting_t));
      i++) {
    writeByte = TX_preferredSettings[i].dat;
    cc_tx_wr_reg (TX_preferredSettings[i].addr, writeByte);
  }
}
/**
 * Performs the setup of the TX CC1120 registers suitable for CW transmission
 */
void
tx_cw_registerConfig ()
{
  unsigned char writeByte;
  unsigned i;
  // Reset radio
  cc_tx_cmd (SRES);

  // Write registers to radio
  for (i = 0; i < (sizeof(CW_preferredSettings) / sizeof(registerSetting_t));
      i++) {
    writeByte = CW_preferredSettings[i].dat;
    cc_tx_wr_reg (CW_preferredSettings[i].addr, writeByte);
  }
}

void
tx_manualCalibration ()
{

  uint8_t original_fs_cal2;
  uint8_t calResults_for_vcdac_start_high[3];
  uint8_t calResults_for_vcdac_start_mid[3];
  uint8_t marcstate;
  uint8_t writeByte;

  // 1) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
  writeByte = 0x00;
  cc_tx_wr_reg (FS_VCO2, writeByte);

  // 2) Start with high VCDAC (original VCDAC_START + 2):
  cc_tx_rd_reg (FS_CAL2, &original_fs_cal2);
  writeByte = original_fs_cal2 + VCDAC_START_OFFSET; // why?
  cc_tx_wr_reg (FS_CAL2, writeByte);

  // 3) Calibrate and wait for calibration to be done
  //   (radio back in IDLE state)
  cc_tx_cmd (SCAL);

  do {
    cc_tx_rd_reg (MARCSTATE, &marcstate);
    marcstate=0x41; // for debugging
  }
  while (marcstate != 0x41);

  // 4) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with
  //    high VCDAC_START value
  cc_tx_rd_reg (FS_VCO2, &calResults_for_vcdac_start_high[FS_VCO2_INDEX]);
  cc_tx_rd_reg (FS_VCO4, &calResults_for_vcdac_start_high[FS_VCO4_INDEX]);
  cc_tx_rd_reg (FS_CHP, &calResults_for_vcdac_start_high[FS_CHP_INDEX]);

  // 5) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
  writeByte = 0x00;
  cc_tx_wr_reg (FS_VCO2, writeByte);

  // 6) Continue with mid VCDAC (original VCDAC_START):
  writeByte = original_fs_cal2;
  cc_tx_wr_reg (FS_CAL2, writeByte);

  // 7) Calibrate and wait for calibration to be done
  //   (radio back in IDLE state)
  cc_tx_cmd (SCAL);

  do {
    cc_tx_rd_reg (MARCSTATE, &marcstate);
    marcstate=0x41; // for debugging
  }
  while (marcstate != 0x41);

  // 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained
  //    with mid VCDAC_START value
  cc_tx_rd_reg (FS_VCO2, &calResults_for_vcdac_start_mid[FS_VCO2_INDEX]);
  cc_tx_rd_reg (FS_VCO4, &calResults_for_vcdac_start_mid[FS_VCO4_INDEX]);
  cc_tx_rd_reg (FS_CHP, &calResults_for_vcdac_start_mid[FS_CHP_INDEX]);

  // 9) Write back highest FS_VCO2 and corresponding FS_VCO4 and FS_CHP result
  if (calResults_for_vcdac_start_high[FS_VCO2_INDEX]
      > calResults_for_vcdac_start_mid[FS_VCO2_INDEX]) {
    writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
    cc_tx_wr_reg (FS_VCO2, writeByte);
    writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
    cc_tx_wr_reg (FS_VCO4, writeByte);
    writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
    cc_tx_wr_reg (FS_CHP, writeByte);
  }
  else {
    writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
    cc_tx_wr_reg (FS_VCO2, writeByte);
    writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
    cc_tx_wr_reg (FS_VCO4, writeByte);
    writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
    cc_tx_wr_reg (FS_CHP, writeByte);
  }
}



