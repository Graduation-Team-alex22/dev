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

#include "persistent_mem.h"
#include "flash.h"

uint32_t
comms_persistent_mem_init()
{
  return flash_INIT();
}

/**
 * Stores a word at the one word long flash memory of the processor
 * @param word the data that are going to be stored
 * @param offset the offset from the start of the flash memory blocksize_t offset
 */
void
comms_write_persistent_word(uint32_t word, size_t offset)
{
  flash_write_trasmit(word, offset);
}

/**
 * Reads a word from the flash memory
 * @param offset the offset from the start of the flash memory blocksize_t offset
 * @return the word value
 */
uint32_t
comms_read_persistent_word(size_t offset)
{
  return flash_read_trasmit(offset);
}
