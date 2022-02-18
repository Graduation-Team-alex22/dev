#ifndef __HOUSEKEEPING_H
#define __HOUSEKEEPING_H

#include <stdint.h>
#include "../../../ecss_services/services/services.h"
#include "adcs.h"

extern void HAL_sys_delay(uint32_t sec);

extern SAT_returnState hk_crt_pkt_TC(tc_tm_pkt *pkt, TC_TM_app_id app_id,
        HK_struct_id sid);
extern SAT_returnState hk_crt_pkt_TM(tc_tm_pkt *pkt, TC_TM_app_id app_id,
        HK_struct_id sid);

SAT_returnState hk_parameters_report(TC_TM_app_id app_id, HK_struct_id sid,
        uint8_t *data, uint8_t len);

SAT_returnState hk_report_parameters(HK_struct_id sid, tc_tm_pkt *pkt);

#endif
