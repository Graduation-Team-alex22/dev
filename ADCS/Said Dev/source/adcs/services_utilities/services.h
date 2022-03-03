#ifndef SERVICES_H__
#define SERVICES_H__

#include "services_utilities.h"

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


#endif
