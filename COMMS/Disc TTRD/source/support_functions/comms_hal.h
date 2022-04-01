#ifndef __COMMS_HAL_H
#define __COMMS_HAL_H

// Processor Header
#include "../main/main.h"
#include "upsat.h"

/* this file include stm32f4xx_hal_conf.h, stm32f4xx_hal_it.h
interrupt section will be removed 100%

*/
#define GPIO_PIN_RESET	0
#define GPIO_PIN_SET 		1

//typedef enum 
//{
//  HSI_OK       = 0x00U,
//  HSI_ERROR    = 0x01U,
//  HSI_BUSY     = 0x02U,
//  HSI_TIMEOUT  = 0x03U
//} HSI_StatusTypeDef;



//ToDo
//  improve return types
//  need to make the change from the two different typedefs

extern struct _comms_data comms_data;

//Init 


//Update


//Setters
SAT_returnState HAL_takeMutex(TC_TM_app_id app_id);

SAT_returnState HAL_giveMutex(TC_TM_app_id app_id);

void HAL_comms_SD_ON();

void HAL_comms_SD_OFF();

SAT_returnState hal_kill_uart(TC_TM_app_id app_id);

SAT_returnState HAL_uart_rx(TC_TM_app_id app_id, struct uart_data *data);

void HAL_reset_source(uint8_t *src);


//Getters
SAT_returnState HAL_uart_tx_check(TC_TM_app_id app_id);

void HAL_uart_tx(TC_TM_app_id app_id, uint8_t *buf, uint16_t size);


/* need replacment functions

void HAL_sys_delay(uint32_t sec);

void HAL_COMMS_UART_IRQHandler(UART_HandleTypeDef *huart);

void UART_COMMS_Receive_IT(UART_HandleTypeDef *huart);

uint32_t HAL_sys_GetTick();

*/
///////////////////////////////////////////////   stm32f4xx_it  ////////////////////////////////

/* need replace
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);
void SysTick_Handler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void DMA1_Stream3_IRQHandler(void);
void ADC_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
void DMA1_Stream7_IRQHandler(void);
void UART5_IRQHandler(void);
void DMA2_Stream0_IRQHandler(void);
void DMA2_Stream3_IRQHandler(void);

#ifdef __cplusplus
}
#endif

*/
///////////////////////////////////////////////   stm32f4xx_hal_conf  ////////////////////////////////

#ifdef __cplusplus
 extern "C" {
#endif
//commented section 
///* ########################## Module Selection ############################## */
///**
//  * @brief This is the list of modules to be used in the HAL driver 
//  */
//#define HAL_MODULE_ENABLED  

//#define HAL_ADC_MODULE_ENABLED
////#define HAL_CAN_MODULE_ENABLED   
////#define HAL_CRC_MODULE_ENABLED   
////#define HAL_CRYP_MODULE_ENABLED   
////#define HAL_DAC_MODULE_ENABLED   
////#define HAL_DCMI_MODULE_ENABLED   
////#define HAL_DMA2D_MODULE_ENABLED   
////#define HAL_ETH_MODULE_ENABLED   
////#define HAL_NAND_MODULE_ENABLED   
////#define HAL_NOR_MODULE_ENABLED   
////#define HAL_PCCARD_MODULE_ENABLED   
////#define HAL_SRAM_MODULE_ENABLED   
////#define HAL_SDRAM_MODULE_ENABLED   
////#define HAL_HASH_MODULE_ENABLED   
//#define HAL_I2C_MODULE_ENABLED
////#define HAL_I2S_MODULE_ENABLED   
//#define HAL_IWDG_MODULE_ENABLED
////#define HAL_LTDC_MODULE_ENABLED   
////#define HAL_RNG_MODULE_ENABLED   
////#define HAL_RTC_MODULE_ENABLED   
////#define HAL_SAI_MODULE_ENABLED   
////#define HAL_SD_MODULE_ENABLED   
//#define HAL_SPI_MODULE_ENABLED
////#define HAL_TIM_MODULE_ENABLED   
//#define HAL_UART_MODULE_ENABLED
////#define HAL_USART_MODULE_ENABLED   
////#define HAL_IRDA_MODULE_ENABLED   
////#define HAL_SMARTCARD_MODULE_ENABLED   
////#define HAL_WWDG_MODULE_ENABLED   
////#define HAL_PCD_MODULE_ENABLED   
////#define HAL_HCD_MODULE_ENABLED   
////#define HAL_DSI_MODULE_ENABLED   
////#define HAL_QSPI_MODULE_ENABLED   
////#define HAL_QSPI_MODULE_ENABLED   
////#define HAL_CEC_MODULE_ENABLED   
////#define HAL_FMPI2C_MODULE_ENABLED   
////#define HAL_SPDIFRX_MODULE_ENABLED   
////#define HAL_DFSDM_MODULE_ENABLED   
////#define HAL_LPTIM_MODULE_ENABLED   
//#define HAL_GPIO_MODULE_ENABLED
//#define HAL_DMA_MODULE_ENABLED
//#define HAL_RCC_MODULE_ENABLED
//#define HAL_FLASH_MODULE_ENABLED
//#define HAL_PWR_MODULE_ENABLED
//#define HAL_CORTEX_MODULE_ENABLED

//commented section 
/* ########################## HSE/HSI Values adaptation ##################### */
/**
  * @brief Adjust the value of External High Speed oscillator (HSE) used in your application.
  *        This value is used by the RCC HAL module to compute the system frequency
  *        (when HSE is used as system clock source, directly or through the PLL).  
  */
//#if !defined  (HSE_VALUE) 
//  #define HSE_VALUE    ((uint32_t)12000000U) /*!< Value of the External oscillator in Hz */
//#endif /* HSE_VALUE */

//#if !defined  (HSE_STARTUP_TIMEOUT)
//  #define HSE_STARTUP_TIMEOUT    ((uint32_t)100U)   /*!< Time out for HSE start up, in ms */
//#endif /* HSE_STARTUP_TIMEOUT */

///**
//  * @brief Internal High Speed oscillator (HSI) value.
//  *        This value is used by the RCC HAL module to compute the system frequency
//  *        (when HSI is used as system clock source, directly or through the PLL). 
//  */
//#if !defined  (HSI_VALUE)
//  #define HSI_VALUE    ((uint32_t)16000000U) /*!< Value of the Internal oscillator in Hz*/
//#endif /* HSI_VALUE */

///**
//  * @brief Internal Low Speed oscillator (LSI) value.
//  */
//#if !defined  (LSI_VALUE) 
// #define LSI_VALUE  ((uint32_t)32000U)       /*!< LSI Typical Value in Hz*/
//#endif /* LSI_VALUE */                      /*!< Value of the Internal Low Speed oscillator in Hz
//                                             The real value may vary depending on the variations
//                                             in voltage and temperature.*/
///**
//  * @brief External Low Speed oscillator (LSE) value.
//  */
//#if !defined  (LSE_VALUE)
// #define LSE_VALUE  ((uint32_t)32768U)    /*!< Value of the External Low Speed oscillator in Hz */
//#endif /* LSE_VALUE */

//#if !defined  (LSE_STARTUP_TIMEOUT)
//  #define LSE_STARTUP_TIMEOUT    ((uint32_t)5000U)   /*!< Time out for LSE start up, in ms */
//#endif /* LSE_STARTUP_TIMEOUT */

///**
//  * @brief External clock source for I2S peripheral
//  *        This value is used by the I2S HAL module to compute the I2S clock source 
//  *        frequency, this source is inserted directly through I2S_CKIN pad. 
//  */
//#if !defined  (EXTERNAL_CLOCK_VALUE)
//  #define EXTERNAL_CLOCK_VALUE    ((uint32_t)12288000U) /*!< Value of the External audio frequency in Hz*/
//#endif /* EXTERNAL_CLOCK_VALUE */

///* Tip: To avoid modifying this file each time you need to use different HSE,
//   ===  you can define the HSE value in your toolchain compiler preprocessor. */

/* ########################### System Configuration ######################### */
/**
  * @brief This is the HAL system configuration section
  */
#define  VDD_VALUE		      ((uint32_t)3300U) /*!< Value of VDD in mv */           
#define  TICK_INT_PRIORITY            ((uint32_t)0U)   /*!< tick interrupt priority */            
#define  USE_RTOS                     0U     
#define  PREFETCH_ENABLE              1U
#define  INSTRUCTION_CACHE_ENABLE     1U
#define  DATA_CACHE_ENABLE            1U

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* ################## Ethernet peripheral configuration ##################### */

/* Section 1 : Ethernet peripheral configuration */

/* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0   2U
#define MAC_ADDR1   0U
#define MAC_ADDR2   0U
#define MAC_ADDR3   0U
#define MAC_ADDR4   0U
#define MAC_ADDR5   0U

/* Definition of the Ethernet driver buffers size and count */   
#define ETH_RX_BUF_SIZE                ETH_MAX_PACKET_SIZE /* buffer size for receive               */
#define ETH_TX_BUF_SIZE                ETH_MAX_PACKET_SIZE /* buffer size for transmit              */
#define ETH_RXBUFNB                    ((uint32_t)4U)       /* 4 Rx buffers of size ETH_RX_BUF_SIZE  */
#define ETH_TXBUFNB                    ((uint32_t)4U)       /* 4 Tx buffers of size ETH_TX_BUF_SIZE  */

/* Section 2: PHY configuration section */

/* DP83848 PHY Address*/ 
#define DP83848_PHY_ADDRESS             0x01U
/* PHY Reset delay these values are based on a 1 ms Systick interrupt*/ 
#define PHY_RESET_DELAY                 ((uint32_t)0x000000FFU)
/* PHY Configuration delay */
#define PHY_CONFIG_DELAY                ((uint32_t)0x00000FFFU)

#define PHY_READ_TO                     ((uint32_t)0x0000FFFFU)
#define PHY_WRITE_TO                    ((uint32_t)0x0000FFFFU)

/* Section 3: Common PHY Registers */

#define PHY_BCR                         ((uint16_t)0x0000U)    /*!< Transceiver Basic Control Register   */
#define PHY_BSR                         ((uint16_t)0x0001U)    /*!< Transceiver Basic Status Register    */
 
#define PHY_RESET                       ((uint16_t)0x8000U)  /*!< PHY Reset */
#define PHY_LOOPBACK                    ((uint16_t)0x4000U)  /*!< Select loop-back mode */
#define PHY_FULLDUPLEX_100M             ((uint16_t)0x2100U)  /*!< Set the full-duplex mode at 100 Mb/s */
#define PHY_HALFDUPLEX_100M             ((uint16_t)0x2000U)  /*!< Set the half-duplex mode at 100 Mb/s */
#define PHY_FULLDUPLEX_10M              ((uint16_t)0x0100U)  /*!< Set the full-duplex mode at 10 Mb/s  */
#define PHY_HALFDUPLEX_10M              ((uint16_t)0x0000U)  /*!< Set the half-duplex mode at 10 Mb/s  */
#define PHY_AUTONEGOTIATION             ((uint16_t)0x1000U)  /*!< Enable auto-negotiation function     */
#define PHY_RESTART_AUTONEGOTIATION     ((uint16_t)0x0200U)  /*!< Restart auto-negotiation function    */
#define PHY_POWERDOWN                   ((uint16_t)0x0800U)  /*!< Select the power down mode           */
#define PHY_ISOLATE                     ((uint16_t)0x0400U)  /*!< Isolate PHY from MII                 */

#define PHY_AUTONEGO_COMPLETE           ((uint16_t)0x0020U)  /*!< Auto-Negotiation process completed   */
#define PHY_LINKED_STATUS               ((uint16_t)0x0004U)  /*!< Valid link established               */
#define PHY_JABBER_DETECTION            ((uint16_t)0x0002U)  /*!< Jabber condition detected            */
  
/* Section 4: Extended PHY Registers */

#define PHY_SR                          ((uint16_t)0x0010U)    /*!< PHY status register Offset                      */
#define PHY_MICR                        ((uint16_t)0x0011U)    /*!< MII Interrupt Control Register                  */
#define PHY_MISR                        ((uint16_t)0x0012U)    /*!< MII Interrupt Status and Misc. Control Register */
 
#define PHY_LINK_STATUS                 ((uint16_t)0x0001U)  /*!< PHY Link mask                                   */
#define PHY_SPEED_STATUS                ((uint16_t)0x0002U)  /*!< PHY Speed mask                                  */
#define PHY_DUPLEX_STATUS               ((uint16_t)0x0004U)  /*!< PHY Duplex mask                                 */

#define PHY_MICR_INT_EN                 ((uint16_t)0x0002U)  /*!< PHY Enable interrupts                           */
#define PHY_MICR_INT_OE                 ((uint16_t)0x0001U)  /*!< PHY Enable output interrupt events              */

#define PHY_MISR_LINK_INT_EN            ((uint16_t)0x0020U)  /*!< Enable Interrupt on change of link status       */
#define PHY_LINK_INTERRUPT              ((uint16_t)0x2000U)  /*!< PHY link status interrupt mask                  */

/* ################## SPI peripheral configuration ########################## */

/* CRC FEATURE: Use to activate CRC feature inside HAL SPI Driver
* Activated: CRC code is present inside driver
* Deactivated: CRC code cleaned from driver
*/

#define USE_SPI_CRC                     0U

/* Includes ------------------------------------------------------------------*/
/**
  * @brief Include module's header file 
  */

//#ifdef HAL_RCC_MODULE_ENABLED
//  #include "stm32f4xx_hal_rcc.h"
//#endif /* HAL_RCC_MODULE_ENABLED */

//#ifdef HAL_GPIO_MODULE_ENABLED
//  #include "stm32f4xx_hal_gpio.h"
//#endif /* HAL_GPIO_MODULE_ENABLED */

//#ifdef HAL_DMA_MODULE_ENABLED
//  #include "stm32f4xx_hal_dma.h"
//#endif /* HAL_DMA_MODULE_ENABLED */
//   
//#ifdef HAL_CORTEX_MODULE_ENABLED
//  #include "stm32f4xx_hal_cortex.h"
//#endif /* HAL_CORTEX_MODULE_ENABLED */

//#ifdef HAL_ADC_MODULE_ENABLED
//  #include "stm32f4xx_hal_adc.h"
//#endif /* HAL_ADC_MODULE_ENABLED */

//#ifdef HAL_CAN_MODULE_ENABLED
//  #include "stm32f4xx_hal_can.h"
//#endif /* HAL_CAN_MODULE_ENABLED */

//#ifdef HAL_CRC_MODULE_ENABLED
//  #include "stm32f4xx_hal_crc.h"
//#endif /* HAL_CRC_MODULE_ENABLED */

//#ifdef HAL_CRYP_MODULE_ENABLED
//  #include "stm32f4xx_hal_cryp.h" 
//#endif /* HAL_CRYP_MODULE_ENABLED */

//#ifdef HAL_DMA2D_MODULE_ENABLED
//  #include "stm32f4xx_hal_dma2d.h"
//#endif /* HAL_DMA2D_MODULE_ENABLED */

//#ifdef HAL_DAC_MODULE_ENABLED
//  #include "stm32f4xx_hal_dac.h"
//#endif /* HAL_DAC_MODULE_ENABLED */

//#ifdef HAL_DCMI_MODULE_ENABLED
//  #include "stm32f4xx_hal_dcmi.h"
//#endif /* HAL_DCMI_MODULE_ENABLED */

//#ifdef HAL_ETH_MODULE_ENABLED
//  #include "stm32f4xx_hal_eth.h"
//#endif /* HAL_ETH_MODULE_ENABLED */

//#ifdef HAL_FLASH_MODULE_ENABLED
//  #include "stm32f4xx_hal_flash.h"
//#endif /* HAL_FLASH_MODULE_ENABLED */
// 
//#ifdef HAL_SRAM_MODULE_ENABLED
//  #include "stm32f4xx_hal_sram.h"
//#endif /* HAL_SRAM_MODULE_ENABLED */

//#ifdef HAL_NOR_MODULE_ENABLED
//  #include "stm32f4xx_hal_nor.h"
//#endif /* HAL_NOR_MODULE_ENABLED */

//#ifdef HAL_NAND_MODULE_ENABLED
//  #include "stm32f4xx_hal_nand.h"
//#endif /* HAL_NAND_MODULE_ENABLED */

//#ifdef HAL_PCCARD_MODULE_ENABLED
//  #include "stm32f4xx_hal_pccard.h"
//#endif /* HAL_PCCARD_MODULE_ENABLED */ 
//  
//#ifdef HAL_SDRAM_MODULE_ENABLED
//  #include "stm32f4xx_hal_sdram.h"
//#endif /* HAL_SDRAM_MODULE_ENABLED */      

//#ifdef HAL_HASH_MODULE_ENABLED
// #include "stm32f4xx_hal_hash.h"
//#endif /* HAL_HASH_MODULE_ENABLED */

//#ifdef HAL_I2C_MODULE_ENABLED
// #include "stm32f4xx_hal_i2c.h"
//#endif /* HAL_I2C_MODULE_ENABLED */

//#ifdef HAL_I2S_MODULE_ENABLED
// #include "stm32f4xx_hal_i2s.h"
//#endif /* HAL_I2S_MODULE_ENABLED */

//#ifdef HAL_IWDG_MODULE_ENABLED
// #include "stm32f4xx_hal_iwdg.h"
//#endif /* HAL_IWDG_MODULE_ENABLED */

//#ifdef HAL_LTDC_MODULE_ENABLED
// #include "stm32f4xx_hal_ltdc.h"
//#endif /* HAL_LTDC_MODULE_ENABLED */

//#ifdef HAL_PWR_MODULE_ENABLED
// #include "stm32f4xx_hal_pwr.h"
//#endif /* HAL_PWR_MODULE_ENABLED */

//#ifdef HAL_RNG_MODULE_ENABLED
// #include "stm32f4xx_hal_rng.h"
//#endif /* HAL_RNG_MODULE_ENABLED */

//#ifdef HAL_RTC_MODULE_ENABLED
// #include "stm32f4xx_hal_rtc.h"
//#endif /* HAL_RTC_MODULE_ENABLED */

//#ifdef HAL_SAI_MODULE_ENABLED
// #include "stm32f4xx_hal_sai.h"
//#endif /* HAL_SAI_MODULE_ENABLED */

//#ifdef HAL_SD_MODULE_ENABLED
// #include "stm32f4xx_hal_sd.h"
//#endif /* HAL_SD_MODULE_ENABLED */

//#ifdef HAL_SPI_MODULE_ENABLED
// #include "stm32f4xx_hal_spi.h"
//#endif /* HAL_SPI_MODULE_ENABLED */

//#ifdef HAL_TIM_MODULE_ENABLED
// #include "stm32f4xx_hal_tim.h"
//#endif /* HAL_TIM_MODULE_ENABLED */

//#ifdef HAL_UART_MODULE_ENABLED
// #include "stm32f4xx_hal_uart.h"
//#endif /* HAL_UART_MODULE_ENABLED */

//#ifdef HAL_USART_MODULE_ENABLED
// #include "stm32f4xx_hal_usart.h"
//#endif /* HAL_USART_MODULE_ENABLED */

//#ifdef HAL_IRDA_MODULE_ENABLED
// #include "stm32f4xx_hal_irda.h"
//#endif /* HAL_IRDA_MODULE_ENABLED */

//#ifdef HAL_SMARTCARD_MODULE_ENABLED
// #include "stm32f4xx_hal_smartcard.h"
//#endif /* HAL_SMARTCARD_MODULE_ENABLED */

//#ifdef HAL_WWDG_MODULE_ENABLED
// #include "stm32f4xx_hal_wwdg.h"
//#endif /* HAL_WWDG_MODULE_ENABLED */

//#ifdef HAL_PCD_MODULE_ENABLED
// #include "stm32f4xx_hal_pcd.h"
//#endif /* HAL_PCD_MODULE_ENABLED */

//#ifdef HAL_HCD_MODULE_ENABLED
// #include "stm32f4xx_hal_hcd.h"
//#endif /* HAL_HCD_MODULE_ENABLED */
//   
//#ifdef HAL_DSI_MODULE_ENABLED
// #include "stm32f4xx_hal_dsi.h"
//#endif /* HAL_DSI_MODULE_ENABLED */

//#ifdef HAL_QSPI_MODULE_ENABLED
// #include "stm32f4xx_hal_qspi.h"
//#endif /* HAL_QSPI_MODULE_ENABLED */

//#ifdef HAL_CEC_MODULE_ENABLED
// #include "stm32f4xx_hal_cec.h"
//#endif /* HAL_CEC_MODULE_ENABLED */

//#ifdef HAL_FMPI2C_MODULE_ENABLED
// #include "stm32f4xx_hal_fmpi2c.h"
//#endif /* HAL_FMPI2C_MODULE_ENABLED */

//#ifdef HAL_SPDIFRX_MODULE_ENABLED
// #include "stm32f4xx_hal_spdifrx.h"
//#endif /* HAL_SPDIFRX_MODULE_ENABLED */

//#ifdef HAL_LPTIM_MODULE_ENABLED
// #include "stm32f4xx_hal_lptim.h"
//#endif /* HAL_LPTIM_MODULE_ENABLED */
   
/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed. 
  *         If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */    

#ifdef __cplusplus
}
#endif



#endif
