#ifndef OBC_COMM_H__
#define	OBC_COMM_H__ 1

#include "../main/project.h"

typedef uint8_t obc_comm_flags_t;

#define TLE_FLAG_BIT       ((obc_comm_flags_t)0x01)

error_t OBC_Comm_Init(void);
error_t OBC_Comm_Update(void);
obc_comm_flags_t OBC_Comm_GetFlags(void);
char* OBC_Comm_GetTLE(void);

#endif
