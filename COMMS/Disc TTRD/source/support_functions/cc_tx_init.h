#ifndef __CC_TX_INIT_H
#define __CC_TX_INIT_H

/*
#include "cc_definitions.h"
#include "cc112x_spi.h"
#include <stdint.h>
*/

// Was used in (ONLY INCLUDED IN) cc_rx_init.c, cc_tx_init.c, cc112x_spi.c, comms_manager.c, and (ONLY INCLUDED IN) main.c

#include "../main/main.h"


//Setters 
void tx_manualCalibration ();
void tx_registerConfig ();
void tx_cw_registerConfig ();

#endif
