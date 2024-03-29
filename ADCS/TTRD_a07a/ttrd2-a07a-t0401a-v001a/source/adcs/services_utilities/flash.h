#ifndef FLASH_H__
#define FLASH_H__

#include <stdint.h>

/* Memory map */
#define BOOT_CNT_BASE_ADDRESS   0x0
#define BOOT_CNT_OFFSET_ADDRESS 1

#define TLE_BASE_ADDRESS 0x00F000
#define TLE_ADDRESS_OFFSET 1

#define GPS_ALARM_BASE_ADDRESS   0x010000
#define GPS_ALARM_OFFSET_ADDRESS 1

#define GPS_LOCK_BASE_ADDRESS   0x020000
#define GPS_LOCK_OFFSET_ADDRESS 1


typedef enum {
    FLASH_ERROR = 0, FLASH_NORMAL
} flash_status_e;  // flash_status


flash_status_e flash_init(void);
flash_status_e flash_write_enable(void);
flash_status_e flash_readID(uint8_t *id);
flash_status_e flash_chip_erase(void);
flash_status_e flash_erase_block4K(uint32_t address);
flash_status_e flash_write_byte(uint8_t data, uint32_t address);
flash_status_e flash_write_page(uint8_t *data, uint8_t data_len, uint32_t address);
flash_status_e flash_read_byte(uint8_t *data, uint32_t address);

#endif
