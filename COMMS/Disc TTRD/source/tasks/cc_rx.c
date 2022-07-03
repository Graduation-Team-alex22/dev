#include "cc_rx.h"

#if CC1101_UART
/***************** DMA for UART **************************/
// ------ Private variables -----------------------------
#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))	
#define CC_RX_SettingDelay 60
static 		uint8_t	CC_RX_Ready=1;
volatile 	uint8_t rx_sync_flag = 0;
uint8_t CC_RX_SET_Counter=0;
struct CC_RX_FLAGS_t CC_RX_FLAGS;


static char CC_RX_Tx_buffer_g [CC_RX_TX_BUFFER_SIZE_BYTES];
static char CC_RX_Tx_buffer_ig[CC_RX_TX_BUFFER_SIZE_BYTES];  // Inverted copy

static char CC_RX_Rx_buffer_g[CC_RX_RX_BUFFER_SIZE_BYTES];
static char CC_RX_Rx_buffer_ig[CC_RX_RX_BUFFER_SIZE_BYTES];  // Inverted copy

// Index of data in the buffer that has been sent (with copy)
static uint32_t CC_RX_Sent_idx_g  = 0;  
static uint32_t CC_RX_Sent_idx_ig = ~0;  

// Index of data in the buffer that has not yet been sent (with inv. copy)
static uint32_t CC_RX_Wait_idx_g  = 0;  
static uint32_t CC_RX_Wait_idx_ig = ~0; 

// Timeout values (used when characters are transmitted)
static uint32_t CC_RX_Timeout_us_g;
static uint32_t CC_RX_Timeout_us_ig;
// ------ Private function prototypes ----------------------------------------
void CC_RX_BUF_O_Send_Char(const char);
void CC_RX_BUF_O_Check_Data_Integrity(void);
//Init
void CC_RX_init(uint32_t BAUD_RATE)
{
	CC_RX_FLAGS.ID_Flag=1;
	CC_RX_BUF_O_Init(BAUD_RATE);
}
/*----------------------------------------------------------------------------*-

  CC_RX_BUF_O_Init()

  Set up CC_RX.

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
void CC_RX_BUF_O_Init(uint32_t BAUD_RATE)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef	 hdma_usart3_rx;
	// USART3 clock enable 
	RCC_APB1PeriphClockCmd(CC_RX_UART_RCC, ENABLE);

	// GPIOB clock enable 
	RCC_AHB1PeriphClockCmd(CC_RX_PORT_RCC, ENABLE);
	 
	// GPIOE clock enable 
	RCC_AHB1PeriphClockCmd(CC_RX_SET_PORT_RCC, ENABLE);
	
	/* DMA controller clock enable */
	RCC_AHB1PeriphClockCmd(CC_RX_DMA_RCC, ENABLE);
	// GPIO config
	GPIO_PinAFConfig(CC_RX_PORT, CC_RX_RX_PIN_SOURCE, CC_RX_AF);	
	GPIO_PinAFConfig(CC_RX_PORT, CC_RX_TX_PIN_SOURCE, CC_RX_AF);
	GPIO_InitStructure.GPIO_Pin   = CC_RX_TX_PIN|CC_RX_RX_PIN; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(CC_RX_PORT, &GPIO_InitStructure); 
	
	// Set pin init 

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed; 
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin   = CC_RX_SET_PIN;
	GPIO_Init(CC_RX_SET_PORT, &GPIO_InitStructure); 

	GPIO_SetBits(CC_RX_SET_PORT, CC_RX_SET_PIN);
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
	//USART_InitStructure.OverSampling = UART_OVERSAMPLING_16;	 
	USART_Init(CC_RX_UARTX, &USART_InitStructure);

	DMA_StructInit(&hdma_usart3_rx);
	hdma_usart3_rx.DMA_Channel = CC_RX_DMA_Channel;
	hdma_usart3_rx.DMA_PeripheralBaseAddr = (uint32_t) (&(CC_RX_UARTX->DR));; 
	hdma_usart3_rx.DMA_Memory0BaseAddr = (uint32_t)CC_RX_Rx_buffer_g;  //
	hdma_usart3_rx.DMA_DIR = DMA_DIR_PeripheralToMemory;
	hdma_usart3_rx.DMA_BufferSize = CC_RX_RX_BUFFER_SIZE_BYTES;
	hdma_usart3_rx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	hdma_usart3_rx.DMA_MemoryInc = DMA_MemoryInc_Enable;
	hdma_usart3_rx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	hdma_usart3_rx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	hdma_usart3_rx.DMA_Mode = DMA_Mode_Circular;
	hdma_usart3_rx.DMA_Priority = DMA_Priority_VeryHigh;
	hdma_usart3_rx.DMA_FIFOMode = DMA_FIFOMode_Disable;
	hdma_usart3_rx.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	hdma_usart3_rx.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	hdma_usart3_rx.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	//DMA_PeripheralBaseAddr,DMA_Memory0BaseAddr
	DMA_Init(CC_RX_DMA_RX_STREAM, &hdma_usart3_rx);

	// Enable CC_RX
	USART_Cmd(CC_RX_UARTX, ENABLE);
	DMA_Cmd (CC_RX_DMA_RX_STREAM, ENABLE);
	USART_DMACmd(CC_RX_UARTX, USART_DMAReq_Rx, ENABLE);

	while(!DMA_GetCmdStatus(CC_RX_DMA_RX_STREAM))
	{
	};

	// Configure buffer
	for (uint32_t i = 0; i < CC_RX_TX_BUFFER_SIZE_BYTES; i++)
		{
		CC_RX_Tx_buffer_g[i] = 'X';
		CC_RX_Tx_buffer_ig[i] = (char) ~'X'; 
		}
		
	// Store the UART register configuration
	REG_CONFIG_CHECKS_UART_Store(USART3);
		
}
 

// update 
uint32_t CC_RX_update(void)
{
	uint32_t Return_value = RETURN_NORMAL_STATE;
	if(CC_RX_Ready)
	{
		CC_RX_DMA_CHECK();
		CC_RX_SET_Parameters();
	}
	return Return_value;
}	

/*----------------------------------------------------------------------------*-

  CC_RX_BUF_O_Update()

  Sends next character from the software transmit buffer

  NOTE: Output-only library (Cannot receive chars)

  Uses on-chip UART hardware.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Sent_idx_g (W)  
     Sent_idx_ig (W)  
     Wait_idx_g  (W)  
     Wait_idx_ig (W) 

  MCU HARDWARE:
     UART2.

  PRE-CONDITION CHECKS:
     UART2_BUF_O_Check_Data_Integrity() is called.

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
uint32_t CC_RX_BUF_O_Update(void)
{
	uint32_t Return_value = RETURN_NORMAL_STATE;
	// Check data integrity
	CC_RX_BUF_O_Check_Data_Integrity();

	// Are there any data ready to send?
	if (CC_RX_Sent_idx_g < CC_RX_Wait_idx_g)
		{
		CC_RX_BUF_O_Send_Char(CC_RX_Tx_buffer_g[CC_RX_Sent_idx_g]);     

		CC_RX_Sent_idx_g++;
		}
	else
		{
		// No data to send - just reset the buffer index
		CC_RX_Wait_idx_g = 0;
		CC_RX_Sent_idx_g = 0;
		}

	// Update the copies
	CC_RX_Wait_idx_ig = ~CC_RX_Wait_idx_g;
	CC_RX_Sent_idx_ig = ~CC_RX_Sent_idx_g;
	return Return_value;
}
// Setters
int32_t  CC_RX_data_packet(uint8_t *out, size_t max_len)
{
	//UART2_BUF_O_Write_String_To_Buffer("CC_RX_data_packet\n");
	uint8_t* data;
	int32_t length;
	if(CC_RX_FLAGS.ID_Flag == 2)// frame recvd from 1
	{
		//UART2_BUF_O_Write_String_To_Buffer("\nrx_update1");
		length = CC_RX_FLAGS.EndPtr1 - CC_RX_FLAGS.StartPtr1+1;
		data = CC_RX_FLAGS.StartPtr1;
		
		
		// clearing the frame parameters
		CC_RX_FLAGS.End1_Flag=0;
		CC_RX_FLAGS.EndPtr1=0;
		CC_RX_FLAGS.Length1=0;
		CC_RX_FLAGS.Start1_Flag=0;
		CC_RX_FLAGS.StartPtr1=0;
	}
	else if(CC_RX_FLAGS.ID_Flag == 3)// frame recvd from 2
	{
		//UART2_BUF_O_Write_String_To_Buffer("\nrx_update2");
		length = CC_RX_FLAGS.EndPtr2 - CC_RX_FLAGS.StartPtr2+1;
		data = CC_RX_FLAGS.StartPtr2;


		// clearing the frame parameters
		CC_RX_FLAGS.Start2_Flag=0;
		CC_RX_FLAGS.End2_Flag=0;
		CC_RX_FLAGS.StartPtr2=0;
		CC_RX_FLAGS.EndPtr2=0;
		CC_RX_FLAGS.Length2=0;
	}
	else if (CC_RX_FLAGS.ID_Flag == 1)// frame recvd from 3
	{
		//UART2_BUF_O_Write_String_To_Buffer("\nrx_update3");
		length = CC_RX_FLAGS.EndPtr3 - CC_RX_FLAGS.StartPtr3+1;
		data = CC_RX_FLAGS.StartPtr3;
		
		// clearing the frame parameters
		CC_RX_FLAGS.Start3_Flag=0;
		CC_RX_FLAGS.End3_Flag=0;
		CC_RX_FLAGS.StartPtr3=0;
		CC_RX_FLAGS.EndPtr3=0;
		CC_RX_FLAGS.Length3=0;
	}
	
	UART2_BUF_O_Write_String_To_Buffer("\n--> Size:");
	UART2_BUF_O_Write_Number04_To_Buffer(length);
	UART2_BUF_O_Write_String_To_Buffer("\n");
	if(length > max_len) // here need to make a request for the corrupted packet
	{
		UART2_BUF_O_Write_String_To_Buffer("\nSize Error:");
		UART2_BUF_O_Write_String_To_Buffer("\n--> Size:");
		UART2_BUF_O_Write_Signed_Number_To_Buffer(length);
		UART2_BUF_O_Write_String_To_Buffer("\n");
		length=AX25_MAX_FRAME_LEN;
	}
	memcpy(out, data, length);
	return length;
}
void CC_RX_Clear_Command(void)
{
	CC_RX_FLAGS.Sent_Flag = 1;
	CC_RX_SP_Command[0]='\0';
	CC_RX_SP_Command[1]='\0';
	CC_RX_SP_Command[2]='\0';
	CC_RX_SP_Command[3]='\0';
	CC_RX_SP_Command[4]='\0';
}
void CC_RX_SET_Parameters(void)
{
	if(CC_RX_FLAGS.SetParam_Flag == 1)
	{
		GPIO_ResetBits(CC_RX_SET_PORT, CC_RX_SET_PIN);
		if((CC_RX_SET_Counter>=CC_RX_SettingDelay)&&(!CC_RX_FLAGS.Sent_Flag))
		{
			switch (CC_RX_SP_Command[3])
			{
				case 'A':
					UART2_BUF_O_Write_String_To_Buffer("in A case\n");
					if(((CC_RX_SP_Command[4]>='0')&&(CC_RX_SP_Command[4]<='9'))&&((CC_RX_SP_Command[5]>='0')&&(CC_RX_SP_Command[5]<='9'))&&((CC_RX_SP_Command[6]>='0')&&(CC_RX_SP_Command[6]<='9')))
					{
						CC_RX_BUF_O_Write_String_To_Buffer(CC_RX_SP_Command);
					}
					else
					{
						UART2_BUF_O_Write_String_To_Buffer("Wrong input:\n");
					}					
					CC_RX_Clear_Command();
					break;
				case 'B':
					UART2_BUF_O_Write_String_To_Buffer("in B case\n");
					if(((CC_RX_SP_Command[4]>='0')&&(CC_RX_SP_Command[4]<='9'))&&((CC_RX_SP_Command[5]>='0')&&(CC_RX_SP_Command[5]<='9'))&&((CC_RX_SP_Command[6]>='0')&&(CC_RX_SP_Command[6]<='9'))&&((CC_RX_SP_Command[7]>='0')&&(CC_RX_SP_Command[7]<='9')))
					{
						CC_RX_BUF_O_Write_String_To_Buffer(CC_RX_SP_Command);
					}
					else
					{
						UART2_BUF_O_Write_String_To_Buffer("Wrong input:\n");
					}		
					CC_RX_Clear_Command();
					break;
				case 'C':
					UART2_BUF_O_Write_String_To_Buffer("in C case\n");
					if(((CC_RX_SP_Command[4]>='0')&&(CC_RX_SP_Command[4]<='1'))&&((CC_RX_SP_Command[5]>='0')&&(CC_RX_SP_Command[5]<='2'))&&((CC_RX_SP_Command[6]>='0')&&(CC_RX_SP_Command[6]<='7')))
					{
						CC_RX_BUF_O_Write_String_To_Buffer(CC_RX_SP_Command);
					}
					else
					{
						UART2_BUF_O_Write_String_To_Buffer("Wrong input:\n");
					}
					CC_RX_Clear_Command();				
					break;
			case 'F':
					UART2_BUF_O_Write_String_To_Buffer("in F case\n");
					if((CC_RX_SP_Command[4]>='U')&&((CC_RX_SP_Command[5]>='1')&&(CC_RX_SP_Command[5]<='4')))
					{
						CC_RX_BUF_O_Write_String_To_Buffer(CC_RX_SP_Command);
					}
					else
					{
						UART2_BUF_O_Write_String_To_Buffer("Wrong input:\n");
					}
					CC_RX_Clear_Command();
					break;
			case 'P':
					UART2_BUF_O_Write_String_To_Buffer("in P case\n");
					if((CC_RX_SP_Command[4]>='1')&&(CC_RX_SP_Command[4]<='8'))
					{
						CC_RX_BUF_O_Write_String_To_Buffer(CC_RX_SP_Command);
					}
					else
					{
						UART2_BUF_O_Write_String_To_Buffer("Wrong input:\n");
					}
					CC_RX_Clear_Command();
					break;
			case 'R':
					UART2_BUF_O_Write_String_To_Buffer("in R case\n");
					if((CC_RX_SP_Command[4]=='A')||(CC_RX_SP_Command[4]=='B')||(CC_RX_SP_Command[4]=='C')||(CC_RX_SP_Command[4]=='F')||(CC_RX_SP_Command[4]=='P')||(CC_RX_SP_Command[4]=='X'))
					{
						CC_RX_BUF_O_Write_String_To_Buffer(CC_RX_SP_Command);
					}
					else if((CC_RX_SP_Command[4]=='E')&&(CC_RX_SP_Command[5]=='E')&&(CC_RX_SP_Command[6]=='S')&&(CC_RX_SP_Command[7]=='E')&&(CC_RX_SP_Command[8]=='T'))
					{
						CC_RX_BUF_O_Write_String_To_Buffer(CC_RX_SP_Command);
					}
					else	
					{
						UART2_BUF_O_Write_String_To_Buffer("Wrong input:\n");
					}
					CC_RX_Clear_Command();
					break;
			case 'U':
					UART2_BUF_O_Write_String_To_Buffer("in U case\n");
					if(((CC_RX_SP_Command[4]=='8')||(CC_RX_SP_Command[4]=='9'))&&((CC_RX_SP_Command[5]=='O')||(CC_RX_SP_Command[5]=='E'||(CC_RX_SP_Command[5]=='N')))&&((CC_RX_SP_Command[6]=='1')||(CC_RX_SP_Command[6]=='2')))
					{
						CC_RX_BUF_O_Write_String_To_Buffer(CC_RX_SP_Command);
					}
					else
					{
						UART2_BUF_O_Write_String_To_Buffer("Wrong input:\n");
					}
					CC_RX_Clear_Command();
					break;
			case 'V':
					UART2_BUF_O_Write_String_To_Buffer("in V case\n");
					CC_RX_BUF_O_Write_String_To_Buffer(CC_RX_SP_Command);
					CC_RX_Clear_Command();

					break;
			case 'S':
					UART2_BUF_O_Write_String_To_Buffer("in S case\n");
					if((CC_RX_SP_Command[4]=='L')&&(CC_RX_SP_Command[5]=='E')&&(CC_RX_SP_Command[6]=='E')&&(CC_RX_SP_Command[7]=='P'))
					{
						CC_RX_BUF_O_Write_String_To_Buffer(CC_RX_SP_Command);
					}
					else
					{
						UART2_BUF_O_Write_String_To_Buffer("Wrong input:\n");
					}
					CC_RX_Clear_Command();
					break;
			case 'E':
				UART2_BUF_O_Write_String_To_Buffer("in E case\n");
				if((CC_RX_SP_Command[4]=='X')&&(CC_RX_SP_Command[5]=='I')&&(CC_RX_SP_Command[6]=='T'))
				{
					GPIO_SetBits(CC_RX_SET_PORT, CC_RX_SET_PIN);
					CC_RX_SET_Counter=0;
					CC_RX_FLAGS.SetParam_Flag = 0;
					CC_RX_Clear_Command();
				}	
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("Wrong input:\n");
				}
				CC_RX_Clear_Command();
				break;
			default:
				if((CC_RX_SP_Command[0]=='A')&&(CC_RX_SP_Command[1]=='T')&&(CC_RX_SP_Command[2]=='*'))
				{
					UART2_BUF_O_Write_String_To_Buffer("in AT case\n");
					CC_RX_BUF_O_Write_String_To_Buffer("AT\n");
				}
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("SCWI: Echo Mode\n");
					for (int i=0; i > 10;i++)
					{
					UART2_BUF_O_Write_Char_To_Buffer(CC_RX_SP_Command[i]);
					if(CC_RX_SP_Command[i]=='*')
					break;
					}
					UART2_BUF_O_Write_Char_To_Buffer('\n');
				}
				CC_RX_Clear_Command();
				break;		
			}
		}
		else if(CC_RX_SET_Counter< CC_RX_SettingDelay)
		{
			CC_RX_SET_Counter++;
			if(CC_RX_SET_Counter==CC_RX_SettingDelay/2)
			{
				CC_RX_BUF_O_Write_String_To_Buffer("AT\n");
			}
		}
	}
}
/*----------------------------------------------------------------------------*-

  CC_RX_BUF_O_Check_Data_Integrity()

  Check data integrity.

  PARAMETER:
     None.

  LONG-TERM DATA:
     CC_RX_Tx_buffer_g[]  (R)
     CC_RX_Tx_buffer_ig[] (R)  
     CC_RX_Timeout_us_g   (R)
     CC_RX_Timeout_us_ig  (R)
     CC_RX_Sent_idx_g     (R)  
     CC_RX_Sent_idx_ig    (R)  
     CC_RX_Wait_idx_g     (R)  
     CC_RX_Wait_idx_ig    (R) 

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
void CC_RX_BUF_O_Check_Data_Integrity(void)
{
	// Check the UART register configuration
	REG_CONFIG_CHECKS_UART_Check(USART3); 

	// Check integrity of 'waiting' index
	if ((CC_RX_Wait_idx_g != (~CC_RX_Wait_idx_ig)))
	{
		// Data have been corrupted: Treat here as Fatal Platform Failure
		PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
	}          

	// Check integrity of data in Tx buffer
	for (uint32_t i = 0; i < CC_RX_TX_BUFFER_SIZE_BYTES; i++)  
	{
		if ((CC_RX_Tx_buffer_g[i] != ((char) ~CC_RX_Tx_buffer_ig[i])))
		 {
			 // Data have been corrupted: Treat here as Fatal Platform Failure
			 PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
		 }            
	}
}

 /*----------------------------------------------------------------------------*-
   
  CC_RX_BUF_O_Send_Char()

  Uses on-chip CC_RX hardware to send a single character.

  PARAMETERS:
     CHARACTER : The data to be sent.

  LONG-TERM DATA:
     Timeout_us_g (R)

  MCU HARDWARE:
     CC_RX.

  PRE-CONDITION CHECKS:
     CC_RX_BUF_O_Check_Data_Integrity() is called.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void CC_RX_BUF_O_Send_Char(const char CHARACTER)
{
	// Check data integrity
	CC_RX_BUF_O_Check_Data_Integrity();

	// We use a baudrate-dependent timeout (based on T3)
	TIMEOUT_T3_USEC_Start();
	while((USART_GetFlagStatus(USART3, USART_FLAG_TXE) == 0) &&(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(CC_RX_Timeout_us_g)));
	USART_SendData(USART3, CHARACTER);
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
void CC_RX_DMA_CHECK(void)
{
	static uint32_t old_pos=0;
	uint32_t pos=0;
	pos = ARRAY_LEN(CC_RX_Rx_buffer_g) - DMA_GetCurrDataCounter(DMA1_Stream1); 
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
				CC_RX_PROCESS_DATA(&CC_RX_Rx_buffer_g[old_pos], pos - old_pos);
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
				CC_RX_PROCESS_DATA(&CC_RX_Rx_buffer_g[old_pos], ARRAY_LEN(CC_RX_Rx_buffer_g) - old_pos);
				if (pos > 0) 
					{
						CC_RX_PROCESS_DATA(&CC_RX_Rx_buffer_g[0], pos);
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
void CC_RX_PROCESS_DATA(void* data, size_t len)
{
	uint8_t* d = data;
	for ( ;len > 0;len--, ++d)
	{
		if(CC_RX_FLAGS.SetParam_Flag)
		{
			UART2_BUF_O_Write_Char_To_Buffer(*d);
		}
		if(*d == 0x7E)
		{
			if(CC_RX_FLAGS.Error_Flag)
			{
				CC_RX_FLAGS.Error_Flag=0;
				continue;
			}
			//UART2_BUF_O_Write_String_To_Buffer("\nCC_RX:rcvd 7E\n");
			if(CC_RX_FLAGS.ID_Flag == 1)
			{
				if((CC_RX_FLAGS.End1_Flag == 0) && (CC_RX_FLAGS.Start1_Flag ==0))
				{
					CC_RX_FLAGS.Start1_Flag=1;	// rises the flag that a frame has started
					CC_RX_FLAGS.StartPtr1=d;		// take that pointer of the start of the frame
				}
				else if((CC_RX_FLAGS.End1_Flag == 0) && (CC_RX_FLAGS.Start1_Flag ==1))
				{
					CC_RX_FLAGS.End1_Flag=1;	// rises the flag that a frame has ended
					CC_RX_FLAGS.EndPtr1=d;		// take that pointer of the end of the frame
					CC_RX_FLAGS.ID_Flag = 2;
					rx_sync_flag =1;
				}
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("CC_RX:Frame Error 1\n");
					CC_RX_FLAGS.Error_Flag=1;
					
					CC_RX_FLAGS.End1_Flag=0;
					CC_RX_FLAGS.Start1_Flag=0;
				}
			}
			else if(CC_RX_FLAGS.ID_Flag == 2)
			{
				if((CC_RX_FLAGS.End2_Flag == 0) && (CC_RX_FLAGS.Start2_Flag ==0))
				{
					CC_RX_FLAGS.Start2_Flag=1;	// rises the flag that a frame has started
					CC_RX_FLAGS.StartPtr2=d;		// take that pointer of the start of the frame
				}
				else if((CC_RX_FLAGS.End2_Flag == 0) && (CC_RX_FLAGS.Start2_Flag ==1))
				{
					CC_RX_FLAGS.End2_Flag=1;	// rises the flag that a frame has ended
					CC_RX_FLAGS.EndPtr2=d;		// take that pointer of the end of the frame
					CC_RX_FLAGS.ID_Flag = 3;
					rx_sync_flag =1;
				}
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("CC_RX:Frame Error 2\n");
					CC_RX_FLAGS.Error_Flag=1;

					CC_RX_FLAGS.End2_Flag=0;
					CC_RX_FLAGS.Start2_Flag=0;
				}
			}
			else if(CC_RX_FLAGS.ID_Flag == 3)
			{
				if((CC_RX_FLAGS.End3_Flag == 0) && (CC_RX_FLAGS.Start3_Flag ==0))
				{
					CC_RX_FLAGS.Start3_Flag=1;	// rises the flag that a frame has started
					CC_RX_FLAGS.StartPtr3=d;		// take that pointer of the start of the frame
				}
				else if((CC_RX_FLAGS.End3_Flag == 0) && (CC_RX_FLAGS.Start3_Flag ==1))
				{
					CC_RX_FLAGS.End3_Flag=1;	// rises the flag that a frame has ended
					CC_RX_FLAGS.EndPtr3=d;		// take that pointer of the end of the frame
					CC_RX_FLAGS.ID_Flag = 1;
					rx_sync_flag =1;
				}
				else
				{
					UART2_BUF_O_Write_String_To_Buffer("CC_RX:Frame Error 3\n");
					CC_RX_FLAGS.Error_Flag=1;

					CC_RX_FLAGS.End3_Flag=0;
					CC_RX_FLAGS.Start3_Flag=0;
				}
			}
		}
	}
}
/*----------------------------------------------------------------------------*-
   
  CC_RX_BUF_O_Write_String_To_Buffer()

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
void CC_RX_BUF_O_Write_String_To_Buffer(const char* const STR_PTR)
{
   uint32_t i = 0;

   while ((STR_PTR[i] != '\0')&&(STR_PTR[i] != '*'))
	{
      CC_RX_BUF_O_Write_Char_To_Buffer(STR_PTR[i]);
      i++;
	}
}
/*----------------------------------------------------------------------------*-
   
  CC_RX_BUF_O_Write_Char_To_Buffer()

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
void CC_RX_BUF_O_Write_Char_To_Buffer(const char CHARACTER)
{
   // Write to the buffer *only* if there is space
   if (CC_RX_Wait_idx_g < CC_RX_TX_BUFFER_SIZE_BYTES)
      {
      CC_RX_Tx_buffer_g[CC_RX_Wait_idx_g] = CHARACTER;
      CC_RX_Tx_buffer_ig[CC_RX_Wait_idx_g] = (char) ~CHARACTER;
      CC_RX_Wait_idx_g++;     
      }
   else
      {
      // Write buffer is full
      // No error handling / reporting here (characters may be lost)
      // Adapt as required to meet the needs of your application
      }

   // Update the copy
   CC_RX_Wait_idx_ig = ~CC_RX_Wait_idx_g;
}
	 /*----------------------------------------------------------------------------*-

  CC_RX_BUF_O_Send_All_Data()

  Sends all data from the software transmit buffer.

  NOTES: 
  * May have very long execution time!  
  * Intended for use when the scheduler is NOT running.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     Sent_idx_g  (W)  
     Sent_idx_ig (W)  
     Wait_idx_g  (W)  
     Wait_idx_ig (W) 

  MCU HARDWARE:
     UART2.

  PRE-CONDITION CHECKS:
     UART2_BUF_O_Check_Data_Integrity() is called.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void CC_RX_BUF_O_Send_All_Data(void)
{
   // Check data integrity
   CC_RX_BUF_O_Check_Data_Integrity();

   while (CC_RX_Sent_idx_g < CC_RX_Wait_idx_g)
      {
      CC_RX_BUF_O_Send_Char(CC_RX_Tx_buffer_g[CC_RX_Sent_idx_g]);

      CC_RX_Sent_idx_g++;
      CC_RX_Sent_idx_ig = ~CC_RX_Sent_idx_g;
      }

   // Reset the indices
   CC_RX_Sent_idx_g = 0;
   CC_RX_Sent_idx_ig = ~0;
   CC_RX_Wait_idx_g = 0;
   CC_RX_Wait_idx_ig = ~0;
}
	 
	 
	 
	 

	 /*********************************************************/
#elif CC1120	 
	 
#elif CC1101_SPI
	
	
#endif	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 