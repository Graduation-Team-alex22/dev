#include "comms_hal.h"


#undef __FILE_ID__
#define __FILE_ID__ 26

extern UART_HandleTypeDef huart5;

SAT_returnState HAL_takeMutex(TC_TM_app_id app_id) {
  return SATR_OK;
}

SAT_returnState HAL_giveMutex(TC_TM_app_id app_id) {
  return SATR_OK;
}


void HAL_sys_delay(uint32_t sec) {
	HAL_Delay(sec);
}

void HAL_comms_SD_ON() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
}

void HAL_comms_SD_OFF() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
}

SAT_returnState hal_kill_uart(TC_TM_app_id app_id) {

    UART_HandleTypeDef *huart;
    HAL_StatusTypeDef res;
    SAT_returnState ret = SATR_ERROR;

    if (app_id == OBC_APP_ID) {
      huart = &huart5;
    }
    else if (app_id == DBG_APP_ID) {
      huart = &huart5;
    }
    else {
      return SATR_ERROR;
    }

    HAL_UART_DeInit(huart);
    res = HAL_UART_Init(huart);
    if (res == HAL_OK)
    {
      ret = SATR_OK;
    }
    return ret;
}

SAT_returnState HAL_uart_tx_check(TC_TM_app_id app_id) {
    
    HAL_UART_StateTypeDef res;
    UART_HandleTypeDef *huart;

    if (app_id == OBC_APP_ID) {
      huart = &huart5;
    }
    else if (app_id == DBG_APP_ID) {
      huart = &huart5;
    }
    else {
      return SATR_ERROR;
    }

    res = HAL_UART_GetState(huart);
    if(res == HAL_UART_STATE_BUSY ||
       res == HAL_UART_STATE_BUSY_TX ||
       res == HAL_UART_STATE_BUSY_TX_RX) { return SATR_ALREADY_SERVICING; }

    return SATR_OK;
}

void HAL_uart_tx(TC_TM_app_id app_id, uint8_t *buf, uint16_t size) {
    
    HAL_StatusTypeDef res;
    UART_HandleTypeDef *huart;

    if (app_id == OBC_APP_ID) {
      huart = &huart5;
    }
    else if (app_id == DBG_APP_ID) {
      huart = &huart5;
    }
    else {
      return;
    }

    //HAL_UART_Transmit(&huart2, buf, size, 10);
    for(;;) { // should use hard limits
        res = HAL_UART_Transmit_DMA(huart, buf, size);
        if(res == HAL_OK) { break; }
        HAL_Delay(1);
    }
}

SAT_returnState HAL_uart_rx(TC_TM_app_id app_id, struct uart_data *data) {

    UART_HandleTypeDef *huart;

    if (app_id == OBC_APP_ID) {
      huart = &huart5;
    }
    else if (app_id == DBG_APP_ID) {
      huart = &huart5;
    }
    else {
      return SATR_ERROR;
    }

    if(huart->RxState == HAL_UART_STATE_READY) {
        data->uart_size = huart->RxXferSize - huart->RxXferCount;
        for(uint16_t i = 0; i < data->uart_size; i++) { data->uart_unpkt_buf[i] = data->uart_buf[i]; }
        HAL_UART_Receive_IT(huart, data->uart_buf, UART_BUF_SIZE);
        return SATR_EOT;
    }
    return SATR_OK;
}

/**
  * @brief  This function handles UART interrupt request.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_COMMS_UART_IRQHandler(UART_HandleTypeDef *huart)
{
  uint32_t tmp1 = 0U, tmp2 = 0U;

  tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
  tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
  /* UART in mode Receiver ---------------------------------------------------*/
  if((tmp1 != RESET) && (tmp2 != RESET))
  { 
    UART_COMMS_Receive_IT(huart);
  }
}

/**
  * @brief  Receives an amount of data in non blocking mode 
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
void UART_COMMS_Receive_IT(UART_HandleTypeDef *huart)
{
    uint8_t c;

    c = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FFU);
    if(huart->RxXferSize == huart->RxXferCount && c == HLDLC_START_FLAG) {
      *huart->pRxBuffPtr++ = c;
      huart->RxXferCount--;
      //start timeout
    } else if(c == HLDLC_START_FLAG && (huart->RxXferSize - huart->RxXferCount) < TC_MIN_PKT_SIZE) {
      //error
      //event log
      //reset buffers & pointers
      //start timeout
    } else if(c == HLDLC_START_FLAG) {
      *huart->pRxBuffPtr++ = c;
      huart->RxXferCount--;
      
      __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

      /* Disable the UART Parity Error Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_PE);

      /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
      __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

	  /* Rx process is completed, restore huart->RxState to Ready */
      huart->RxState = HAL_UART_STATE_READY;
    } else if(huart->RxXferSize > huart->RxXferCount) {
      *huart->pRxBuffPtr++ = c;
      huart->RxXferCount--;
    }

    if(huart->RxXferCount == 0U) // errror
    {


    }

}

void HAL_reset_source(uint8_t *src) {

    *src = __HAL_RCC_GET_FLAG(RCC_FLAG_BORRST);
    *src |= (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) << 1);
    *src |= (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) << 2);
    *src |= (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) << 3);
    *src |= (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) << 4);
    *src |= (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) << 5);
    *src |= (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) << 6);
 
    __HAL_RCC_CLEAR_RESET_FLAGS();

}

uint32_t HAL_sys_GetTick() {
  return HAL_GetTick();
}
