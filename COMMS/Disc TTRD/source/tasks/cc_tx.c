#include "cc_tx.h"
#include "../support_functions/cw.h"


#if CC1101_UART
/***************** DMA for UART **************************/
// ------ Private variables -----------------------------
#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))	
#define CC_TX_SettingDelay 60
extern uint8_t CC_TX_Sent_Flag;
static uint8_t	CC_TX_Ready=1;
uint8_t CC_TX_SET_Counter=0;

static char CC_TX_Tx_buffer_g [CC_TX_TX_BUFFER_SIZE_BYTES];
static char CC_TX_Tx_buffer_ig[CC_TX_TX_BUFFER_SIZE_BYTES];  // Inverted copy

static char* CC_TX_Tx_buffer2_g = (char*)0x2001C000;
static char  CC_TX_Tx_buffer2_ig[CC_TX_TX_BUFFER_SIZE_BYTES];  // Inverted copy

static char CC_TX_Rx_buffer_g[CC_TX_RX_BUFFER_SIZE_BYTES];
static char CC_TX_Rx_buffer_ig[CC_TX_RX_BUFFER_SIZE_BYTES];  // Inverted copy
 
// Index of data in the buffer that has not yet been sent (with inv. copy)
static uint32_t CC_TX_Wait_idx_g  = 0;  
static uint32_t CC_TX_Wait_idx_ig = ~0; 

// buffer select to switch between the two buffers
static uint8_t CC_TX_Tx_buffer_select_g = 1;

// ------ Private function prototypes ----------------------------------------
void CC_TX_BUF_O_Check_Data_Integrity(void);

// ---------------------------------------------------------------------------

//Init
void CC_TX_init(uint32_t BAUD_RATE)
{
	CC_TX_BUF_O_Init(BAUD_RATE);
}
/*----------------------------------------------------------------------------*-

  CC_TX_BUF_O_Init()

  Set up CC_TX.

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
void CC_TX_BUF_O_Init(uint32_t BAUD_RATE)
{
	// Set up timeout mechanism
  TIMEOUT_T3_USEC_Init();
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef	 hdma_usart4_rx;
	DMA_InitTypeDef	 hdma_usart4_tx;
	// USART3 clock enable 
	RCC_APB1PeriphClockCmd(CC_TX_UART_RCC, ENABLE);

	// GPIOB clock enable 
	RCC_AHB1PeriphClockCmd(CC_TX_PORT_RCC, ENABLE);
	 
	/* DMA controller clock enable */
	RCC_AHB1PeriphClockCmd(CC_TX_DMA_RCC, ENABLE);
	// GPIO config
	GPIO_PinAFConfig(CC_TX_PORT, CC_TX_RX_PIN_SOURCE, CC_TX_AF);	
	GPIO_PinAFConfig(CC_TX_PORT, CC_TX_TX_PIN_SOURCE, CC_TX_AF);
	GPIO_InitStructure.GPIO_Pin   = CC_TX_TX_PIN|CC_TX_RX_PIN; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(CC_TX_PORT, &GPIO_InitStructure); 
	 // Set pin init 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed; 
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin   = CC_TX_SET_PIN;
	GPIO_Init(CC_TX_PORT, &GPIO_InitStructure); 

	GPIO_SetBits(CC_TX_PORT, CC_TX_SET_PIN);
	// USART3 configuration
	// - BaudRate as specified in function parameter
	// - Word Length = 8 Bits
	// - One Stop Bit
	// - No parity
	// - Hardware flow control disabled (RTS and CTS signals)
	// - Tx & Rx enabled

	USART_InitStructure.USART_BaudRate = BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(CC_TX_UARTX, &USART_InitStructure);
	
	//DMA RX init
	DMA_StructInit(&hdma_usart4_rx);
	hdma_usart4_rx.DMA_Channel = CC_TX_DMA_Channel;
	hdma_usart4_rx.DMA_PeripheralBaseAddr = (uint32_t) (&(CC_TX_UARTX->DR));; 
	hdma_usart4_rx.DMA_Memory0BaseAddr = (uint32_t)CC_TX_Rx_buffer_g;  //
	hdma_usart4_rx.DMA_DIR = DMA_DIR_PeripheralToMemory;
	hdma_usart4_rx.DMA_BufferSize = CC_TX_RX_BUFFER_SIZE_BYTES;
	hdma_usart4_rx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	hdma_usart4_rx.DMA_MemoryInc = DMA_MemoryInc_Enable;
	hdma_usart4_rx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	hdma_usart4_rx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	hdma_usart4_rx.DMA_Mode = DMA_Mode_Circular;
	hdma_usart4_rx.DMA_Priority = DMA_Priority_Low;
	hdma_usart4_rx.DMA_FIFOMode = DMA_FIFOMode_Disable;
	hdma_usart4_rx.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	hdma_usart4_rx.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	hdma_usart4_rx.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(CC_TX_DMA_RX_STREAM, &hdma_usart4_rx);
	
	//DMA TX init 
 DMA_StructInit(&hdma_usart4_tx); 
 hdma_usart4_tx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
 hdma_usart4_tx.DMA_MemoryInc = DMA_MemoryInc_Enable;
 hdma_usart4_tx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
 hdma_usart4_tx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
 hdma_usart4_tx.DMA_Priority = DMA_Priority_VeryHigh;
 hdma_usart4_tx.DMA_FIFOMode = DMA_FIFOMode_Disable;
 hdma_usart4_tx.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;//DMA_FIFOThreshold_1QuarterFull
 hdma_usart4_tx.DMA_MemoryBurst = DMA_MemoryBurst_Single;
 hdma_usart4_tx.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
 
 hdma_usart4_tx.DMA_Channel = CC_TX_DMA_Channel;
 hdma_usart4_tx.DMA_PeripheralBaseAddr = (uint32_t) (&(CC_TX_UARTX->DR)); 
 hdma_usart4_tx.DMA_Memory0BaseAddr = (uint32_t)CC_TX_Rx_buffer_g;
 hdma_usart4_tx.DMA_DIR = DMA_DIR_MemoryToPeripheral;
 hdma_usart4_tx.DMA_BufferSize = CC_TX_TX_BUFFER_SIZE_BYTES+1;
 hdma_usart4_tx.DMA_Mode = DMA_Mode_Normal;
 DMA_Init(CC_TX_DMA_TX_STREAM, &hdma_usart4_tx);

   // Enable UART4
   USART_Cmd(CC_TX_UARTX, ENABLE);
	 
	 
//   Enable USART DMA TX Requsts 
	 USART_DMACmd(CC_TX_UARTX, USART_DMAReq_Tx, ENABLE);
//	 while(!DMA_GetCmdStatus(CC_TX_DMA_TX_STREAM))
//	 {
//	 };
	 
	 // Enable USART DMA RX Requsts
   DMA_Cmd (CC_TX_DMA_RX_STREAM, ENABLE);
	 USART_DMACmd(CC_TX_UARTX, USART_DMAReq_Rx, ENABLE);
	 while(!DMA_GetCmdStatus(CC_TX_DMA_RX_STREAM))
	 {
	 };

	// Configure buffer
	for (uint32_t i = 0; i < CC_TX_TX_BUFFER_SIZE_BYTES; i++)
		{
		CC_TX_Tx_buffer_g[i] = 'X';
		CC_TX_Tx_buffer2_g[i] = 'G';
			
		CC_TX_Tx_buffer_ig[i] = (char) ~'X'; 
		CC_TX_Tx_buffer2_ig[i] =(char) ~'G';
		}
//	for (uint32_t i = 0; i < CC_TX_RX_BUFFER_SIZE_BYTES; i++)
//		{
//		CC_TX_Rx_buffer_g[i] = 'K';
//			
//		CC_TX_Rx_buffer_ig[i] = (char) ~'K'; 
//		}
	// Store the UART register configuration
	REG_CONFIG_CHECKS_UART_Store(UART4);
		
}
// update
uint32_t CC_TX_update(void)
{
	uint32_t Return_value = RETURN_NORMAL_STATE;
	if(CC_TX_Ready)
	{
		CC_TX_DMA_CHECK();
		CC_TX_SET_Parameters();
		CC_TX_BUF_O_Update();
	}
	else
	{// need here to make init function for automatic setting values
		//CC_RX
	}		
	return Return_value;
}	
/*----------------------------------------------------------------------------*-

  CC_TX_BUF_O_Update()

  Sends next character from the software transmit buffer

  NOTE: Output-only library (Cannot receive chars)

  Uses on-chip UART hardware.

  PARAMETERS:
     None.
     Wait_idx_g  (W)  
     Wait_idx_ig (W) 

  MCU HARDWARE:
     UART2.

  PRE-CONDITION CHECKS:
     CC_TX_BUF_O_Check_Data_Integrity() is called.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  WCET:
     Not yet determined.

  BCET:
     Not yet determined.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void CC_TX_BUF_O_Update(void)
{
	if(CC_TX_Wait_idx_g > 0)
	{  
		CC_TX_BUF_O_Send_All_Data();
	}  
	
}
// Setters
int32_t  CC_TX_data_packet(const uint8_t *data, size_t size)
{
	const uint8_t* d = data;
	for ( ;size > 0;size--, ++d)
	{
		CC_TX_BUF_O_Write_Char_To_Buffer(*d);
	}
	return COMMS_STATUS_OK;
}
int32_t  CC_TX_data_packet_cw(const cw_pulse_t *in, size_t len)
{
//	const cw_pulse_t* d = data;
//	for ( ;size > 0;size--, ++d)
	{
//		CC_TX_BUF_O_Write_Char_To_Buffer(*d);
	}
	return COMMS_STATUS_OK;
}

void CC_TX_Clear_Command(void)
{
	CC_TX_Sent_Flag=1;
	CC_TX_SP_Command[0]='\0';
	CC_TX_SP_Command[1]='\0';
	CC_TX_SP_Command[2]='\0';
	CC_TX_SP_Command[3]='\0';
	CC_TX_SP_Command[4]='\0';
}
void  	 CC_TX_SET_Parameters(void)
{
	if(CC_TX_SP_Flag == 1)
	{
		GPIO_ResetBits(CC_TX_PORT, CC_TX_SET_PIN);
		if((CC_TX_SET_Counter>=CC_TX_SettingDelay)&&(!CC_TX_Sent_Flag))
		{
			switch (CC_TX_SP_Command[3])
			{
				case 'A':
					UART2_BUF_O_Write_String_To_Buffer("TX:in A case\n");
					if(((CC_TX_SP_Command[4]>='0')&&(CC_TX_SP_Command[4]<='9'))&&((CC_TX_SP_Command[5]>='0')&&(CC_TX_SP_Command[5]<='9'))&&((CC_TX_SP_Command[6]>='0')&&(CC_TX_SP_Command[6]<='9')))
					{
						CC_TX_BUF_O_Write_String_To_Buffer(CC_TX_SP_Command);
					}
					else
					{
						UART2_BUF_O_Write_String_To_Buffer("TX:Wrong input:\n");
					}					
					CC_TX_Clear_Command();
					break;
				case 'B':
					UART2_BUF_O_Write_String_To_Buffer("TX:in B case\n");
					if(((CC_TX_SP_Command[4]>='0')&&(CC_TX_SP_Command[4]<='9'))&&((CC_TX_SP_Command[5]>='0')&&(CC_TX_SP_Command[5]<='9'))&&((CC_TX_SP_Command[6]>='0')&&(CC_TX_SP_Command[6]<='9'))&&((CC_TX_SP_Command[7]>='0')&&(CC_TX_SP_Command[7]<='9')))
					{
						CC_TX_BUF_O_Write_String_To_Buffer(CC_TX_SP_Command);
					}
					else
					{
						UART2_BUF_O_Write_String_To_Buffer("TX:Wrong input:\n");
					}	
					CC_TX_Clear_Command();
					break;
				case 'C':
					UART2_BUF_O_Write_String_To_Buffer("TX:in C case\n");
					if(((CC_TX_SP_Command[4]>='0')&&(CC_TX_SP_Command[4]<='1'))&&((CC_TX_SP_Command[5]>='0')&&(CC_TX_SP_Command[5]<='2'))&&((CC_TX_SP_Command[6]>='0')&&(CC_TX_SP_Command[6]<='7')))
					{
						CC_TX_BUF_O_Write_String_To_Buffer(CC_TX_SP_Command);
					}
					else
					{
						UART2_BUF_O_Write_String_To_Buffer("TX:Wrong input:\n");
					}
					CC_TX_Clear_Command();				
					break;
			case 'F':
					UART2_BUF_O_Write_String_To_Buffer("TX:in F case\n");
					if((CC_TX_SP_Command[4]>='U')&&((CC_TX_SP_Command[5]>='1')&&(CC_TX_SP_Command[5]<='4')))
					{
						CC_TX_BUF_O_Write_String_To_Buffer(CC_TX_SP_Command);
					}
					else
					{
						UART2_BUF_O_Write_String_To_Buffer("TX:Wrong input:\n");
					}
					CC_TX_Clear_Command();
					break;
			case 'P':
					UART2_BUF_O_Write_String_To_Buffer("TX:in P case\n");
					if((CC_TX_SP_Command[4]>='1')&&(CC_TX_SP_Command[4]<='8'))
					{
						CC_TX_BUF_O_Write_String_To_Buffer(CC_TX_SP_Command);
					}
					else
					{
						UART2_BUF_O_Write_String_To_Buffer("TX:Wrong input:\n");
					}
					CC_TX_Clear_Command();
					break;
			case 'R':
					UART2_BUF_O_Write_String_To_Buffer("TX:in R case\n");
					if((CC_TX_SP_Command[4]=='A')||(CC_TX_SP_Command[4]=='B')||(CC_TX_SP_Command[4]=='C')||(CC_TX_SP_Command[4]=='F')||(CC_TX_SP_Command[4]=='P')||(CC_TX_SP_Command[4]=='X'))
					{
						CC_TX_BUF_O_Write_String_To_Buffer(CC_TX_SP_Command);
					}
					else if((CC_TX_SP_Command[4]=='E')&&(CC_TX_SP_Command[5]=='E')&&(CC_TX_SP_Command[6]=='S')&&(CC_TX_SP_Command[7]=='E')&&(CC_TX_SP_Command[8]=='T'))
					{
						CC_TX_BUF_O_Write_String_To_Buffer(CC_TX_SP_Command);
					}
					else	
					{
						UART2_BUF_O_Write_String_To_Buffer("TX:Wrong input:\n");
					}
					CC_TX_Clear_Command();
					break;
			case 'U':
					UART2_BUF_O_Write_String_To_Buffer("TX:in U case\n");
					if(((CC_TX_SP_Command[4]=='8')||(CC_TX_SP_Command[4]=='9'))&&((CC_TX_SP_Command[5]=='O')||(CC_TX_SP_Command[5]=='E'||(CC_TX_SP_Command[5]=='N')))&&((CC_TX_SP_Command[6]=='1')||(CC_TX_SP_Command[6]=='2')))
					{
						CC_TX_BUF_O_Write_String_To_Buffer(CC_TX_SP_Command);
					}
					else
					{
						UART2_BUF_O_Write_String_To_Buffer("TX:Wrong input:\n");
					}
					CC_TX_Clear_Command();
					break;
			case 'V':
					UART2_BUF_O_Write_String_To_Buffer("TX:in V case\n");
					CC_TX_BUF_O_Write_String_To_Buffer(CC_TX_SP_Command);
					CC_TX_Clear_Command();

					break;
			case 'S':
					UART2_BUF_O_Write_String_To_Buffer("TX:in S case\n");
					if((CC_TX_SP_Command[4]=='L')&&(CC_TX_SP_Command[5]=='E')&&(CC_TX_SP_Command[6]=='E')&&(CC_TX_SP_Command[7]=='P'))
					{
						CC_TX_BUF_O_Write_String_To_Buffer(CC_TX_SP_Command);
					}
					else
					{
						UART2_BUF_O_Write_String_To_Buffer("TX:Wrong input:\n");
					}
					CC_TX_Clear_Command();
					break;
			case 'E':
				UART2_BUF_O_Write_String_To_Buffer("TX:in E case\n");
				if((CC_TX_SP_Command[4]=='X')&&(CC_TX_SP_Command[5]=='I')&&(CC_TX_SP_Command[6]=='T'))
				{
					GPIO_SetBits(CC_TX_PORT, CC_TX_SET_PIN);
					CC_TX_SET_Counter=0;
					CC_TX_SP_Flag = 0;
					CC_TX_Clear_Command();
				}	
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("TX:Wrong input:\n");
				}
				CC_TX_Clear_Command();
				break;
			default:
				if((CC_TX_SP_Command[0]=='A')&&(CC_TX_SP_Command[1]=='T')&&(CC_TX_SP_Command[2]=='*'))
				{
					UART2_BUF_O_Write_String_To_Buffer("TX:in AT case\n");
					CC_TX_BUF_O_Write_String_To_Buffer("AT\n");
				}
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("TX:SCWI: Echo Mode\n");
					for (int i=0; i > 10;i++)
					{
					UART2_BUF_O_Write_Char_To_Buffer(CC_TX_SP_Command[i]);
					if(CC_TX_SP_Command[i]=='*')
					break;
					}
					UART2_BUF_O_Write_Char_To_Buffer('\n');
				}
				CC_TX_Clear_Command();
				break;		
			}
		}
		else if(CC_TX_SET_Counter< CC_TX_SettingDelay)
		{
			CC_TX_SET_Counter++;
			if(CC_TX_SET_Counter==CC_TX_SettingDelay/2)
			{
				CC_TX_BUF_O_Write_String_To_Buffer("AT\n");
			}
		}
	}
}

/*----------------------------------------------------------------------------*-

  CC_TX_BUF_O_Check_Data_Integrity()

  Check data integrity.

  PARAMETER:
     None.

  LONG-TERM DATA:
     CC_TX_Tx_buffer_g[]  (R)
     CC_TX_Tx_buffer_ig[] (R)   
     CC_TX_Wait_idx_g     (R)  
     CC_TX_Wait_idx_ig    (R) 

  MCU HARDWARE:
     CC_RX.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     Data integrity and register checks.
     Forces shutdown in the event of failure.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void CC_TX_BUF_O_Check_Data_Integrity(void)
{
	// Check the UART register configuration
	REG_CONFIG_CHECKS_UART_Check(UART4);  

	// Check integrity of 'waiting' index
	if ((CC_TX_Wait_idx_g != (~CC_TX_Wait_idx_ig)))
		{
		// Data have been corrupted: Treat here as Fatal Platform Failure
		PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
		}          

	// Check integrity of data in Tx buffer
	for (uint32_t i = 0; i < CC_TX_TX_BUFFER_SIZE_BYTES; i++)  
		{
		if ((CC_TX_Tx_buffer_g[i] != ((char) ~CC_TX_Tx_buffer_ig[i])))
			 {
			 // Data have been corrupted: Treat here as Fatal Platform Failure
			 PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
			 }            
		}
}
	 /**----------------------------------------------------------------------------*-
 * \brief           Check for new data received with DMA
 *
 * Not doing reads fast enough may cause DMA to overflow unread received bytes,
 * hence application will lost useful data.
 *
 * Solutions to this are:
 * - Improve architecture design to achieve faster reads
 * - Increase raw buffer size and allow DMA to write more data before this function is called
 */
void CC_TX_DMA_CHECK(void)
{
static uint32_t old_pos=0;
uint32_t pos=0;
//__HAL_DMA_GET_COUNTER(Sucess) IS_DMA_BUFFER_SIZE  IS_DMA_PERIPHERAL_DATA_SIZE  IS_DMA_MEMORY_DATA_SIZE
pos = ARRAY_LEN(CC_TX_Rx_buffer_g) - DMA_GetCurrDataCounter(CC_TX_DMA_RX_STREAM);
if (pos>=CC_TX_RX_BUFFER_SIZE_BYTES)//CC_TX_RX_BUFFER_SIZE_BYTES
 pos=0;  
 if (pos != old_pos) 
	 {                       /* Check change in received data */
	if (pos > old_pos) 
		{                    /* Current position is over previous one */
			/*
			 * Processing is done in "linear" mode.
			 *
			 * Application processing is fast with single data block,
			 * length is simply calculated by subtracting pointers
			 *
			 * [   0   ]
			 * [   1   ] <- old_pos |------------------------------------|
			 * [   2   ]            |                                    |
			 * [   3   ]            | Single block (len = pos - old_pos) |
			 * [   4   ]            |                                    |
			 * [   5   ]            |------------------------------------|
			 * [   6   ] <- pos
			 * [   7   ]
			 * [ N - 1 ]
			 */
			CC_TX_PROCESS_DATA(&CC_TX_Rx_buffer_g[old_pos], pos - old_pos);
		} 
	else 
		{
			/*
			 * Processing is done in "overflow" mode..
			 *
			 * Application must process data twice,
			 * since there are 2 linear memory blocks to handle
			 *
			 * [   0   ]            |---------------------------------|
			 * [   1   ]            | Second block (len = pos)        |
			 * [   2   ]            |---------------------------------|
			 * [   3   ] <- pos
			 * [   4   ] <- old_pos |---------------------------------|
			 * [   5   ]            |                                 |
			 * [   6   ]            | First block (len = N - old_pos) |
			 * [   7   ]            |                                 |
			 * [ N - 1 ]            |---------------------------------|
			 */
			CC_TX_PROCESS_DATA(&CC_TX_Rx_buffer_g[old_pos], ARRAY_LEN(CC_TX_Rx_buffer_g) - old_pos);
			if (pos > 0) 
				{
					CC_TX_PROCESS_DATA(&CC_TX_Rx_buffer_g[0], pos);
				}
		}
		old_pos = pos;                          /* Save current position as old for next transfers */
	}
}
/**
 * \brief           Process received data over UART
 * \note            Either process them directly or copy to other bigger buffer
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
void CC_TX_PROCESS_DATA(const void* data, size_t len)
{
	 const uint8_t* d = data;

   for ( ;len > 0;len--, ++d)
		{
		UART2_BUF_O_Write_Char_To_Buffer(*d);
		}
}
/*----------------------------------------------------------------------------*-
   
  CC_TX_BUF_O_Write_String_To_Buffer()

  Copies a (null terminated) string to the module Tx buffer.  

  PARAMETERS:
     None.

  LONG-TERM DATA:
     None.

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void CC_TX_BUF_O_Write_String_To_Buffer(const char* const STR_PTR)
{
 uint32_t i = 0;
 while ((STR_PTR[i] != '\0')&&(STR_PTR[i] != '*'))
	{
		CC_TX_BUF_O_Write_Char_To_Buffer(STR_PTR[i]);
		i++;
	}
}
void CC_TX_BUF_O_Write_Frame_To_Buffer(const void* data, size_t len)
{
	 const uint8_t* d = data;
   for ( ;len > 0;len--, ++d)
	{
//		if(*d==0x7E)
//			continue;
		CC_TX_BUF_O_Write_Char_To_Buffer(*d);
	}
}
/*----------------------------------------------------------------------------*-
   
  CC_TX_BUF_O_Write_Char_To_Buffer()

  Stores a single character in the Tx buffer.

  PARAMETERS:
     CHARACTER is character to be stored.

  LONG-TERM DATA:
     Tx_buffer_g[] (W)
     Tx_buffer_ig[] (W)  
     Wait_idx_g  (W)  
     Wait_idx_ig (W) 

  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void CC_TX_BUF_O_Write_Char_To_Buffer(const char CHARACTER)
{
	// Write to the buffer *only* if there is space
	if (CC_TX_Wait_idx_g < CC_TX_TX_BUFFER_SIZE_BYTES)
		{
			if( CC_TX_Tx_buffer_select_g == 1 )
			{
				CC_TX_Tx_buffer_g[CC_TX_Wait_idx_g] = CHARACTER;
				CC_TX_Tx_buffer_ig[CC_TX_Wait_idx_g] = (char) ~CHARACTER;
			}
			else
			{
				CC_TX_Tx_buffer2_g[CC_TX_Wait_idx_g] = CHARACTER;
				CC_TX_Tx_buffer2_ig[CC_TX_Wait_idx_g] = (char) ~CHARACTER;
			}
			CC_TX_Wait_idx_g++;     
		}
	else
		{
		// Write buffer is full
		// No error handling / reporting here (characters may be lost)
		// Adapt as required to meet the needs of your application
			UART2_BUF_O_Write_String_To_Buffer("Error inside CC_TX Write buffer is full\n");
		}

	// Update the copy
	CC_TX_Wait_idx_ig = ~CC_TX_Wait_idx_g;
}
/*----------------------------------------------------------------------------*-

  CC_TX_BUF_O_Send_All_Data()

  Sends all data from the software transmit buffer.

  NOTES: 
  * May have very long execution time!  
  * Intended for use when the scheduler is NOT running.

  PARAMETERS:
     None.

  LONG-TERM DATA:

     Wait_idx_g  (W)  
     Wait_idx_ig (W) 

  MCU HARDWARE:
     UART2.

  PRE-CONDITION CHECKS:
     CC_TX_BUF_O_Check_Data_Integrity() is called.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void CC_TX_BUF_O_Send_All_Data(void)
{
	uint32_t T3;
	// wait until the previous transfer is done
	if(CC_TX_DMA_TX_STREAM->NDTR == 0 || CC_TX_DMA_TX_STREAM->NDTR == CC_TX_TX_BUFFER_SIZE_BYTES+1)
	{
		// Check data integrity
		CC_TX_BUF_O_Check_Data_Integrity();
		// set the dma transfer data length
		CC_TX_DMA_TX_STREAM->NDTR = CC_TX_Wait_idx_g;
		// set memory base address to one of two buffers
		if( CC_TX_Tx_buffer_select_g == 1 )
		{
			CC_TX_DMA_TX_STREAM->M0AR = (uint32_t)CC_TX_Tx_buffer_g ;
			CC_TX_Tx_buffer_select_g = 2;
		}
		else
		{
			CC_TX_DMA_TX_STREAM->M0AR = (uint32_t)CC_TX_Tx_buffer2_g ;
			CC_TX_Tx_buffer_select_g = 1;
		}
		
		// Clear DMA Stream Flags// needs to be changed for every Stream
		DMA1->HIFCR |= (0x3D);
		
		// Clear USART Transfer Complete Flags
		USART_ClearFlag(CC_TX_UARTX,USART_FLAG_TC);  
		
		// Enable DMA USART TX Stream 
		DMA_Cmd(CC_TX_DMA_TX_STREAM,ENABLE);
		
		// wait until it's enabled
		// while( DMA_GetCmdStatus(DMA1_Stream6) == DISABLE );
		
		// Reset the indices
		CC_TX_Wait_idx_g = 0;
		CC_TX_Wait_idx_ig = ~0;
	}
}







#elif CC1120	 
	 
#elif CC1101_SPI
	
	
#endif	 








