#include "obc_comm.h"

// private variables
static obc_comm_flags_t obc_comm_flags = 0;
static char obc_buffer_tx[] = "HELLO";


error_t OBC_Comm_Init(void)
{
   
   return NO_ERROR;
}

error_t OBC_Comm_Update(void)
{
   // send a string
   // check if stream is free
   // wait until the previous transfer is done
   if( DMA1_Stream3->NDTR == 0 )
   {    
      // set the dma transfer data length
      DMA1_Stream3->NDTR = 6;

      // Clear DMA Stream Flags
      DMA1->LIFCR |= (0x3D << 22);

      // Clear USART Transfer Complete Flags
      USART_ClearFlag(USART3,USART_FLAG_TC);  

      // Enable DMA USART TX Stream 
      DMA_Cmd(DMA1_Stream3,ENABLE);
      
   }

   // check if TLE data has been received
   if(0)
   {
      obc_comm_flags |= TLE_FLAG_BIT;
   }
   
   // check if Solar Panel data has been received
   if(0)
   {
      obc_comm_flags |= SLR_FLAG_BIT;
   }
   
   return NO_ERROR;
}

obc_comm_flags_t OBC_Comm_GetFlags(void)
{
   return obc_comm_flags;
}

error_t OBC_Comm_GetTLE(char* pTLE_Str)
{
   // if data is available
   if( obc_comm_flags & TLE_FLAG_BIT )
   {
      // clear the flag
      obc_comm_flags &= ~TLE_FLAG_BIT;
   }
   // if data is not available
   else
   {
      return ERROR_OBC_COMM_BAD_GETTER;
   }
   return NO_ERROR;
}

error_t OBC_Comm_GetSLR(uint8_t* pSLR_Values)
{
   // if data is available
   if( obc_comm_flags & SLR_FLAG_BIT )
   {
      // clear the flag
      obc_comm_flags &= ~SLR_FLAG_BIT;
   }
   // if data is not available
   else
   {
      return ERROR_OBC_COMM_BAD_GETTER;
   }
   return NO_ERROR;
}

char* OBC_Comm_GetpBuffer_TX(void)
{
   return obc_buffer_tx;   
}
