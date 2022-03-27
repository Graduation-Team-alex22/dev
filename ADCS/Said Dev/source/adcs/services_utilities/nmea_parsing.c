#include "nmea_parsing.h"
#include "../services_utilities/common.h"

//-- PRIVATE DEFINITIONS -----------------
#define MAX_ROOT_CHILDS		2
#define NMEA_SENTENCE_MAX_CHAR 96

#define ERROR_CODE_FIELD_LENGTH	190
#define ERROR_CODE_BAD_NODE_RV	191

//-- PRIVATE VARIABLE --------------------
static parser_output_t parser_output;

//-- NODE STRUCTURES ---------------------
typedef struct _node_t{
	char* ID_str;
	uint8_t feilds_num;               // number of fields in NMEA format
	void* pData;                      // arbitrary data member (each node may/can use it differently)
	char* pStr;                       // NMEA string to proccess
	uint8_t (*fun_ptr)(void*);				// the node proccessing function pointer
	struct _node_t* child1;           // pointer to next node
} node_t;

typedef struct {
	char* pStr;
	node_t* childs[MAX_ROOT_CHILDS];
} root_node_t;

//-- NODES INSTANCES ---------------------
// GPGGA nodes
static node_t tr_altitude = {.feilds_num = 1, .pData = &parser_output.alt, .child1 = 0};
static node_t tr_satnum = {.feilds_num = 2, .pData = &parser_output.num_sat, .child1 = &tr_altitude};
static node_t tr_fix = {.feilds_num = 1, .pData = &parser_output.fix, .child1 = &tr_satnum};
static node_t tr_longitude = {.feilds_num = 2, .pData = &parser_output.lon, .child1 = &tr_fix};
static node_t tr_latitude = {.feilds_num = 2, .pData = &parser_output.lat, .child1 = &tr_longitude};
static node_t tr_time = {.feilds_num = 1, .pData = &parser_output.time, .child1 = &tr_latitude};
static node_t tr_gpgga = {.ID_str="GPGGA", .feilds_num = 1, .pData = 0, .child1 = &tr_time};
// GPGSA nodes
static node_t tr_vdop = {.feilds_num = 1, .pData = (void*)(&parser_output.DOP[2]), .child1 = 0};
static node_t tr_hdop = {.feilds_num = 1, .pData = (void*)(&parser_output.DOP[1]), .child1 = &tr_vdop};
static node_t tr_pdop = {.feilds_num = 1, .pData = (void*)(&parser_output.DOP[0]), .child1 = &tr_hdop};
static node_t tr_gpgsa = {.ID_str="GPGSA", .feilds_num = 15, .pData = 0, .child1 = &tr_pdop};
// ROOT $
static root_node_t tr_root = {.childs[0] = &tr_gpgsa, .childs[1] = &tr_gpgga};
//----------------------------------------

//-- NODE PROCCESSING FUNCTION -----------
uint8_t xnode_proccess(node_t* this_node)
{
	// check on node ID if exists
	if(this_node->ID_str != 0)
	{
		for(uint8_t i = 0; i < 5; i++)
		{
			if(this_node->pStr[i] != this_node->ID_str[i]) return 2;
		}
	}
	
	// execute the node proccessing function if required
	if(this_node->fun_ptr != 0)
	{
		uint8_t node_rv = (this_node->fun_ptr)(this_node);
		if(node_rv) return ERROR_CODE_BAD_NODE_RV;
	}
	
	// if a child node doesn't exist, return 0
	if(this_node->child1 == 0) return 0;
	
	// passs the string to the child node after skipping
	// the string part related to the current node
	uint8_t comma_count = 0;
	for(uint8_t i=0; i<NMEA_SENTENCE_MAX_CHAR; i++)
	{
		if(',' == this_node->pStr[i]) comma_count++;
		if(comma_count == this_node->feilds_num)
		{
			this_node->child1->pStr = this_node->pStr + i + 1;
			// execute the child node
			return xnode_proccess(this_node->child1);
		}
	}
	
	// we shouldn't be here
	// if we are here, there is a problem in node feild lentgh
	return ERROR_CODE_FIELD_LENGTH;
}

//-- PARSER AUXILARY NODE FUNCTIONS ------
// Root node function
uint8_t root_f(void* arg){
	// compare the string to ID "$". if not equal, return
	if(tr_root.pStr[0] != '$') return 1;
	
	// loop over childs and execute them
	uint8_t rv = 0;
	for(uint8_t i = 0; i < MAX_ROOT_CHILDS; i++)
	{
		// check if a child exists at this location
		if(tr_root.childs[i] != 0)
		{
			tr_root.childs[i]->pStr = tr_root.pStr + 1;
			rv += xnode_proccess(tr_root.childs[i]) - 2;
		}
	}
	
	return rv + 2;
}

uint8_t satnum_f(void* self_node)
{
	node_t* this_node = (node_t*)self_node;
	
	uint8_t value = 0;
	
	value = (this_node->pStr[0] - '0') * 10
				+ (this_node->pStr[1] - '0') ;
	
	*(uint8_t*)(this_node)->pData = value;
	
	return 0;
}

uint8_t fix_f(void* self_node)
{
	node_t* this_node = (node_t*)self_node;
	
	*(uint8_t*)(this_node)->pData = (this_node->pStr[0] - '0');
	
	return 0;
}

uint8_t lla_f(void* self_node){
	double lla_value;
	node_t* this_node = (node_t*)self_node;
	
	uint8_t str_len = 0;
	
	// extract float from pStr
	lla_value = str_to_float_nl(this_node->pStr, &str_len);
	
	// check for direction N/S
	if(this_node->pStr[str_len+2] == 'S' || this_node->pStr[str_len+2] == 'W') lla_value = -lla_value;
	
	// store it
	*(double*)(this_node)->pData = lla_value;
	
	return 0;
	
}

uint8_t time_f(void* self_node)
{
	float time;
	node_t* this_node = (node_t*)self_node;
	
	// extract float from pStr
	time = str_to_float(this_node->pStr, 9 );

	// store it
	*(float*)(this_node)->pData = time;
	
	return 0;
}

// pdop, hdop, vdop nodes
uint8_t xdop_f(void* self_node)
{
	float xdop_value = 1.5;
	
	node_t* this_node = (node_t*)self_node;
	
	// extract float vdop from pStr
	xdop_value = str_to_float(this_node->pStr, 4);
	
	// store it
	*(float*)(this_node)->pData = xdop_value;
	
	return 0;
}

uint8_t nmea_parser_init(void)
{
	tr_pdop.fun_ptr = xdop_f;
	tr_hdop.fun_ptr = xdop_f;
	tr_vdop.fun_ptr = xdop_f;
	tr_time.fun_ptr = time_f;
	tr_altitude.fun_ptr = lla_f;
	tr_latitude.fun_ptr = lla_f;
	tr_longitude.fun_ptr = lla_f;
	tr_fix.fun_ptr = fix_f;
	tr_satnum.fun_ptr = satnum_f;
	
	return 0;
}

uint8_t nmea_sentence_parsing(char* nmea_sen_str)
{
	tr_root.pStr = nmea_sen_str;
	return root_f(0);
}

parser_output_t nmea_sentence_getOutput(void)
{
	return parser_output;
}
