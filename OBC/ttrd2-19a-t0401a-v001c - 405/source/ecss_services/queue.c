#include "queue.h"

#include "pkt_pool.h"


#undef __FILE_ID__
#define __FILE_ID__ 34

struct _queue {
    tc_tm_pkt *fifo[POOL_PKT_TOTAL_SIZE];
    uint8_t head;
    uint8_t tail;
};

static struct _queue queueEPS = { .head = 0, .tail = 0};
static struct _queue queueADCS = { .head = 0, .tail = 0};
static struct _queue queueCOMMS = { .head = 0, .tail = 0};


SAT_returnState queuePush(tc_tm_pkt *pkt, TC_TM_app_id app_id) {
		
	  if(app_id == EPS_APP_ID) 
     {
       if(queueEPS.head == (((queueEPS.tail - 1) + POOL_PKT_TOTAL_SIZE) % POOL_PKT_TOTAL_SIZE)) {
          return SATR_QUEUE_FULL;
       }
       queueEPS.fifo[queueEPS.head] = pkt;
       queueEPS.head = (queueEPS.head + 1) % POOL_PKT_TOTAL_SIZE;			 
		 }
    else if(app_id == ADCS_APP_ID) 
     {
       if(queueADCS.head == (((queueADCS.tail - 1) + POOL_PKT_TOTAL_SIZE) % POOL_PKT_TOTAL_SIZE)) {
          return SATR_QUEUE_FULL;
       }
       queueADCS.fifo[queueADCS.head] = pkt;
       queueADCS.head = (queueADCS.head + 1) % POOL_PKT_TOTAL_SIZE;
		 }
    else if(app_id == COMMS_APP_ID ) 
     {
       if(queueCOMMS.head == (((queueCOMMS.tail - 1) + POOL_PKT_TOTAL_SIZE) % POOL_PKT_TOTAL_SIZE)) {
          return SATR_QUEUE_FULL;
       }
       queueCOMMS.fifo[queueCOMMS.head] = pkt;
       queueCOMMS.head = (queueCOMMS.head + 1) % POOL_PKT_TOTAL_SIZE;
		 }

    return SATR_OK;
}

tc_tm_pkt * queuePop(TC_TM_app_id app_id) {
	
	  tc_tm_pkt *pkt;
	  if(app_id == EPS_APP_ID) 
     {
       if(queueEPS.head == queueEPS.tail) { return 0; }
       pkt = queueEPS.fifo[queueEPS.tail];
       queueEPS.tail = (queueEPS.tail + 1) % POOL_PKT_TOTAL_SIZE;			 
		 }
    else if(app_id == COMMS_APP_ID) 
     {
       if(queueCOMMS.head == queueCOMMS.tail) { return 0; }
       pkt = queueCOMMS.fifo[queueCOMMS.tail];
       queueCOMMS.tail = (queueCOMMS.tail + 1) % POOL_PKT_TOTAL_SIZE;	
		 }
    else if(app_id == ADCS_APP_ID) 
     {
       if(queueADCS.head == queueADCS.tail) { return 0; }
       pkt = queueADCS.fifo[queueADCS.tail];
       queueADCS.tail = (queueADCS.tail + 1) % POOL_PKT_TOTAL_SIZE;	
		 }
		 
    return pkt;
}

uint8_t queueSize(TC_TM_app_id app_id) {
	
	  if(app_id == EPS_APP_ID) 
     {
       if(queueEPS.head == queueEPS.tail) { return 0; }

       return queueEPS.head - queueEPS.tail;		 
		 }
    else if(app_id == COMMS_APP_ID) 
     {
       if(queueCOMMS.head == queueCOMMS.tail) { return 0; }

       return queueCOMMS.head - queueCOMMS.tail;
		 }
    else if(app_id == ADCS_APP_ID) 
     {
       if(queueADCS.head == queueADCS.tail) { return 0; }

       return queueADCS.head - queueADCS.tail;
		 }
		 return 0;
}

tc_tm_pkt * queuePeak(TC_TM_app_id app_id) {
	
	  if(app_id == EPS_APP_ID) 
     {
       if(queueEPS.head == queueEPS.tail) { return 0; }
       return queueEPS.fifo[queueEPS.head];          
		 }
    else if(app_id == COMMS_APP_ID) 
     {
       if(queueCOMMS.head == queueCOMMS.tail) { return 0; }
       return queueCOMMS.fifo[queueCOMMS.head];  
		 }
    else if(app_id == ADCS_APP_ID) 
     {
       if(queueADCS.head == queueADCS.tail) { return 0; }
       return queueADCS.fifo[queueADCS.head];  
		 }
		 return 0;
}

void queue_IDLE(TC_TM_app_id app_id) {
	  
	  tc_tm_pkt *pkt;
	
	  if(app_id == EPS_APP_ID) 
     {
       pkt = queuePeak(app_id);
       if(pkt != NULL) { return; }
       if(is_free_pkt(pkt) == true) { queuePop(app_id);}          
		 }
    else if(app_id == COMMS_APP_ID) 
     {
       pkt = queuePeak(app_id);
       if(pkt != NULL) { return; }
       if(is_free_pkt(pkt) == true) { queuePop(app_id);}
		 }
    else if(app_id == ADCS_APP_ID) 
     {
       pkt = queuePeak(app_id);
       if(pkt != NULL) { return; }
       if(is_free_pkt(pkt) == true) { queuePop(app_id);}
		 }
}
