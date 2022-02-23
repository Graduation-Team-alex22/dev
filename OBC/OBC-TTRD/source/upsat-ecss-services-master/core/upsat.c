#include "upsat.h"

#include "pkt_pool.h"
#include "service_utilities.h"
#include "hldlc.h"
#include "verification_service.h"
#include "test_service.h"
#include "ecss_stats.h"

#undef __FILE_ID__
#define __FILE_ID__ 31

#define SYS_HOUR 360000

extern SAT_returnState route_pkt(tc_tm_pkt *pkt);

extern uint32_t HAL_sys_GetTick();

extern SAT_returnState hal_kill_uart(TC_TM_app_id app_id);

extern SAT_returnState HAL_uart_tx_check(TC_TM_app_id app_id);

extern SAT_returnState HAL_uart_rx(TC_TM_app_id app_id, struct uart_data *data);

extern tc_tm_pkt * queuePeak(TC_TM_app_id app_id);
extern tc_tm_pkt * queuePop(TC_TM_app_id app_id);

SAT_returnState import_pkt(TC_TM_app_id app_id, struct uart_data *data) {

    tc_tm_pkt *pkt;
    uint16_t size = 0;

    SAT_returnState res;
    SAT_returnState res_deframe;

    res = HAL_uart_rx(app_id, data);
    if( res == SATR_EOT ) {

        size = data->uart_size;
        res_deframe = HLDLC_deframe(data->uart_unpkt_buf, data->deframed_buf, &size);
        if(res_deframe == SATR_EOT) {

            data->last_com_time = HAL_sys_GetTick();/*update the last communication time, to be used for timeout discovery*/

            pkt = get_pkt(size);

            if(!C_ASSERT(pkt != NULL) == true) { return SATR_ERROR; }
            if((res = unpack_pkt(data->deframed_buf, pkt, size)) == SATR_OK) {
                stats_inbound(pkt->type, pkt->app_id, pkt->dest_id, pkt);
                route_pkt(pkt); }
            else {
                stats_dropped_upack();
                pkt->verification_state = res;
            }
            verification_app(pkt);

            TC_TM_app_id dest = 0;

            if(pkt->type == TC)         { dest = pkt->app_id; }
            else if(pkt->type == TM)    { dest = pkt->dest_id; }

            if(dest == SYSTEM_APP_ID) {
                free_pkt(pkt);
            }
        }
        else {
            stats_dropped_hldlc();
        }
    }

    return SATR_OK;
}

//WIP
SAT_returnState export_pkt(TC_TM_app_id app_id, struct uart_data *data) {

    tc_tm_pkt *pkt = 0;
    uint16_t size = 0;
    SAT_returnState res = SATR_ERROR;

    /* Checks if the tx is busy */
    if((res = HAL_uart_tx_check(app_id)) == SATR_ALREADY_SERVICING) { return res; }

    /* Checks if that the pkt that was transmitted is still in the queue */
    if((pkt = queuePop(app_id)) ==  NULL) { return SATR_OK; }

    stats_outbound(pkt->type, pkt->app_id, pkt->dest_id, pkt);

    pack_pkt(data->uart_pkted_buf,  pkt, &size);

    res = HLDLC_frame(data->uart_pkted_buf, data->framed_buf, &size);
    if(res == SATR_ERROR) { return SATR_ERROR; }

    if(!C_ASSERT(size > 0) == true) { return SATR_ERROR; }

    HAL_uart_tx(app_id, data->framed_buf, size);

    free_pkt(pkt);

    return SATR_OK;
}

/**
 * @brief      This function should be called every 2 mins in order to refresh the timeout timer of the EPS.
 *             In case the subsystem fails to send a correct packet within of 10m, the EPS will reset the subsystem.
 */
void sys_refresh() {

    tc_tm_pkt *temp_pkt = 0;

    test_crt_heartbeat(&temp_pkt);
    if(!C_ASSERT(temp_pkt != NULL) == true) { return ; }

    route_pkt(temp_pkt);
}

SAT_returnState test_crt_heartbeat(tc_tm_pkt **pkt) {

    *pkt = get_pkt(PKT_NORMAL);
    if(!C_ASSERT(*pkt != NULL) == true) { return SATR_ERROR; }
    crt_pkt(*pkt, EPS_APP_ID, TC, TC_ACK_NO, TC_TEST_SERVICE, TC_CT_PERFORM_TEST, SYSTEM_APP_ID);

    (*pkt)->len = 0;

    return SATR_OK;
}

SAT_returnState firewall(tc_tm_pkt *pkt) {

    if(!C_ASSERT(pkt != NULL && pkt->data != NULL) == true) {
        return SATR_ERROR;
    }

    if(!C_ASSERT(!(pkt->type == TC &&
                 pkt->app_id == EPS_APP_ID &&
                 pkt->ser_type == TC_FUNCTION_MANAGEMENT_SERVICE &&
                 pkt->ser_subtype == TC_FM_PERFORM_FUNCTION &&
                 pkt->data[0] == P_OFF &&
                 pkt->data[1] == COMMS_DEV_ID))) {
        pkt->verification_state = SATR_FIREWALLED;
        return SATR_FIREWALLED;
    }

    if(!C_ASSERT(!(pkt->type == TC &&
                 pkt->app_id == EPS_APP_ID &&
                 pkt->ser_type == TC_FUNCTION_MANAGEMENT_SERVICE &&
                 pkt->ser_subtype == TC_FM_PERFORM_FUNCTION &&
                 pkt->data[0] == P_OFF &&
                 pkt->data[1] == OBC_DEV_ID))) {
        pkt->verification_state = SATR_FIREWALLED;
        return SATR_FIREWALLED;
    }

    if(!C_ASSERT(!(pkt->type == TC &&
                 pkt->app_id == EPS_APP_ID &&
                 pkt->ser_type == TC_FUNCTION_MANAGEMENT_SERVICE &&
                 pkt->ser_subtype == TC_FM_PERFORM_FUNCTION &&
                 pkt->data[0] == P_OFF &&
                 pkt->data[1] == EPS_DEV_ID))) {
        pkt->verification_state = SATR_FIREWALLED;
        return SATR_FIREWALLED;
    }

    return SATR_OK;
}
