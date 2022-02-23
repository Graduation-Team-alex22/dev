#ifndef LARGE_DATA_SERVICE_H
#define LARGE_DATA_SERVICE_H

#include <stdint.h>
#include "services.h"

#define LD_PKT_DATA             195 /*MAX_PKT_DATA - LD_PKT_DATA_HDR_SIZE*/
#define LD_PKT_DATA_HDR_SIZE    3

#define LD_TIMEOUT              6000

#define LD_MAX_TRANSFER_TIME    30000

#define MIN(A,B) 			\
   ({ __typeof__ (A) _A = (A); 		\
       __typeof__ (B) _B = (B); 	\
     _A < _B ? _A : _B; })

/**
 * The different states of the Large data stae machine
 */
typedef enum {
    LD_STATE_FREE           = 1,//!< LD_STATE_FREE No large data activity
    LD_STATE_RECEIVING      = 2,//!< LD_STATE_RECEIVING the large data FSM receives data
    LD_STATE_TRANSMITING    = 3,//!< LD_STATE_TRANSMITING the large data FSM transmits data
    LD_STATE_TRANSMIT_FIN   = 4,//!< LD_STATE_TRANSMIT_FIN the large data TX finished. The engine just waits for possible lost frame requests from the ground
    LD_STATE_RECV_OK        = 5,//!< LD_STATE_RECV_OK the large data FSM successfully received the last frame
    LAST_STATE              = 6 //!< LAST_STATE
}LD_states;

/**
 * Status of the large data transfer
 */
struct _ld_status {
    LD_states state;        		/**< Service state machine, state variable*/
    TC_TM_app_id app_id;    		/**< Destination app id */
    uint8_t ld_num;         		/**< Sequence number of last fragmented packet stored */
    uint32_t timeout;       		/**< Time of last large data action */
    uint32_t started;                   /**< Time that the large data transfer started */

    uint8_t buf[MAX_PKT_EXT_DATA]; 	/**< Buffer that holds the sequential fragmented packets */
    uint16_t rx_size;         		/**< The number of bytes stored already in the buffer */
    uint8_t rx_lid;         /**/
    uint8_t tx_lid;         /**/
    uint8_t tx_pkt;         /**/
    uint16_t tx_size;         /**/
};

SAT_returnState large_data_app(tc_tm_pkt *pkt);


SAT_returnState large_data_firstRx_api(tc_tm_pkt *pkt);

SAT_returnState large_data_intRx_api(tc_tm_pkt *pkt);

SAT_returnState large_data_lastRx_api(tc_tm_pkt *pkt);


SAT_returnState large_data_downlinkTx_api(tc_tm_pkt *pkt);

SAT_returnState large_data_ackTx_api(tc_tm_pkt *pkt);

SAT_returnState large_data_retryTx_api(tc_tm_pkt *pkt);

SAT_returnState large_data_abort_api(tc_tm_pkt *pkt);


SAT_returnState large_data_updatePkt(tc_tm_pkt *pkt, uint16_t size, uint8_t subtype);

SAT_returnState large_data_downlinkPkt(tc_tm_pkt **pkt, uint8_t lid, uint16_t n, uint16_t dest_id);

SAT_returnState large_data_verifyPkt(tc_tm_pkt **pkt, uint8_t lid, uint16_t n, uint16_t dest_id);

SAT_returnState large_data_abortPkt(tc_tm_pkt **pkt, uint8_t lid, uint16_t dest_id, uint8_t subtype);

void large_data_IDLE();

void large_data_init();

SAT_returnState large_data_timeout();

#endif
