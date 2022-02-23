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

#include "scrambler.h"
#include "log.h"
#include "stm32f4xx_hal.h"

int32_t
scrambler_init (scrambler_handle_t *h, uint32_t pol, uint32_t seed,
		uint32_t order)
{
  if(!h){
    return -1;
  }
  h->prev_bit = 0;
  return lfsr_init(&h->lfsr, pol, seed, order);
}

int32_t
scrambler_reset (scrambler_handle_t *h)
{
  if(!h){
    return -1;
  }
  h->prev_bit = 0;
  return lfsr_reset(&h->lfsr);
}

int32_t
descrambler_init (scrambler_handle_t *h, uint32_t pol, uint32_t seed,
		uint32_t order)
{
  return scrambler_init(h, pol, seed, order);
}

int32_t
descrambler_reset (scrambler_handle_t *h)
{
  return scrambler_reset(h);
}

int32_t
scramble_data (scrambler_handle_t *h, uint8_t *out, const uint8_t *in,
	       size_t len)
{
  size_t i;
  if(!h){
    return -1;
  }

  for(i = 0; i < len; i++){
    out[i] = lfsr_next_bit_scramble(&h->lfsr, in[i] & 0x1);
    out[i] |= (lfsr_next_bit_scramble(&h->lfsr, (in[i] >> 1) & 0x1) << 1);
    out[i] |= (lfsr_next_bit_scramble(&h->lfsr, (in[i] >> 2) & 0x1) << 2);
    out[i] |= (lfsr_next_bit_scramble(&h->lfsr, (in[i] >> 3) & 0x1) << 3);
    out[i] |= (lfsr_next_bit_scramble(&h->lfsr, (in[i] >> 4) & 0x1) << 4);
    out[i] |= (lfsr_next_bit_scramble(&h->lfsr, (in[i] >> 5) & 0x1) << 5);
    out[i] |= (lfsr_next_bit_scramble(&h->lfsr, (in[i] >> 6) & 0x1) << 6);
    out[i] |= (lfsr_next_bit_scramble(&h->lfsr, (in[i] >> 7) & 0x1) << 7);
  }
  return 0;
}

/**
 * Performs NRZI encoding and scrambling at the same time
 * @param h the scrambler handler
 * @param out the output buffer
 * @param in the input buffer
 * @param len the size of the input buffer
 * @return 0 on success or a negative number in case of error
 */
int32_t
scramble_data_nrzi (scrambler_handle_t *h, uint8_t *out, const uint8_t *in,
		    size_t len)
{
  size_t i;
  uint8_t new_bit;
  uint8_t inv;

  if(!h){
    return -1;
  }

  for(i = 0; i < len; i++){
    /* NRZI */
    inv = ~in[i];

    new_bit = ( (inv & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] = lfsr_next_bit_scramble(&h->lfsr, new_bit);

    new_bit = (( (inv >> 1)  & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] |= lfsr_next_bit_scramble(&h->lfsr, new_bit) << 1;

    new_bit = (( (inv >> 2)  & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] |= lfsr_next_bit_scramble(&h->lfsr, new_bit) << 2;

    new_bit = (( (inv >> 3)  & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] |= lfsr_next_bit_scramble(&h->lfsr, new_bit) << 3;

    new_bit = (( (inv >> 4)  & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] |= lfsr_next_bit_scramble(&h->lfsr, new_bit) << 4;

    new_bit = (( (inv >> 5)  & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] |= lfsr_next_bit_scramble(&h->lfsr, new_bit) << 5;

    new_bit = (( (inv >> 6)  & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] |= lfsr_next_bit_scramble(&h->lfsr, new_bit) << 6;

    new_bit = (( (inv >> 7)  & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] |= lfsr_next_bit_scramble(&h->lfsr, new_bit) << 7;
  }
  return 0;
}

int32_t
descramble_data (scrambler_handle_t *h, uint8_t *out, const uint8_t *in,
		 size_t len)
{
  size_t i;
  if(!h){
    return -1;
  }

  for(i = 0; i < len; i++){
    out[i] = lfsr_next_bit_descramble(&h->lfsr, in[i] & 0x1);
    out[i] |= lfsr_next_bit_descramble(&h->lfsr, (in[i] >> 1) & 0x1) << 1;
    out[i] |= lfsr_next_bit_descramble(&h->lfsr, (in[i] >> 2) & 0x1) << 2;
    out[i] |= lfsr_next_bit_descramble(&h->lfsr, (in[i] >> 3) & 0x1) << 3;
    out[i] |= lfsr_next_bit_descramble(&h->lfsr, (in[i] >> 4) & 0x1) << 4;
    out[i] |= lfsr_next_bit_descramble(&h->lfsr, (in[i] >> 5) & 0x1) << 5;
    out[i] |= lfsr_next_bit_descramble(&h->lfsr, (in[i] >> 6) & 0x1) << 6;
    out[i] |= lfsr_next_bit_descramble(&h->lfsr, (in[i] >> 7) & 0x1) << 7;
  }
  return 0;
}

/**
 * Descramble and perform NRZI decoding at once
 * @param h
 * @param out
 * @param in
 * @param len
 * @return
 */
int32_t
descramble_data_nrzi (scrambler_handle_t *h, uint8_t *out, const uint8_t *in,
		      size_t len)
{
  size_t i;
  uint8_t new_bit;
  uint8_t curr_bit;

  if(!h){
    return -1;
  }

  for(i = 0; i < len; i++){
    curr_bit = lfsr_next_bit_descramble(&h->lfsr, in[i] & 0x1);
    new_bit = (~( (curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] = new_bit;

    curr_bit = lfsr_next_bit_descramble (&h->lfsr, (in[i] >> 1) & 0x1);
    new_bit= (~((curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] |= new_bit << 1;

    curr_bit = lfsr_next_bit_descramble (&h->lfsr, (in[i] >> 2) & 0x1);
    new_bit= (~((curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] |= new_bit << 2;

    curr_bit = lfsr_next_bit_descramble (&h->lfsr, (in[i] >> 3) & 0x1);
    new_bit= (~((curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] |= new_bit << 3;

    curr_bit = lfsr_next_bit_descramble (&h->lfsr, (in[i] >> 4) & 0x1);
    new_bit= (~((curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] |= new_bit << 4;

    curr_bit = lfsr_next_bit_descramble (&h->lfsr, (in[i] >> 5) & 0x1);
    new_bit= (~((curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] |= new_bit << 5;

    curr_bit = lfsr_next_bit_descramble (&h->lfsr, (in[i] >> 6) & 0x1);
    new_bit= (~((curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] |= new_bit << 6;

    curr_bit = lfsr_next_bit_descramble (&h->lfsr, (in[i] >> 7) & 0x1);
    new_bit= (~((curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] |= new_bit << 7;
  }
  return 0;
}

int32_t
scramble_one_bit_per_byte (scrambler_handle_t *h, uint8_t *out,
			   const uint8_t *in, size_t bit_cnt)
{
  size_t i;
  if(!h){
    return -1;
  }
  for(i = 0; i < bit_cnt; i++){
    out[i] = in[i] ^ lfsr_next_bit(&h->lfsr);
  }
  return 0;
}

int32_t
descramble_one_bit_per_byte (scrambler_handle_t *h, uint8_t *out,
			     const uint8_t *in, size_t bit_cnt)
{
  size_t i;
  if(!h){
    return -1;
  }
  for(i = 0; i < bit_cnt; i++){
    out[i] = lfsr_next_bit_descramble(&h->lfsr, in[i]);
  }
  return 0;
}
