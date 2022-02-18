/**
 ******************************************************************************
 * @file    Project/STM32F0xx_StdPeriph_Templates/main.c
 * @author  MCD Application Team
 * @version V1.1.0
 * @date    31-July-2013
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
//#define USE_STDPERIPH_DRIVER=1  //Just for Eclipse Indexer, also defined in makefile for build
// Upd. define it under Path and Symbols

#include "hw_init.h"
#include <stdio.h>

/** @addtogroup STM32F0xx_StdPeriph_Templates
 * @{
 */



//Local pck to be used by IRQs
I2C_RX_pck rx_pck;
I2C_TX_pck tx_pck;
int32_t io_mtx;

static int32_t I2C_state = I2C_Idle_Mode;



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void)
{
	uint32_t tic;

	hw_init();
	I2C_init();

	CNTr_ = setDirection(RPM2CNT(10));

	tx_pck.dummy = 0;
	while (1)
	{
		// Too short, do it within IRQ to save mtxs.
		//		if( (I2C_state != I2C_RX_Mode) && (rx_pck.flag&Pck_Flag_NewAndReady) ){ // KEEP flag check right..
		//			RPMr_ = setDirection(rx_pck.RPRr);
		//			rx_pck.flag &= (!Pck_Flag_NewAndReady);
		//		}
		//		if( (I2C_state != I2C_TX_Mode)){  // Keep tx_pck updated
		//
		//			ADC_ITConfig(ADC1,ADC_IT_AWD,DISABLE);
		//			tx_pck.RPRm = RPMr_;
		//			tx_pck.flag = 123456789;//rx_pck.flag;
		//			ADC_ITConfig(ADC1,ADC_IT_AWD,ENABLE);
		//
		//			delay_us(1000);
		//			CRC_ResetDR();
		//			CRC_CalcCRC(tx_pck.flag);
		//			CRC_CalcCRC(tx_pck.RPRm);
		//			CRC_CalcCRC(tx_pck.dummy);
		//			tx_pck.crc = CRC->DR;
		//		}

		if(I2C_state == I2C_Idle_Mode){
			tic = toc;
		}
		if( (toc - tic)> (48*50000)){ //toc runs in 1/48us
			// I2C has started but seems stucked, pending bytes etc..
			I2C_init(); // Slave only, just to release SCL, SDA.
			I2C_state = I2C_Idle_Mode;
		}

	}
}

void SysTick_Handler(void){// move the main loop to main to avoid preemption..
}


void I2C1_IRQHandler(void){

	static int32_t cnt=0;
	static volatile uint8_t data;
	static uint8_t *ptr;
	static uint32_t *ptr32;
	static uint32_t  crc[3];


	if(I2C_GetFlagStatus(I2C1,I2C_FLAG_ADDR)){
		if((I2C1->ISR & I2C_ISR_DIR)){
			cnt = 0;
			ptr = (uint8_t *)&tx_pck;
			I2C_state = I2C_TX_Mode;

			// Stresses SCL for <5us, no problem, do it here
			// tx_pck.RPRm, tx_pck.flag Updated in ADC_IRQ, (higher priority)
			CRC_ResetDR();
			ADC_ITConfig(ADC1,ADC_IT_AWD,DISABLE);
			CRC_CalcCRC(tx_pck.flag);
			CRC_CalcCRC(tx_pck.CNTm);
			CRC_CalcCRC(tx_pck.dummy);
			tx_pck.crc = CRC->DR;
			ADC_ITConfig(ADC1,ADC_IT_AWD,ENABLE);
			//------------------------------------------

			I2C_ITConfig(I2C1,I2C_IT_TXI,ENABLE);
		}
		else{// receiver mode, means Master transmits..
			cnt = 0;
			ptr = (uint8_t *)&rx_pck;
			ptr32 = (uint32_t *)&rx_pck;
			I2C_state = I2C_RX_Mode;
			I2C_ITConfig(I2C1,I2C_IT_RXI,ENABLE);
		}
		I2C_ClearITPendingBit(I2C1,I2C_IT_ADDR);
	}

	if(I2C_GetFlagStatus(I2C1,I2C_FLAG_RXNE)){
		data = I2C_ReceiveData(I2C1); //must read always to reset RX flag
		if ( cnt < pck_Payload){
			if ( ptr != NULL){
				*ptr++ = data;
			}
		}
		cnt++;
		if (cnt == pck_Payload){
			CRC_ResetDR();
			CRC_CalcCRC(*ptr32++);
			CRC_CalcCRC(*ptr32++);
			CRC_CalcCRC(*ptr32++);
			if( CRC->DR == (*ptr32) ){
				//				rx_pck.flag |= Pck_Flag_NewAndReady;
				CNTr_ = setDirection(rx_pck.CNTr);
			}
			I2C_state = I2C_Idle_Mode;
			ptr = NULL;
			I2C_ITConfig(I2C1,I2C_IT_RXI,DISABLE);
		}
	}

	if(I2C_GetFlagStatus(I2C1,I2C_FLAG_TXIS) && (I2C_state == I2C_TX_Mode)){
		if ( cnt < pck_Payload){
			if ( ptr != NULL){
				I2C_SendData(I2C1,*ptr++);
			}
		}
		cnt++;
		if (cnt == pck_Payload){
			ptr = NULL;
			I2C_state = I2C_Idle_Mode;
			I2C_SendData(I2C1,0);
			I2C_ITConfig(I2C1,I2C_IT_TXI,DISABLE);
		}
	}
}


#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/**
 * @}
 */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
