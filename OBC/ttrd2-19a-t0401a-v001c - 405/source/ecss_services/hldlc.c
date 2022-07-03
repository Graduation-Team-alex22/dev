#include "hldlc.h"
#include "cubesat_mod.h"


#undef __FILE_ID__
#define __FILE_ID__ 36


// removing 0x7E from the start and the end of the packet
SAT_returnState HLDLC_deframe(uint8_t *buf_in, uint8_t *buf_out, uint16_t *size) {

    if((!C_ASSERT(buf_in != 0 && buf_out != 0)) == true)    { return SATR_ERROR; }
    if((!C_ASSERT(buf_in[0] == HLDLC_START_FLAG)) == true)  { return SATR_ERROR; } /*the first char should be a start flag*/
    if((!C_ASSERT(*size <= UART_BUF_SIZE)) == true)         { return SATR_ERROR; } //hard limits, check

    uint16_t cnt = 0;

    for(uint16_t i = 1; i < *size; i++) {
        if(buf_in[i] == HLDLC_START_FLAG) {
            *size = cnt;
            return SATR_EOT;
        } else if(buf_in[i] == HLDLC_CONTROL_FLAG) {
            i++;
            if((!C_ASSERT(i < (*size) - 1)) == true)       { return SATR_ERROR; }
            if(buf_in[i] == 0x5E) { buf_out[cnt++] = 0x7E; }
            else if(buf_in[i] == 0x5D) { buf_out[cnt++] = 0x7D; }
            else { return SATR_ERROR; }
        } else {
            buf_out[cnt++] = buf_in[i];
        }
    }
    return SATR_ERROR;
}

// adding 0x7E to the start and the end of the packet   --- 0x7E packet 0x7E ----
SAT_returnState HLDLC_frame(uint8_t *buf_in, uint8_t *buf_out, uint16_t *size) {

    if((!C_ASSERT(buf_in != NULL && buf_out != NULL && size != NULL)) == true) { return SATR_ERROR; }
    if((!C_ASSERT(*size <= MAX_PKT_SIZE)) == true)                      { return SATR_ERROR; }

    uint16_t cnt = 2;

    for(uint16_t i = 0; i < *size; i++) {
        if(i == 0) {
            buf_out[0] = HLDLC_START_FLAG;
            buf_out[1] = buf_in[0];
        } else if(i == (*size) - 1) {
            if(buf_in[i] == HLDLC_START_FLAG) {
                buf_out[cnt++] = HLDLC_CONTROL_FLAG;
                buf_out[cnt++] = 0x5E;
            } else if(buf_in[i] == HLDLC_CONTROL_FLAG) {
                buf_out[cnt++] = HLDLC_CONTROL_FLAG;
                buf_out[cnt++] = 0x5D;
            } else { buf_out[cnt++] = buf_in[i]; }
            buf_out[cnt++] = HLDLC_START_FLAG;
            *size = cnt;
            return SATR_EOT;
        } else if(buf_in[i] == HLDLC_START_FLAG) {
            buf_out[cnt++] = HLDLC_CONTROL_FLAG;
            buf_out[cnt++] = 0x5E;
        } else if(buf_in[i] == HLDLC_CONTROL_FLAG) {
            buf_out[cnt++] = HLDLC_CONTROL_FLAG;
            buf_out[cnt++] = 0x5D;
        } else {
            buf_out[cnt++] = buf_in[i];
        }

    }

    return SATR_ERROR;
}
