#ifndef INC_COMMS_MANAGER_H_
#define INC_COMMS_MANAGER_H_
// We added Comms.h here
// Processor Header
#include "../main/main.h"
#include "../support_functions/upsat.h"
#include "../support_functions/cc_tx_init.h"
#include "../support_functions/cc_rx_init.h"
#include "../support_functions/ax_25.h"	
	

	
#define FRAME_OK	1
//temp
#define TEST_ARRAY 1024
/*restriction for 8 char filename, for conversion from num to file name*/
#define MAX_FILE_NUM 0x5F5E0FF

/**
 * Struct containing a set of TX jobs that the dispatcher should take care
 */
typedef struct {
  uint8_t tx_cw; 	/**< TX a CW beacon */
  uint8_t tx_wod;	/**< TX a WOD beacon */
  uint8_t tx_ext_wod;   /**< TX an exWOD beacon */
} comms_tx_job_list_t;

typedef struct {
  uint32_t time_ms;
  uint8_t is_armed;
} comms_cmd_ctrl_t;


struct _comms_data
{
    uint16_t comms_seq_cnt;

    struct uart_data obc_uart;  // need UPSat.h
};

extern struct _comms_data comms_data;

//Init 
void comms_init();

//Update
int32_t comms_routine_dispatcher(comms_tx_job_list_t *tx_jobs);


//Setters
void set_cmd_and_ctrl_period(uint8_t enable); // Implemented.

int32_t recv_payload(uint8_t *out, size_t len, size_t timeout_ms);

SAT_returnState recv_ecss(uint8_t *payload, const uint16_t payload_size);//originally rx_ecss

//Getters
int32_t send_payload(const uint8_t *in, size_t len, uint8_t is_wod, size_t timeout_ms);

int32_t send_payload_cw(const uint8_t *in, size_t len);

int32_t send_cw_beacon();

uint8_t is_tx_enabled(); // Implemented.

uint8_t is_cmd_ctrl_enabled(); // Implemented.

SAT_returnState send_ecss(tc_tm_pkt *pkt); //originally tx_ecss

SAT_returnState route_pkt(tc_tm_pkt *pkt);

SAT_returnState check_timeouts();

#endif /* INC_COMMS_MANAGER_H_ */
