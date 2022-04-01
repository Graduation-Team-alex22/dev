#ifndef _MASS_STORAGE_H
#define _MASS_STORAGE_H 1

#include "../main/main.h"

// ------ Public function prototypes (declarations) --------------------------

void     Mass_Storage_Init(void);
uint32_t Mass_Storage_Update(tc_tm_pkt *mass_storagepkt);

// ------ Public variables ---------------------------------------------------

extern tc_tm_pkt *mass_storagepkt;
extern uint8_t mass_storeFLAG;

#endif

/*----------------------------------------------------------------------------*-
   ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
