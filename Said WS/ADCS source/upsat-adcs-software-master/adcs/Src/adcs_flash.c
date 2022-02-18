/*
 * adcs_flash.c
 *
 *  Created on: Jul 4, 2016
 *      Author: azisi
 */

#include "stm32f4xx_hal.h"
#include "adcs_flash.h"
#include "service_utilities.h"

#define WRITE_ENABLE    0x06
#define WRITE_BUSY      0x05
#define READ_ID         0x9f
#define FLASH_ID        0x9D
#define CHIP_ERASE      0xC7
#define BLOCK_ERASE_4K  0x20
#define WRITE_BYTE      0x02
#define READ_BYTE       0x03

#define FLASH_TIME_OUT  1000

extern SPI_HandleTypeDef hspi2;

static uint8_t flash_isWrite_busy(flash_status *status);

flash_status flash_init() {
    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(FM_nHLD_GPIO_Port, FM_nHLD_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(FM_nWP_GPIO_Port, FM_nWP_Pin, GPIO_PIN_SET);

    return FLASH_NORMAL;
}

flash_status flash_write_enable() {
    uint8_t spi_in_temp[1];

    spi_in_temp[0] = WRITE_ENABLE;

    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi2, spi_in_temp, 1, FLASH_TIME_OUT) != HAL_OK) {
        return FLASH_ERROR;
    }
    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_SET);

    return FLASH_NORMAL;
}

uint8_t flash_isWrite_busy(flash_status *flash_status_value) {
    uint8_t spi_in_temp[1];
    uint8_t spi_out_temp[1];

    spi_in_temp[0] = WRITE_BUSY;
    spi_out_temp[0] = 0x00;
    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi2, spi_in_temp, 1, FLASH_TIME_OUT) != HAL_OK) {
        *flash_status_value = FLASH_ERROR;
    }
    if (HAL_SPI_Receive(&hspi2, spi_out_temp, 1, FLASH_TIME_OUT) != HAL_OK) {
        *flash_status_value = FLASH_ERROR;
    }
    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_SET);

    *flash_status_value = FLASH_NORMAL;
    return (spi_out_temp[0] & 0x01);
}

flash_status flash_readID(uint8_t *id) {

    uint8_t spi_in_temp[1];
    uint8_t spi_out_temp[1];

    spi_in_temp[0] = READ_ID;

    spi_out_temp[0] = 0x00;

    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi2, spi_in_temp, 1, FLASH_TIME_OUT) != HAL_OK) {
        return FLASH_ERROR;
    }
    if (HAL_SPI_Receive(&hspi2, spi_out_temp, 1, FLASH_TIME_OUT) != HAL_OK) {
        return FLASH_ERROR;
    }
    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_SET);

    *id = spi_out_temp[0];

    if(*id != FLASH_ID) {
        return FLASH_ERROR;
    }

    return FLASH_NORMAL;
}

flash_status flash_chip_erase() {

    uint8_t spi_in_temp[1];

    flash_write_enable();
    spi_in_temp[0] = CHIP_ERASE;

    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi2, spi_in_temp, 1, FLASH_TIME_OUT) != HAL_OK) {
        return FLASH_ERROR;
    }
    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_SET);
    // Wait for write end
    flash_status flash_status_value = FLASH_NORMAL;
    while (flash_isWrite_busy(&flash_status_value)) {
        if (flash_status_value == FLASH_ERROR) {
            return FLASH_ERROR;
        }
    }

    return FLASH_NORMAL;

}

flash_status flash_erase_block4K(uint32_t address) {

    uint8_t spi_in_temp[4];

    flash_write_enable();
    // Command
    spi_in_temp[0] = BLOCK_ERASE_4K;
    // Address
    spi_in_temp[1] = (address >> 16);
    spi_in_temp[2] = (address >> 8);
    spi_in_temp[3] = address;

    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi2, spi_in_temp, 4, FLASH_TIME_OUT) != HAL_OK) {
        return FLASH_ERROR;
    }
    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_SET);
    // Wait for write end
    flash_status flash_status_value = FLASH_NORMAL;
    while (flash_isWrite_busy(&flash_status_value)) {
        if (flash_status_value == FLASH_ERROR) {
            return FLASH_ERROR;
        }
    }

    return FLASH_NORMAL;

}

flash_status flash_write_byte(uint8_t data, uint32_t address) {

    uint8_t spi_in_temp[5];

    flash_write_enable();
    // Command
    spi_in_temp[0] = WRITE_BYTE;
    // Address
    spi_in_temp[1] = (address >> 16);
    spi_in_temp[2] = (address >> 8);
    spi_in_temp[3] = address;
    // Data
    spi_in_temp[4] = data;

    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi2, spi_in_temp, 5, FLASH_TIME_OUT) != HAL_OK) {
        return FLASH_ERROR;
    }
    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_SET);

    // Wait for write end
    flash_status flash_status_value = FLASH_NORMAL;
    while (flash_isWrite_busy(&flash_status_value)) {
        if (flash_status_value == FLASH_ERROR) {
            return FLASH_ERROR;
        }
    }

    return FLASH_NORMAL;

}

flash_status flash_write_page(uint8_t *data, uint8_t data_len, uint32_t address) {

    uint8_t spi_in_temp[4];

    flash_write_enable();
    // Command
    spi_in_temp[0] = WRITE_BYTE;
    // Address
    spi_in_temp[1] = (address >> 16);
    spi_in_temp[2] = (address >> 8);
    spi_in_temp[3] = address;

    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi2, spi_in_temp, 4, FLASH_TIME_OUT) != HAL_OK) {
        return FLASH_ERROR;
    }
    if (HAL_SPI_Transmit(&hspi2, data, data_len, FLASH_TIME_OUT) != HAL_OK) {
        return FLASH_ERROR;
    }
    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_SET);

    // Wait for write end
    flash_status flash_status_value = FLASH_NORMAL;
    while (flash_isWrite_busy(&flash_status_value)) {
        if (flash_status_value == FLASH_ERROR) {
            return FLASH_ERROR;
        }
    }

    return FLASH_NORMAL;

}

flash_status flash_read_byte(uint8_t *data, uint32_t address) {
    uint8_t spi_in_temp[4];
    uint8_t spi_out_temp[1] = { 0 };

    // Command
    spi_in_temp[0] = READ_BYTE;
    // Address
    spi_in_temp[1] = (address >> 16);
    spi_in_temp[2] = (address >> 8);
    spi_in_temp[3] = address;

    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi2, spi_in_temp, 4, FLASH_TIME_OUT) != HAL_OK) {
        return FLASH_ERROR;
    }
    if (HAL_SPI_TransmitReceive(&hspi2, spi_out_temp, spi_out_temp, 1, FLASH_TIME_OUT)
            != HAL_OK) {
        return FLASH_ERROR;
    }
    HAL_GPIO_WritePin(FM_nCE_GPIO_Port, FM_nCE_Pin, GPIO_PIN_SET);

    *data = spi_out_temp[0];

    return FLASH_NORMAL;

}
