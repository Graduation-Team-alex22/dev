
#include "IDLE_task.h"
#include "queue.h"

void IDLE_Init(void)
{

}

/*----------------------------------------------------------------------------*-

  IDLE_Update(void)

  Checks the sanity of packet pool and queues.


  PARAMETERS:
     None.

  LONG-TERM DATA:
      None.
   
  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.
		 
  WCET:
     Not yet determined.

  BCET:
     Not yet determined.
		 
  RETURN VALUE:
    RETURN_NORMAL_STATE.

-*----------------------------------------------------------------------------*/
uint32_t IDLE_Update(void)
{
//	 // pkt_pool_IDLE(time);
   queue_IDLE(EPS_APP_ID);
   queue_IDLE(COMMS_APP_ID);
   queue_IDLE(ADCS_APP_ID);
	
	return RETURN_NORMAL_STATE;
}	

