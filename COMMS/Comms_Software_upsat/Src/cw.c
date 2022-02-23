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

#include "cw.h"
#include <stdlib.h>
#include <string.h>

static cw_char_t cw_lut[256];

/**
 * Lookup table to easily retrieve the duration of each pulse and pause
 */
static const uint32_t cw_duration_lut[6] =
  {   CW_DOT_DURATION_MS, CW_DASH_DURATION_MS,
      CW_CHAR_SPACE_MS, CW_WORD_SPACE_MS,
      CW_SYMBOL_SPACE_MS, 0 };


/**
 * Converts the input characters into CW Morse on-off symbols
 * @param out the output buffer with the CW on-off symbols
 * @param out_len the number of the resulting on-off symbols
 * @param in the input buffer
 * @param len the input buffer length
 * @return CW_OK if the encoding was successful or CW_ERROR if an illegal
 * character was encountered.
 */
int32_t
cw_encode(cw_pulse_t *out, size_t *out_len, const uint8_t *in, size_t len)
{
  size_t i;
  size_t j;
  size_t pulse_cnt = 0;
  cw_char_t c;
  for(i = 0; i < len; i++) {
    c = cw_lut[in[i]];
    if(!c.is_valid){
      return CW_ERROR;
    }
    for(j = 0; j < c.s_num; j++){
      if(c.s[j] == CW_DOT || c.s[j] == CW_DASH){
	out[pulse_cnt].cw_on = 1;
      }
      else{
	out[pulse_cnt].cw_on = 0;
      }
      /* Set the duration */
      out[pulse_cnt].duration_ms = cw_duration_lut[c.s[j]];
      pulse_cnt++;
    }
  }
  *out_len = pulse_cnt;
  return CW_OK;
}

/**
 * Get the CW character that corresponds to the uptime days
 * @param h a valid pointer to a \p comms_rf_stat_t struct.
 * @return CW character that corresponds to the uptime days. If the pointer
 * is invalid the no data character ('0') will be returned.
 */
char
cw_get_uptime_hours_char (comms_rf_stat_t *h)
{
  char ret = '0';
  if (h == NULL) {
    return '0';
  }

  if (h->uptime_h < 1) {
    ret = 'A';
  }
  else if (h->uptime_h < 2) {
    ret = 'B';
  }
  else if (h->uptime_h < 3) {
    ret = 'C';
  }
  else if (h->uptime_h < 4) {
    ret = 'D';
  }
  else if (h->uptime_h < 5) {
    ret = 'E';
  }
  else if (h->uptime_h < 6) {
    ret = 'F';
  }
  else if (h->uptime_h < 8) {
    ret = 'G';
  }
  else if (h->uptime_h < 10) {
    ret = 'H';
  }
  else if (h->uptime_h < 12) {
    ret = 'I';
  }
  else if (h->uptime_h < 16) {
    ret = 'J';
  }
  else if (h->uptime_h < 20) {
    ret = 'K';
  }
  else if (h->uptime_h < 24) {
    ret = 'L';
  }
  else if (h->uptime_h < 30) {
    ret = 'M';
  }
  else if (h->uptime_h < 36) {
    ret = 'N';
  }
  else if (h->uptime_h < 44) {
    ret = 'O';
  }
  else if (h->uptime_h < 52) {
    ret = 'P';
  }
  else if (h->uptime_h < 60) {
    ret = 'Q';
  }
  else if (h->uptime_h < 70) {
    ret = 'R';
  }
  else if (h->uptime_h < 80) {
    ret = 'S';
  }
  else if (h->uptime_h < 90) {
    ret = 'T';
  }
  else if (h->uptime_h < 100) {
    ret = 'U';
  }
  else if (h->uptime_h < 150) {
    ret = 'V';
  }
  else if (h->uptime_h < 200) {
    ret = 'W';
  }
  else if (h->uptime_h < 300) {
    ret = 'X';
  }
  else if (h->uptime_h < 400) {
    ret = 'Y';
  }
  else {
    ret = 'Z';
  }
  return ret;
}

/**
 * Get the CW character that corresponds to the uptime days
 * @param h a valid pointer to a \p comms_rf_stat_t struct.
 * @return CW character that corresponds to the uptime days. If the pointer
 * is invalid the no data character ('0') will be returned.
 */
char
cw_get_uptime_mins_char (comms_rf_stat_t *h)
{
  char ret = '0';

  if (h == NULL) {
    return '0';
  }

  if (h->uptime_m < 1) {
    ret =  'A';
  }
  else if (h->uptime_m < 2) {
    ret =  'B';
  }
  else if (h->uptime_m < 3) {
    ret =  'C';
  }
  else if (h->uptime_m < 4) {
    ret =  'D';
  }
  else if (h->uptime_m < 5) {
    ret =  'E';
  }
  else if (h->uptime_m < 6) {
    ret =  'F';
  }
  else if (h->uptime_m < 7) {
    ret =  'G';
  }
  else if (h->uptime_m < 8) {
    ret =  'H';
  }
  else if (h->uptime_m < 9) {
    ret =  'I';
  }
  else if (h->uptime_m < 10) {
    ret =  'J';
  }
  else if (h->uptime_m < 11) {
    ret =  'K';
  }
  else if (h->uptime_m < 12) {
    ret =  'L';
  }
  else if (h->uptime_m < 14) {
    ret =  'M';
  }
  else if (h->uptime_m < 16) {
    ret =  'N';
  }
  else if (h->uptime_m < 18) {
    ret =  'O';
  }
  else if (h->uptime_m < 20) {
    ret =  'P';
  }
  else if (h->uptime_m < 24) {
    ret =  'Q';
  }
  else if (h->uptime_m < 28) {
    ret =  'R';
  }
  else if (h->uptime_m < 32) {
    ret =  'S';
  }
  else if (h->uptime_m < 36) {
    ret =  'T';
  }
  else if (h->uptime_m < 40) {
    ret =  'U';
  }
  else if (h->uptime_m < 44) {
    ret =  'V';
  }
  else if (h->uptime_m < 48) {
    ret =  'W';
  }
  else if (h->uptime_m < 52) {
    ret =  'X';
  }
  else if (h->uptime_m < 56) {
    ret =  'Y';
  }
  else {
    ret =  'Z';
  }
  return ret;
}

char
cw_get_temp_char(comms_rf_stat_t *h)
{
  char ret = '0';

  if (h == NULL) {
    return '0';
  }

  float temp = h->comms_temperature;
  if (temp < -10.0) {
    ret =  'A';
  }
  else if (temp < -8.0) {
    ret =  'B';
  }
  else if (temp < -6.0) {
    ret =  'C';
  }
  else if (temp < -4.0) {
    ret =  'D';
  }
  else if (temp < -2.0) {
    ret =  'E';
  }
  else if (temp < 0.0) {
    ret =  'F';
  }
  else if (temp < 2.0) {
    ret =  'G';
  }
  else if (temp < 4.0) {
    ret =  'H';
  }
  else if (temp < 6.0) {
    ret =  'I';
  }
  else if (temp < 8.0) {
    ret =  'J';
  }
  else if (temp < 10.0) {
    ret =  'K';
  }
  else if (temp < 12.0) {
    ret =  'L';
  }
  else if (temp < 14.0) {
    ret =  'M';
  }
  else if (temp < 16.0) {
    ret =  'N';
  }
  else if (temp < 20.0) {
    ret =  'O';
  }
  else if (temp < 24.0) {
    ret =  'P';
  }
  else if (temp < 28.0) {
    ret =  'Q';
  }
  else if (temp < 32.0) {
    ret =  'R';
  }
  else if (temp < 36.0) {
    ret =  'S';
  }
  else if (temp < 40.0) {
    ret =  'T';
  }
  else if (temp < 42.0) {
    ret =  'U';
  }
  else if (temp < 44.0) {
    ret =  'V';
  }
  else if (temp < 46.0) {
    ret =  'W';
  }
  else if (temp < 48.0) {
    ret =  'X';
  }
  else if (temp < 50.0) {
    ret =  'Y';
  }
  else {
    ret =  'Z';
  }
  return ret;
}

char
cw_get_cont_errors_char(comms_rf_stat_t *h)
{
  size_t err_tx;
  size_t err_rx;
  char ret = '0';

  if (h == NULL) {
    return '0';
  }

  err_tx = h->tx_frames_cnt;
  err_rx = h->rx_frames_cnt;

  if(err_tx == 0 && err_rx == 0){
    ret = 'A';
  }
  else if(err_tx == 1 && err_rx == 0){
    ret = 'B';
  }
  else if(err_tx == 2 && err_rx == 0){
    ret = 'C';
  }
  else if(err_tx > 2 && err_rx == 0){
    ret = 'D';
  }
  if(err_tx == 0 && err_rx == 1){
    ret = 'E';
  }
  else if(err_tx == 1 && err_rx == 1){
    ret = 'F';
  }
  else if(err_tx == 2 && err_rx == 1){
    ret = 'G';
  }
  else if(err_tx > 2 && err_rx == 1){
    ret = 'H';
  }
  if(err_tx == 0 && err_rx == 2){
    ret = 'I';
  }
  else if(err_tx == 1 && err_rx == 2){
    ret = 'J';
  }
  else if(err_tx == 2 && err_rx == 2){
    ret = 'K';
  }
  else if(err_tx > 2 && err_rx == 2){
    ret = 'L';
  }
  if(err_tx == 0 && err_rx > 2){
    ret = 'M';
  }
  else if(err_tx == 1 && err_rx > 2){
    ret = 'N';
  }
  else if(err_tx == 2 && err_rx > 2){
    ret = 'O';
  }
  else if(err_tx > 2 && err_rx > 2){
    ret = 'P';
  }
  else{
    ret = 'Z';
  }
  return ret;
}

char
cw_get_bat_voltage_char(comms_rf_stat_t *h)
{
  char ret = '0';
  uint32_t voltage;

  if (h == NULL) {
    return '0';
  }

  voltage = h->battery_mV;
  if(voltage < 8000 ) {
    ret = '0';
  }
  else if (voltage < 8200) {
    ret =  'A';
  }
  else if (voltage < 8400) {
    ret =  'B';
  }
  else if (voltage < 8600) {
    ret =  'C';
  }
  else if (voltage < 8800) {
    ret =  'D';
  }
  else if (voltage < 9000) {
    ret =  'E';
  }
  else if (voltage < 9200) {
    ret =  'F';
  }
  else if (voltage < 9400) {
    ret =  'G';
  }
  else if (voltage < 9600) {
    ret =  'H';
  }
  else if (voltage < 9800) {
    ret =  'I';
  }
  else if (voltage < 10000) {
    ret =  'J';
  }
  else if (voltage < 10200) {
    ret =  'K';
  }
  else if (voltage < 10400) {
    ret =  'L';
  }
  else if (voltage < 10600) {
    ret =  'M';
  }
  else if (voltage < 10800) {
    ret =  'N';
  }
  else if (voltage < 11000) {
    ret =  'O';
  }
  else if (voltage < 11200) {
    ret =  'P';
  }
  else if (voltage < 11400) {
    ret =  'Q';
  }
  else if (voltage < 11600) {
    ret =  'R';
  }
  else if (voltage < 11800) {
    ret =  'S';
  }
  else if (voltage < 12000) {
    ret =  'T';
  }
  else if (voltage < 12200) {
    ret =  'U';
  }
  else if (voltage < 12400) {
    ret =  'V';
  }
  else if (voltage < 12600) {
    ret =  'W';
  }
  else if (voltage < 12800) {
    ret =  'X';
  }
  else if (voltage < 13000) {
    ret =  'Y';
  }
  else {
    ret =  'Z';
  }
  return ret;
}

char
cw_get_bat_current_char(comms_rf_stat_t *h)
{
  char ret = '0';
  int32_t current;

  if (h == NULL) {
    return '0';
  }

  current = h->battery_mA;
  if(current < -1000 ) {
    ret = '0';
  }
  else if (current < -920) {
    ret =  'A';
  }
  else if (current < -840) {
    ret =  'B';
  }
  else if (current < -760) {
    ret =  'C';
  }
  else if (current < -680) {
    ret =  'D';
  }
  else if (current < -600) {
    ret =  'E';
  }
  else if (current < -520) {
    ret =  'F';
  }
  else if (current < -440) {
    ret =  'G';
  }
  else if (current < -360) {
    ret =  'H';
  }
  else if (current < -280) {
    ret =  'I';
  }
  else if (current < -200) {
    ret =  'J';
  }
  else if (current < -120) {
    ret =  'K';
  }
  else if (current < -40) {
    ret =  'L';
  }
  else if (current < 40) {
    ret =  'M';
  }
  else if (current < 120) {
    ret =  'N';
  }
  else if (current < 200) {
    ret =  'O';
  }
  else if (current < 280) {
    ret =  'P';
  }
  else if (current < 360) {
    ret =  'Q';
  }
  else if (current < 440) {
    ret =  'R';
  }
  else if (current < 520) {
    ret =  'S';
  }
  else if (current < 600) {
    ret =  'T';
  }
  else if (current < 680) {
    ret =  'U';
  }
  else if (current < 760) {
    ret =  'V';
  }
  else if (current < 840) {
    ret =  'W';
  }
  else if (current < 920) {
    ret =  'X';
  }
  else if (current < 1000) {
    ret =  'Y';
  }
  else {
    ret =  'Z';
  }
  return ret;
}

char
cw_get_last_error_char(comms_rf_stat_t *h)
{
  int32_t err_code;
  char ret = '0';

  if (h == NULL) {
    return '0';
  }

  /* Give a priority at the TX error code. */
  if(h->last_tx_error_code){
    err_code = h->last_tx_error_code;
  }
  else{
    err_code = h->last_rx_error_code;
  }

  if(err_code == -9){
    ret = 'A';
  }
  else if(err_code == -8){
    ret = 'B';
  }
  else if(err_code == -7){
    ret = 'C';
  }
  else if(err_code == -6){
    ret = 'D';
  }
  else if(err_code == -5){
    ret = 'E';
  }
  else if(err_code == -4){
    ret = 'F';
  }
  else if(err_code == -3){
    ret = 'G';
  }
  else if(err_code == -2){
    ret = 'H';
  }
  else if(err_code == -1){
    ret = 'I';
  }
  else if(err_code == -56){
    ret = 'J';
  }
  else if(err_code == -55){
    ret = 'K';
  }
  else if(err_code == -54){
    ret = 'L';
  }
  else if(err_code == -53){
    ret = 'M';
  }
  else if(err_code == -52){
    ret = 'N';
  }
  else if(err_code == -51){
    ret = 'O';
  }
  else if(err_code == -61){
    ret = 'P';
  }
  else if(err_code == 0){
    ret = 'Z';
  }
  else{
    ret = 'Q';
  }
  return ret;
}

/**
 * Initializes the internal structures for CW encoding.
 *
 * The CW code is based at the ITU international Morse code
 */
void
cw_init ()
{
  uint8_t i;
  memset(cw_lut, 0, 256 * sizeof(cw_char_t));

  /*Perform the CW mapping based on the ITU international code */
  i = 0;
  cw_lut['A'].is_valid = 1;
  cw_lut['A'].s[i++] = CW_DOT;
  cw_lut['A'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['A'].s[i++] = CW_DASH;
  cw_lut['A'].s[i++] = CW_CHAR_DELIM;
  cw_lut['A'].s_num = i;

  i = 0;
  cw_lut['B'].is_valid = 1;
  cw_lut['B'].s[i++] = CW_DASH;
  cw_lut['B'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['B'].s[i++] = CW_DOT;
  cw_lut['B'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['B'].s[i++] = CW_DOT;
  cw_lut['B'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['B'].s[i++] = CW_DOT;
  cw_lut['B'].s[i++] = CW_CHAR_DELIM;
  cw_lut['B'].s_num = i;

  i = 0;
  cw_lut['C'].is_valid = 1;
  cw_lut['C'].s[i++] = CW_DASH;
  cw_lut['C'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['C'].s[i++] = CW_DOT;
  cw_lut['C'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['C'].s[i++] = CW_DASH;
  cw_lut['C'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['C'].s[i++] = CW_DOT;
  cw_lut['C'].s[i++] = CW_CHAR_DELIM;
  cw_lut['C'].s_num = i;

  i = 0;
  cw_lut['D'].is_valid = 1;
  cw_lut['D'].s[i++] = CW_DASH;
  cw_lut['D'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['D'].s[i++] = CW_DOT;
  cw_lut['D'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['D'].s[i++] = CW_DOT;
  cw_lut['D'].s[i++] = CW_CHAR_DELIM;
  cw_lut['D'].s_num = i;

  i = 0;
  cw_lut['E'].is_valid = 1;
  cw_lut['E'].s[i++] = CW_DOT;
  cw_lut['E'].s[i++] = CW_CHAR_DELIM;
  cw_lut['E'].s_num = i;

  i = 0;
  cw_lut['F'].is_valid = 1;
  cw_lut['F'].s[i++] = CW_DOT;
  cw_lut['F'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['F'].s[i++] = CW_DOT;
  cw_lut['F'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['F'].s[i++] = CW_DASH;
  cw_lut['F'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['F'].s[i++] = CW_DOT;
  cw_lut['F'].s[i++] = CW_CHAR_DELIM;
  cw_lut['F'].s_num = i;

  i = 0;
  cw_lut['G'].is_valid = 1;
  cw_lut['G'].s[i++] = CW_DASH;
  cw_lut['G'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['G'].s[i++] = CW_DASH;
  cw_lut['G'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['G'].s[i++] = CW_DOT;
  cw_lut['G'].s[i++] = CW_CHAR_DELIM;
  cw_lut['G'].s_num = i;

  i = 0;
  cw_lut['H'].is_valid = 1;
  cw_lut['H'].s[i++] = CW_DOT;
  cw_lut['H'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['H'].s[i++] = CW_DOT;
  cw_lut['H'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['H'].s[i++] = CW_DOT;
  cw_lut['H'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['H'].s[i++] = CW_DOT;
  cw_lut['H'].s[i++] = CW_CHAR_DELIM;
  cw_lut['H'].s_num = i;

  i = 0;
  cw_lut['I'].is_valid = 1;
  cw_lut['I'].s[i++] = CW_DOT;
  cw_lut['I'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['I'].s[i++] = CW_DOT;
  cw_lut['I'].s[i++] = CW_CHAR_DELIM;
  cw_lut['I'].s_num = i;

  i = 0;
  cw_lut['J'].is_valid = 1;
  cw_lut['J'].s[i++] = CW_DOT;
  cw_lut['J'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['J'].s[i++] = CW_DASH;
  cw_lut['J'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['J'].s[i++] = CW_DASH;
  cw_lut['J'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['J'].s[i++] = CW_DASH;
  cw_lut['J'].s[i++] = CW_CHAR_DELIM;
  cw_lut['J'].s_num = i;

  i = 0;
  cw_lut['K'].is_valid = 1;
  cw_lut['K'].s[i++] = CW_DASH;
  cw_lut['K'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['K'].s[i++] = CW_DOT;
  cw_lut['K'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['K'].s[i++] = CW_DASH;
  cw_lut['K'].s[i++] = CW_CHAR_DELIM;
  cw_lut['K'].s_num = i;

  i = 0;
  cw_lut['L'].is_valid = 1;
  cw_lut['L'].s[i++] = CW_DOT;
  cw_lut['L'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['L'].s[i++] = CW_DASH;
  cw_lut['L'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['L'].s[i++] = CW_DOT;
  cw_lut['L'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['L'].s[i++] = CW_DOT;
  cw_lut['L'].s[i++] = CW_CHAR_DELIM;
  cw_lut['L'].s_num = i;

  i = 0;
  cw_lut['M'].is_valid = 1;
  cw_lut['M'].s[i++] = CW_DASH;
  cw_lut['M'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['M'].s[i++] = CW_DASH;
  cw_lut['M'].s[i++] = CW_CHAR_DELIM;
  cw_lut['M'].s_num = i;

  i = 0;
  cw_lut['N'].is_valid = 1;
  cw_lut['N'].s[i++] = CW_DASH;
  cw_lut['N'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['N'].s[i++] = CW_DOT;
  cw_lut['N'].s[i++] = CW_CHAR_DELIM;
  cw_lut['N'].s_num = i;

  i = 0;
  cw_lut['O'].is_valid = 1;
  cw_lut['O'].s[i++] = CW_DASH;
  cw_lut['O'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['O'].s[i++] = CW_DASH;
  cw_lut['O'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['O'].s[i++] = CW_DASH;
  cw_lut['O'].s[i++] = CW_CHAR_DELIM;
  cw_lut['O'].s_num = i;

  i = 0;
  cw_lut['P'].is_valid = 1;
  cw_lut['P'].s[i++] = CW_DOT;
  cw_lut['P'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['P'].s[i++] = CW_DASH;
  cw_lut['P'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['P'].s[i++] = CW_DASH;
  cw_lut['P'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['P'].s[i++] = CW_DOT;
  cw_lut['P'].s[i++] = CW_CHAR_DELIM;
  cw_lut['P'].s_num = i;

  i = 0;
  cw_lut['Q'].is_valid = 1;
  cw_lut['Q'].s[i++] = CW_DASH;
  cw_lut['Q'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['Q'].s[i++] = CW_DASH;
  cw_lut['Q'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['Q'].s[i++] = CW_DOT;
  cw_lut['Q'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['Q'].s[i++] = CW_DASH;
  cw_lut['Q'].s[i++] = CW_CHAR_DELIM;
  cw_lut['Q'].s_num = i;

  i = 0;
  cw_lut['R'].is_valid = 1;
  cw_lut['R'].s[i++] = CW_DOT;
  cw_lut['R'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['R'].s[i++] = CW_DASH;
  cw_lut['R'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['R'].s[i++] = CW_DOT;
  cw_lut['R'].s[i++] = CW_CHAR_DELIM;
  cw_lut['R'].s_num = i;

  i = 0;
  cw_lut['S'].is_valid = 1;
  cw_lut['S'].s[i++] = CW_DOT;
  cw_lut['S'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['S'].s[i++] = CW_DOT;
  cw_lut['S'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['S'].s[i++] = CW_DOT;
  cw_lut['S'].s[i++] = CW_CHAR_DELIM;
  cw_lut['S'].s_num = i;

  i = 0;
  cw_lut['T'].is_valid = 1;
  cw_lut['T'].s[i++] = CW_DASH;
  cw_lut['T'].s[i++] = CW_CHAR_DELIM;
  cw_lut['T'].s_num = i;

  i = 0;
  cw_lut['U'].is_valid = 1;
  cw_lut['U'].s[i++] = CW_DOT;
  cw_lut['U'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['U'].s[i++] = CW_DOT;
  cw_lut['U'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['U'].s[i++] = CW_DASH;
  cw_lut['U'].s[i++] = CW_CHAR_DELIM;
  cw_lut['U'].s_num = i;

  i = 0;
  cw_lut['V'].is_valid = 1;
  cw_lut['V'].s[i++] = CW_DOT;
  cw_lut['V'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['V'].s[i++] = CW_DOT;
  cw_lut['V'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['V'].s[i++] = CW_DOT;
  cw_lut['V'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['V'].s[i++] = CW_DASH;
  cw_lut['V'].s[i++] = CW_CHAR_DELIM;
  cw_lut['V'].s_num = i;

  i = 0;
  cw_lut['W'].is_valid = 1;
  cw_lut['W'].s[i++] = CW_DOT;
  cw_lut['W'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['W'].s[i++] = CW_DASH;
  cw_lut['W'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['W'].s[i++] = CW_DASH;
  cw_lut['W'].s[i++] = CW_CHAR_DELIM;
  cw_lut['W'].s_num = i;

  i = 0;
  cw_lut['X'].is_valid = 1;
  cw_lut['X'].s[i++] = CW_DASH;
  cw_lut['X'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['X'].s[i++] = CW_DOT;
  cw_lut['X'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['X'].s[i++] = CW_DOT;
  cw_lut['X'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['X'].s[i++] = CW_DASH;
  cw_lut['X'].s[i++] = CW_CHAR_DELIM;
  cw_lut['X'].s_num = i;

  i = 0;
  cw_lut['Y'].is_valid = 1;
  cw_lut['Y'].s[i++] = CW_DASH;
  cw_lut['Y'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['Y'].s[i++] = CW_DOT;
  cw_lut['Y'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['Y'].s[i++] = CW_DASH;
  cw_lut['Y'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['Y'].s[i++] = CW_DASH;
  cw_lut['Y'].s[i++] = CW_CHAR_DELIM;
  cw_lut['Y'].s_num = i;

  i = 0;
  cw_lut['Z'].is_valid = 1;
  cw_lut['Z'].s[i++] = CW_DASH;
  cw_lut['Z'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['Z'].s[i++] = CW_DASH;
  cw_lut['Z'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['Z'].s[i++] = CW_DOT;
  cw_lut['Z'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['Z'].s[i++] = CW_DOT;
  cw_lut['Z'].s[i++] = CW_CHAR_DELIM;
  cw_lut['Z'].s_num = i;

  i = 0;
  cw_lut['1'].is_valid = 1;
  cw_lut['1'].s[i++] = CW_DOT;
  cw_lut['1'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['1'].s[i++] = CW_DASH;
  cw_lut['1'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['1'].s[i++] = CW_DASH;
  cw_lut['1'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['1'].s[i++] = CW_DASH;
  cw_lut['1'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['1'].s[i++] = CW_DASH;
  cw_lut['1'].s[i++] = CW_CHAR_DELIM;
  cw_lut['1'].s_num = i;

  i = 0;
  cw_lut['2'].is_valid = 1;
  cw_lut['2'].s[i++] = CW_DOT;
  cw_lut['2'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['2'].s[i++] = CW_DOT;
  cw_lut['2'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['2'].s[i++] = CW_DASH;
  cw_lut['2'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['2'].s[i++] = CW_DASH;
  cw_lut['2'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['2'].s[i++] = CW_DASH;
  cw_lut['2'].s[i++] = CW_CHAR_DELIM;
  cw_lut['2'].s_num = i;

  i = 0;
  cw_lut['3'].is_valid = 1;
  cw_lut['3'].s[i++] = CW_DOT;
  cw_lut['3'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['3'].s[i++] = CW_DOT;
  cw_lut['3'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['3'].s[i++] = CW_DOT;
  cw_lut['3'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['3'].s[i++] = CW_DASH;
  cw_lut['3'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['3'].s[i++] = CW_DASH;
  cw_lut['3'].s[i++] = CW_CHAR_DELIM;
  cw_lut['3'].s_num = i;

  i = 0;
  cw_lut['4'].is_valid = 1;
  cw_lut['4'].s[i++] = CW_DOT;
  cw_lut['4'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['4'].s[i++] = CW_DOT;
  cw_lut['4'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['4'].s[i++] = CW_DOT;
  cw_lut['4'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['4'].s[i++] = CW_DOT;
  cw_lut['4'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['4'].s[i++] = CW_DASH;
  cw_lut['4'].s[i++] = CW_CHAR_DELIM;
  cw_lut['4'].s_num = i;

  i = 0;
  cw_lut['5'].is_valid = 1;
  cw_lut['5'].s[i++] = CW_DOT;
  cw_lut['5'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['5'].s[i++] = CW_DOT;
  cw_lut['5'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['5'].s[i++] = CW_DOT;
  cw_lut['5'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['5'].s[i++] = CW_DOT;
  cw_lut['5'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['5'].s[i++] = CW_DOT;
  cw_lut['5'].s[i++] = CW_CHAR_DELIM;
  cw_lut['5'].s_num = i;

  i = 0;
  cw_lut['6'].is_valid = 1;
  cw_lut['6'].s[i++] = CW_DASH;
  cw_lut['6'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['6'].s[i++] = CW_DOT;
  cw_lut['6'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['6'].s[i++] = CW_DOT;
  cw_lut['6'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['6'].s[i++] = CW_DOT;
  cw_lut['6'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['6'].s[i++] = CW_DOT;
  cw_lut['6'].s[i++] = CW_CHAR_DELIM;
  cw_lut['6'].s_num = i;

  i = 0;
  cw_lut['7'].is_valid = 1;
  cw_lut['7'].s[i++] = CW_DASH;
  cw_lut['7'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['7'].s[i++] = CW_DASH;
  cw_lut['7'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['7'].s[i++] = CW_DOT;
  cw_lut['7'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['7'].s[i++] = CW_DOT;
  cw_lut['7'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['7'].s[i++] = CW_DOT;
  cw_lut['7'].s[i++] = CW_CHAR_DELIM;
  cw_lut['7'].s_num = i;

  i = 0;
  cw_lut['8'].is_valid = 1;
  cw_lut['8'].s[i++] = CW_DASH;
  cw_lut['8'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['8'].s[i++] = CW_DASH;
  cw_lut['8'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['8'].s[i++] = CW_DASH;
  cw_lut['8'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['8'].s[i++] = CW_DOT;
  cw_lut['8'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['8'].s[i++] = CW_DOT;
  cw_lut['8'].s[i++] = CW_CHAR_DELIM;
  cw_lut['8'].s_num = i;

  i = 0;
  cw_lut['9'].is_valid = 1;
  cw_lut['9'].s[i++] = CW_DASH;
  cw_lut['9'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['9'].s[i++] = CW_DASH;
  cw_lut['9'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['9'].s[i++] = CW_DASH;
  cw_lut['9'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['9'].s[i++] = CW_DASH;
  cw_lut['9'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['9'].s[i++] = CW_DOT;
  cw_lut['9'].s[i++] = CW_CHAR_DELIM;
  cw_lut['9'].s_num = i;

  i = 0;
  cw_lut['0'].is_valid = 1;
  cw_lut['0'].s[i++] = CW_DASH;
  cw_lut['0'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['0'].s[i++] = CW_DASH;
  cw_lut['0'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['0'].s[i++] = CW_DASH;
  cw_lut['0'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['0'].s[i++] = CW_DASH;
  cw_lut['0'].s[i++] = CW_SYMBOL_DELIM;
  cw_lut['0'].s[i++] = CW_DASH;
  cw_lut['0'].s[i++] = CW_CHAR_DELIM;
  cw_lut['0'].s_num = i;

  cw_lut[' '].is_valid = 1;
  cw_lut[' '].s[0] = CW_WORD_DELIM;
  cw_lut[' '].s_num = 1;
}

