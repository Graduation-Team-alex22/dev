#ifndef OBC_COMM_H__
#define	OBC_COMM_H__ 1

#include "../main/project.h"

typedef uint8_t obc_comm_flags_t;

#define TLE_FLAG_BIT       ((obc_comm_flags_t)0x01 << 0)
#define SLR_FLAG_BIT       ((obc_comm_flags_t)0x01 << 1)      // solar panel readings


error_t OBC_Comm_Init(void);
error_t OBC_Comm_Update(void);
obc_comm_flags_t OBC_Comm_GetFlags(void);
error_t OBC_Comm_GetTLE(char* pTLE_Str);
error_t OBC_Comm_GetSLR(uint8_t* pSLR_Values);
char* OBC_Comm_GetpBuffer_TX(void);
#endif
