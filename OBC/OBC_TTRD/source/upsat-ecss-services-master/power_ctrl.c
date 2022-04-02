#include "power_ctrl.h"
#include "obc.h"

#undef __FILE_ID__
#define __FILE_ID__ 14

/*Must use real pins*/
SAT_returnState power_control_api(FM_dev_id did, FM_fun_id fid, uint8_t *state) {

    if(!C_ASSERT(did == OBC_SD_DEV_ID || did == IAC_DEV_ID) == true)     { return SATR_ERROR; }
    if(!C_ASSERT(fid == P_OFF || fid == P_ON || fid == P_RESET) == true) { return SATR_ERROR; }

    if(did == OBC_SD_DEV_ID && fid == P_ON)         { HAL_obc_SD_ON(); }
    else if(did == OBC_SD_DEV_ID && fid == P_OFF)   { HAL_obc_SD_OFF(); }
 
    return SATR_OK;
}