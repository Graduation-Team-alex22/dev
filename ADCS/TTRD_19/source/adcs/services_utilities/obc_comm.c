#include "obc_comm.h"

// private variables
static obc_comm_flags_t obc_comm_flags = 0;

error_t OBC_Comm_Init(void)
{
   
   return NO_ERROR;
}

error_t OBC_Comm_Update(void)
{
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
