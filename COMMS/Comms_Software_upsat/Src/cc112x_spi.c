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

#include "cc112x_spi.h"
#include "cc1120_config.h"
#include "utils.h"
#include "log.h"
#include "status.h"
#include "scrambler.h"
#include "cc_tx_init.h"
#include "sysview.h"
#include <string.h>

#undef __FILE_ID__
#define __FILE_ID__ 26

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern UART_HandleTypeDef huart5;
volatile extern uint8_t tx_thr_flag;
volatile extern uint8_t tx_fin_flag;
volatile extern uint8_t rx_sync_flag;
volatile extern uint8_t rx_finished_flag;
volatile extern uint8_t rx_thr_flag;

static uint8_t tx_frag_buf[2 + CC1120_TX_FIFO_SIZE];
static uint8_t rx_spi_tx_buf[2 + CC1120_RX_FIFO_SIZE];
static uint8_t rx_tmp_buf[2 + CC1120_RX_FIFO_SIZE];

/**
 * Delay for \p us microseconds.
 * NOTE: This delay function is not so accurate!!!
 * @param us how many microseconds to delay the execution
 */
static inline void
delay_us (uint32_t us)
{
  volatile uint32_t cycles = (SystemCoreClock / 1000000L) * us;
  volatile uint32_t start = 0;
  do {
    start++;
  }
  while (start < cycles);
}

/**
 * Reads a register from the TX CC1120
 * @param addr the desired register address
 * @param data memory to store the value of the register
 * @return the first byte of the SPI buffer. Can be used for error checking
 */
uint8_t
cc_tx_rd_reg (uint16_t addr, uint8_t *data)
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

  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive (&hspi1, (uint8_t *) temp_TxBuffer,
			   (uint8_t *) temp_RxBuffer, len, 5000);
  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_15, GPIO_PIN_SET);

  *data = temp_RxBuffer[len - 1];

  return temp_RxBuffer[0];
}

/**
 * Writes a value to a register of the TX CC1120
 * @param addr the address of the register
 * @param data the data to be written
 * @return the first byte of the SPI buffer. Can be used for error checking
 */
uint8_t
cc_tx_wr_reg (uint16_t addr, uint8_t data)
{

  uint8_t aTxBuffer[4];
  uint8_t aRxBuffer[4] = { 0, 0, 0, 0 };
  uint8_t len = 0;

  if (addr >= CC_EXT_ADD) {
    len = 3;

    aTxBuffer[0] = 0x2F;
    aTxBuffer[1] = (uint8_t) (0x00FF & addr);
    aTxBuffer[2] = data;
  }
  else {
    len = 2;

    aTxBuffer[0] = (uint8_t) (0x00FF & addr);
    aTxBuffer[1] = data;
  }

  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
  delay_us(4);
  HAL_SPI_TransmitReceive (&hspi1, (uint8_t *) aTxBuffer, (uint8_t *) aRxBuffer,
			   len, 5000); //send and receive 3 bytes
  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_15, GPIO_PIN_SET);

  return aRxBuffer[0];
}

/**
 * Executes a command at the TX CC1120
 * @param CMDStrobe the command code
 * @return the first byte of the SPI buffer. Can be used for error checking
 */
uint8_t
cc_tx_cmd (uint8_t CMDStrobe)
{

  uint8_t tx_buf;
  uint8_t rx_buf;

  tx_buf = CMDStrobe;

  /* chip select LOw */
  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
  /* Send-receive 1 byte */
  HAL_SPI_TransmitReceive (&hspi1, &tx_buf, &rx_buf, sizeof(uint8_t), 5000);

  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_15, GPIO_PIN_SET);

  /*
   * TODO: Return the whole RX buffer
   */
  return rx_buf;
}

/**
 * Write to the TX FIFO \p len bytes using the SPI bus
 * @param data the input buffer containing the data
 * @param spi_rx_data the SPI buffer for the return bytes
 * @param len the number of bytes to be sent
 * @return 0 on success of HAL_StatusTypeDef appropriate error code
 */
HAL_StatusTypeDef
cc_tx_spi_write_fifo(const uint8_t *data, uint8_t *spi_rx_data, size_t len)
{
  HAL_StatusTypeDef ret;
  /* Write the Burst flag at the start of the buffer */
  tx_frag_buf[0] = BURST_TXFIFO;
  memcpy(tx_frag_buf + 1, data, len);

  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
  delay_us(4);
  ret = HAL_SPI_TransmitReceive (&hspi1, tx_frag_buf, spi_rx_data, len + 1,
				 COMMS_DEFAULT_TIMEOUT_MS);
  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
  return ret;
}

/**
 * Sets the register configuration for CW transmission
 */
static inline void
set_tx_cw_regs()
{
  tx_cw_registerConfig();
  tx_manualCalibration ();
}

/**
 * Sets the register configuration for FSK transmission
 */
static inline void
set_tx_fsk_regs()
{
  tx_registerConfig();
  tx_manualCalibration ();
}

/**
 * Transmits data using CW
 * @param in an array containing the CW symbols that should be sent
 * @param len the length of the array
 * @return CW_OK on success of an appropriate negative number with the
 * appropriate error
 */
int32_t
cc_tx_cw(const cw_pulse_t *in, size_t len)
{
  size_t i;
  uint8_t t[4] = {0, 0, 0, 0};
  uint8_t t2[16] = {0, 0};

  /* Set the CC1120 into unmodulated continuous FM mode */
  set_tx_cw_regs();
  cc_tx_cmd (SFTX);

  /*At least one byte should be written at the FIFO */
  cc_tx_spi_write_fifo (t, t2, 4);

  /*
   * Switch on and off the carrier for the proper amount of time
   */
  for(i = 0; i < len; i++) {
    if(in[i].cw_on){
      cc_tx_cmd (STX);
      HAL_Delay(in[i].duration_ms);
      cc_tx_cmd (SIDLE);
    }
    else{
      HAL_Delay(in[i].duration_ms);
    }
  }
  cc_tx_cmd (SIDLE);
  HAL_Delay(10);

  /* Restore the FSK configuration */
  cc_tx_cmd (SRES);
  set_tx_fsk_regs();
  cc_tx_cmd (SIDLE);
  cc_tx_cmd (SFTX);
  return CW_OK;
}

/**
 * Transmit data continuously using the framing format suitable for legacy
 * hardware radios.
 * @param data the buffer with the encoded data
 * @param size the size of the buffer
 * @param rec_data receive data buffer for storing the feedback from the SPI
 * @param timeout_ms timeout in milliseconds
 * @return the number of bytes transmitted or an appropriate error code
 */
int32_t
cc_tx_data_continuous (const uint8_t *data, size_t size, uint8_t *rec_data,
		       size_t timeout_ms)
{
  size_t bytes_left = size;
  size_t gone = 0;
  size_t in_fifo = 0;
  size_t issue_len;
  size_t processed;
  uint8_t first_burst = 1;
  uint32_t start_tick;
  uint8_t mode;
  uint8_t timeout;
  uint8_t tmp;

  /* Reset the packet transmitted flag */
  tx_fin_flag = 0;

  /* Set the TX into infinite packet length mode only if the frame is
   * larger than the maximum CC1120 allowed frame
   */
  if(size > CC1120_TX_MAX_FRAME_LEN){
    mode = CC1120_INFINITE_PKT_LEN;
  }
  else{
    mode = CC1120_FIXED_PKT_LEN;
  }
  cc_tx_wr_reg(PKT_CFG0, mode);
  /* Pre-program the packet length register */
  cc_tx_wr_reg(PKT_LEN, size % (CC1120_TX_MAX_FRAME_LEN + 1));

  /* The clock is ticking... */
  start_tick = HAL_GetTick ();
  while( gone + in_fifo < size) {
    /* Reset the FIFO interrupt flag */
    tx_thr_flag = 0;

    if(first_burst){
      first_burst = 0;
      issue_len = min(CC1120_TX_FIFO_SIZE, size);
      cc_tx_spi_write_fifo (data, rec_data, issue_len);

      /* Start the TX procedure */
      cc_tx_cmd (STX);
      SYSVIEW_PRINT("CC TX: Issue: %u", issue_len);
    }
    else{
      issue_len = min(CC1120_TXFIFO_AVAILABLE_BYTES, size - gone - in_fifo);
      cc_tx_spi_write_fifo (data + gone + in_fifo, rec_data, issue_len);
      SYSVIEW_PRINT("CC TX: Issue: %u GN: %u INFIFO: %u", issue_len,
		    gone, in_fifo);
    }
    bytes_left -= issue_len;

    /* Track the number of bytes in the TX FIFO*/
    in_fifo += issue_len;

    /*
     * If the remaining bytes are less than the maximum frame size switch to
     * fixed packet length mode
     */
    if (bytes_left < ((CC1120_TX_MAX_FRAME_LEN + 1) - in_fifo)
	&& (mode == CC1120_INFINITE_PKT_LEN) ) {
      cc_tx_wr_reg(PKT_CFG0, CC1120_FIXED_PKT_LEN);
      mode = CC1120_FIXED_PKT_LEN;
    }

    /* If the data in the FIFO is above the IRQ limit wait for that IRQ */
    if (in_fifo >= CC1120_TXFIFO_IRQ_THR && size != issue_len && bytes_left) {
      timeout = 1;
      while (HAL_GetTick () - start_tick < timeout_ms) {
        /* Remove this and the satellite will EXPLODE! */
	delay_us(800);
        if (tx_thr_flag) {
          timeout = 0;
          break;
        }
      }

      /* Timeout occurred. Abort */
      if (timeout) {
	SYSVIEW_PRINT("CC TX: Timeout %u", __LINE__);
	delay_us(1000);
	cc_tx_cmd (SIDLE);
	cc_tx_cmd (SFTX);
	return COMMS_STATUS_TIMEOUT;
      }

      processed = in_fifo - issue_len ;
      gone += processed;
      in_fifo -= processed;
    }
    else {
      gone += issue_len;
      in_fifo -= issue_len;
    }
  }

  /* Wait the FIFO to empty */
  timeout = 1;
  while (HAL_GetTick () - start_tick < timeout_ms) {
    delay_us(800);
    cc_tx_rd_reg(NUM_TXBYTES, &tmp);
    if (tmp == 0) {
      timeout = 0;
      break;
    }
  }

  /* Timeout occurred. Abort */
  if (timeout) {
    SYSVIEW_PRINT("CC TX: Timeout %u", __LINE__);
    delay_us(1000);
    cc_tx_cmd (SIDLE);
    cc_tx_cmd (SFTX);
    return COMMS_STATUS_TIMEOUT;
  }

  /* If you change this, you deserve to die trying to debug... */
  delay_us(1000);
  cc_tx_cmd (SIDLE);
  cc_tx_cmd (SFTX);
  return gone + in_fifo;
}

/**
 * Reads a register from the RX CC1120 chip
 * @param addr the desired register address
 * @param data memory to store the value of the register
 * @return the first byte of the SPI buffer. Can be used for error checking
 */
uint8_t
cc_rx_rd_reg (uint16_t addr, uint8_t *data)
{

  uint8_t temp_TxBuffer[4];
  uint8_t temp_RxBuffer[4] =
    { 0, 0, 0, 0 };
  uint8_t len = 0;

  if (addr >= CC_EXT_ADD) {
    len = 3;

    temp_TxBuffer[0] = 0xAF;
    /* extended address */
    temp_TxBuffer[1] = (uint8_t) (0x00FF & addr);
    /* send dummy so that i can read data */
    temp_TxBuffer[2] = 0;
  }
  else {
    len = 2;
    /*bit masked for read function*/
    addr |= 0x0080;
    /* extended address */
    temp_TxBuffer[0] = (uint8_t) (0x00FF & addr);
    temp_TxBuffer[1] = 0;
  }

  HAL_GPIO_WritePin (GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
  delay_us(4);
  HAL_SPI_TransmitReceive (&hspi2, (uint8_t *) temp_TxBuffer,
			   (uint8_t *) temp_RxBuffer, len, 5000);
  HAL_GPIO_WritePin (GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
  delay_us(4);
  *data = temp_RxBuffer[len - 1];

  return temp_RxBuffer[0];
}

/**
 * Write to the RX FIFO \p len bytes using the SPI bus
 * @param data the input buffer containing the data
 * @param spi_rx_data the SPI buffer for the return bytes
 * @param len the number of bytes to be sent
 * @return 0 on success of HAL_StatusTypeDef appropriate error code
 */
HAL_StatusTypeDef
cc_rx_spi_write_fifo(uint8_t *data, uint8_t *spi_rx_data, size_t len)
{
  HAL_StatusTypeDef ret;
  HAL_GPIO_WritePin (GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
  delay_us(4);
  ret = HAL_SPI_TransmitReceive (&hspi2, data, spi_rx_data, len,
				 COMMS_DEFAULT_TIMEOUT_MS);
  HAL_GPIO_WritePin (GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
  return ret;
}

/**
 * Writes a value to a register of the RX CC1120
 * @param addr the address of the register
 * @param data the data to be written
 * @return the first byte of the SPI buffer. Can be used for error checking
 */
uint8_t
cc_rx_wr_reg (uint16_t add, uint8_t data)
{

  uint8_t aTxBuffer[4];
  uint8_t aRxBuffer[4] = { 0, 0, 0, 0 };
  uint8_t len = 0;

  if (add >= CC_EXT_ADD) {
    len = 3;

    aTxBuffer[0] = 0x2F;
    /* extended address */
    aTxBuffer[1] = (uint8_t) (0x00FF & add);
    /* send dummy so that i can read data */
    aTxBuffer[2] = data;
  }
  else {
    len = 2;
    /* extended address */
    aTxBuffer[0] = (uint8_t) (0x00FF & add);
    /* send dummy so that i can read data */
    aTxBuffer[1] = data;
  }

  HAL_GPIO_WritePin (GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
  delay_us(10);
  HAL_SPI_TransmitReceive (&hspi2, (uint8_t *) aTxBuffer, (uint8_t *) aRxBuffer,
			   len, COMMS_DEFAULT_TIMEOUT_MS);
  HAL_GPIO_WritePin (GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
  delay_us(10);

  return aRxBuffer[0];
}

/**
 * Resets all the RX related IRQs
 */
static inline void
cc_rx_reset_irqs()
{
  rx_sync_flag = 0;
  rx_finished_flag = 0;
  rx_thr_flag = 0;
}

/**
 * Get a frame from the air. This method blocks for \p timeout_ms milliseconds
 * until a valid frame is received.
 * When this called is triggered the SYNC word interupt MUST be already
 * triggered.
 *
 * @param out the output buffer
 * @param len the size of the output buffer
 * @param timeout_ms the timeout period in milliseconds
 * @return the size of the frame in bytes or a negative number indicating
 * the appropriate error
 */
int32_t
cc_rx_data_packet (uint8_t *out, size_t len, size_t timeout_ms)
{
  HAL_StatusTypeDef ret;
  uint8_t frame_len;
  uint8_t received = 0;
  uint32_t start_tick;
  uint8_t timeout = 1;
  uint8_t rx_n_bytes;

  /*Reset all the RX-related flags */
  cc_rx_reset_irqs();

  /* A FIFO error can occur */
  if(!C_ASSERT(cc_rx_check_fifo_status())) {
    return COMMS_STATUS_RXFIFO_ERROR;
  }

  start_tick = HAL_GetTick ();

  /*
   * The SYNC word has been already received.
   * Time to extract the frame length,. This is indicated by the first byte
   * after the SYNC word
   */
  do {
    cc_rx_rd_reg (NUM_RXBYTES, &rx_n_bytes);
  }
  while (rx_n_bytes < 2);

  /* One byte FIFO access */
  cc_rx_rd_reg (SINGLE_RXFIFO, &frame_len);

  /*
   * Now that we have the frame length check if the FIFO should be dequeued
   * multiple times or not
   */
  while (frame_len - received > CC1120_RXFIFO_THR) {
    /* Wait for the RX FIFO above threshold interrupt */
    timeout = 1;
    while (HAL_GetTick () - start_tick < timeout_ms) {
      delay_us(800);
      if (rx_thr_flag) {
	timeout = 0;
	break;
      }
    }

    if (timeout) {
      SYSVIEW_PRINT("RX timeout: %u", __LINE__);
      /* Flush and restart! */
      cc_rx_cmd (SFRX);
      cc_rx_cmd (SIDLE);
      cc_rx_reset_irqs();
      cc_rx_cmd (SRX);
      return COMMS_STATUS_TIMEOUT;
    }

    /* We can now dequeue CC1120_BYTES_IN_RX_FIF0 bytes */
    ret = cc_rx_spi_read_fifo (out + received, CC1120_BYTES_IN_RX_FIF0);
    /*Reset the flag */
    rx_thr_flag = 0;

    if (ret) {
      SYSVIEW_PRINT("RX FIFO ERROR: %u", __LINE__);
      /* Flush and restart! */
      cc_rx_cmd (SFRX);
      cc_rx_cmd (SIDLE);
      cc_rx_reset_irqs();
      cc_rx_cmd (SRX);
      return COMMS_STATUS_NO_DATA;
    }
    received += CC1120_BYTES_IN_RX_FIF0;
  }

  /* Wait for the packet end interrupt */
  timeout = 1;
  while (HAL_GetTick () - start_tick < timeout_ms) {
    delay_us(800);
    if (rx_finished_flag) {
      timeout = 0;
      break;
    }
  }
  if (timeout) {
    SYSVIEW_PRINT("RX timeout: %u", __LINE__);
    /* Flush and restart! */
    cc_rx_cmd (SFRX);
    cc_rx_cmd (SIDLE);
    cc_rx_reset_irqs();
    cc_rx_cmd (SRX);
    return COMMS_STATUS_RXFIFO_ERROR;
  }

  /* Now dequeue the remaining bytes in the FIFO if any left*/
  if (frame_len - received > 1) {
    ret = cc_rx_spi_read_fifo (out + received, frame_len - received);
    if (ret) {
      /* Flush and restart! */
      cc_rx_cmd (SFRX);
      cc_rx_cmd (SIDLE);
      cc_rx_reset_irqs();
      cc_rx_cmd (SRX);
      return COMMS_STATUS_NO_DATA;
    }
  }
  else if (frame_len - received == 1) {
    /* One byte FIFO access */
    cc_rx_rd_reg (SINGLE_RXFIFO, out + received);
  }
  /* Flush and restart! */
  cc_rx_cmd (SFRX);
  cc_rx_cmd (SIDLE);
  cc_rx_reset_irqs();
  cc_rx_cmd (SRX);
  return frame_len;
}



/**
 * Read len bytes from the RX FIFO using the SPI
 * @param out the output buffer
 * @param len the number of bytes to be read
 * @return 0 on success of HAL_StatusTypeDef appropriate error code
 */
HAL_StatusTypeDef
cc_rx_spi_read_fifo(uint8_t *out, size_t len)
{
  HAL_StatusTypeDef ret;
  /* Reset the send buffer */
  memset(rx_spi_tx_buf, 0, sizeof(rx_spi_tx_buf));
  rx_spi_tx_buf[0] = BURST_RXFIFO;
  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15, GPIO_PIN_RESET);
  delay_us(4);
  /* Remove the response SPI byte */
  ret = HAL_SPI_TransmitReceive (&hspi2, rx_spi_tx_buf, rx_tmp_buf, len + 1,
				 COMMS_DEFAULT_TIMEOUT_MS);
  delay_us(4);
  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15, GPIO_PIN_SET);
  memcpy(out, rx_tmp_buf + 1, len);
  return ret;
}

/**
 * Executes a command at the TX CC1120
 * @param CMDStrobe the command code
 * @return the first byte of the SPI buffer. Can be used for error checking
 */
uint8_t
cc_rx_cmd (uint8_t CMDStrobe)
{

  uint8_t aTxBuffer[1];
  uint8_t aRxBuffer[1];

  aTxBuffer[0] = CMDStrobe;

  HAL_GPIO_WritePin (GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
  delay_us(4);
  HAL_SPI_TransmitReceive (&hspi2, (uint8_t*) aTxBuffer, (uint8_t *) aRxBuffer,
			   1, COMMS_DEFAULT_TIMEOUT_MS);
  HAL_GPIO_WritePin (GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
  delay_us(4);
  return aRxBuffer[0];
}

/**
 * Checks if the RX FIFO of the CC1120 encountered any error. If yes,
 * the FIFO is flushed and the RX CC1120 is set again in normal RX mode.
 *
 * @return 1 in case the CC1120 FIFO is ok, 0 otherwise
 */
uint8_t
cc_rx_check_fifo_status()
{
  uint8_t reg;
  cc_rx_rd_reg(MODEM_STATUS1, &reg);
  /* Underflow or overflow are asserted */
  if(reg & 0xc){
    cc_rx_cmd (SFRX);
    cc_rx_cmd (SIDLE);
    cc_rx_reset_irqs();
    cc_rx_cmd (SRX);
    return 0;
  }
  return 1;
}

