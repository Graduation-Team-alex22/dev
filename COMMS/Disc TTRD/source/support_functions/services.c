#include "../main/main.h"






////////////////////////////////////////////////// FLASH //////////////////////////////////////////////////////////
#define TRANSMIT_VAR_ADD   0x0800C000

uint32_t flash_read_trasmit(size_t offset) {

    uint32_t *val = (uint32_t*)(TRANSMIT_VAR_ADD + offset);

    return *val;
}

////////////////////////////////////////////// PERSISTENT MEM /////////////////////////////////////////////////////
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
