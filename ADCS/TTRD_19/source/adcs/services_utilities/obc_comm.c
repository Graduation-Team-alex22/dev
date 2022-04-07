#include "obc_comm.h"

// private variables
static obc_comm_flags_t obc_comm_flags;

error_t OBC_Comm_Init(void)
{
   obc_comm_flags = 0;
   return NO_ERROR;
}

error_t OBC_Comm_Update(void)
{
   // check if TLE data has been received
   if(0)
   {
      obc_comm_flags |= TLE_FLAG_BIT;
   }
   
   return NO_ERROR;
}

obc_comm_flags_t OBC_Comm_GetFlags(void)
{
   return obc_comm_flags;
}


char* OBC_Comm_GetTLE(void)
{
   
   obc_comm_flags &= ~TLE_FLAG_BIT;
   return 0;
}
