#include "housekeeping.h"

#include "housekeeping_service.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#include "obc.h"
#include "time_management_service.h"
#include "mass_storage_service.h"
#include "stm32f4xx.h"
#include "obc_hsi.h"
#include "uart_hsi.h"
#include "ecss_stats.h"
#include "service_utilities.h"

#undef __FILE_ID__
#define __FILE_ID__ 16


// flags to determine if packet is  -received from a subsystem or - 
uint8_t ADCS_EX_WOD_FLAG = false;
uint8_t EPS_EX_WOD_FLAG = false;
uint8_t COMMS_EX_WOD_FLAG = false;

uint8_t EPS_Health_Flag = true;
uint8_t COMMS_Health_Flag = false;
uint8_t COMMS_WOD_Flag = false;
uint8_t EPS_exHealth_Flag = false;
uint8_t ADCS_exHealth_Flag = false;
uint8_t COMMS_exHealth_Flag = false;
uint8_t COMMS_exWOD_Flag = false;

extern SAT_returnState hk_crt_pkt_TC(tc_tm_pkt *pkt, TC_TM_app_id app_id, HK_struct_id sid);
extern SAT_returnState hk_crt_pkt_TM(tc_tm_pkt *pkt, TC_TM_app_id app_id, HK_struct_id sid);

extern void get_time_QB50(uint32_t *qb);
extern SAT_returnState Wod_log(void);
extern SAT_returnState wod_log_load(uint8_t *buf);

struct _sat_status sat_status;

static tc_tm_pkt hk_pkt;
static uint8_t hk_pkt_data[264];
static uint8_t ext_wod_buffer[SYS_EXT_WOD_SIZE];

// sets the buffer that is allocated for the reserved outgoing housekeeping packet
void hk_INIT() {
   hk_pkt.data = hk_pkt_data;
}

void EPS_Health_Req_SCH() {
	 hk_crt_pkt_TC(&hk_pkt, EPS_APP_ID, HEALTH_REP);
   route_pkt(&hk_pkt);
}
		
void COMMS_Health_Req_SCH() {
	 hk_crt_pkt_TC(&hk_pkt, COMMS_APP_ID, HEALTH_REP);
   route_pkt(&hk_pkt);
}
	
void COMMS_WOD_Rep_SCH() {
	 Wod_log();
   clear_wod();
   hk_crt_pkt_TM(&hk_pkt, COMMS_APP_ID, WOD_REP);
   route_pkt(&hk_pkt);	
}
	
void EPS_exHealth_Req_SCH() {
	 hk_crt_pkt_TC(&hk_pkt, EPS_APP_ID, EX_HEALTH_REP);
   route_pkt(&hk_pkt);	
}
	
void COMMS_exHealth_Req_SCH() {
   hk_crt_pkt_TC(&hk_pkt, COMMS_APP_ID, EX_HEALTH_REP);
   route_pkt(&hk_pkt);
}
	
void ADCS_exHealth_Req_SCH() {
   hk_crt_pkt_TC(&hk_pkt, ADCS_APP_ID, EX_HEALTH_REP);
   route_pkt(&hk_pkt);	
}
	
void COMMS_EXT_WOD_Rep_SCH() {
	 hk_crt_pkt_TM(&hk_pkt, COMMS_APP_ID, EXT_WOD_REP);
   route_pkt(&hk_pkt);
  clear_ext_wod();
}

void clear_wod() {
    sat_status.batt_curr = 0;
    sat_status.batt_volt = 0;
    sat_status.bus_3v3_curr = 0;
    sat_status.bus_5v_curr = 0;
    sat_status.temp_eps = 0;
    sat_status.temp_batt = 0;
    sat_status.temp_comms = 0;
}

void clear_ext_wod() {
    memset(ext_wod_buffer, 0, SYS_EXT_WOD_SIZE);
}

SAT_returnState hk_parameters_report(TC_TM_app_id app_id, HK_struct_id sid, uint8_t *data, uint8_t len) {
    
    if(app_id == EPS_APP_ID && sid == HEALTH_REP) {
        sat_status.batt_volt = data[1];
        sat_status.batt_curr = data[2];
        sat_status.bus_3v3_curr = data[3];
        sat_status.bus_5v_curr = data[4];
        sat_status.temp_batt = data[5];
        sat_status.temp_eps = data[6];
			  UART2_BUF_O_Write_String_To_Buffer("H EPS");
    } else if(app_id == COMMS_APP_ID && sid == HEALTH_REP) {
        sat_status.temp_comms = data[1];			  
			  UART2_BUF_O_Write_String_To_Buffer("H COMMS");
    } 
		  else if(app_id == ADCS_APP_ID && sid == EX_HEALTH_REP) {
        
        if((!C_ASSERT(len != ADCS_EXT_WOD_SIZE - 1)) == true) { return SATR_ERROR; }
        
//        for(uint8_t i = 0; i < SCI_REP_SIZE; i++) {
//            su_sci_header[i + SCI_ARR_OFFSET] = data[i + ADCS_EXT_SCI_OFFSET]; 
//        }
        memcpy(&ext_wod_buffer[ADCS_EXT_WOD_OFFSET], &data[1], ADCS_EXT_WOD_SIZE);
        ADCS_EX_WOD_FLAG = true;
				UART2_BUF_O_Write_String_To_Buffer("EX H ADCS");
			}
     else if(app_id == EPS_APP_ID && sid == EX_HEALTH_REP) {
      
        if((!C_ASSERT(len != EPS_EXT_WOD_SIZE - 1)) == true) { return SATR_ERROR; }

        uint32_t tick = 0;

        cnv8_32(&data[1], &tick);
        update_eps_boot_counter(tick);

        memcpy(&ext_wod_buffer[EPS_EXT_WOD_OFFSET], &data[1], EPS_EXT_WOD_SIZE);
        EPS_EX_WOD_FLAG = true;				
        UART2_BUF_O_Write_String_To_Buffer("EX H EPS");

    } else if(app_id == COMMS_APP_ID && sid == EX_HEALTH_REP) {
    
        if((!C_ASSERT(len != COMMS_EXT_WOD_SIZE - 1)) == true) { return SATR_ERROR; }

        uint32_t tick = 0;

        cnv8_32(&data[1], &tick);
        update_comms_boot_counter(tick);
        
        memcpy(&ext_wod_buffer[COMMS_EXT_WOD_OFFSET], &data[1], COMMS_EXT_WOD_SIZE);
				COMMS_EX_WOD_FLAG = true;
        UART2_BUF_O_Write_String_To_Buffer("EX H COMMS");
    
    } else {
        return SATR_ERROR; // this should change to inv pkt
    }

    return SATR_OK;
}

SAT_returnState hk_report_parameters(HK_struct_id sid, tc_tm_pkt *pkt) {

   pkt->data[0] = (HK_struct_id)sid;

   if(sid == EX_HEALTH_REP) 
	 {
			uint16_t size = 1;

			uint32_t sys_epoch = 0;
			uint8_t rsrc = 0;
			uint32_t boot_counter = 0;
			uint8_t ms_res = 0;
			uint8_t ms_status = 0;
			uint16_t ms_l = 0;

			ms_get_state(&ms_res, &ms_status, &ms_l);

			get_time_QB50(&sys_epoch);

			get_reset_source(&rsrc);
			get_boot_counter(&boot_counter);

			cnv32_8( getCurrentTick(), &pkt->data[size]);
			size += 4;

			cnv32_8( sys_epoch, &pkt->data[size]);
			size += 4;

			pkt->data[size] = rsrc;
			size += 1;

			cnv32_8( boot_counter, &pkt->data[size]);
			size += 4;

			cnv32_8( *obc_data.comms_boot_cnt, &pkt->data[size]);
			size += 2;
			cnv32_8( *obc_data.eps_boot_cnt, &pkt->data[size]);
			size += 2;

			pkt->data[size] = assertion_last_file;
			size += 1;
			cnv16_8(assertion_last_line,&pkt->data[size]);
			size += 2;

			cnv16_8(obc_data.vbat, &pkt->data[size]);
			size += 2;

			pkt->data[size] = ms_res;
			size += 1;
			pkt->data[size] = ms_status;
			size += 1;
			cnv16_8((uint16_t)ms_l, &pkt->data[size]);
			size += 2;

			pkt->len = size;
    } 
		else if(sid == WOD_REP) 
		{
			uint32_t time_temp;
			get_time_QB50(&time_temp);

			cnv32_8(time_temp, &pkt->data[1]);
			wod_log_load(&pkt->data[5]);
		
			//uint16_t size = 4+(32*7);
			//mass_storage_storeFile(WOD_LOG, 0, &pkt->data[1], &size);
			pkt->len = 1+4+(32*7);
     }
		 else if(sid == EXT_WOD_REP)
		 {
			 uint16_t size = 1;
			 
			 //*************************************** obc_REP*****************************************//
			 if( (ADCS_EX_WOD_FLAG &&  EPS_EX_WOD_FLAG && COMMS_EX_WOD_FLAG) == true )
			 {				 
					uint32_t sys_epoch = 0;
					uint8_t rsrc = 0;
					uint32_t boot_counter = 0;
					uint8_t ms_res = 0;
					uint8_t ms_status = 0;
					uint16_t ms_l = 0;

					ms_get_state(&ms_res, &ms_status, &ms_l);

					get_time_QB50(&sys_epoch);

					get_reset_source(&rsrc);
					get_boot_counter(&boot_counter);

					cnv32_8( getCurrentTick(), &pkt->data[size]);
					size += 4;

					cnv32_8( sys_epoch, &pkt->data[size]);
					size += 4;

					pkt->data[size] = rsrc;
					size += 1;

					cnv32_8( boot_counter, &pkt->data[size]);
					size += 4;

					//Boot Counter comms
					size += 2;
					//Boot Counter eps
					size += 2;

					pkt->data[size] = assertion_last_file;
					size += 1;
					cnv16_8(assertion_last_line,&pkt->data[size]);
					size += 2;

					cnv16_8(obc_data.vbat, &pkt->data[size]);
					size += 2;

					pkt->data[size] = ms_res;
					size += 1;
					pkt->data[size] = ms_status;
					size += 1;
					cnv16_8((uint16_t)ms_l, &pkt->data[size]);
					size += 2;					
			 }
			 else
			 {
					uint8_t      Boot_Cnt_OBC =94;
					pkt->data[size] = Boot_Cnt_OBC;
					size += 1;
					
					uint16_t      Boot_Cnt_COMMS =285;
					cnv16_8(Boot_Cnt_COMMS, &pkt->data[size]);
					size +=2;	
					
					uint16_t     Boot_Cnt_EPS =49362;
					cnv16_8(Boot_Cnt_EPS, &pkt->data[size]);
					size +=2;	
					
					uint8_t     IAC_State =0;
					pkt->data[size] = IAC_State;
					size += 1;
					
					uint8_t    obc_Last_Assertion_F =1;
					pkt->data[size] = obc_Last_Assertion_F;
					size += 1;
					
					uint8_t     obc_Last_Assertion_L =29;
					pkt->data[size] = obc_Last_Assertion_L;
					size += 1;	
					
					uint8_t    MS_Err_line =0;
					pkt->data[size] = MS_Err_line;
					size += 1;
					
					uint8_t    MS_Last_Err =115;
					pkt->data[size] = MS_Last_Err;
					size += 1;
					
					uint32_t    obc_QB50 =524649046; 
					cnv32_8(obc_QB50, &pkt->data[size]);
					size +=4;
					
					uint8_t    obc_RST_source =7;
					pkt->data[size] = obc_RST_source;
					size += 1;	
					
					double    RTC_VBAT =3.00512672;
					cnvD_8(RTC_VBAT, &pkt->data[size]);
					size +=8;	
					
					uint8_t     SD_Enabled =1;
					pkt->data[size] = SD_Enabled;
					size += 1;
					
					double     SU_Init_Func_Run_time =524625.105;
					cnvD_8(SU_Init_Func_Run_time, &pkt->data[size]);
					size +=8;
					
					uint8_t     SU_Last_Active_script =1;
					pkt->data[size] = SU_Last_Active_script;
					size += 1;	
					
					uint8_t    SU_Script_Sch_Active =1;
					pkt->data[size] = SU_Script_Sch_Active;
					size += 1;	
					
					uint8_t     SU_Service_Sch_Active =0;
					pkt->data[size] = SU_Service_Sch_Active;
					size += 1;	
					
					float    Task_time_HK =27.274;
					cnvF_8(Task_time_HK, &pkt->data[size]);
					size +=4;	
					
					float    Task_time_IDLE =35.57;
					cnvF_8(Task_time_IDLE, &pkt->data[size]);
					size +=4;	
					
					float    Task_time_SCH =27.142;
					cnvF_8(Task_time_SCH, &pkt->data[size]);
					size +=4;	
					
					float    Task_time_SU =0.0;
					cnvF_8(Task_time_SU, &pkt->data[size]);
					size +=4;	
					
					float    Task_time_UART =23942.914;
					cnvF_8(Task_time_UART, &pkt->data[size]);
					size +=4;	
					
					float    obc_Time =23948.706;
					cnvF_8(obc_Time, &pkt->data[size]);
					size +=4;		
					
					uint8_t     tt_perm_exec_on_span_count =1;
					pkt->data[size] = tt_perm_exec_on_span_count;
					size += 1;	
					
					uint8_t    tt_perm_norm_exec_count =20;
					pkt->data[size] = tt_perm_norm_exec_count;
					size += 1;						 
			 }
				
 //*************************************** ADCS_REP*****************************************//
       if( ADCS_EX_WOD_FLAG == true)
			 {
				 memcpy( &pkt->data[size], &ext_wod_buffer[ADCS_EXT_WOD_OFFSET], ADCS_EXT_WOD_SIZE);
				 size += ADCS_EXT_WOD_SIZE;
			 }
			 else
			 {
					uint8_t  Boot_Cnt =74;
					pkt->data[size] = Boot_Cnt;
					size += 1;	
					
					double  ECI_X=633.5;
					cnvD_8(ECI_X, &pkt->data[size]);
					size +=8;
					
					double  ECI_Y=-6093.0; 
					cnvD_8(ECI_Y, &pkt->data[size]);
					size +=8;
					
					double  ECI_Z=2896.5;
					cnvD_8(ECI_Z, &pkt->data[size]);
					size +=8;	
					
					uint8_t  GPS_Sats=255; 
					pkt->data[size] = GPS_Sats;
					size += 1;
					
					uint8_t  GPS_Status=1;
					pkt->data[size] = GPS_Status;
					size += 1;	
					
					uint32_t  GPS_Time=4294967295; 
					cnv32_8(GPS_Time, &pkt->data[size]);
					size +=4;	
					
					uint16_t  GPS_Week=65535;
					cnv16_8(GPS_Week, &pkt->data[size]);
					size +=2;
					
					double  Gyr_X=-0.01;
					cnvD_8(Gyr_X, &pkt->data[size]);
					size +=8;
					
					double  Gyr_Y=0.025; 
					cnvD_8(Gyr_Y, &pkt->data[size]);
					size +=8;
					
					double  Gyr_Z=-0.006; 
					cnvD_8(Gyr_Z, &pkt->data[size]);
					size +=8;
					
					uint8_t  Last_Assertion_F=37;
					pkt->data[size] = Last_Assertion_F;
					size += 1;
					
					uint8_t  Last_Assertion_L=50; 
					pkt->data[size] = Last_Assertion_L;
					size += 1;
					
					uint8_t  MG_Torq_I_Y=0;
					pkt->data[size] = MG_Torq_I_Y;
					size += 1;
					
					uint8_t  MG_Torq_I_Z=0;
					pkt->data[size] = MG_Torq_I_Z;
					size += 1;
					
					float  Pitch =58.43;
					cnvF_8(Pitch, &pkt->data[size]);
					size +=4;
					
					double  Pitch_Dot=0.023; 
					cnvD_8(Pitch_Dot, &pkt->data[size]);
					size +=8;
					
					uint32_t  QB50=524649019; 
					cnv32_8(QB50, &pkt->data[size]);
					size +=4;	
					
					uint16_t RM_X=21750;
					cnv16_8(RM_X, &pkt->data[size]);
					size +=2;
					
					uint8_t      RM_Y =40;
					pkt->data[size] = RM_Y;
					size += 1;
					
					double        RM_Z=-12890; 
					cnvD_8(RM_Z, &pkt->data[size]);
					size +=8;	
					
					uint8_t     RST_source=18; 
					pkt->data[size] = RST_source;
					size += 1;
					
					double       Roll=157.57;
					cnvD_8(Roll, &pkt->data[size]);
					size +=8;	
					
					double      Roll_Dot=-0.209; 
					cnvD_8(Roll_Dot, &pkt->data[size]);
					size +=8;
					
					uint8_t     Spin_RPM=0;
					pkt->data[size] = Spin_RPM;
					size += 1;
					
					float      Sun_V_0 = 3.49;
					cnvF_8(Sun_V_0, &pkt->data[size]);
					size +=4;	
					
					float      Sun_V_1=3.47;
					cnvF_8(Sun_V_1, &pkt->data[size]);
					size +=4;		
					
					float      Sun_V_2=3.49; 
					cnvF_8(Sun_V_2, &pkt->data[size]);
					size +=4;	
					
					float      Sun_V_3=3.49;
					cnvF_8(Sun_V_3, &pkt->data[size]);
					size +=4;	
					
					float     Sun_V_4=3.49;
					cnvF_8(Sun_V_4, &pkt->data[size]);
					size +=4;
					
					uint8_t     TX_error=5;
					pkt->data[size] = TX_error;
					size += 1;
					
					float    Temp=34.74; 
					cnvF_8(Temp, &pkt->data[size]);
					size +=4;	
					
					double  Time =2113.356;
					cnvD_8(Time, &pkt->data[size]);
					size +=8;
					
					uint16_t       XM_X=24910;
					cnv16_8(XM_X, &pkt->data[size]);
					size +=2;
					
					uint16_t       XM_Y=13060; 
					cnv16_8(XM_Y, &pkt->data[size]);
					size +=2;
					
					double    XM_Z=-6230;
					cnvD_8(XM_Z, &pkt->data[size]);
					size +=8;
					
					float    Yaw=60.19;
					cnvF_8(Yaw, &pkt->data[size]);
					size +=4;
					
					float    Yaw_Dot=0.197;
					cnvF_8(Yaw_Dot, &pkt->data[size]);
					size +=4;							
			 }

//*************************************** comms_REP*****************************************//
       if( COMMS_EX_WOD_FLAG == true )
			 {
				 memcpy( &pkt->data[size], &ext_wod_buffer[COMMS_EXT_WOD_OFFSET], COMMS_EXT_WOD_SIZE);
				 size += COMMS_EXT_WOD_SIZE;				 
			 }
			 else
			 {
					uint8_t     Beacon_pattern =72;
					pkt->data[size] = Beacon_pattern;
					size += 1;
					
					uint32_t    Flash_read_transmit =371609220; 
					cnv32_8(Flash_read_transmit, &pkt->data[size]);
					size +=4;
					
					uint8_t     Invalid_Dest_Frames_Cnt =0;
					pkt->data[size] = Invalid_Dest_Frames_Cnt;
					size += 1;
					
					uint8_t     comms_Last_Assertion_F =1;
					pkt->data[size] = comms_Last_Assertion_F;
					size += 1;
					
					uint8_t     comms_Last_Assertion_L =29;
					pkt->data[size] = comms_Last_Assertion_L;
					size += 1;
					
					uint16_t     Last_RX_Error =65530; 
					cnv16_8(Last_RX_Error, &pkt->data[size]);
					size +=2;		
					
					uint16_t    Last_TX_Error =65532;
					cnv16_8(Last_TX_Error, &pkt->data[size]);
					size +=2;	
					
					uint8_t     comms_RST_source =7;		
					pkt->data[size] = comms_RST_source;
					size += 1;	
					
					uint8_t    RX_CRC_Failed =0;
					pkt->data[size] = RX_CRC_Failed;
					size += 1;
					
					uint8_t    RX_Failed =0;
					pkt->data[size] = RX_Failed;
					size += 1;
					
					uint8_t    RX_Frames =2;
					pkt->data[size] = RX_Frames;
					size += 1;	
					
					uint8_t   TX_Failed =0;
					pkt->data[size] = TX_Failed;
					size += 1;
					
					uint16_t    TX_Frames =695;
					cnv16_8(TX_Frames, &pkt->data[size]);
					size +=2;
					
					double    comms_Time =23926.658;
					cnvD_8(comms_Time, &pkt->data[size]);
					size +=8;						 
			 }

 //*************************************** eps_REP*****************************************//
       if( EPS_EX_WOD_FLAG == true )
			 {
				 memcpy( &pkt->data[size], &ext_wod_buffer[EPS_EXT_WOD_OFFSET], EPS_EXT_WOD_SIZE);
				 size += EPS_EXT_WOD_SIZE;				 
			 }
			 else
			 {
					uint8_t    Batt_Temp_Health_Status=0; 
					pkt->data[size] = Batt_Temp_Health_Status;
					size += 1;
					
					uint8_t     Deployment_Status=0;
					pkt->data[size] = Deployment_Status;
					size += 1;	
					
					uint8_t    Heater_status=0;
					pkt->data[size] = Heater_status;
					size += 1;	
					
					uint8_t    eps_Last_Assertion_F=0;
					pkt->data[size] = eps_Last_Assertion_F;
					size += 1;	
					
					uint8_t    eps_Last_Assertion_L =0;
					pkt->data[size] = eps_Last_Assertion_L;
					size += 1;
					
					uint8_t    eps_RST_source =184;
					pkt->data[size] = eps_RST_source;
					size += 1;	
					
					uint8_t    Safety_Battery_Mode =0;
					pkt->data[size] = Safety_Battery_Mode;
					size += 1;	
					
					uint8_t    Safety_Temp_Mode =0;
					pkt->data[size] = Safety_Temp_Mode;
					size += 1;
					
					uint8_t   Soft_error_status =0; 
					pkt->data[size] = Soft_error_status;
					size += 1;	
					
					uint8_t   Switch_ADCS =0;
					pkt->data[size] = Switch_ADCS;
					size += 1;	
					
					uint8_t   Switch_COMMS =0;
					pkt->data[size] = Switch_COMMS;
					size += 1;	
					
					uint8_t    Switch_OBC =0;
					pkt->data[size] = Switch_OBC;
					size += 1;	
					
					uint8_t    Switch_SU =1;
					pkt->data[size] = Switch_SU;
					size += 1;	
					
					uint8_t    Temp_sensor_PWR_SW =0;
					pkt->data[size] = Temp_sensor_PWR_SW;
					size += 1;
					
					double eps_Time =23226.8;
					cnvD_8(eps_Time, &pkt->data[size]);
					size +=8;
					
					uint8_t    Xplus_Current =56;
					pkt->data[size] = Xplus_Current;
					size += 1;	
					
					uint8_t    Xplus_Duty =16;
					pkt->data[size] = Xplus_Duty;
					size += 1;
					
					uint8_t    Xplus_Voltage =99;
					pkt->data[size] = Xplus_Voltage;
					size += 1;	
					
					uint8_t    Xminus_Current =49;
					pkt->data[size] = Xminus_Current;
					size += 1;
					
					uint8_t    Xminus_Duty =16;
					pkt->data[size] = Xminus_Duty;
					size += 1;
					
					uint8_t    Xminus_Voltage =57;
					pkt->data[size] = Xminus_Voltage;
					size += 1;	
					
					uint8_t   Yplus_Current =168;
					pkt->data[size] = Yplus_Current;
					size += 1;
					
					uint8_t    Yplus_Duty =16;
					pkt->data[size] = Yplus_Duty;
					size += 1;
					
					uint8_t    Yplus_Voltage =210;
					pkt->data[size] = Yplus_Voltage;
					size += 1;
					
					uint8_t    Y_minus_Current =48;
					pkt->data[size] = Y_minus_Current;
					size += 1;
					
					uint8_t    Yminus_Duty =16;
					pkt->data[size] = Yminus_Duty;
					size += 1;
					
					uint8_t    Yminus_Voltage =51;
					pkt->data[size] = Yminus_Voltage;
					size += 1;				 
			 }
			 
			 // memcpy( &pkt->data[size], &ext_wod_buffer[COMMS_EXT_WOD_OFFSET], SUB_SYS_EXT_WOD_SIZE);
       // size += SUB_SYS_EXT_WOD_SIZE;
       //mass_storage_storeFile(EXT_WOD_LOG, 0, &pkt->data[1], &size);
        pkt->len = size;
    } 
		else if(sid == ECSS_STATS_REP)
		{

        uint16_t size = ecss_stats_hk(&pkt->data[1]);

        pkt->len = size + 1;
    }

    return SATR_OK;
}
