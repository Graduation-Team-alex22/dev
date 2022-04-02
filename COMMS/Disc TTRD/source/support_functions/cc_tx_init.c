#include "cc_tx_init.h"


#ifdef CC1101_UART

#elif CC1120

uint8_t cc_tx_rd_reg (uint16_t addr, uint8_t *data)
{
	uint8_t temp_TxBuffer[4];
  uint8_t temp_RxBuffer[4] = { 0, 0, 0, 0 };
  uint8_t len = 0;
	 if (addr >= CC_EXT_ADD) {
    len = 3;

    temp_TxBuffer[0] = 0xAF;
    temp_TxBuffer[1] = (uint8_t) (0x00FF & addr);
    temp_TxBuffer[2] = 0;
  }
  else {
    len = 2;
    /* bit masked for read function */
    addr |= 0x0080;
    temp_TxBuffer[0] = (uint8_t) (0x00FF & addr);
    temp_TxBuffer[1] = 0;
  }
	GPIO_WriteBit(CS_SPI1_TX_GPIO_Port, CS_SPI1_TX_Pin, GPIO_PIN_RESET);
	//HAL_SPI_TransmitReceive (&hspi1, (uint8_t *) temp_TxBuffer,(uint8_t *) temp_RxBuffer, len, 5000); need to be changed
	GPIO_WriteBit(CS_SPI1_TX_GPIO_Port, CS_SPI1_TX_Pin, GPIO_PIN_SET);
	
	*data = temp_RxBuffer[len - 1];

  return temp_RxBuffer[0];
}

#elif CC1101_SPI


#endif



