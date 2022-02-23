#include "obc.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "housekeeping.h"
#include "mass_storage_service.h"
#include "wdg.h"
#include "su_mnlp.h"

#undef __FILE_ID__
#define __FILE_ID__ 15

extern SAT_returnState export_pkt(TC_TM_app_id app_id, struct uart_data *data);

extern uint32_t * HAL_obc_BKPSRAM_BASE();

extern SAT_returnState free_pkt(tc_tm_pkt *pkt);

extern SAT_returnState verification_app(tc_tm_pkt *pkt);
extern SAT_returnState hk_app(tc_tm_pkt *pkt);
extern SAT_returnState function_management_app(tc_tm_pkt *pkt);
extern SAT_returnState mass_storage_app(tc_tm_pkt *pkt);
extern SAT_returnState mass_storage_storeLogs(MS_sid sid, uint8_t *buf, uint16_t *size);
extern SAT_returnState test_app(tc_tm_pkt *pkt);


const uint8_t services_verification_OBC_TC[MAX_SERVICES][MAX_SUBTYPES] = { 
/*    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /*TC_VERIFICATION_SERVICE*/
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0 }, /*TC_HOUSEKEEPING_SERVICE*/
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /*TC_FUNCTION_MANAGEMENT_SERVICE*/
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /*TC_SCHEDULING_SERVICE*/
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 }, /*TC_LARGE_DATA_SERVICE*/
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, /*TC_MASS_STORAGE_SERVICE*/
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /*TC_TEST_SERVICE*/
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};
 
struct _obc_data obc_data = { .dbg_uart.init_time = 0,
                              .comms_uart.init_time = 0,
                              .adcs_uart.init_time = 0,
                              .eps_uart.init_time = 0,
                              .iac_timeout = 0,
                              .vbat = 0,
                              .adc_time = 0,
                              .adc_flag = false };

struct _task_times task_times;
static struct _sys_data sys_data;

SAT_returnState route_pkt(tc_tm_pkt *pkt) {

    SAT_returnState res;
    TC_TM_app_id id;

    if(!C_ASSERT(pkt != NULL && pkt->data != NULL) == true)                         { free_pkt(pkt); return SATR_ERROR; }
    if(!C_ASSERT(pkt->type == TC || pkt->type == TM) == true)                       { free_pkt(pkt); return SATR_ERROR; }
    if(!C_ASSERT(pkt->app_id < LAST_APP_ID && pkt->dest_id < LAST_APP_ID) == true)  { free_pkt(pkt); return SATR_ERROR; }

    if(pkt->type == TC)         { id = pkt->app_id; } 
    else if(pkt->type == TM)    { id = pkt->dest_id; }

    if(id == SYSTEM_APP_ID && pkt->ser_type == TC_VERIFICATION_SERVICE) {
        res = verification_app(pkt);
    } else if(id == SYSTEM_APP_ID && pkt->ser_type == TC_EVENT_SERVICE) {
        //C_ASSERT(pkt->ser_subtype == 21 || pkt->ser_subtype == 23) { free_pkt(pkt); return SATR_ERROR; }
        res = event_app(pkt);
    } else if(id == SYSTEM_APP_ID && pkt->ser_type == TC_HOUSEKEEPING_SERVICE) {
        //C_ASSERT(pkt->ser_subtype == 21 || pkt->ser_subtype == 23) { free_pkt(pkt); return SATR_ERROR; }
        res = hk_app(pkt);
    } else if(id == SYSTEM_APP_ID && pkt->ser_type == TC_FUNCTION_MANAGEMENT_SERVICE) {
        res = function_management_app(pkt);
    } else if(id == SYSTEM_APP_ID && pkt->ser_type == TC_TIME_MANAGEMENT_SERVICE) {
        //TODO: ADD C_ASSERT
        res = time_management_app(pkt);
    } else if(id == SYSTEM_APP_ID && pkt->ser_type == TC_SCHEDULING_SERVICE) {
        //TODO: ADD C_ASSERT
        res = scheduling_app(pkt);
    } else if(id == SYSTEM_APP_ID && pkt->ser_type == TC_MASS_STORAGE_SERVICE) {
        //C_ASSERT(pkt->ser_subtype == 1 || pkt->ser_subtype == 2 || pkt->ser_subtype == 9 || pkt->ser_subtype == 11 || pkt->ser_subtype == 12 || pkt->ser_subtype == 13) { free_pkt(pkt); return SATR_ERROR; }
        res = mass_storage_app(pkt);
    } else if(id == SYSTEM_APP_ID && pkt->ser_type == TC_TEST_SERVICE) {
        //C_ASSERT(pkt->ser_subtype == 1 || pkt->ser_subtype == 2 || pkt->ser_subtype == 9 || pkt->ser_subtype == 11 || pkt->ser_subtype == 12 || pkt->ser_subtype == 13) { free_pkt(pkt); return SATR_ERROR; }
        res = test_app(pkt);
    }else if(id == SYSTEM_APP_ID && pkt->ser_type == TC_SU_MNLP_SERVICE) {
        //TODO: ADD C_ASSERT
        res = su_nmlp_app(pkt);
    }
    else if(id == EPS_APP_ID) {
        queuePush(pkt, EPS_APP_ID);
    }
    else if(id == ADCS_APP_ID) {
        queuePush(pkt, ADCS_APP_ID);
    }
    else if(id == COMMS_APP_ID) {
        queuePush(pkt, COMMS_APP_ID);
    }
    else if(id == GND_APP_ID) {
        queuePush(pkt, COMMS_APP_ID);
    }
    else if(id == DBG_APP_ID) {
        queuePush(pkt, DBG_APP_ID);
    } else {
        free_pkt(pkt);
    }

    return SATR_OK;
}

SAT_returnState obc_INIT() {

    pkt_pool_INIT();
    HAL_obc_enableBkUpAccess();
    bkup_sram_INIT();

    HAL_reset_source(&sys_data.rsrc);
    update_boot_counter();

    uint32_t cnt = 0;
    get_boot_counter(&cnt);
    event_boot(sys_data.rsrc, cnt);

    HAL_obc_SD_ON();
   
    mass_storage_init();

    su_INIT();

    scheduling_init_service();
    return SATR_OK;
}

SAT_returnState sram_hard_delete() {

    uint8_t *base_pointer = (uint8_t*)HAL_obc_BKPSRAM_BASE();

    for(uint32_t i = 0; i < 2048; i++) {
        base_pointer[i] = 0;
    }

    return SATR_OK;
}

void bkup_sram_INIT() {

    uint8_t *base_pointer = (uint8_t*)HAL_obc_BKPSRAM_BASE();

    obc_data.log_cnt = (uint32_t*)base_pointer;
    base_pointer += sizeof(uint32_t);

    obc_data.log_state = (uint32_t*)base_pointer;
    base_pointer += sizeof(uint32_t);

    sys_data.boot_counter = (uint32_t*)base_pointer;
    base_pointer += sizeof(uint32_t);

    obc_data.wod_cnt = (uint32_t*)base_pointer;
    base_pointer += sizeof(uint32_t);

    obc_data.comms_boot_cnt = (uint16_t*)base_pointer;
    base_pointer += sizeof(uint16_t);

    obc_data.eps_boot_cnt = (uint16_t*)base_pointer;
    base_pointer += sizeof(uint16_t);

    obc_data.comms_tick = (uint32_t*)base_pointer;
    base_pointer += sizeof(uint32_t);

    obc_data.eps_tick = (uint32_t*)base_pointer;
    base_pointer += sizeof(uint32_t);

    MNLP_data.su_init_func_run_time = (uint32_t *) base_pointer; //264; //265;
    base_pointer += sizeof(uint32_t);

    MNLP_data.tt_perm_norm_exec_count = (uint16_t *) base_pointer;
    base_pointer += sizeof(uint16_t);

    MNLP_data.tt_perm_exec_on_span_count = (uint16_t *) base_pointer;
    base_pointer += sizeof(uint16_t);

    obc_data.fs_su_head = (uint16_t*)base_pointer;
    base_pointer += sizeof(uint16_t);

    obc_data.fs_wod_head = (uint16_t*)base_pointer;
    base_pointer += sizeof(uint16_t);

    obc_data.fs_ext_head = (uint16_t*)base_pointer;
    base_pointer += sizeof(uint16_t);

    obc_data.fs_ev_head = (uint16_t*)base_pointer;
    base_pointer += sizeof(uint16_t);

    obc_data.fs_su_tail = (uint16_t*)base_pointer;
    base_pointer += sizeof(uint16_t);

    obc_data.fs_wod_tail = (uint16_t*)base_pointer;
    base_pointer += sizeof(uint16_t);

    obc_data.fs_ext_tail = (uint16_t*)base_pointer;
    base_pointer += sizeof(uint16_t);

    obc_data.fs_ev_tail = (uint16_t*)base_pointer;
    base_pointer += sizeof(uint16_t);

    obc_data.fs_fotos = (uint16_t*)base_pointer;
    base_pointer += sizeof(uint16_t);

    MNLP_data.su_nmlp_script_scheduler_active =  (uint8_t*) base_pointer;
    base_pointer += sizeof(uint8_t);

    MNLP_data.su_service_scheduler_active = (uint8_t*) base_pointer;
    base_pointer += sizeof(uint8_t);
    
    MNLP_data.su_nmlp_last_active_script = (uint8_t*) base_pointer;
    base_pointer += sizeof(uint8_t);
    
    obc_data.log = (uint8_t *)base_pointer;
    base_pointer += sizeof(uint8_t);
    
    obc_data.wod_log = (uint8_t *)base_pointer + (EV_MAX_BUFFER);
    base_pointer += sizeof(uint8_t);

    if(!C_ASSERT(*obc_data.log_cnt < EV_MAX_BUFFER) == true)      { *obc_data.log_cnt = 0; }
    if(!C_ASSERT(*obc_data.wod_cnt < EV_MAX_BUFFER) == true)      { *obc_data.wod_cnt = 0; }
    if(!C_ASSERT(*obc_data.fs_su_head < MS_MAX_FILES) == true)    { *obc_data.fs_su_head = 1; }
    if(!C_ASSERT(*obc_data.fs_wod_head < MS_MAX_FILES) == true)   { *obc_data.fs_wod_head = 1; }
    if(!C_ASSERT(*obc_data.fs_ext_head < MS_MAX_FILES) == true)   { *obc_data.fs_ext_head = 1; }
    if(!C_ASSERT(*obc_data.fs_ev_head < MS_MAX_FILES) == true)    { *obc_data.fs_ev_head = 1; }
    if(!C_ASSERT(*obc_data.fs_su_tail < MS_MAX_FILES) == true)    { *obc_data.fs_su_tail = 1; }
    if(!C_ASSERT(*obc_data.fs_wod_tail < MS_MAX_FILES) == true)   { *obc_data.fs_wod_tail = 1; }
    if(!C_ASSERT(*obc_data.fs_ext_tail < MS_MAX_FILES) == true)   { *obc_data.fs_ext_tail = 1; }
    if(!C_ASSERT(*obc_data.fs_ev_tail < MS_MAX_FILES) == true)    { *obc_data.fs_ev_tail = 1; }
    if(!C_ASSERT(*obc_data.fs_fotos < MS_MAX_FILES) == true)      { *obc_data.fs_fotos = 1; }
    if(!C_ASSERT( (*MNLP_data.su_nmlp_script_scheduler_active == false) ||
                  (*MNLP_data.su_nmlp_script_scheduler_active == true) ) == true) {*MNLP_data.su_nmlp_script_scheduler_active = true; }
    if(!C_ASSERT( (*MNLP_data.su_service_scheduler_active == false ) || 
                  (*MNLP_data.su_service_scheduler_active == true) ) == true) { *MNLP_data.su_service_scheduler_active = true; }
    if(!C_ASSERT(*MNLP_data.su_nmlp_last_active_script <= SU_SCRIPT_7 ) == true) { *MNLP_data.su_nmlp_last_active_script = SU_NOSCRIPT; }
}

SAT_returnState event_log(uint8_t *buf, const uint16_t size) {
  
    uint32_t tmp_time;

    union _cnv cnv;

    get_time_QB50(&tmp_time);

    cnv.cnv32 = tmp_time;

    for(uint16_t i = 0; i < 4; i++) {
        buf[i + 6] = cnv.cnv8[i];
    }

    for(uint16_t i = 0; i < size; i++) {
        obc_data.log[*obc_data.log_cnt] = buf[i];
        (*obc_data.log_cnt)++;
        if(*obc_data.log_cnt >= EV_MAX_BUFFER) { *obc_data.log_cnt = 0; }
    }

    return SATR_OK;
}

SAT_returnState event_log_load(uint8_t *buf, const uint16_t pointer, const uint16_t size) {
   
   if(!C_ASSERT(size < EV_MAX_BUFFER) == true) { return SATR_ERROR; }

   for(uint16_t i = 0; i < size; i++) {
        buf[i] = obc_data.log[i];
   }
   return SATR_OK;
}

    uint16_t cnt = 0;
    uint8_t buf[TEST_ARRAY];

SAT_returnState event_log_IDLE() {

    if(!C_ASSERT(*obc_data.log_state < LAST_EV_STATE) == true) { *obc_data.log_state = EV_P0; }

    int16_t len = *obc_data.log_cnt - (*obc_data.log_state * EV_BUFFER_PART);
    if(len < 0) { len = EV_MAX_BUFFER - len; }
    if(len > EV_BUFFER_PART) {

        uint16_t size = EV_BUFFER_PART;

        mass_storage_storeFile(EVENT_LOG, 0, &obc_data.log[*obc_data.log_state * EV_BUFFER_PART], &size);
        if(++(*obc_data.log_state) > EV_P4) { *obc_data.log_state = EV_P0; }

    } 
    
     return SATR_OK;
}

SAT_returnState wod_log() {

//check endianess
    obc_data.wod_log[*obc_data.wod_cnt] = sat_status.temp_batt;
    (*obc_data.wod_cnt)++;
    if(*obc_data.wod_cnt >= WOD_MAX_BUFFER) { *obc_data.wod_cnt = 0; }

    obc_data.wod_log[*obc_data.wod_cnt] = sat_status.temp_eps;
    (*obc_data.wod_cnt)++;
    if(*obc_data.wod_cnt >= WOD_MAX_BUFFER) { *obc_data.wod_cnt = 0; }

    obc_data.wod_log[*obc_data.wod_cnt] = sat_status.temp_comms;
    (*obc_data.wod_cnt)++;
    if(*obc_data.wod_cnt >= WOD_MAX_BUFFER) { *obc_data.wod_cnt = 0; }

    obc_data.wod_log[*obc_data.wod_cnt] = sat_status.bus_5v_curr;
    (*obc_data.wod_cnt)++;
    if(*obc_data.wod_cnt >= WOD_MAX_BUFFER) { *obc_data.wod_cnt = 0; }

    obc_data.wod_log[*obc_data.wod_cnt] = sat_status.bus_3v3_curr;
    (*obc_data.wod_cnt)++;
    if(*obc_data.wod_cnt >= WOD_MAX_BUFFER) { *obc_data.wod_cnt = 0; }

    obc_data.wod_log[*obc_data.wod_cnt] = sat_status.batt_curr;
    (*obc_data.wod_cnt)++;
    if(*obc_data.wod_cnt >= WOD_MAX_BUFFER) { *obc_data.wod_cnt = 0; }

    obc_data.wod_log[*obc_data.wod_cnt] = sat_status.batt_volt;
    (*obc_data.wod_cnt)++;
    if(*obc_data.wod_cnt >= WOD_MAX_BUFFER) { *obc_data.wod_cnt = 0; }

    return SATR_OK;
}

SAT_returnState wod_log_load(uint8_t *buf) {

   uint8_t rev_wod_cnt = *obc_data.wod_cnt;
   if(rev_wod_cnt == 0) { rev_wod_cnt = WOD_MAX_BUFFER; }

   uint16_t buf_cnt = 0;
   for(uint16_t i = 0; i < WOD_MAX_BUFFER; i++) {

        rev_wod_cnt--;
        buf[buf_cnt++] = obc_data.wod_log[rev_wod_cnt]; 
        if(rev_wod_cnt == 0) { rev_wod_cnt = WOD_MAX_BUFFER; }
   }
   return SATR_OK;
}

void timeout_start_IAC() {
    uint32_t t = HAL_sys_GetTick();
    obc_data.iac_timeout = t;
}

void timeout_stop_IAC() {
    obc_data.iac_timeout = 0;
}

SAT_returnState check_subsystems_timeouts(uint32_t sys_t_now) {
    
    if(obc_data.iac_timeout != 0 && ((sys_t_now - obc_data.iac_timeout) >= TIMEOUT_V_IAC)) { 
        /*Handle IAC subsystem's timeout*/
        power_control_api(IAC_DEV_ID, P_OFF);
        obc_data.iac_timeout = 0;
    }

    return SATR_OK;
}

void set_reset_source(const uint8_t rsrc) {
    sys_data.rsrc = rsrc;
}

void get_reset_source(uint8_t *rsrc) {
    *rsrc = sys_data.rsrc;
}

void update_boot_counter() {
    (*sys_data.boot_counter)++;
}

void get_boot_counter(uint32_t *cnt) {
    *cnt = *sys_data.boot_counter;
}

void update_eps_boot_counter(uint32_t tick) {
    if(tick != 0 && tick < *obc_data.eps_tick) {
        (*obc_data.eps_boot_cnt)++;
    }
    *obc_data.eps_tick = tick;
}

void update_comms_boot_counter(uint32_t tick) {
    if(tick != 0 && tick < *obc_data.comms_tick) {
        (*obc_data.comms_boot_cnt)++;
    }
    *obc_data.comms_tick = tick;
}
