// 'eps_obc_communication'
#include "eps_obc_communication.h"
#include "../main/main.h"
#include "../support_functions/ttrd2-05a-t0401a-v001a_timeout_t3.h"

// The transmit buffer length
#define uart_temp 200
// UART2 Tx interface
#define UART3_PORT         GPIOC
#define UART3_TX_PIN       GPIO_Pin_10
#define UART3_RX_PIN       GPIO_Pin_11

// ------ Private variables --------------------------------------------------

static char Tx_buffer_g[uart_temp];
static char Tx_buffer_ig[uart_temp];  // Inverted copy

// Index of data in the buffer that has been sent (with copy)
static uint32_t Sent_idx_g  = 0;  
static uint32_t Sent_idx_ig = ~0;  

// Index of data in the buffer that has not yet been sent (with inv. copy)
static uint32_t Wait_idx_g  = 0;  
static uint32_t Wait_idx_ig = ~0; 

// Timeout values (used when characters are transmitted)
static uint32_t Timeout_us_g;
static uint32_t Timeout_us_ig;
/** @addtogroup obc_wrapper_Functions
  * @{
  */

/**
  * @brief Initialize communication with obc. Must be called once before the while loop.
  * @param  void.
  * @retval Error status for handling and debugging.
  */
	// EPS_soft_error_status



/*----------------------------------------------------------------------------*-

  UART2_BUF_O_Init()

  Set up UART2.

  PARAMETER:
     Required baud rate.

  LONG-TERM DATA:
     Tx_buffer_g[] (W)
     Tx_buffer_ig[] (W)  
     Timeout_us_g (W)
     Timeout_us_ig (W)

  MCU HARDWARE:
     UART2.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void EPS_obc_communication_init(void){
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;

   // USART6 clock enable 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

   // GPIOA clock enable 
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

   // GPIO config
   GPIO_InitStructure.GPIO_Pin   = UART3_TX_PIN; 
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
   GPIO_Init(UART3_PORT, &GPIO_InitStructure);

   GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_USART3);

   // USART2 configuration
   // - BaudRate as specified in function parameter
   // - Word Length = 8 Bits
   // - One Stop Bit
   // - No parity
   // - Hardware flow control disabled (RTS and CTS signals)
   // - Tx &Rx enabled
   USART_InitStructure.USART_BaudRate = 9600;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode =USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART3, &USART_InitStructure);

   // Enable UART3
   USART_Cmd(USART3, ENABLE);
   
   // Set up timeout mechanism
   TIMEOUT_T3_USEC_Init();

   // Time taken to send one character at given baud rate (microseconds)
   // We add 10 usec leeway
   Timeout_us_g = (10000000 / 9600) + 10;
   Timeout_us_ig = ~Timeout_us_g;

   // Configure buffer
   for (uint32_t i = 0; i < uart_temp; i++)
      {
      Tx_buffer_g[i] = 'X';
      Tx_buffer_ig[i] = (char) ~'X'; 
      }

   // Store the UART register configuration
   REG_CONFIG_CHECKS_UART_Store(USART3);
   

	//pkt_pool_INIT();

	uint16_t size = 0;

	/*Uart inits*/
	//HAL_UART_Receive_IT(&huart3, eps_data.obc_uart.uart_buf, UART_BUF_SIZE);

//	return EPS_SOFT_ERROR_OBC_COMM_INIT_COMPLETE;

			
//EPS_obc_communication_service();

}
	