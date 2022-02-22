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

#ifndef INC_SCRAMBLER_H_
#define INC_SCRAMBLER_H_

#include <stdlib.h>
#include "lfsr.h"

typedef struct
{
  lfsr_handle_t lfsr;
  uint8_t prev_bit;
} scrambler_handle_t;

int32_t
scrambler_init (scrambler_handle_t *h, uint32_t pol, uint32_t seed,
		uint32_t order);

int32_t
scrambler_reset (scrambler_handle_t *h);

int32_t
descrambler_init (scrambler_handle_t *h, uint32_t pol, uint32_t seed,
		uint32_t order);

int32_t
descrambler_reset (scrambler_handle_t *h);

int32_t
scramble_data (scrambler_handle_t *h, uint8_t *out, const uint8_t *in,
	       size_t len);

int32_t
scramble_data_nrzi (scrambler_handle_t *h, uint8_t *out, const uint8_t *in,
		    size_t len);

int32_t
descramble_data (scrambler_handle_t *h, uint8_t *out, const uint8_t *in,
		 size_t len);

int32_t
descramble_data_nrzi (scrambler_handle_t *h, uint8_t *out, const uint8_t *in,
		      size_t len);

int32_t
scramble_one_bit_per_byte (scrambler_handle_t *h, uint8_t *out,
			   const uint8_t *in, size_t bit_cnt);

int32_t
descramble_one_bit_per_byte (scrambler_handle_t *h, uint8_t *out,
			     const uint8_t *in, size_t bit_cnt);



#endif /* INC_SCRAMBLER_H_ */
