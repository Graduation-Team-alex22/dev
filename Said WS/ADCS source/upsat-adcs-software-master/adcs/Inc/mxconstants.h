/**
  ******************************************************************************
  * File Name          : mxconstants.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MXCONSTANT_H
#define __MXCONSTANT_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define GPS_TX_Pin GPIO_PIN_0
#define GPS_TX_GPIO_Port GPIOA
#define GPS_RX_Pin GPIO_PIN_1
#define GPS_RX_GPIO_Port GPIOA
#define OBC_TX_Pin GPIO_PIN_2
#define OBC_TX_GPIO_Port GPIOA
#define OBC_RX_Pin GPIO_PIN_3
#define OBC_RX_GPIO_Port GPIOA
#define GPS_EN_Pin GPIO_PIN_4
#define GPS_EN_GPIO_Port GPIOA
#define SENS_EN_Pin GPIO_PIN_2
#define SENS_EN_GPIO_Port GPIOB
#define FM_nHLD_Pin GPIO_PIN_12
#define FM_nHLD_GPIO_Port GPIOB
#define CNV_Pin GPIO_PIN_9
#define CNV_GPIO_Port GPIOC
#define RM_CS_Pin GPIO_PIN_8
#define RM_CS_GPIO_Port GPIOA
#define FM_nCE_Pin GPIO_PIN_11
#define FM_nCE_GPIO_Port GPIOA
#define FM_nWP_Pin GPIO_PIN_12
#define FM_nWP_GPIO_Port GPIOA
#define MTz1_Pin GPIO_PIN_4
#define MTz1_GPIO_Port GPIOB
#define MTz2_Pin GPIO_PIN_5
#define MTz2_GPIO_Port GPIOB
#define MTy1_Pin GPIO_PIN_6
#define MTy1_GPIO_Port GPIOB
#define MTy2_Pin GPIO_PIN_7
#define MTy2_GPIO_Port GPIOB
#define MTx1_Pin GPIO_PIN_8
#define MTx1_GPIO_Port GPIOB
#define MTx2_Pin GPIO_PIN_9
#define MTx2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MXCONSTANT_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
