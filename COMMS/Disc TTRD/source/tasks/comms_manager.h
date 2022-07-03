#ifndef INC_COMMS_MANAGER_H_
#define INC_COMMS_MANAGER_H_
// We added Comms.h here
// Processor Header
#include "../main/main.h"
#include "../support_functions/ax_25.h"	
#include "../support_functions/sha256.h"
#include "../support_functions/large_data_services.h"
#include "../support_functions/upsat.h"

#include "rx_manager.h"	
#include "tx_manager.h"	
#include "cc_tx.h"
#include "cc_rx.h"

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

#define UART_BUF_SIZE 4096 /*(POOL_PKT_EXT*2)*/

struct _uart_data 
{
	uint8_t uart_buf[UART_BUF_SIZE];
	uint8_t uart_unpkt_buf[UART_BUF_SIZE];
	uint8_t deframed_buf[MAX_PKT_SIZE];
	uint8_t uart_pkted_buf[UART_BUF_SIZE];
	uint8_t framed_buf[UART_BUF_SIZE];
	uint16_t uart_size;
	uint32_t last_com_time;
	uint32_t init_time;
};
extern struct _uart_data uart_data;
struct _comms_data
{
	uint16_t comms_seq_cnt;

  struct _uart_data obc_uart; 
};

extern struct _comms_data comms_data;

//Init 
void comms_init(void);  

//Update
//updates are in rx_manager & tx_manager

//Setters
void set_cmd_and_ctrl_period(uint8_t enable);  

int32_t recv_payload(uint8_t *out, size_t len); 

SAT_returnState recv_ecss(uint8_t *payload, const uint16_t payload_size);//originally rx_ecss  

//Getters
int32_t send_payload(const uint8_t *in, size_t len, uint8_t is_wod);  

int32_t send_payload_cw(const uint8_t *in, size_t len); 

int32_t send_cw_beacon(void); 

uint8_t is_tx_enabled(void);  

uint8_t is_cmd_ctrl_enabled(void);  

SAT_returnState send_ecss(tc_tm_pkt *pkt); //originally tx_ecss  

SAT_returnState route_pkt(tc_tm_pkt *pkt); 


#endif /* INC_COMMS_MANAGER_H_ */
