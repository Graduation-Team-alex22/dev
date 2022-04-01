#ifndef __SERVICES_H
#define __SERVICES_H

/* 
	This file includes also subsystems_ids.h, ecss_stats.h, flash.h, hldlc.h, housekeeping_services.h, 
	housekeeping.h, queue.h, service_utilities.h, status.h, system.h, test_service.h, verification_service.h, utils.h, 
	log.h, persistent_mem.h, packet_pool.h, power_ctrl.h
*/
// Processor Header
#include "../main/main.h"
//////////////////////////////////////////////// STRING ////////////////////////////////////

size_t	 strnlen (const char *s, size_t maxlen);

//////////////////////////////////////////////// STDBOOL ////////////////////////////////////
#define bool	_Bool
#define true	1
#define false	0
//////////////////////////////////////////////// SYSTEM ////////////////////////////////////
#define SYSTEM_APP_ID _COMMS_APP_ID_
#define POOL_PKT_EXT

#define SYSTEM_ENDIANNESS SYS_LITTLE_ENDIAN

//////////////////////////////////////////////// SERVICES ////////////////////////////////////

#define _OBC_APP_ID_   1
#define _EPS_APP_ID_   2
#define _ADCS_APP_ID_  3
#define _COMMS_APP_ID_ 4
#define _IAC_APP_ID_   5
#define _GND_APP_ID_   6
#define _DBG_APP_ID_   7
#define _LAST_APP_ID_  8


/* TM TC services*/
#define ECSS_VER_NUMBER             0
#define ECSS_DATA_FIELD_HDR_FLG     1

#define ECSS_PUS_VER            1
#define ECSS_SEC_HDR_FIELD_FLG  0

/*sequence definitions*/
/*we only support TC_TM_SEQ_SPACKET*/
#define TC_TM_SEQ_FPACKET 0x01
#define TC_TM_SEQ_CPACKET 0x00
#define TC_TM_SEQ_LPACKET 0x02
#define TC_TM_SEQ_SPACKET 0x03

/*services ack req*/
/*should confirm endianess*/
#define TC_ACK_NO           0x00
#define TC_ACK_ACC          0x01
#define TC_ACK_EXE_START    0x02
#define TC_ACK_EXE_PROG     0x04
#define TC_ACK_EXE_COMP     0x08
#define TC_ACK_ALL          0x0F

#define ECSS_HEADER_SIZE        6
#define ECSS_DATA_HEADER_SIZE   4
#define ECSS_CRC_SIZE           2

#define ECSS_DATA_OFFSET        10  /*ECSS_HEADER_SIZE + ECSS_DATA_HEADER_SIZE*/

#define MAX_APP_ID      20
#define MAX_SERVICES    20
#define MAX_SUBTYPES    26

#define TC 1
#define TM 0

//needs to redifine
#define MAX_PKT_LEN         210 /*ECSS_HEADER_SIZE + ECSS_DATA_HEADER_SIZE + MAX_PKT_DATA + ECSS_CRC_SIZE*/

#define MAX_PKT_DATA        198
#define TC_MAX_PKT_SIZE     210
#define TC_MIN_PKT_SIZE     11 //12  /*ECSS_HEADER_SIZE + ECSS_DATA_HEADER_SIZE + ECSS_CRC_SIZE*/

#define MAX_PKT_EXT_DATA    2051
#define TC_MAX_PKT_EXT_SIZE 2063

#if (SYSTEM_APP_ID == _EPS_APP_ID_)
#define MAX_PKT_SIZE  210
#else
#define MAX_PKT_SIZE  2063
#endif

#define MS_SU_LOG_SIZE  196
#define MS_WOD_LOG_SIZE  228



typedef enum {
    OBC_APP_ID      = _OBC_APP_ID_,
    EPS_APP_ID      = _EPS_APP_ID_,
    ADCS_APP_ID     = _ADCS_APP_ID_,
    COMMS_APP_ID    = _COMMS_APP_ID_,
    IAC_APP_ID      = _IAC_APP_ID_,
    GND_APP_ID      = _GND_APP_ID_,
    DBG_APP_ID      = _DBG_APP_ID_,
    LAST_APP_ID     = _LAST_APP_ID_
}TC_TM_app_id;

typedef enum {
    SATR_PKT_ILLEGAL_APPID     = 0,
    SATR_PKT_INV_LEN           = 1,
    SATR_PKT_INC_CRC           = 2,
    SATR_PKT_ILLEGAL_PKT_TP    = 3,
    SATR_PKT_ILLEGAL_PKT_STP   = 4,
    SATR_PKT_ILLEGAL_APP_DATA  = 5,
    SATR_OK                    = 6,
    SATR_ERROR                 = 7,
    SATR_EOT                   = 8,
    SATR_CRC_ERROR             = 9,
    SATR_PKT_ILLEGAL_ACK       = 10,
    SATR_ALREADY_SERVICING     = 11,
    SATR_MS_MAX_FILES          = 12,
    SATR_PKT_INIT              = 13,
    SATR_INV_STORE_ID          = 14,
    SATR_INV_DATA_LEN          = 15,
    /* Scheduling Service Error State Codes
    * from
    */
    SATR_SCHS_FULL             = 16, /*Schedule array is full */
    SATR_SCHS_ID_INVALID       = 17, /*Sub-schedule ID invalid */
    SATR_SCHS_NMR_OF_TC_INVLD  = 18, /*Number of telecommands invalid */
    SATR_SCHS_INTRL_ID_INVLD   = 19, /*Interlock ID invalid */
    SATR_SCHS_ASS_INTRL_ID_INVLD = 20, /*Assess Interlock ID invalid */
    SATR_SCHS_ASS_TP_ID_INVLD  = 21, /*Assessment type id invalid*/
    SATR_SCHS_RLS_TT_ID_INVLD  = 22, /*Release time type ID invalid */
    SATR_SCHS_DST_APID_INVLD   = 23, /*Destination APID in embedded TC is invalid */
    SATR_SCHS_TIM_INVLD        = 24, /*Release time of TC is invalid */
    SATR_QBTIME_INVALID        = 25, /*Time management reported erroneous time*/
    SATR_SCHS_TIM_SPEC_INVLD   = 26, /*Release time of TC is specified in a invalid representation*/
    SATR_SCHS_INTRL_LGC_ERR    = 27, /*The release time of telecommand is in the execution window of its interlocking telecommand.*/
    SATR_SCHS_DISABLED         = 28,
    SATR_SCHS_NOT_IMLP         = 29, /*Not implemented function of scheduling service*/
    /*FatFs*/
    SATRF_OK                   = 30, /* (0) Succeeded */
    SATRF_DISK_ERR             = 31, /* (1) A hard error occurred in the low level disk I/O layer */
    SATRF_INT_ERR              = 32, /* (2) Assertion failed */
    SATRF_NOT_READY            = 33, /* (3) The physical drive cannot work */
    SATRF_NO_FILE              = 34, /* (4) Could not find the file */
    SATRF_NO_PATH              = 35, /* (5) Could not find the path */
    SATRF_INVALID_NAME         = 36, /* (6) The path name format is invalid */
    SATRF_DENIED               = 37, /* (7) Access denied due to prohibited access or directory full */
    SATRF_EXIST                = 38, /* (8) Access denied due to prohibited access */
    SATRF_INVALID_OBJECT       = 39, /* (9) The file/directory object is invalid */
    SATRF_WRITE_PROTECTED      = 40, /* (10) The physical drive is write protected */
    SATRF_INVALID_DRIVE        = 41, /* (11) The logical drive number is invalid */
    SATRF_NOT_ENABLED          = 42, /* (12) The volume has no work area */
    SATRF_NO_FILESYSTEM        = 43, /* (13) There is no valid FAT volume */
    SATRF_MKFS_ABORTED         = 44, /* (14) The f_mkfs() aborted due to any parameter error */
    SATRF_TIMEOUT              = 45, /* (15) Could not get a grant to access the volume within defined period */
    SATRF_LOCKED               = 46, /* (16) The operation is rejected according to the file sharing policy */
    SATRF_NOT_ENOUGH_CORE      = 47, /* (17) LFN working buffer could not be allocated */
    SATRF_TOO_MANY_OPEN_FILES  = 48, /* (18) Number of open files > _FS_SHARE */
    SATRF_INVALID_PARAMETER    = 49, /* (19) Given parameter is invalid */
    SATRF_DIR_ERROR            = 50,
    SATR_SD_DISABLED           = 51,
    SATR_QUEUE_FULL            = 52,
    SATR_WRONG_DOWNLINK_OFFSET = 53,
    SATR_VER_ERROR             = 54,
	SATR_FIREWALLED            = 55,
    /*LAST*/
    SATR_LAST                  = 56
}SAT_returnState;


/*services types*/
#define TC_VERIFICATION_SERVICE         1
#define TC_HOUSEKEEPING_SERVICE         3
#define TC_EVENT_SERVICE                5
#define TC_FUNCTION_MANAGEMENT_SERVICE  8
#define TC_TIME_MANAGEMENT_SERVICE      9
#define TC_SCHEDULING_SERVICE           11
#define TC_LARGE_DATA_SERVICE           13
#define TC_MASS_STORAGE_SERVICE         15
#define TC_TEST_SERVICE                 17
#define TC_SU_MNLP_SERVICE              18 /*service number out of ECSS standard, mission specific for mnlp su*/

/*services subtypes*/
#define TM_VR_ACCEPTANCE_SUCCESS        1
#define TM_VR_ACCEPTANCE_FAILURE        2

#define TC_HK_REPORT_PARAMETERS         21
#define TM_HK_PARAMETERS_REPORT         23

#define TM_EV_NORMAL_REPORT         	1
#define TM_EV_ERROR_REPORT         	4

#define TC_FM_PERFORM_FUNCTION          1

#define TC_SC_ENABLE_RELEASE            1
#define TC_SC_DISABLE_RELEASE           2
#define TC_SC_RESET_SCHEDULE            3
#define TC_SC_INSERT_TC                 4
#define TC_SC_DELETE_TC                 5
#define TC_SC_TIME_SHIFT_SPECIFIC       7
#define TC_SC_TIME_SHIFT_SELECTED_OTP   8
#define TC_SC_TIME_SHIFT_ALL            15

#define TM_LD_FIRST_DOWNLINK            1
#define TC_LD_FIRST_UPLINK              9
#define TM_LD_INT_DOWNLINK              2
#define TC_LD_INT_UPLINK                10
#define TM_LD_LAST_DOWNLINK             3
#define TC_LD_LAST_UPLINK               11
#define TC_LD_ACK_DOWNLINK              5
#define TM_LD_ACK_UPLINK                14
#define TC_LD_REPEAT_DOWNLINK           6
#define TM_LD_REPEAT_UPLINK             15
#define TM_LD_REPEATED_DOWNLINK         7
#define TC_LD_REPEATED_UPLINK           12
#define TM_LD_ABORT_SE_DOWNLINK         4
#define TC_LD_ABORT_SE_UPLINK           13
#define TC_LD_ABORT_RE_DOWNLINK         8
#define TM_LD_ABORT_RE_UPLINK           16

#define TC_MS_ENABLE                    1
#define TC_MS_DISABLE                   2
#define TM_MS_CONTENT                   8
#define TC_MS_DOWNLINK                  9
#define TC_MS_DELETE                    11
#define TC_MS_REPORT                    12
#define TM_MS_CATALOGUE_REPORT          13
#define TC_MS_UPLINK                    14
#define TC_MS_FORMAT                    15 /*custom service*/
#define TC_MS_LIST                      16 /*custom service*/
#define TM_MS_CATALOGUE_LIST            17

#define TC_CT_PERFORM_TEST              1
#define TM_CT_REPORT_TEST               2

/*mNLP science unit sub-service definitions*/
#define TC_SU_ON                        1  /*subservice 1*/
#define TC_SU_OFF                       2  /*subservice 2*/
#define TC_SU_RESET                     3  /*subservice 3*/
#define TC_SU_LOAD_P                    4  /*subservice 4*/
#define TC_SU_HC                        5  /*subservice 5*/
#define TC_SU_CAL                       6  /*subservice 6*/
#define TC_SU_SCI                       7  /*subservice 7*/
#define TC_SU_HK                        8  /*subservice 8*/
#define TC_SU_STM                       9  /*subservice 9*/
#define TC_SU_DUMP                      10 /*subservice 10*/
#define TC_SU_BIAS_ON                   11 /*subservice 11*/
#define TC_SU_BIAS_OFF                  12 /*subservice 12*/
#define TC_SU_MTEE_ON                   13 /*subservice 13*/
#define TC_SU_MTEE_OFF                  14 /*subservice 14*/
#define TC_SU_SCH_TASK_NTF              15 /*subservice 15*/
#define TC_SU_STATUS_REPORT             16 /*subservice 16*/
#define TM_SU_STATUS_REPORT             17 /*subservice 17*/
#define TC_SU_MANAGE                    18 /*subservice 18*/

/*TIME MANAGEMENT SERVICE*/
#define TM_TIME_SET_IN_UTC              1 /*subservice 1*/
#define TM_TIME_SET_IN_QB50             2 /*subservice 2*/
#define TM_REPORT_TIME_IN_UTC           3 /*subservice 3, Telecommand to report time in UTC*/
#define TM_REPORT_TIME_IN_QB50          4 /*subservice 4, Telecommand to report time in QB50*/
#define TM_TIME_REPORT_IN_UTC           5 /*subservice 5, Telemetry response time in UTC*/
#define TM_TIME_REPORT_IN_QB50          6 /*subservice 6, Telemetry response time in QB50*/

/*SCHEDULING SERVICE*/
#define SCHS_ENABLE_RELEASE             1 /*subservice 01, Telecommand to enable the release of telecommands from schedule pool*/
#define SCHS_DISABLE_RELEASE            2 /*subservice 02, Telecommand to disable the release of telecommands from schedule pool*/
#define SCHS_RESET_SCH                  3 /*subservice 03, Telecommand to reset the schedule pool*/
#define SCHS_INSERT_TC_IN_SCH           4 /*subservice 04, Telecommand to insert a tc_tm_pkt in schedule pool*/
#define SCHS_DELETE_TC_FROM_SCH         5 /*subservice 05, Telecommand to delete a tc_tm_pkt from schedule pool*/
#define SCHS_DELETE_TC_FROM_SCH_OTP     6 /*subservice 06, Telecommand to delete tc_tm_pkts from schedule pool over a time period*/
#define SCHS_TIME_SHIFT_SEL_TC          7 /*subservice 07, Telecommand to time shift (+/-) selected active schedule packet*/
#define SCHS_TIME_SHIFT_SEL_TC_OTP      8 /*subservice 08, Telecommand to time shift (+/-) selected active schedule packets over a time period*/
#define SCHS_DETAILED_SCH_REPORT        10 /*subservice 10, Telemerty response to (to TC no:16) report schedules in detailed form*/
#define SCHS_SIMPLE_SCH_REPORT          13 /*subservice 13, Telemerty response to (to TC no:17) report schedules in summary form*/
#define SCHS_TIME_SHIFT_ALL_TCS         15 /*subservice 15, Telecommand to time shift (+/-) all active schedule packets*/
#define SCHS_REPORT_SCH_DETAILED        16 /*subservice 16, Telecommand to report schedules in detailed form*/
#define SCHS_REPORT_SCH_SUMMARY         17 /*subservice 17, Telecommand to report schedules in summary form*/
#define SCHS_LOAD_SCHEDULES             22 /*subservice 22, Telecommand to load schedules from perm storage*/
#define SCHS_SAVE_SCHEDULES             23 /*subservice 23, Telecommand to save schedules on perm storage*/

typedef enum {
    HEALTH_REP      = 1,
    EX_HEALTH_REP   = 2,
    EVENTS_REP      = 3,
    WOD_REP         = 4,
    EXT_WOD_REP     = 5,
    SU_SCI_HDR_REP  = 6,
    ADCS_TLE_REP    = 7,
    EPS_FLS_REP     = 8,
    ECSS_STATS_REP  = 9,
    LAST_STRUCT_ID  = 10
}HK_struct_id;

typedef enum {
    P_OFF       = 0,
    P_ON        = 1,
    P_RESET     = 2,
    SET_VAL     = 3,
    LAST_FUN_ID = 4
}FM_fun_id;

typedef enum {
    OBC_DEV_ID      = 1,
    EPS_DEV_ID      = 2,
    ADCS_DEV_ID     = 3,
    COMMS_DEV_ID    = 4,
    IAC_DEV_ID      = 5,
    SU_DEV_ID       = 6,
    GPS_DEV_ID      = 7,
    OBC_SD_DEV_ID   = 8,
    ADCS_SD_DEV_ID  = 9,
    ADCS_SENSORS    = 10,
    ADCS_GPS        = 11,
    ADCS_MAGNETO    = 12,
    ADCS_SPIN       = 13,
    ADCS_TLE        = 14,
    ADCS_CTRL_GAIN  = 15,
    ADCS_SET_POINT  = 16,
    EPS_WRITE_FLS   = 17,
    SYS_DBG         = 18,
    COMMS_WOD_PAT   = 19,
    LAST_DEV_ID     = 20
}FM_dev_id;

/*Mass storage ids*/
typedef enum {  
    SU_NOSCRIPT     = 0, /*used when no script has ever been chosen to run, saved on SRAM*/
    SU_SCRIPT_1     = 1,
    SU_SCRIPT_2     = 2,
    SU_SCRIPT_3     = 3,
    SU_SCRIPT_4     = 4,
    SU_SCRIPT_5     = 5,
    SU_SCRIPT_6     = 6,
    SU_SCRIPT_7     = 7,
    SU_LOG          = 8,
    WOD_LOG         = 9,
    EXT_WOD_LOG     = 10,
    EVENT_LOG       = 11,
    FOTOS           = 12,
    SCHS            = 13,
    SRAM            = 14,
    LAST_SID        = 15
}MS_sid;

typedef enum {
    ALL         = 0,
    TO          = 1,
    BETWEEN     = 2,
    SPECIFIC    = 3,
    LAST_PART   = 4,
    NO_MODE     = 5,
    HARD_DELETE = 6,
    DELETE_ALL  = 7,
    FATFS_RESET = 8,
    LAST_MODE   = 9
}MS_mode;

typedef enum {
    SU_POWERED_OFF = 1,
    SU_POWERED_ON  = 2,
    SU_IDLE        = 3,
    SU_FINISHED    = 4,
    LAST_SU_STATE  = 5
}SU_state;

typedef enum {
    EV_inc_pkt           = 1,
    EV_pkt_ack_er        = 2,
    EV_sys_boot          = 3,
    EV_pwr_level         = 4,
    EV_comms_tx_off      = 5,
    EV_sys_timeout       = 6,
    EV_sys_shutdown      = 7,
    EV_assertion         = 8,
    EV_su_error          = 9,
    EV_su_scr_start      = 10,
    EV_pkt_pool_timeout  = 11,
    EV_ms_err            = 12,
    LAST_EV_EVENT        = 13
}EV_event;

#define C_ASSERT(e)    ((e) ? (true) : (tst_debugging(__FILE_ID__, __LINE__, #e)))

union _cnv {
    double cnvD;
    float cnvF;
    uint32_t cnv32;
    uint16_t cnv16[4];
    uint8_t cnv8[8];
};
//getter
extern void HAL_uart_tx(TC_TM_app_id app_id, uint8_t *buf, uint16_t size);
//setter
extern SAT_returnState event_log(uint8_t *buf, const uint16_t size);
//missing
extern SAT_returnState event_crt_pkt_api(uint8_t *buf, uint8_t *f, uint16_t fi, uint32_t l, uint8_t *e, uint16_t *size, SAT_returnState mode);


typedef struct {
    /* packet id */
    //uint8_t ver; /* 3 bits, should be equal to 0 */

    //uint8_t data_field_hdr; /* 1 bit, data_field_hdr exists in data = 1 */
    TC_TM_app_id app_id; /* TM: app id = 0 for time packets, = 0xff for idle packets. should be 11 bits only 8 are used though */
    uint8_t type; /* 1 bit, tm = 0, tc = 1 */

    /* packet sequence control */
    uint8_t seq_flags; /* 3 bits, definition in TC_SEQ_xPACKET */
    uint16_t seq_count; /* 14 bits, packet counter, should be unique for each app id */

    uint16_t len; /* 16 bits, C = (Number of octets in packet data field) - 1, on struct is the size of data without the headers. on array is with the headers */

    uint8_t ack; /* 4 bits, definition in TC_ACK_xxxx 0 if its a TM */
    uint8_t ser_type; /* 8 bit, service type */
    uint8_t ser_subtype; /* 8 bit, service subtype */

    /*optional*/
    //uint8_t pckt_sub_cnt; /* 8 bits*/
    TC_TM_app_id dest_id;   /*on TC is the source id, on TM its the destination id*/

    uint8_t *data; /* pkt data */

    /*this is not part of the header. it is used from the software and the verification service,
     *when the packet wants ACK.
     *the type is SAT_returnState and it either stores R_OK or has the error code (failure reason).
     *it is initialized as R_ERROR and the service should be responsible to make it R_OK or put the corresponding error.
     */
    SAT_returnState verification_state;
/*  uint8_t padding;  x bits, padding for word alligment */

//  uint16_t crc; /* CRC or checksum, mission specific*/
}tc_tm_pkt;

/*Lookup table that returns if a service with its subtype with TC or TM is supported and valid*/

// SF_S
extern const uint8_t services_verification_TC_TM[MAX_SERVICES][MAX_SUBTYPES][2];
extern uint8_t assertion_last_file;
extern uint16_t assertion_last_line;
uint8_t tst_debugging(uint16_t fi, uint32_t l, char *e);


/////////////////////////////////////////////////// ECSS_STATS /////////////////////////////////////////////////
//Init

//Update

//Setter
void stats_inbound(uint8_t type, TC_TM_app_id app_id, TC_TM_app_id dest_id, tc_tm_pkt *pkt);

void stats_outbound(uint8_t type, TC_TM_app_id app_id, TC_TM_app_id dest_id, tc_tm_pkt *pkt);

void stats_dropped_hldlc();

void stats_dropped_upack();

// SF_G
uint16_t ecss_stats_hk(uint8_t *buffer);

////////////////////////////////////////////////// FLASH /////////////////////////////////
//Init
uint32_t flash_INIT();

//Update

// SF_G
uint32_t flash_read_trasmit(size_t offset);

// SF_S
void flash_write_trasmit(uint32_t data, size_t offset);
/////////////////////////////////////////////// HLDLC /////////////////////////////////////////////
#define HLDLC_START_FLAG        0x7E
#define HLDLC_CONTROL_FLAG      0x7D

//TF
SAT_returnState HLDLC_deframe(uint8_t *buf_in, uint8_t *buf_out, uint16_t *size);

SAT_returnState HLDLC_frame(uint8_t *buf_in, uint8_t *buf_out, uint16_t *size);
////////////////////////////////////////////// HOUSEKEEPING_SERVICES ////////////////////////////////////

#define OBC_EXT_WOD_SIZE      50
#define OBC_EXT_WOD_OFFSET     1
#define COMMS_EXT_WOD_SIZE    29
#define COMMS_EXT_WOD_OFFSET  51
#define ADCS_EXT_WOD_SIZE     83
#define ADCS_EXT_WOD_OFFSET   80
#define EPS_EXT_WOD_SIZE      34
#define EPS_EXT_WOD_OFFSET   163
#define SYS_EXT_WOD_SIZE     196
#define SUB_SYS_EXT_WOD_SIZE 146
#define SCI_REP_SIZE		  18
#define SCI_ARR_OFFSET		   4
#define ADCS_EXT_SCI_OFFSET	  18

//Setters
// S
SAT_returnState hk_app(tc_tm_pkt *pkt);
// SF_S
SAT_returnState hk_crt_pkt_TC(tc_tm_pkt *pkt, const TC_TM_app_id app_id, const HK_struct_id sid);
// SF_S
SAT_returnState hk_crt_pkt_TM(tc_tm_pkt *pkt, const TC_TM_app_id app_id, const HK_struct_id sid);
// S
SAT_returnState hk_crt_empty_pkt_TM(tc_tm_pkt **pkt, const TC_TM_app_id app_id, const HK_struct_id sid);

////////////////////////////////////////////////// HOUSEKEEPING ////////////////////////////////////////////////
//SF_S
SAT_returnState hk_parameters_report(TC_TM_app_id app_id, HK_struct_id sid, uint8_t *data, uint8_t len);
SAT_returnState hk_report_parameters(HK_struct_id sid, tc_tm_pkt *pkt);

//extern void HAL_sys_delay(uint32_t sec); // REMOVE

////////////////////////////////////////////////// QUEUE ////////////////////////////////////////////////

// Init


// Update


// Setters
// S
tc_tm_pkt * queuePop(TC_TM_app_id app_id);
// SF_S
void queue_IDLE(TC_TM_app_id app_id);


// Getters
// G
SAT_returnState queuePush(tc_tm_pkt *pkt, TC_TM_app_id app_id);
// SF_G
uint8_t queueSize(TC_TM_app_id app_id);
// G
tc_tm_pkt * queuePeak(TC_TM_app_id app_id);


////////////////////////////////////////////////// SERVICE_UTILITIES ////////////////////////////////////////////////

// Init


// Update


// Setters
SAT_returnState crt_pkt(tc_tm_pkt *pkt, TC_TM_app_id app_id, uint8_t type, uint8_t ack, uint8_t ser_type, uint8_t ser_subtype, TC_TM_app_id dest_id);



// Getters
SAT_returnState checkSum(const uint8_t *data, const uint16_t size, uint8_t *res_crc);


//SF_TF
SAT_returnState unpack_pkt(const uint8_t *buf, tc_tm_pkt *pkt, const uint16_t size);

SAT_returnState pack_pkt(uint8_t *buf, tc_tm_pkt *pkt, uint16_t *size);
 
void cnv32_8(const uint32_t from, uint8_t *to);

void cnv16_8(const uint16_t from, uint8_t *to);

void cnv8_32(uint8_t *from, uint32_t *to);

void cnv8_16(uint8_t *from, uint16_t *to);

void cnv8_16LE(uint8_t *from, uint16_t *to);

void cnvF_8(const float from, uint8_t *to);

void cnv8_F(uint8_t *from, float *to);

void cnvD_8(const double from, uint8_t *to);

void cnv8_D(uint8_t *from, double *to);

/*
// not used + missing part of code (#define ...)
uint16_t htons(uint16_t x);
uint16_t ntohs(uint16_t x);
*/

////////////////////////////////////////////////// STATUS ////////////////////////////////////////////////

enum {
  COMMS_STATUS_DMA_ERROR = -9,
  COMMS_STATUS_RF_OFF = -8,
  COMMS_STATUS_RF_SWITCH_CMD = -7,
  COMMS_STATUS_RXFIFO_ERROR = -6,
  COMMS_STATUS_INVALID_FRAME = -5,
  COMMS_STATUS_TIMEOUT = -4,
  COMMS_STATUS_NO_DATA = -3,
  COMMS_STATUS_BUFFER_OVERFLOW = -2,
  COMMS_STATUS_BUFFER_UNDERFLOW = -1,
  COMMS_STATUS_OK = 0,
};

////////////////////////////////////////////////// TEST_SERVICE ////////////////////////////////////////////////

// Init


// Update


// Setters



// Getters
SAT_returnState test_app(tc_tm_pkt *pkt);
// SF_G
SAT_returnState test_crt_pkt(tc_tm_pkt **pkt, TC_TM_app_id dest_id);

////////////////////////////////////////////////// VERIFICATION_SERVICE ////////////////////////////////////////////////

// Init


// Update


// Setters



// Getters
SAT_returnState verification_app(const tc_tm_pkt *pkt);
// SF_G
SAT_returnState verification_crt_pkt(const tc_tm_pkt *pkt, tc_tm_pkt **out, SAT_returnState res);

////////////////////////////////////////////////// UTILS ////////////////////////////////////////////////
/**
 * Lookup table for the CCITT CRC16
 */
 //SF
static const uint16_t crc16_ccitt_table_reverse[256] =
  { 0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF, 0x8C48,
      0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7, 0x1081, 0x0108,
      0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E, 0x9CC9, 0x8D40, 0xBFDB,
      0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876, 0x2102, 0x308B, 0x0210, 0x1399,
      0x6726, 0x76AF, 0x4434, 0x55BD, 0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E,
      0xFAE7, 0xC87C, 0xD9F5, 0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E,
      0x54B5, 0x453C, 0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD,
      0xC974, 0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
      0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3, 0x5285,
      0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A, 0xDECD, 0xCF44,
      0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72, 0x6306, 0x728F, 0x4014,
      0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9, 0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5,
      0xA96A, 0xB8E3, 0x8A78, 0x9BF1, 0x7387, 0x620E, 0x5095, 0x411C, 0x35A3,
      0x242A, 0x16B1, 0x0738, 0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862,
      0x9AF9, 0x8B70, 0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E,
      0xF0B7, 0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
      0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036, 0x18C1,
      0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E, 0xA50A, 0xB483,
      0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5, 0x2942, 0x38CB, 0x0A50,
      0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD, 0xB58B, 0xA402, 0x9699, 0x8710,
      0xF3AF, 0xE226, 0xD0BD, 0xC134, 0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7,
      0x6E6E, 0x5CF5, 0x4D7C, 0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1,
      0xA33A, 0xB2B3, 0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72,
      0x3EFB, 0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
      0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A, 0xE70E,
      0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1, 0x6B46, 0x7ACF,
      0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9, 0xF78F, 0xE606, 0xD49D,
      0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330, 0x7BC7, 0x6A4E, 0x58D5, 0x495C,
      0x3DE3, 0x2C6A, 0x1EF1, 0x0F78 };
//TF
static const uint16_t crc16_ccitt_table[256] =
  { 0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 0x8108,
      0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF, 0x1231, 0x0210,
      0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6, 0x9339, 0x8318, 0xB37B,
      0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE, 0x2462, 0x3443, 0x0420, 0x1401,
      0x64E6, 0x74C7, 0x44A4, 0x5485, 0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE,
      0xF5CF, 0xC5AC, 0xD58D, 0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6,
      0x5695, 0x46B4, 0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D,
      0xC7BC, 0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
      0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B, 0x5AF5,
      0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, 0xDBFD, 0xCBDC,
      0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A, 0x6CA6, 0x7C87, 0x4CE4,
      0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, 0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD,
      0xAD2A, 0xBD0B, 0x8D68, 0x9D49, 0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13,
      0x2E32, 0x1E51, 0x0E70, 0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A,
      0x9F59, 0x8F78, 0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E,
      0xE16F, 0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
      0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E, 0x02B1,
      0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256, 0xB5EA, 0xA5CB,
      0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D, 0x34E2, 0x24C3, 0x14A0,
      0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xA7DB, 0xB7FA, 0x8799, 0x97B8,
      0xE75F, 0xF77E, 0xC71D, 0xD73C, 0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657,
      0x7676, 0x4615, 0x5634, 0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9,
      0xB98A, 0xA9AB, 0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882,
      0x28A3, 0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
      0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92, 0xFD2E,
      0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9, 0x7C26, 0x6C07,
      0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1, 0xEF1F, 0xFF3E, 0xCF5D,
      0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, 0x6E17, 0x7E36, 0x4E55, 0x5E74,
      0x2E93, 0x3EB2, 0x0ED1, 0x1EF0 };

// Init


// Update


// Setters



//SF_G
static inline uint16_t
update_crc16_ccitt_reversed (uint16_t crc, const uint8_t *data, size_t len)
{
  size_t i;
  for (i = 0; i < len; i++) {
    crc = (crc >> 8) ^ crc16_ccitt_table_reverse[(crc ^ data[i]) & 0xff];
  }
  return crc;
}
//SF_G
static inline uint16_t
crc16_ccitt_reversed (const uint8_t *data, size_t len)
{
  return update_crc16_ccitt_reversed (0xFFFF, data, len) ^ 0xFFFF;
}
//SF_G
static uint16_t
update_crc16_ccitt (uint16_t crc, const uint8_t *buf, size_t len)
{
  while (len-- != 0) {
    crc = crc16_ccitt_table[((crc >> 8) ^ *buf++) & 0xff] ^ (crc << 8);
  }
  return crc;
}
//SF_G
static inline uint16_t
crc16_ccitt (const uint8_t *data, size_t len)
{
  return update_crc16_ccitt (0x0000, data, len);
}

/**
 * Counts the number of active bits in x
 */
//SF_G
static inline unsigned int
bit_count (unsigned int x)
{
  /*
   * Some more magic from
   * http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
   */
  x = x - ((x >> 1) & 0x55555555);
  x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
  return (((x + (x >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}
//SF_G
static const uint8_t _bytes_reversed[256] =
  { 0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0,
      0x30, 0xB0, 0x70, 0xF0, 0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
      0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 0x04, 0x84, 0x44, 0xC4,
      0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
      0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC,
      0x3C, 0xBC, 0x7C, 0xFC, 0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
      0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 0x0A, 0x8A, 0x4A, 0xCA,
      0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
      0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6,
      0x36, 0xB6, 0x76, 0xF6, 0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
      0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE, 0x01, 0x81, 0x41, 0xC1,
      0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
      0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9,
      0x39, 0xB9, 0x79, 0xF9, 0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
      0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5, 0x0D, 0x8D, 0x4D, 0xCD,
      0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
      0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3,
      0x33, 0xB3, 0x73, 0xF3, 0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
      0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB, 0x07, 0x87, 0x47, 0xC7,
      0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
      0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF,
      0x3F, 0xBF, 0x7F, 0xFF };

/**
 * Reverse the bits of the byte b.
 * @param b the byte to be mirrored.
 */
//TF
static inline uint8_t
reverse_byte (uint8_t b)
{
  return _bytes_reversed[b];
}
//SF_G
static inline uint32_t
reverse_uint32_bytes (uint32_t i)
{
  return (_bytes_reversed[i & 0xff] << 24)
      | (_bytes_reversed[(i >> 8) & 0xff] << 16)
      | (_bytes_reversed[(i >> 16) & 0xff] << 8)
      | (_bytes_reversed[(i >> 24) & 0xff]);
}
//TF
static inline uint32_t
min(uint32_t x, uint32_t y)
{
  return x < y ? x : y;
}
//SF_G
static inline uint32_t
max(uint32_t x, uint32_t y)
{
  return x > y ? x : y;
}
//TF
static inline float
minf(float x, float y)
{
  return x < y ? x : y;
}
//TF
static inline float
maxf(float x, float y)
{
  return x > y ? x : y;
}

////////////////////////////////////////////////// LOG ///////////////////////////////////////////////////////////

#include "config.h"
// SF
static uint8_t _log_uart_buffer[COMMS_UART_BUF_LEN];
static uint8_t _ecss_dbg_buffer[200];
static size_t _ecss_dbg_buffer_len;
//Getter
extern uint8_t dbg_msg;

#if COMMS_UART_DBG_EN

#if COMMS_UART_DEST_OBC
#define LOG_UART_DBG(huart, M, ...)					\
	if(dbg_msg == 1 || dbg_msg == 2) { \
	snprintf(_log_uart_buffer, COMMS_UART_BUF_LEN,			\
			"[DEBUG] %s:%d: " M "\n",			\
			 __FILE__, __LINE__, ##__VA_ARGS__);		\
      event_dbg_api (_ecss_dbg_buffer, _log_uart_buffer,		\
                         &_ecss_dbg_buffer_len);	\
      HAL_uart_tx (DBG_APP_ID, _ecss_dbg_buffer, _ecss_dbg_buffer_len);	} \

#define LOG_UART_ERROR(huart, M, ...) 					\
	if(dbg_msg == 1 || dbg_msg == 2) {\
	snprintf(_log_uart_buffer, COMMS_UART_BUF_LEN, 			\
			"[ERROR] %s:%d: " M "\n",			\
			 __FILE__, __LINE__, ##__VA_ARGS__);		\
      event_dbg_api (_ecss_dbg_buffer, _log_uart_buffer, \
      	&_ecss_dbg_buffer_len);		\
      HAL_uart_tx (DBG_APP_ID, _ecss_dbg_buffer, _ecss_dbg_buffer_len);	}\

#else

#define LOG_UART_DBG(huart, M, ...) 									\
	snprintf(_log_uart_buffer, COMMS_UART_BUF_LEN, 							\
			"[DEBUG] %s:%d: " M "\n",							\
			 __FILE__, __LINE__, ##__VA_ARGS__);						\
	HAL_UART_Transmit (huart, _log_uart_buffer,							\
			   strlen (_log_uart_buffer), COMMS_DEFAULT_TIMEOUT_MS);	\

#define LOG_UART_ERROR(huart, M, ...) 									\
	snprintf(_log_uart_buffer, COMMS_UART_BUF_LEN, 							\
			"[ERROR] %s:%d: " M "\n",							\
			 __FILE__, __LINE__, ##__VA_ARGS__);						\
	HAL_UART_Transmit (huart, _log_uart_buffer,							\
			   strlen (_log_uart_buffer), COMMS_DEFAULT_TIMEOUT_MS);	\

#endif /* COMMS_UART_DEST_OBC */

#else
#define LOG_UART_DBG(huart, M, ...)
#define LOG_UART_ERROR(huart, M, ...)
#endif

////////////////////////////////////////// PERSISTENT_MEM ////////////////////////////////////////////////////////////////
// Init
uint32_t
comms_persistent_mem_init();

//Update


//Setters
void comms_write_persistent_word(uint32_t word, size_t offset);

//Getters
uint32_t comms_read_persistent_word(size_t offset);

//////////////////////////////////////// PACKET_POOL //////////////////////////////////////////////////////////////////////

#define PKT_TIMEOUT 60000 /*in mseconds*/
#define PKT_NORMAL  198   /*MAX_PKT_DATA*/

#if (SYSTEM_APP_ID == _EPS_APP_ID_)
#define POOL_PKT_SIZE        10
#define POOL_PKT_EXT_SIZE     0
#define POOL_PKT_TOTAL_SIZE  10
#else
#define POOL_PKT_SIZE        16
#define POOL_PKT_EXT_SIZE     4
#define POOL_PKT_TOTAL_SIZE  20
#endif

//Init
SAT_returnState pkt_pool_INIT();

//Update


//Getters
tc_tm_pkt * get_pkt(uint16_t size);
uint8_t is_free_pkt(tc_tm_pkt *pkt);

//Setters
SAT_returnState free_pkt(tc_tm_pkt *pkt);
void pkt_pool_IDLE(uint32_t tmp_time);

///////////////////////////////////////////////// POWER_CONTROL ///////////////////////////////////////////////////////////
//Setters
extern void HAL_comms_SD_ON();
extern void HAL_comms_SD_OFF();
//extern void HAL_sys_delay(uint32_t sec); // REMOVE

//SF_S
SAT_returnState power_control_api(FM_dev_id did, FM_fun_id fid, uint8_t *state);


#endif
