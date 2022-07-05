#ifndef INC_WOD_HANDLING_H_
#define INC_WOD_HANDLING_H_



#include "../main/main.h"
#include "ax_25.h"
/**
 * The number of bytes in every WOD data set WITHOUT the mode bit
 */
#define WOD_DATASET_SIZE 7
#define AX25_MAX_FRAME_LEN 256

/**
 * The maximum number of datasets that each WOD can carry
 */
#define WOD_MAX_DATASETS 32

#define WOD_SIZE (sizeof(uint32_t)+ ((WOD_DATASET_SIZE*8+1) * WOD_MAX_DATASETS / 8))

/**
 * WOD packet as it handled inside the COMMS
 */
typedef struct 
{
  uint8_t wod[WOD_SIZE];
  size_t len;
  size_t tx_cnt;
  uint8_t valid;
} comms_wod_t;

typedef struct 
{
  uint8_t ex_wod[AX25_MAX_FRAME_LEN];
  size_t len;
  size_t tx_cnt;
  uint8_t valid;
} comms_ex_wod_t;



//Init
void comms_wod_init(void);
void comms_ex_wod_init(void);

//Update

//Getters
int32_t comms_wod_send(uint8_t bypass_check); // originally comms_wod_tx

int32_t comms_ex_wod_send(void); // originally comms_ex_wod_tx

uint8_t wod_convert_temperature(float val);

uint8_t bat_voltage_valid(uint8_t val);

uint8_t bat_current_valid(uint8_t val);

uint8_t bus_3300mV_current_valid(uint8_t val);

uint8_t bus_5000mV_current_valid(uint8_t val); // this function returns a call to the getter bus_3300mV_current_valid

uint8_t comms_temp_valid(uint8_t val);

uint8_t eps_temp_valid(uint8_t val); // this function returns a call to the getter comms_temp_valid

uint8_t bat_temp_valid(uint8_t val); // this function returns a call to the getter comms_temp_valid

//Setters
void store_wod_obc(const uint8_t *obc_wod, size_t len); // obc_wod is the buffer with the WOD, as it comes from the OBC

void store_ex_wod_obc(const uint8_t *obc_wod, size_t len);


#endif /* INC_WOD_HANDLING_H_ */

