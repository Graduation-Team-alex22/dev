#ifndef SHA256_H
#define SHA256_H

// Processor Header
#include "../main/main.h"

/****************************** MACROS ******************************/
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;             // 8-bit byte
typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines

typedef struct {
	BYTE data[64];
	WORD datalen;
	unsigned long long bitlen;
	WORD state[8];
} SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
// Init
void sha256_init(SHA256_CTX *ctx);


// Update
void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len);


// Setters



// Getters


// Setters / Getters it takes the input ctx and encode it then give it back in the hash ptr
void sha256_final(SHA256_CTX *ctx, BYTE hash[]);


#endif // SHA256_H