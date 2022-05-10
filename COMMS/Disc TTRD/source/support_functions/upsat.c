#include "upsat.h"

#undef __FILE_ID__
#define __FILE_ID__ 31

#define SYS_HOUR 360000

extern SAT_returnState route_pkt(tc_tm_pkt *pkt);

extern tc_tm_pkt * queuePeak(TC_TM_app_id app_id);
extern tc_tm_pkt * queuePop(TC_TM_app_id app_id);
//extern struct _uart_data uart_data;




//update
uint32_t sys_refresh(void)
{
	static uint32_t heartbeat_tick;
	uint32_t now;
	uint32_t Return_value = RETURN_NORMAL_STATE;
	now = getCurrentTick();
	if(now < heartbeat_tick	|| now - heartbeat_tick > __HEARTBEAT_EPS_INTERVAL_MS)//__HEARTBEAT_EPS_INTERVAL_MS
	{
		heartbeat_tick = now;
		tc_tm_pkt *temp_pkt = 0;

		test_crt_heartbeat(&temp_pkt);
		if(!(C_ASSERT(temp_pkt != NULL) == true)) { return Return_value; }
		UART2_BUF_O_Write_pkt_To_Buffer(temp_pkt);
		route_pkt(temp_pkt);
		UART2_BUF_O_Write_String_To_Buffer("HEARTBEAT to EPS\n");
		
//		UART2_BUF_O_Write_String_To_Buffer("Testing to crt packet\n");
//		crt_pkt(temp_pkt, _OBC_APP_ID_, TC, TC_ACK_NO, TC_HOUSEKEEPING_SERVICE, TM_CT_REPORT_TEST, SYSTEM_APP_ID);
//		UART2_BUF_O_Write_pkt_To_Buffer(temp_pkt);
//		uint8_t *payload;
//		uint16_t *payload_size;
//		temp_pkt->data=(uint8_t *)"Hello World";
//		pack_pkt(payload, temp_pkt, payload_size);
//		int32_t ret = send_payload(payload, (size_t)payload_size, 0,COMMS_DEFAULT_TIMEOUT_MS);
//		if(ret < 1)
//		{
//			return SATR_ERROR;
//		}
//		free_pkt (temp_pkt);
	 
		}
	return Return_value;
}

//Setter
SAT_returnState import_pkt(TC_TM_app_id app_id, struct _uart_data *data)// working on it
{
	tc_tm_pkt *pkt;
	uint16_t size = 0;

	SAT_returnState res;
	SAT_returnState res_deframe;

	OBC_T_DMA_CHECK(data);
	
	res = OBC_C_UART_recv(data); //<-------------------------------------------------------------------
	if( res == SATR_EOT ) 
	{
		//UART2_BUF_O_Write_String_To_Buffer("got msg\n");
		size = data->uart_size;
		res_deframe = HLDLC_deframe(data->uart_unpkt_buf, data->deframed_buf, &size); //<------------------------------------
		if(res_deframe == SATR_EOT) 
		{
			data->last_com_time = getCurrentTick();/*update the last communication time, to be used for timeout discovery*/

			pkt = get_pkt(size);

			if(!(C_ASSERT(pkt != NULL) == true)) { return SATR_ERROR; }
			if((res = unpack_pkt(data->deframed_buf, pkt, size)) == SATR_OK) 
			{
					stats_inbound(pkt->type, pkt->app_id, pkt->dest_id, pkt);
					route_pkt(pkt); 
			}
			else 
			{
					stats_dropped_upack();
					pkt->verification_state = res;
			}
			verification_app(pkt);

			TC_TM_app_id dest = 0;

			if(pkt->type == TC)         { dest = pkt->app_id; }
			else if(pkt->type == TM)    { dest = pkt->dest_id; }

			if(dest == SYSTEM_APP_ID) 
			{
				free_pkt(pkt);
			}
		}
		else 
		{
			stats_dropped_hldlc();
		}
	}

	return SATR_OK;
}


//Getter
SAT_returnState export_pkt(TC_TM_app_id app_id, struct _uart_data *data)// working on it
{
	tc_tm_pkt *pkt = 0;
	uint16_t size = 0;
	SAT_returnState res = SATR_ERROR;


	/* Checks if that the pkt that was transmitted is still in the queue */
	if((pkt = queuePop(app_id)) ==  NULL) { return SATR_OK; }

	stats_outbound(pkt->type, pkt->app_id, pkt->dest_id, pkt);

	pack_pkt(data->uart_pkted_buf,  pkt, &size);

	res = HLDLC_frame(data->uart_pkted_buf, data->framed_buf, &size); 
	if(res == SATR_ERROR) { return SATR_ERROR; }

	if(!(C_ASSERT(size > 0) == true)) { return SATR_ERROR; }

	OBC_C_UART_send(data->framed_buf, size); 

	free_pkt(pkt);

	return SATR_OK;
}

SAT_returnState test_crt_heartbeat(tc_tm_pkt **pkt)
{
	*pkt = get_pkt(PKT_NORMAL);
	if(!(C_ASSERT(*pkt != NULL) == true)) { return SATR_ERROR; }
	crt_pkt(*pkt, EPS_APP_ID, TC, TC_ACK_NO, TC_TEST_SERVICE, TC_CT_PERFORM_TEST, SYSTEM_APP_ID);

	(*pkt)->len = 0;

	return SATR_OK;
}

SAT_returnState firewall(tc_tm_pkt *pkt)
{
	if(!(C_ASSERT(pkt != NULL && pkt->data != NULL) == true)) 
	{
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


