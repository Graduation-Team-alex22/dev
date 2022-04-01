#include "function_management_service.h"

#include "time_management_service.h"

#include "service_utilities.h"
#include "pkt_pool.h"
#include "power_ctrl.h"
#include <stdint.h>

#undef __FILE_ID__
#define __FILE_ID__ 10

SAT_returnState function_management_app(tc_tm_pkt *pkt) {

    if(!C_ASSERT(pkt != NULL && pkt->data != NULL) == true)             { return SATR_ERROR; }
    if(!C_ASSERT(pkt->ser_subtype == TC_FM_PERFORM_FUNCTION) == true)   { return SATR_ERROR; }

    const FM_fun_id fun_id = (FM_fun_id)pkt->data[0];

    if(!C_ASSERT(fun_id < LAST_FUN_ID) == true)             { return SATR_ERROR; }
    if(!C_ASSERT(pkt != NULL && pkt->data != NULL) == true) { return SATR_ERROR; }

    if(fun_id == P_OFF || fun_id == P_ON || fun_id == P_RESET) {  /*power management*/

        //if(!C_ASSERT(pkt->len < PKT_LEN_FM_PWRCTRL) == true) { return SATR_INV_DATA_LEN; }

        pkt->verification_state = SATR_OK; 
        power_control_api((FM_dev_id)pkt->data[1], fun_id, NULL);

    } else if(fun_id == SET_VAL) {

        pkt->verification_state = SATR_OK;
        power_control_api((FM_dev_id)pkt->data[1], fun_id, &pkt->data[2]);

    }

    return SATR_OK;
}

SAT_returnState function_management_pctrl_crt_pkt_api(tc_tm_pkt **pkt, const TC_TM_app_id dest_id, const FM_fun_id fun_id, const FM_dev_id did) {

    *pkt = get_pkt(PKT_NORMAL);
    if(!C_ASSERT(*pkt != NULL) == true) {
        return SATR_ERROR;
    }
    crt_pkt(*pkt, dest_id , TC, TC_ACK_NO, TC_FUNCTION_MANAGEMENT_SERVICE, TC_FM_PERFORM_FUNCTION, (TC_TM_app_id)SYSTEM_APP_ID);

    (*pkt)->data[0] = fun_id;
    (*pkt)->data[1] = did;

    (*pkt)->len = 2;

    return SATR_OK;
}

SAT_returnState function_management_pctrl_ack_crt_pkt_api(tc_tm_pkt **pkt, const TC_TM_app_id dest_id, const FM_fun_id fun_id, const FM_dev_id did) {

    *pkt = get_pkt(PKT_NORMAL);
    if(!C_ASSERT(*pkt != NULL) == true) { return SATR_ERROR; }
    crt_pkt(*pkt, dest_id , TC, TC_ACK_ACC, TC_FUNCTION_MANAGEMENT_SERVICE, TC_FM_PERFORM_FUNCTION, (TC_TM_app_id)SYSTEM_APP_ID);

    (*pkt)->data[0] = fun_id;
    (*pkt)->data[1] = did;

    (*pkt)->len = 2;

    return SATR_OK;
}
