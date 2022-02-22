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

#include "lfsr.h"
#include "utils.h"

/**
 * Creates and initializes the LFSR
 * @param h the LFSR handle. Should point to a valid memory region
 * @param mask the polynomial of the LFSR
 * @param seed the initial seed
 * @param order the order of the LFSR
 * @return 0 on success or a negative number in case of error
 */
int32_t
lfsr_init(lfsr_handle_t *h, uint32_t mask, uint32_t seed, uint32_t order)
{
  if(!h || order > 31){
    return -1;
  }
  h->mask = mask;
  h->seed = seed;
  h->shift_reg = seed;
  h->order = order;
  return 0;
}

/**
 * Get the next output bit from the LFSR
 * @param h the LFSR handle. Should be a VALID LFSR handle and it is
 * responsibility of the caller to ensure this. Otherwise the result is
 * undefined.
 * @return  the output bit of the LFSR
 */
uint8_t
lfsr_next_bit(lfsr_handle_t *h)
{
  uint8_t bit;
  uint8_t out_bit;
  out_bit = h->shift_reg & 0x1;
  bit = bit_count(h->shift_reg & h->mask) % 2;
  h->shift_reg = (h->shift_reg >> 1) | (bit << h->order);
  return out_bit;
}

/**
 * Feeds the LFSR with the input bit and return the scrambled output
 * @param h the LFSR handle. Should be a VALID LFSR handle and it is
 * responsibility of the caller to ensure this. Otherwise the result is
 * undefined.
 * @param bit the input bit. Should be placed at LS bit.
 * @return the scrambled bit, placed at the LS bit of the output byte.
 */
uint8_t
lfsr_next_bit_scramble(lfsr_handle_t *h, uint8_t bit)
{
  uint8_t out_bit;
  uint8_t new_bit;
  out_bit = h->shift_reg & 0x1;
  new_bit = (bit_count(h->shift_reg & h->mask) % 2) ^ (bit & 0x1);
  h->shift_reg = (h->shift_reg >> 1) | (new_bit << h->order);
  return out_bit;
}

/**
 * Feeds the LFSR with the input bit and return the descrambled output
 * @param h the LFSR handle. Should be a VALID LFSR handle and it is
 * responsibility of the caller to ensure this. Otherwise the result is
 * undefined.
 * @param bit the input bit. Should be placed at LS bit.
 * @return the descrambled bit, placed at the LS bit of the output byte.
 */
uint8_t
lfsr_next_bit_descramble(lfsr_handle_t *h, uint8_t bit)
{
  uint8_t out_bit;
  out_bit = (bit_count(h->shift_reg & h->mask) % 2) ^ (bit & 0x1);
  h->shift_reg = (h->shift_reg >> 1) | ((bit & 0x1) << h->order);
  return out_bit;
}

/**
 * Resets the LFSR at the initial state
 * @param h the LFSR handle.
 * @return 0 on success or a negative number in case of error
 */
int32_t
lfsr_reset(lfsr_handle_t *h)
{
  if(!h){
    return -1;
  }

  h->shift_reg = h->seed;
  return 0;
}
