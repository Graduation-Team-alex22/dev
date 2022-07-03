#include "../main/main.h"
#include "services.h"

////////////////////////////////////////////////// STRING //////////////////////////////////////////////////////////
size_t	 strnlen (const char *s, size_t maxlen)
{
	//strlen (s) < maxlen ? strlen (s) : maxlen ;
	if(strlen (s) < maxlen)
		return(strlen (s));
	else
		return(maxlen);
}
//////////////////////////////////////////////// STDBOOL ////////////////////////////////////

//////////////////////////////////////////////// SYSTEM ////////////////////////////////////


//////////////////////////////////////////////// SERVICES ////////////////////////////////////
#undef __FILE_ID__
#define __FILE_ID__ 4

/*Lookup table that returns if a service with its subtype with TC or TM is supported and valid*/
const uint8_t services_verification_TC_TM[MAX_SERVICES][MAX_SUBTYPES][2] = 
{ 
	/*       0       1       2       3       4       5       6       7       8       9      10      11      12      13      14      15      16      17      18      19      20      21      22      23      24      25   */
	/*              TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC  TM  TC */    
	/*00*/{ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	/*01*/{ {0, 0}, {1, 0}, {1, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }, /*TC_VERIFICATION_SERVICE*/
	/*02*/{ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	/*03*/{ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 1}, {0, 0}, {1, 0}, {0, 0}, {0, 0} }, /*TC_HOUSEKEEPING_SERVICE*/
	/*04*/{ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	/*05*/{ {0, 0}, {1, 0}, {0, 0}, {0, 0}, {1, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }, /*TC_EVENT_SERVICE*/
	/*06*/{ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	/*07*/{ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	/*08*/{ {0, 0}, {0, 1}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }, /*TC_FUNCTION_MANAGEMENT_SERVICE*/
	/*09*/{ {0, 0}, {1, 1}, {1, 1}, {0, 1}, {0, 1}, {1, 0}, {1, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }  /*TIME MANAGEMENT SERVICE*/,
	/*10*/{ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	/*11*/{ {0, 0}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 0}, {0, 1}, {0, 1}, {0, 0}, {1, 0}, {0, 0}, {0, 0}, {1, 0}, {0, 0}, {0, 1}, {0, 1}, {0, 1}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 1}, {0, 1}, {0, 1}, {0, 1} }, /*TC_SCHEDULING_SERVICE*/
	/*12*/{ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	/*13*/{ {0, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {0, 1}, {0, 1}, {1, 0}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {0, 1}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }, /*TC_LARGE_DATA_SERVICE*/
	/*14*/{ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	/*15*/{ {0, 0}, {1, 0}, {1, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {1, 0}, {0, 1}, {0, 0}, {0, 1}, {0, 1}, {1, 0}, {0, 1}, {0, 1}, {0, 1}, {1, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }, /*TC_MASS_STORAGE_SERVICE*/
	/*16*/{ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	/*17*/{ {0, 0}, {0, 1}, {1, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }, /*TC_TEST_SERVICE*/
	/*18*/{ {0, 0}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {1, 0}, {0, 1}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }, /*MNLP TEST SERVICE*/
	/*19*/{ {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }
};

uint8_t assertion_last_file = 0;
uint16_t assertion_last_line = 0;

uint8_t tst_debugging(uint16_t fi, uint32_t l, char *e) 
{

  traceASSERTION(fi, l);
  //SYSVIEW_PRINT("AS_ER %u %u", fi, l);
	UART2_BUF_O_Write_String_To_Buffer("\nAS_ER ");
	UART2_BUF_O_Write_Number04_To_Buffer(fi);
	UART2_BUF_O_Write_String_To_Buffer(", ");
	UART2_BUF_O_Write_Number04_To_Buffer(l);
	UART2_BUF_O_Write_String_To_Buffer("\n");
	
  assertion_last_file = fi;
  assertion_last_line = l;

  return false;
}

/////////////////////////////////////////////////// ECSS_STATS DONE /////////////////////////////////////////////////
#undef __FILE_ID__
#define __FILE_ID__ 39
struct _ecss_stats
{
    uint16_t inbound_packet[LAST_APP_ID][LAST_APP_ID];
    uint16_t outbound_packet[LAST_APP_ID][LAST_APP_ID];
    uint16_t dropped_hldlc;
    uint16_t dropped_unpack;
};
static struct _ecss_stats ecss_stats = { .inbound_packet = {{ 0, 0},
                                                            { 0, 0},
                                                            { 0, 0},
                                                            { 0, 0},
                                                            { 0, 0},
                                                            { 0, 0},
                                                            { 0, 0}} ,
                                        .outbound_packet = {{ 0, 0},
                                                            { 0, 0},
                                                            { 0, 0},
                                                            { 0, 0},
                                                            { 0, 0},
                                                            { 0, 0},
                                                            { 0, 0}} ,
                                         .dropped_hldlc = 0,
                                         .dropped_unpack = 0 }; 
//Setter																 
void stats_inbound(uint8_t type, TC_TM_app_id app_id, TC_TM_app_id dest_id, tc_tm_pkt *pkt) 
{
	if(!(C_ASSERT(pkt != NULL && pkt->data != NULL) == true))               { return ; }
	if(!(C_ASSERT(app_id < LAST_APP_ID && dest_id < LAST_APP_ID) == true))  { return ; }

	TC_TM_app_id source = 0;
	TC_TM_app_id dest = 0;

	if(type == TC) 
	{
		source = app_id;
		dest = dest_id;
	}
	else if(type == TM) 
	{
		dest = app_id;
		source = dest_id;
	}
	ecss_stats.inbound_packet[source][dest]++;
	//SYSVIEW_PRINT("IN %u,%u,%u,%u", source, dest, pkt->seq_count, pkt->ser_type);
	UART2_BUF_O_Write_String_To_Buffer("IN ");
	UART2_BUF_O_Write_Number04_To_Buffer(source);
	UART2_BUF_O_Write_String_To_Buffer(", ");
	UART2_BUF_O_Write_Number04_To_Buffer(dest);
	UART2_BUF_O_Write_String_To_Buffer(", ");
	UART2_BUF_O_Write_Number04_To_Buffer(pkt->seq_count);
	UART2_BUF_O_Write_String_To_Buffer(", ");
	UART2_BUF_O_Write_Number04_To_Buffer(pkt->ser_type);
}

void stats_outbound(uint8_t type, TC_TM_app_id app_id, TC_TM_app_id dest_id, tc_tm_pkt *pkt) 
{
	if(!(C_ASSERT(pkt != NULL && pkt->data != NULL) == true))               { return ; }
	if(!(C_ASSERT(app_id < LAST_APP_ID && dest_id < LAST_APP_ID) == true))  { return ; }

	TC_TM_app_id source = 0;
	TC_TM_app_id dest = 0;

	if(type == TC) 
	{
		source = app_id;
		dest = dest_id;
	}
	else if(type == TM) 
	{
			dest = app_id;
			source = dest_id;
	}

	ecss_stats.outbound_packet[source][dest]++;

	//SYSVIEW_PRINT("OUT %u,%u,%u,%u", source, dest, pkt->seq_count, pkt->ser_type);
	//UART2_BUF_O_Write_pkt_To_Buffer(pkt);
	
	UART2_BUF_O_Write_String_To_Buffer("OUT(");
	UART2_BUF_O_Write_String_To_Buffer("type: ");
	if (pkt->type == TC) 
		UART2_BUF_O_Write_String_To_Buffer("TC");
	else if (pkt->type == TM)
		UART2_BUF_O_Write_String_To_Buffer("TM");
	UART2_BUF_O_Write_String_To_Buffer(",Source: ");
		if (source == OBC_APP_ID) 
		UART2_BUF_O_Write_String_To_Buffer("OBC");
	else if (source == EPS_APP_ID) 
		UART2_BUF_O_Write_String_To_Buffer("EPS");
	else if (source == ADCS_APP_ID)
		UART2_BUF_O_Write_String_To_Buffer("ADCS");
	else if (source == COMMS_APP_ID) 
		UART2_BUF_O_Write_String_To_Buffer("COMMS");
	else if (source == GND_APP_ID) 
		UART2_BUF_O_Write_String_To_Buffer("GND");
	else
	{
		UART2_BUF_O_Write_Number02_To_Buffer(source);
	}
	UART2_BUF_O_Write_String_To_Buffer(",dest: ");
	if (dest== OBC_APP_ID)
		UART2_BUF_O_Write_String_To_Buffer("OBC");
	else if (dest== EPS_APP_ID) 
		UART2_BUF_O_Write_String_To_Buffer("EPS");
	else if (dest== ADCS_APP_ID) 
		UART2_BUF_O_Write_String_To_Buffer("ADCS");
	else if (dest== COMMS_APP_ID) 
		UART2_BUF_O_Write_String_To_Buffer("COMMS");
	else if (dest== GND_APP_ID) 
		UART2_BUF_O_Write_String_To_Buffer("GND");
	else
	{
		UART2_BUF_O_Write_Number04_To_Buffer(dest);
	}
	UART2_BUF_O_Write_String_To_Buffer(",seq_count: ");
	UART2_BUF_O_Write_Number04_To_Buffer(pkt->seq_count);
	UART2_BUF_O_Write_String_To_Buffer(",ser_type: ");
	UART2_BUF_O_Write_Number04_To_Buffer(pkt->ser_type);
	UART2_BUF_O_Write_String_To_Buffer(")\n");
}
void stats_dropped_hldlc(void) 
{
	ecss_stats.dropped_hldlc++;
}

void stats_dropped_upack(void) 
{
	ecss_stats.dropped_unpack++;
}																				 
	
// SF_G
uint16_t ecss_stats_hk(uint8_t *buffer)
{
	uint16_t pointer = 0;

	cnv16_8(ecss_stats.dropped_hldlc, &buffer[pointer]);
	pointer += sizeof(uint16_t);

	cnv16_8(ecss_stats.dropped_unpack, &buffer[pointer]);
	pointer += sizeof(uint16_t);

	for(uint8_t a = 1; a < LAST_APP_ID; a++) 
	{
		for(uint8_t b = 1; b < LAST_APP_ID; b++) 
		{
			cnv16_8(ecss_stats.inbound_packet[a][b], &buffer[pointer]);
			pointer += sizeof(uint16_t);
		}
	}
	for(uint8_t a = 1; a < LAST_APP_ID; a++) 
	{
		for(uint8_t b = 1; b < LAST_APP_ID; b++) 
		{
			cnv16_8(ecss_stats.outbound_packet[a][b], &buffer[pointer]);
			pointer += sizeof(uint16_t);
		}
	}
	return pointer;
}

////////////////////////////////////////////////// FLASH //////////////////////////////////////////////////////////
#undef __FILE_ID__
#define __FILE_ID__ 38

#define SECTOR_11_SIZE	   0x1000
#define TRANSMIT_VAR_ADD   0x080E0000
#define COMMS_FLASH_CONF_WORDS 2

#ifdef __GNUC__
const uint32_t __attribute__((section (".comms_storage_section"))) occupy_sector[SECTOR_11_SIZE] __attribute__ ((aligned (4))) = { 0x16264e84, 0x48};
#else

	#pragma location = 0x080E0000
	const uint32_t occupy_sector[SECTOR_11_SIZE] = { 0x16264e84, 0x48 };
#endif

//Init
uint32_t flash_INIT(void)
{
	return occupy_sector[0];
}

//Update

// SF_G
uint32_t flash_read_trasmit(size_t offset) 
{
	uint32_t *val = (uint32_t*)(TRANSMIT_VAR_ADD + offset);

	return *val;
}
////// SF_S
//////void flash_write_trasmit(uint32_t data, size_t offset);




/////////////////////////////////////////////// HLDLC /////////////////////////////////////////////
#undef __FILE_ID__
#define __FILE_ID__ 36

//TF
SAT_returnState HLDLC_deframe(uint8_t *buf_in, uint8_t *buf_out, uint16_t *size) 
{
	if(!(C_ASSERT(buf_in != 0 && buf_out != 0) == true))    { return SATR_ERROR; }
	if(!(C_ASSERT(buf_in[0] == HLDLC_START_FLAG) == true))  { return SATR_ERROR; } /*the first char should be a start flag*/
	if(!(C_ASSERT(*size <= UART_BUF_SIZE) == true))         { return SATR_ERROR; } //hard limits, check

	uint16_t cnt = 0;

	for(uint16_t i = 1; i < *size; i++) 
	{
		if(buf_in[i] == HLDLC_START_FLAG) 
		{
			*size = cnt;
			return SATR_EOT;
		} 
		else if(buf_in[i] == HLDLC_CONTROL_FLAG) 
		{
				i++;
				if(!(C_ASSERT(i < (*size) - 1) == true))       { return SATR_ERROR; }
				if(buf_in[i] == 0x5E) { buf_out[cnt++] = 0x7E; }
				else if(buf_in[i] == 0x5D) { buf_out[cnt++] = 0x7D; }
				else { return SATR_ERROR; }
		} 
		else 
		{
			buf_out[cnt++] = buf_in[i];
		}
	}
	return SATR_ERROR;
}

//used for DMA
SAT_returnState HLDLC_frame(uint8_t *buf_in, uint8_t *buf_out, uint16_t *size) 
{
	if(!(C_ASSERT(buf_in != NULL && buf_out != NULL && size != NULL) == true))  { return SATR_ERROR; }
	if(!(C_ASSERT(*size <= MAX_PKT_SIZE) == true))                      	      { return SATR_ERROR; }

	uint16_t cnt = 2;

	for(uint16_t i = 0; i < *size; i++) 
	{
		if(i == 0) 
		{
			buf_out[0] = HLDLC_START_FLAG;
			buf_out[1] = buf_in[0];
		} 
		else if(i == (*size) - 1) 
		{
			if(buf_in[i] == HLDLC_START_FLAG) 
			{
				buf_out[cnt++] = HLDLC_CONTROL_FLAG;
				buf_out[cnt++] = 0x5E;
			} 
			else if(buf_in[i] == HLDLC_CONTROL_FLAG) 
			{
				buf_out[cnt++] = HLDLC_CONTROL_FLAG;
				buf_out[cnt++] = 0x5D;
			} 
			else { buf_out[cnt++] = buf_in[i]; }
			
			buf_out[cnt++] = HLDLC_START_FLAG;
			*size = cnt;
			return SATR_EOT;
		} 
		else if(buf_in[i] == HLDLC_START_FLAG) 
		{
			buf_out[cnt++] = HLDLC_CONTROL_FLAG;
			buf_out[cnt++] = 0x5E;
		}
		else if(buf_in[i] == HLDLC_CONTROL_FLAG) 
		{
			buf_out[cnt++] = HLDLC_CONTROL_FLAG;
			buf_out[cnt++] = 0x5D;
		} 
		else 
		{
			buf_out[cnt++] = buf_in[i];
		}
	}
	return SATR_ERROR;
}


////////////////////////////////////////////// HOUSEKEEPING_SERVICES DONE ////////////////////////////////////
#undef __FILE_ID__
#define __FILE_ID__ 9
extern SAT_returnState route_pkt(tc_tm_pkt *pkt);

//Setters
// S
SAT_returnState hk_app(tc_tm_pkt *pkt)
{
	if(!(C_ASSERT(pkt != NULL && pkt->data != NULL) == true)) { return SATR_ERROR; }
	if(!(C_ASSERT(pkt->ser_subtype == TC_HK_REPORT_PARAMETERS || pkt->ser_subtype == TM_HK_PARAMETERS_REPORT) == true)) {return SATR_ERROR; }

	if(pkt->ser_subtype == TC_HK_REPORT_PARAMETERS) 
	{
		tc_tm_pkt *temp_pkt = 0;
		HK_struct_id sid = (HK_struct_id)pkt->data[0];

		hk_crt_empty_pkt_TM(&temp_pkt, (TC_TM_app_id)pkt->dest_id, sid);

		if(!(C_ASSERT(temp_pkt != NULL) == true)) 
		{
			return SATR_ERROR;
		}
		route_pkt(temp_pkt);
	} 
	else if(pkt->ser_subtype == TM_HK_PARAMETERS_REPORT) 
	{
		const SAT_returnState res = hk_parameters_report(pkt->app_id, (HK_struct_id)pkt->data[0],  pkt->data, pkt->len);

		if(res == SATR_OK) 
		{
			pkt->verification_state = SATR_OK;
		}
	}
	return SATR_OK;
}

// SF_S
SAT_returnState hk_crt_pkt_TC(tc_tm_pkt *pkt, const TC_TM_app_id app_id, const HK_struct_id sid)
{
	if(!(C_ASSERT(app_id < LAST_APP_ID) == true))  { return SATR_ERROR; }

	crt_pkt(pkt, app_id, TC, TC_ACK_NO, TC_HOUSEKEEPING_SERVICE, TC_HK_REPORT_PARAMETERS, (TC_TM_app_id)SYSTEM_APP_ID);

	pkt->data[0] = (char)sid;
	pkt->len = 1;

	return SATR_OK;
}

// SF_S
SAT_returnState hk_crt_pkt_TM(tc_tm_pkt *pkt, const TC_TM_app_id app_id, const HK_struct_id sid)
{
	hk_report_parameters(sid, pkt);

	crt_pkt(pkt, (TC_TM_app_id)SYSTEM_APP_ID, TM, TC_ACK_NO, TC_HOUSEKEEPING_SERVICE, TM_HK_PARAMETERS_REPORT, app_id);

	return SATR_OK;
}

// S
SAT_returnState hk_crt_empty_pkt_TM(tc_tm_pkt **pkt, const TC_TM_app_id app_id, const HK_struct_id sid)
{
	*pkt = get_pkt(PKT_NORMAL);
	if(!(C_ASSERT(*pkt != NULL) == true)) { return SATR_ERROR; }

	hk_crt_pkt_TM(*pkt, app_id, sid);
	return SATR_OK;
}
////////////////////////////////////////////////// HOUSEKEEPING DONE ////////////////////////////////////////////////
extern comms_rf_stat_t comms_stats;

#undef __FILE_ID__
#define __FILE_ID__ 24

//SF_S
SAT_returnState hk_parameters_report(TC_TM_app_id app_id, HK_struct_id sid, uint8_t *data, uint8_t len)
{
	if(sid == EXT_WOD_REP) 
	{
		//SYSVIEW_PRINT("Storing exWOD of size %u\n");
		UART2_BUF_O_Write_String_To_Buffer("Storing exWOD of size ");
		UART2_BUF_O_Write_Number04_To_Buffer(len);
		UART2_BUF_O_Write_String_To_Buffer("\n");
		
		store_ex_wod_obc(data, len);
	}
	return SATR_ERROR;
}
SAT_returnState hk_report_parameters(HK_struct_id sid, tc_tm_pkt *pkt)
{
	float temp;
	size_t i;
	uint8_t b;
	pkt->data[0] = (HK_struct_id)sid;
	if(sid == HEALTH_REP) 
	{
		temp = comms_rf_stats_get_temperature(&comms_stats);
		if(isnan(temp))
		{
			/* Shit hack */
			pkt->data[1] = 1;
		}
		else 
		{
			pkt->data[1] = wod_convert_temperature(temp);
		}
		pkt->len = 2;
		//SYSVIEW_PRINT("COMMS HEALTH: Temp %u", pkt->data[1]);
		UART2_BUF_O_Write_String_To_Buffer("COMMS HEALTH: Temp ");
		UART2_BUF_O_Write_Number04_To_Buffer(pkt->data[1]);
		UART2_BUF_O_Write_String_To_Buffer("\n\n");
	}
	else if(sid == EX_HEALTH_REP) 
	{
		i = 1;
		cnv32_8(getCurrentTick(), &pkt->data[i]);
		i += sizeof(uint32_t);
		/* Store the reset source */
		b = comms_stats.rst_src;
		pkt->data[i] = b;
		i += sizeof(uint8_t);
		pkt->data[i] = assertion_last_file;
		i += sizeof(uint8_t);
		cnv16_8(assertion_last_line, &pkt->data[i]);
		i += sizeof(uint16_t);
//		cnv32_8(flash_read_trasmit(__COMMS_RF_KEY_FLASH_OFFSET),&pkt->data[i]); //<-------------------------
		i += sizeof(uint32_t); 
//		b = flash_read_trasmit(__COMMS_HEADLESS_TX_FLASH_OFFSET); //<-------------------------
		pkt->data[i] = b;
		i += sizeof(uint8_t);
		cnv16_8(comms_stats.rx_failed_cnt, &pkt->data[i]);
		i += sizeof(uint16_t);
		cnv16_8(comms_stats.rx_crc_failed_cnt, &pkt->data[i]);
		i += sizeof(uint16_t);
		cnv16_8(comms_stats.tx_failed_cnt, &pkt->data[i]);
		i += sizeof(uint16_t);
		cnv16_8(comms_stats.tx_frames_cnt, &pkt->data[i]);
		i += sizeof(uint16_t);
		cnv16_8(comms_stats.rx_frames_cnt, &pkt->data[i]);
		i += sizeof(uint16_t);
		cnv16_8(comms_stats.last_tx_error_code, &pkt->data[i]);
		i += sizeof(int16_t);
		cnv16_8(comms_stats.last_rx_error_code, &pkt->data[i]);
		i += sizeof(int16_t);
		cnv16_8(comms_stats.invalid_dest_frames_cnt, &pkt->data[i]);
		i += sizeof(uint16_t);
		pkt->len = i;
		//SYSVIEW_PRINT("COMMS EX HEALTH REP Len %u", pkt->len);
		UART2_BUF_O_Write_String_To_Buffer("COMMS EX HEALTH REP Len ");
		UART2_BUF_O_Write_Number04_To_Buffer(pkt->len);
	}
	else if(sid == EXT_WOD_REP) 
	{
		/*zero padding*/
		memset(&pkt->data[1], 0, SYS_EXT_WOD_SIZE);

		i = COMMS_EXT_WOD_OFFSET;
		cnv32_8(getCurrentTick(), &pkt->data[i]);
		i += sizeof(uint32_t);
		/* Store the reset source */
		b = comms_stats.rst_src;
		pkt->data[i] = b;
		i += sizeof(uint8_t);
		pkt->data[i] = assertion_last_file;
		i += sizeof(uint8_t);
		cnv16_8(assertion_last_line, &pkt->data[i]);
		i += sizeof(uint16_t);
		//cnv32_8(flash_read_trasmit(__COMMS_RF_KEY_FLASH_OFFSET),&pkt->data[i]); //<-------------------------
		i += sizeof(uint32_t);
		//b = flash_read_trasmit(__COMMS_HEADLESS_TX_FLASH_OFFSET); //<-------------------------
		pkt->data[i] = b;
		i += sizeof(uint8_t);
		cnv16_8(comms_stats.rx_failed_cnt, &pkt->data[i]);
		i += sizeof(uint16_t);
		cnv16_8(comms_stats.rx_crc_failed_cnt, &pkt->data[i]);
		i += sizeof(uint16_t);
		cnv16_8(comms_stats.tx_failed_cnt, &pkt->data[i]);
		i += sizeof(uint16_t);
		cnv16_8(comms_stats.tx_frames_cnt, &pkt->data[i]);
		i += sizeof(uint16_t);
		cnv16_8(comms_stats.rx_frames_cnt, &pkt->data[i]);
		i += sizeof(uint16_t);
		cnv16_8(comms_stats.last_tx_error_code, &pkt->data[i]);
		i += sizeof(int16_t);
		cnv16_8(comms_stats.last_rx_error_code, &pkt->data[i]);
		i += sizeof(int16_t);
		cnv16_8(comms_stats.invalid_dest_frames_cnt, &pkt->data[i]);
		i += sizeof(uint16_t);
		pkt->len = SYS_EXT_WOD_SIZE + 1;
		//SYSVIEW_PRINT("COMMS ONLY EX WOD: Len %u", pkt->len);
		UART2_BUF_O_Write_String_To_Buffer("COMMS ONLY EX WOD: Len ");
		UART2_BUF_O_Write_Number04_To_Buffer(pkt->len);
	}
	else if(sid == ECSS_STATS_REP) 
	{
		uint16_t size = ecss_stats_hk(&pkt->data[1]);
		pkt->len = size + 1;
	}
	else if(sid == WOD_REP) 
	{
		comms_wod_send(1);
		pkt->len = 0;
	}
	else if(sid == EXT_WOD_REP)
		return SATR_OK;

	return SATR_OK;
}
////////////////////////////////////////////////// QUEUE DONE ////////////////////////////////////////////////
#undef __FILE_ID__
#define __FILE_ID__ 34
struct _queue {
    tc_tm_pkt *fifo[POOL_PKT_TOTAL_SIZE];
    uint8_t head;
    uint8_t tail;
};
static struct _queue queue = { .head = 0, .tail = 0};


// Setters
// S
tc_tm_pkt * queuePop(TC_TM_app_id app_id)
{
	tc_tm_pkt *pkt;

	if(queue.head == queue.tail) { return 0; }

	pkt = queue.fifo[queue.tail];
	queue.tail = (queue.tail + 1) % POOL_PKT_TOTAL_SIZE;

	return pkt;
}
// SF_S
void queue_IDLE(TC_TM_app_id app_id)
{
	tc_tm_pkt *pkt;

	pkt = queuePeak(app_id);
	if(pkt != NULL) { return; }

	if(is_free_pkt(pkt) == true) 
	{
		queuePop(app_id);
		traceGC_QUEUE_PKT();
	}
}

// Getters
// G
SAT_returnState queuePush(tc_tm_pkt *pkt, TC_TM_app_id app_id)
{
	if(queue.head == (((queue.tail - 1) + POOL_PKT_TOTAL_SIZE) % POOL_PKT_TOTAL_SIZE)) 
	{
		return SATR_QUEUE_FULL;
	}

	queue.fifo[queue.head] = pkt;

	queue.head = (queue.head + 1) % POOL_PKT_TOTAL_SIZE;

	return SATR_OK;
}
// SF_G
uint8_t queueSize(TC_TM_app_id app_id)
{
	if(queue.head == queue.tail) { return 0; }

	return queue.head - queue.tail;
}

// G
tc_tm_pkt * queuePeak(TC_TM_app_id app_id)
{
	if(queue.head == queue.tail) { return 0; }

	return queue.fifo[queue.head];
}

////////////////////////////////////////////////// SERVICE_UTILITIES DONE ////////////////////////////////////////////////
#undef __FILE_ID__
#define __FILE_ID__ 5

struct _pkt_state 
{
	uint8_t seq_cnt[LAST_APP_ID];
};
static struct _pkt_state pkt_state;

// Setters
SAT_returnState crt_pkt(tc_tm_pkt *pkt, TC_TM_app_id app_id, uint8_t type, uint8_t ack, uint8_t ser_type, uint8_t ser_subtype, TC_TM_app_id dest_id)
{
	if(!(C_ASSERT(pkt != NULL && pkt->data != NULL) == true))                 { return SATR_ERROR; }
	if(!(C_ASSERT(app_id < LAST_APP_ID && dest_id < LAST_APP_ID ) == true))   { return SATR_ERROR; }
	if(!(C_ASSERT(type == TC || type == TM) == true))                         { return SATR_ERROR; }
	if(!(C_ASSERT(ack == TC_ACK_NO || ack == TC_ACK_ACC) == true))            { return SATR_ERROR; }

	pkt->type = type;
	pkt->app_id = app_id;
	pkt->dest_id = dest_id;
	pkt->ser_type = ser_type;
	pkt->ser_subtype = ser_subtype;
	pkt->verification_state = SATR_PKT_INIT;

	return SATR_OK;
}



// Getters
SAT_returnState checkSum(const uint8_t *data, const uint16_t size, uint8_t *res_crc)
{
	if(!(C_ASSERT(data != NULL && size != 0) == true))                         { return SATR_ERROR; }
	if(!(C_ASSERT(size > TC_MIN_PKT_SIZE-3 && size < MAX_PKT_SIZE-2) == true)) { return SATR_ERROR; }

	*res_crc = 0;
	for(int i=0; i<=size; i++)
	{
		*res_crc = *res_crc ^ data[i];
	}
	return SATR_OK;
}


//SF_TF
/*Must check for endianess*/
/*size: is the bytes of the buf*/
SAT_returnState unpack_pkt(const uint8_t *buf, tc_tm_pkt *pkt, const uint16_t size)
{
	uint8_t tmp_crc[2];

	uint8_t ver, dfield_hdr, ccsds_sec_hdr, tc_pus;

	if(!(C_ASSERT(buf != NULL && pkt != NULL && pkt->data != NULL) == true))  { return SATR_ERROR; }
	if(!(C_ASSERT(size > TC_MIN_PKT_SIZE && size < MAX_PKT_SIZE) == true))    { return SATR_ERROR; }

	tmp_crc[0] = buf[size - 1];
	checkSum(buf, size-2, &tmp_crc[1]); /* -2 for excluding the checksum bytes*/

	ver = buf[0] >> 5;

	pkt->type = (buf[0] >> 4) & 0x01;
	dfield_hdr = (buf[0] >> 3) & 0x01;

	pkt->app_id = (TC_TM_app_id)buf[1];

	pkt->seq_flags = buf[2] >> 6;

	cnv8_16((uint8_t*)&buf[2], &pkt->seq_count);
	pkt->seq_count &= 0x3FFF;

	cnv8_16((uint8_t*)&buf[4], &pkt->len);

	ccsds_sec_hdr = buf[6] >> 7;

	tc_pus = buf[6] >> 4;

	pkt->ack = 0x07 & buf[6];

	pkt->ser_type = buf[7];
	pkt->ser_subtype = buf[8];
	pkt->dest_id = (TC_TM_app_id)buf[9];

	pkt->verification_state = SATR_PKT_INIT;

	
	if(!(C_ASSERT(pkt->app_id < LAST_APP_ID && pkt->dest_id < LAST_APP_ID && pkt->app_id != pkt->dest_id) == true)) 
	{
		pkt->verification_state = SATR_PKT_ILLEGAL_APPID;
		return SATR_PKT_ILLEGAL_APPID; 
	}
	if(!(C_ASSERT(pkt->len <= size - ECSS_HEADER_SIZE - 1) == true)) 
	{
		pkt->verification_state = SATR_PKT_INV_LEN;
		return SATR_PKT_INV_LEN; 
	}
	pkt->len = pkt->len - ECSS_DATA_HEADER_SIZE - ECSS_CRC_SIZE + 1;

	if(!(C_ASSERT(tmp_crc[0] == tmp_crc[1]) == true)) 
	{
		pkt->verification_state = SATR_PKT_INC_CRC;
		return SATR_PKT_INC_CRC; 
	}

	if(!(C_ASSERT(pkt->ser_type < MAX_SERVICES && pkt->ser_subtype < MAX_SUBTYPES && pkt->type <= TC) == true)) 
	{ 
		pkt->verification_state = SATR_PKT_ILLEGAL_PKT_TP;
		return SATR_PKT_ILLEGAL_PKT_TP; 
	}

	if(!(C_ASSERT(services_verification_TC_TM[pkt->ser_type][pkt->ser_subtype][pkt->type] == 1) == true)) 
	{ 
		pkt->verification_state = SATR_PKT_ILLEGAL_PKT_TP;
		//SYSVIEW_PRINT("INV TP %u,%u,%u,%u,%u", pkt->type, pkt->app_id, pkt->dest_id, pkt->ser_type, pkt->ser_subtype);
		UART2_BUF_O_Write_String_To_Buffer("INV TP ");
		UART2_BUF_O_Write_Number04_To_Buffer(pkt->type);
		UART2_BUF_O_Write_String_To_Buffer(", ");
		UART2_BUF_O_Write_Number04_To_Buffer(pkt->app_id);
		UART2_BUF_O_Write_String_To_Buffer(", ");
		UART2_BUF_O_Write_Number04_To_Buffer(pkt->dest_id);
		UART2_BUF_O_Write_String_To_Buffer(", ");
		UART2_BUF_O_Write_Number04_To_Buffer(pkt->ser_type);
		UART2_BUF_O_Write_String_To_Buffer(", ");
		UART2_BUF_O_Write_Number04_To_Buffer(pkt->ser_subtype);
		
		
		return SATR_PKT_ILLEGAL_PKT_TP; 
	}

	if(!(C_ASSERT(ver == ECSS_VER_NUMBER) == true)) 
	{
		pkt->verification_state = SATR_ERROR;
		return SATR_ERROR; 
	}

	if(!(C_ASSERT(tc_pus == ECSS_PUS_VER) == true))
	{
		pkt->verification_state = SATR_ERROR;
		return SATR_ERROR;
	}

	if(!(C_ASSERT(ccsds_sec_hdr == ECSS_SEC_HDR_FIELD_FLG) == true))
	{
		pkt->verification_state = SATR_ERROR;
		return SATR_ERROR;
	}

	if(!(C_ASSERT(pkt->type == TC || pkt->type == TM) == true))
	{
		pkt->verification_state = SATR_ERROR;
		return SATR_ERROR;
	}

	if(!(C_ASSERT(dfield_hdr == ECSS_DATA_FIELD_HDR_FLG) == true)) 
	{
		pkt->verification_state = SATR_ERROR;
		return SATR_ERROR;
	}

	if(!(C_ASSERT(pkt->ack == TC_ACK_NO || pkt->ack == TC_ACK_ACC) == true)) 
	{
		pkt->verification_state = SATR_ERROR;
		return SATR_ERROR;
	}

	if(!(C_ASSERT(pkt->seq_flags == TC_TM_SEQ_SPACKET) == true))
	{
		pkt->verification_state = SATR_ERROR;
		return SATR_ERROR;
	}

	/*assertion for data size depanding on pkt type*/
	//if(!C_ASSERT(pkt->len == pkt_size[app_id][type][subtype][generic] == true) {
	//    pkt->verification_state = SATR_ERROR;
	//    return SATR_ERROR;
	//}    

	for(int i = 0; i < pkt->len; i++) 
	{
		pkt->data[i] = buf[ECSS_DATA_OFFSET+i];
	}
	UART2_BUF_O_Write_pkt_To_Buffer(pkt);
	return SATR_OK;
}

/*buf: buffer to store the data to be sent, pkt: the data to be stored in the buffer, size: size of the array*/
SAT_returnState pack_pkt(uint8_t *buf, tc_tm_pkt *pkt, uint16_t *size)
{
	union _cnv cnv;
	uint16_t buf_pointer;

	if(!(C_ASSERT(buf != NULL && pkt != NULL && pkt->data != NULL  && size != NULL) == true))                        { return SATR_ERROR; }
	if(!(C_ASSERT(pkt->type == TC || pkt->type == TM) == true))                                                      { return SATR_ERROR; }
	if(!(C_ASSERT(pkt->app_id < LAST_APP_ID && pkt->dest_id < LAST_APP_ID && pkt->app_id != pkt->dest_id) == true))  { return SATR_ERROR; }

	cnv.cnv16[0] = pkt->app_id;

	buf[0] = ( ECSS_VER_NUMBER << 5 | pkt->type << 4 | ECSS_DATA_FIELD_HDR_FLG << 3 | cnv.cnv8[1]);
	buf[1] = cnv.cnv8[0];

	/*if the pkt was created in COMMS, it updates the counter*/
	if(pkt->type == TC && pkt->dest_id == SYSTEM_APP_ID)      { pkt->seq_count = pkt_state.seq_cnt[pkt->app_id]++; }
	else if(pkt->type == TM && pkt->app_id == SYSTEM_APP_ID)  { pkt->seq_count = pkt_state.seq_cnt[pkt->dest_id]++; }

	pkt->seq_flags = TC_TM_SEQ_SPACKET;
	cnv.cnv16[0] = pkt->seq_count;
	buf[2] = (pkt->seq_flags << 6 | cnv.cnv8[1]);
	buf[3] = cnv.cnv8[0];

	/* TYPE = 0 TM, TYPE = 1 TC*/
	if(pkt->type == TM) 
	{
		buf[6] = ECSS_PUS_VER << 4 ;
	} 
	else if(pkt->type == TC) 
	{
		buf[6] = ( ECSS_SEC_HDR_FIELD_FLG << 7 | ECSS_PUS_VER << 4 | pkt->ack);
	}

	buf[7] = pkt->ser_type;
	buf[8] = pkt->ser_subtype;
	buf[9] = pkt->dest_id; /*source or destination*/

	buf_pointer = ECSS_DATA_OFFSET;

	for(int i = 0; i < pkt->len; i++) 
	{
		buf[buf_pointer++] = pkt->data[i];
	}

	pkt->len += ECSS_DATA_HEADER_SIZE + ECSS_CRC_SIZE - 1;

	/*check if this is correct*/
	cnv.cnv16[0] = pkt->len;
	buf[4] = cnv.cnv8[1];
	buf[5] = cnv.cnv8[0];

	/*added it for ecss conformity, checksum in the ecss is defined to have 16 bits, we only use 8*/
	buf[buf_pointer++] = 0;
	checkSum(buf, buf_pointer-2, &buf[buf_pointer]);
	*size = buf_pointer + 1;

	if(!(C_ASSERT(*size > TC_MIN_PKT_SIZE && *size < MAX_PKT_SIZE) == true))       { return SATR_ERROR; }

	return SATR_OK;
}
 
void cnv32_8(const uint32_t from, uint8_t *to)
{
	union _cnv cnv;

	cnv.cnv32 = from;
	to[0] = cnv.cnv8[0];
	to[1] = cnv.cnv8[1];
	to[2] = cnv.cnv8[2];
	to[3] = cnv.cnv8[3];	
}

void cnv16_8(const uint16_t from, uint8_t *to)
{
	union _cnv cnv;

	cnv.cnv16[0] = from;
	to[0] = cnv.cnv8[0];
	to[1] = cnv.cnv8[1];
}

void cnv8_32(uint8_t *from, uint32_t *to)
{
 union _cnv cnv;

	cnv.cnv8[3] = from[3];
	cnv.cnv8[2] = from[2];
	cnv.cnv8[1] = from[1];
	cnv.cnv8[0] = from[0];
	*to = cnv.cnv32;
}

void cnv8_16(uint8_t *from, uint16_t *to)
{
	union _cnv cnv;

	cnv.cnv8[1] = from[0];
	cnv.cnv8[0] = from[1];
	*to = cnv.cnv16[0];
}

void cnv8_16LE(uint8_t *from, uint16_t *to)
{
	union _cnv cnv;

	cnv.cnv8[1] = from[1];
	cnv.cnv8[0] = from[0];
	*to = cnv.cnv16[0];
}

void cnvF_8(const float from, uint8_t *to)
{
 union _cnv cnv;

	cnv.cnvF = from;
	to[0] = cnv.cnv8[0];
	to[1] = cnv.cnv8[1];
	to[2] = cnv.cnv8[2];
	to[3] = cnv.cnv8[3];
}
void cnv8_F(uint8_t *from, float *to)
{
	union _cnv cnv;

	cnv.cnv8[3] = from[3];
	cnv.cnv8[2] = from[2];
	cnv.cnv8[1] = from[1];
	cnv.cnv8[0] = from[0];
	*to = cnv.cnvF;
}
void cnvD_8(const double from, uint8_t *to)
{
	union _cnv cnv;

	cnv.cnvD = from;
	to[0] = cnv.cnv8[0];
	to[1] = cnv.cnv8[1];
	to[2] = cnv.cnv8[2];
	to[3] = cnv.cnv8[3];
	to[4] = cnv.cnv8[4];
	to[5] = cnv.cnv8[5];
	to[6] = cnv.cnv8[6];
	to[7] = cnv.cnv8[7];
}
void cnv8_D(uint8_t *from, double *to)
{
	union _cnv cnv;

	cnv.cnv8[7] = from[7];
	cnv.cnv8[6] = from[6];
	cnv.cnv8[5] = from[5];
	cnv.cnv8[4] = from[4];
	cnv.cnv8[3] = from[3];
	cnv.cnv8[2] = from[2];
	cnv.cnv8[1] = from[1];
	cnv.cnv8[0] = from[0];
	*to = cnv.cnvD;
}

// Private functions
SAT_returnState sys_data_INIT(void) 
{
  for(uint8_t i = 0; i < LAST_APP_ID; i++) { pkt_state.seq_cnt[i] = 0; }
  return SATR_OK;
}



////////////////////////////////////////////////// STATUS //////////////////////////////////////////////////////////////


////////////////////////////////////////////////// TEST_SERVICE DONE ////////////////////////////////////////////////////////
#undef __FILE_ID__
#define __FILE_ID__ 3
extern SAT_returnState route_pkt(tc_tm_pkt *pkt);

// Getters
SAT_returnState test_app(tc_tm_pkt *pkt)
{
	UART2_BUF_O_Write_String_To_Buffer("test_app");
	tc_tm_pkt *temp_pkt = 0;

	if(!(C_ASSERT(pkt != NULL && pkt->data != NULL) == true))       { return SATR_ERROR; }
	if(!(C_ASSERT(pkt->ser_subtype == TC_CT_PERFORM_TEST || pkt->ser_subtype == TM_CT_REPORT_TEST) == true)) { return SATR_ERROR; }

	if(pkt->ser_subtype == TM_CT_REPORT_TEST) { return SATR_ERROR; }

	test_crt_pkt(&temp_pkt, pkt->dest_id);
	if(!(C_ASSERT(temp_pkt != NULL) == true)) { return SATR_ERROR; }

	route_pkt(temp_pkt);
	UART2_BUF_O_Write_pkt_To_Buffer(temp_pkt);
	return SATR_OK;
}
// SF_G
SAT_returnState test_crt_pkt(tc_tm_pkt **pkt, TC_TM_app_id dest_id)
{
	*pkt = get_pkt(PKT_NORMAL);
	if(!(C_ASSERT(*pkt != NULL) == true)) { return SATR_ERROR; }
	crt_pkt(*pkt, SYSTEM_APP_ID, TM, TC_ACK_NO, TC_TEST_SERVICE, TM_CT_REPORT_TEST, dest_id);

	(*pkt)->len = 0;

	return SATR_OK;
}

////////////////////////////////////////////////// VERIFICATION_SERVICE DONE ////////////////////////////////////////////////
#define ECSS_VR_DATA_LEN_SUCCESS 4
#define ECSS_VR_DATA_LEN_FAILURE 5

#undef __FILE_ID__
#define __FILE_ID__ 1

extern SAT_returnState route_pkt(tc_tm_pkt *pkt);

// Getters
SAT_returnState verification_app(const tc_tm_pkt *pkt)
{
	if(!(C_ASSERT(pkt != NULL && pkt->data != NULL) == true)) { return SATR_ERROR; }

	if(pkt->ser_type == TC_VERIFICATION_SERVICE) 
	{
		if(!(C_ASSERT(pkt->ser_subtype == TM_VR_ACCEPTANCE_SUCCESS || pkt->ser_subtype == TM_VR_ACCEPTANCE_FAILURE) == true)) { return SATR_ERROR; }
		//SYSVIEW_PRINT("V %d", pkt->app_id);
		UART2_BUF_O_Write_String_To_Buffer("V ");
		UART2_BUF_O_Write_Number04_To_Buffer(pkt->app_id);

	}
	else 
	{
		if(!(C_ASSERT(pkt->ack == TC_ACK_ACC || pkt->ack == TC_ACK_NO) == true)) { return SATR_ERROR; }
		if(pkt->type == TM) { return SATR_OK; }
		if(pkt->app_id != SYSTEM_APP_ID && (pkt->verification_state == SATR_OK || pkt->verification_state == SATR_PKT_INIT)) { return SATR_OK; }
		if(pkt->ack == TC_ACK_NO) { return SATR_OK; }
		else if(pkt->ack == TC_ACK_ACC) 
		{
			tc_tm_pkt *temp_pkt = 0;

			verification_crt_pkt(pkt, &temp_pkt, pkt->verification_state);
			if(!(C_ASSERT(temp_pkt != NULL) == true)) { return SATR_ERROR; }

			route_pkt(temp_pkt);
		}
	}
	return SATR_OK;
}
// SF_G
SAT_returnState verification_crt_pkt(const tc_tm_pkt *pkt, tc_tm_pkt **out, SAT_returnState res)
{
		uint8_t subtype;

	if(!(C_ASSERT(pkt != NULL && pkt->data != NULL) == true)) { return SATR_ERROR; }
	if(!(C_ASSERT(res < SATR_LAST) == true))                  { res = SATR_VER_ERROR; }
	*out = get_pkt(PKT_NORMAL);
	if(!(C_ASSERT(*out != NULL) == true)) { return SATR_ERROR; }

	subtype = TM_VR_ACCEPTANCE_SUCCESS;

	(*out)->data[0] = (ECSS_VER_NUMBER << 5 | pkt->type << 4 | ECSS_DATA_FIELD_HDR_FLG << 3);
	(*out)->data[1] = (uint8_t)pkt->app_id;

	cnv16_8(pkt->seq_count, &(*out)->data[2]);
	(*out)->data[2] |= (pkt->seq_flags << 6 );

	(*out)->len = ECSS_VR_DATA_LEN_SUCCESS;

	if(res != SATR_OK) 
	{
		(*out)->data[4] = (uint8_t)res;        /*failure reason*/
		subtype = TM_VR_ACCEPTANCE_FAILURE;
		(*out)->len = ECSS_VR_DATA_LEN_FAILURE;
	}

	crt_pkt(*out, (TC_TM_app_id)SYSTEM_APP_ID, TM, TC_ACK_NO, TC_VERIFICATION_SERVICE, subtype, pkt->dest_id);

	return SATR_OK;
}

////////////////////////////////////////////////// UTILS ////////////////////////////////////////////////


////////////////////////////////////////////////// LOG ///////////////////////////////////////////////////////////



////////////////////////////////////////////// PERSISTENT MEM DONE /////////////////////////////////////////////////////
uint32_t comms_persistent_mem_init(void)
{
  return flash_INIT();
}

/**
 * Stores a word at the one word long flash memory of the processor
 * @param word the data that are going to be stored
 * @param offset the offset from the start of the flash memory blocksize_t offset
 */
//void comms_write_persistent_word(uint32_t word, size_t offset)
//{
//  flash_write_trasmit(word, offset);
//}

/**
 * Reads a word from the flash memory
 * @param offset the offset from the start of the flash memory blocksize_t offset
 * @return the word value
 */
uint32_t comms_read_persistent_word(size_t offset)
{
  return flash_read_trasmit(offset);
}


////////////////////////////////////////////// PACKET_POOL DONE //////////////////////////////////////////////////////////////////////
#undef __FILE_ID__
#define __FILE_ID__ 35

struct _pkt_pool
{
	tc_tm_pkt pkt[POOL_PKT_SIZE];
	uint8_t free[POOL_PKT_SIZE];
	uint32_t time[POOL_PKT_SIZE];
	uint8_t data[POOL_PKT_SIZE][MAX_PKT_DATA];
	uint32_t time_delta[POOL_PKT_SIZE];

	tc_tm_pkt pkt_ext[POOL_PKT_EXT_SIZE];
	uint8_t free_ext[POOL_PKT_EXT_SIZE];
	uint32_t time_ext[POOL_PKT_EXT_SIZE];
	uint8_t data_ext[POOL_PKT_EXT_SIZE][MAX_PKT_EXT_DATA];
	uint32_t time_delta_ext[POOL_PKT_EXT_SIZE];
};
static struct _pkt_pool pkt_pool;


//Init
SAT_returnState pkt_pool_INIT(void) 
{
	for(uint8_t i = 0; i < POOL_PKT_SIZE; i++) 
	{
		 pkt_pool.free[i] = true;
		 pkt_pool.pkt[i].data = &pkt_pool.data[i][0];
	}

	for(uint8_t i = 0; i < POOL_PKT_EXT_SIZE; i++) 
	{
		 pkt_pool.free_ext[i] = true;
		 pkt_pool.pkt_ext[i].data = &pkt_pool.data_ext[i][0];
	}

	return SATR_OK;
}

//Getters
tc_tm_pkt * get_pkt(const uint16_t size) 
{
	if(size <= PKT_NORMAL) 
	{
		for(uint8_t i = 0; i < POOL_PKT_SIZE; i++) 
		{
			if(pkt_pool.free[i] == true) 
			{
				traceGET_PKT(i);//may be will be removed <-------------------------------------
				pkt_pool.free[i] = false;
				pkt_pool.time[i] = getCurrentTick();
				pkt_pool.pkt[i].verification_state = SATR_PKT_INIT;
				return &pkt_pool.pkt[i];
			}
		}
	} 
	else 
	{
		for(uint8_t i = 0; i < POOL_PKT_EXT_SIZE; i++) 
		{
			if(pkt_pool.free_ext[i] == true) 
			{
				traceGET_PKT(i + POOL_PKT_SIZE);//may be will be removed <--------------------------------
				pkt_pool.free_ext[i] = false;
				pkt_pool.time_ext[i] = getCurrentTick();
				pkt_pool.pkt_ext[i].verification_state = SATR_PKT_INIT;
				return &pkt_pool.pkt_ext[i];
			}
		}
	}
	return NULL;
}
uint8_t is_free_pkt(tc_tm_pkt *pkt) 
{
	for(uint8_t i = 0; i <= POOL_PKT_SIZE; i++) 
	{
		if(&pkt_pool.pkt[i] == pkt) 
		{
			if(pkt_pool.free[i] == true) 
			{
				return true;
			}
			return false;
		}
	}

	for(uint8_t i = 0; i < POOL_PKT_EXT_SIZE; i++) 
	{
		if(&pkt_pool.pkt_ext[i] == pkt) 
		{
			if(pkt_pool.free_ext[i] == true) 
			{
				return true;
			}
			return false;
		}
	}
	return true;
}
//Setters
SAT_returnState free_pkt(tc_tm_pkt *pkt) 
{
	if(!(C_ASSERT(pkt != NULL) == true)) { return SATR_ERROR; }

	for(uint8_t i = 0; i <= POOL_PKT_SIZE; i++) 
	{
		if(&pkt_pool.pkt[i] == pkt) 
		{
			traceFREE_PKT(i);
			pkt_pool.free[i] = true;
			pkt_pool.time_delta[i]= getCurrentTick() - pkt_pool.time[i];
			return SATR_OK;
		}
	}

	for(uint8_t i = 0; i < POOL_PKT_EXT_SIZE; i++) 
	{
		if(&pkt_pool.pkt_ext[i] == pkt) 
		{
			traceFREE_PKT(i + POOL_PKT_SIZE);
			pkt_pool.free_ext[i] = true;
			pkt_pool.time_delta_ext[i]= getCurrentTick() - pkt_pool.time_ext[i];
			return SATR_OK;
		}
	}
	return SATR_ERROR;
}

void pkt_pool_IDLE(uint32_t tmp_time) 
{
	for(uint8_t i = 0; i < POOL_PKT_SIZE; i++) 
	{
		if(pkt_pool.free[i] == false && pkt_pool.time[i] - tmp_time > PKT_TIMEOUT) 
		{
			pkt_pool.free[i] = true;
			traceGC_POOL_PKT(i);
		}
	}

	for(uint8_t i = 0; i < POOL_PKT_EXT_SIZE; i++) 
	{
		if(pkt_pool.free_ext[i] == false && pkt_pool.time_ext[i] - tmp_time > PKT_TIMEOUT) 
		{
			pkt_pool.free_ext[i] = true;
			traceGC_POOL_PKT(i + POOL_PKT_SIZE);
		}
	}
}

///////////////////////////////////////////////// POWER_CONTROL ///////////////////////////////////////////////////////////


///////////////////////////////////////////////// HSI Added functions ///////////////////////////////////////////////////////////
void HSI_reset_source(uint8_t *src) 
{

    *src =   RCC_GetFlagStatus(RCC_FLAG_BORRST);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_PINRST) << 1);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_PORRST) << 2);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_SFTRST) << 3);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_IWDGRST)<< 4);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_WWDGRST)<< 5);
    *src |= (RCC_GetFlagStatus(RCC_FLAG_LPWRRST)<< 6);
 
    RCC_ClearFlag();

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




