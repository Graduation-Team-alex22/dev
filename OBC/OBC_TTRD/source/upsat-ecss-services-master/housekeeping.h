#ifndef __HOUSEKEEPING_H
#define __HOUSEKEEPING_H

#include <stdint.h>
#include "services.h"

struct _sat_status {
    uint8_t batt_curr;
    uint8_t batt_volt;
    uint8_t bus_3v3_curr;
    uint8_t bus_5v_curr;
    uint8_t temp_eps;
    uint8_t temp_batt;
    uint8_t temp_comms;
};

extern struct _sat_status sat_status;

void hk_INIT(void);

void EPS_Health_Req_SCH(void);

void COMMS_Health_Req_SCH(void);

void COMMS_WOD_Rep_SCH(void);

void EPS_exHealth_Req_SCH(void);

void COMMS_exHealth_Req_SCH(void);

void ADCS_exHealth_Req_SCH(void);

void COMMS_EXT_WOD_Rep_SCH(void);

void clear_wod(void);

void clear_ext_wod(void);

SAT_returnState hk_parameters_report(TC_TM_app_id app_id, HK_struct_id sid, uint8_t *data, uint8_t len);

SAT_returnState hk_report_parameters(HK_struct_id sid, tc_tm_pkt *pkt);

#endif
