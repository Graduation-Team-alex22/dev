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

#ifndef INC_LFSR_H_
#define INC_LFSR_H_

#include <stdint.h>

typedef struct {
  uint32_t mask;
  uint32_t seed;
  uint32_t order;
  uint32_t shift_reg;
} lfsr_handle_t;

int32_t
lfsr_init(lfsr_handle_t *h, uint32_t mask, uint32_t seed, uint32_t order);

uint8_t
lfsr_next_bit(lfsr_handle_t *h);

uint8_t
lfsr_next_bit_scramble(lfsr_handle_t *h, uint8_t bit);

uint8_t
lfsr_next_bit_descramble(lfsr_handle_t *h, uint8_t bit);

int32_t
lfsr_reset(lfsr_handle_t *h);


#endif /* INC_LFSR_H_ */
