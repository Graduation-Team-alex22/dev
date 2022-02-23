#ifndef INC_LFSR_H_
#define INC_LFSR_H_
/* 
LFSR --> Linear feedback shift register 
				 is a shift register whose input bit is a linear function of its previous state.


*/

// Processor Header
#include "../main/main.h"

typedef struct {
  uint32_t mask;
  uint32_t seed;
  uint32_t order;
  uint32_t shift_reg;
} lfsr_handle_t;



//Init 
int32_t
lfsr_init(lfsr_handle_t *h, uint32_t mask, uint32_t seed, uint32_t order);

//Update



//Setter
int32_t
lfsr_reset(lfsr_handle_t *h);



//Getter

uint8_t
lfsr_next_bit(lfsr_handle_t *h);

uint8_t
lfsr_next_bit_scramble(lfsr_handle_t *h, uint8_t bit);

uint8_t
lfsr_next_bit_descramble(lfsr_handle_t *h, uint8_t bit);




















#endif /* INC_LFSR_H_ */
