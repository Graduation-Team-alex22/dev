#include "mgn_sensor.h"
#include "../support_functions/ttrd2-05a-t0401a-v001a_timeout_t3.h"

/************* LOCAL DEFINITIONS ***************/
// costants
#define 	I2C_EVENT_WAIT				50			// microseconds
#define 	MGN_ID_A_VALUE				0x48

#ifdef HMC_DEVICE
// register addresses
#define 	MGN_REG_CONFIG_A			0x00
#define 	MGN_REG_CONFIG_B			0x01
#define 	MGN_REG_MODE					0x02
#define 	MGN_REG_DATA_XH				0x03
#define 	MGN_REG_DATA_XL				0x04
#define 	MGN_REG_DATA_YH				0x05
#define 	MGN_REG_DATA_YL				0x06
#define 	MGN_REG_DATA_ZH				0x07
#define 	MGN_REG_DATA_ZL				0x08
#define 	MGN_REG_STATUS				0x09
#define 	MGN_REG_ID_A					0x0A
#define 	MGN_REG_ID_B					0x0B
#define 	MGN_REG_ID_C					0x0C


/************* LOCAL VARIABLES ***************/
static I2C_TypeDef* local_I2Cx;
static float mgn_range = 0;
static mgn_sensor_t mgn_data;

/************* PRIVATE FUNCTIONS ************/
uint8_t MGN_Configure(mgn_init_t* mgn_init);
static inline float calculate_range(uint8_t gain);

uint8_t MGN_Sensor_Init(I2C_TypeDef* I2Cx, mgn_init_t* mgn_init)
{
	uint8_t error_code;
	
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	assert_param(IS_AVERAGE_SAMPLES_X(mgn_init->average_samples));
	assert_param(IS_OUTPUT_RATE_X(mgn_init->output_rate));
	assert_param(IS_BIAS_MODE_X(mgn_init->bias_mode));
	assert_param(IS_GAIN_X(mgn_init->gain));
	assert_param(IS_HIGH_SPEED_F(mgn_init->high_speed));
	assert_param(IS_OP_MODE_X(mgn_init->op_mode));
	
	// update local variables
	local_I2Cx = I2Cx;
	mgn_range = calculate_range(mgn_init->gain);
	
	// Set up timeout mechanism
  TIMEOUT_T3_USEC_Init();
	
	// check if the device is connected
	error_code = I2Cx_Is_Device_Connected(I2Cx, 0x1A, MGN_REG_ID_A, MGN_ID_A_VALUE, I2C_EVENT_WAIT);
	if(error_code){ return error_code+100;}
	
	// configure the device
	error_code = MGN_Configure(mgn_init);
	if(error_code){ return error_code+160;}
	
	// wait for 6 ms
	TIMEOUT_T3_USEC_Start();
	while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(6000));
	
	return 0;
}

uint8_t MGN_Configure(mgn_init_t* mgn_init)
{
	uint8_t error_code, tmpreg[3] = {0, 0, 0};
	
	// Configuration register A : Samples, output rate, bias mode
	tmpreg[0] = ( mgn_init->average_samples | mgn_init->output_rate | mgn_init->bias_mode );
	
	// Configuration register B : gain
	tmpreg[1] = mgn_init->gain ;
	
	// Mode register : operation mode, high speed
	tmpreg[2] = mgn_init->op_mode | mgn_init->high_speed ;
	
	// write to the registers
	error_code = I2Cx_Send_Bytes(local_I2Cx, MGN_I2C_ADD, MGN_REG_CONFIG_A, tmpreg, 3, I2C_EVENT_WAIT);
	
	return error_code;
}

inline float calculate_range(uint8_t gain)
{
	float range;
	switch(gain)
	{
		case GAIN_1370:
			range = 0.88;
		break;
		
		case GAIN_1090:
			range = 1.3;
		break;
		
		case GAIN_820:
			range = 1.9;
		break;
		
		case GAIN_660:
			range = 2.5;
		break;
		
		case GAIN_440:
			range = 4;
		break;
		
		case GAIN_390:
			range = 4.7;
		break;
		
		case GAIN_330:
			range = 5.6;
		break;
		
		case GAIN_230:
			range = 8.1;
		break;
		
		default:
			// we should not be here
			return -1;
	}
	return range;
}

uint8_t MGN_Sensor_Update(void)
{
	uint8_t error_code, data[6] = {0};
	
	error_code =I2Cx_Recv_Bytes(local_I2Cx, MGN_I2C_ADD, MGN_REG_DATA_XH, data, 6, I2C_EVENT_WAIT);
	if(error_code) {return error_code ;}
	
	// extract raw data
	mgn_data.raw[0] = ((int16_t) data[0] << 8) | data[1] ;
	mgn_data.raw[1] = ((int16_t) data[2] << 8) | data[3] ;
	mgn_data.raw[2] = ((int16_t) data[4] << 8) | data[5] ;
	
	// calculate value in Gauss
	float scale_factor = mgn_range/2048.0;
	mgn_data.mag[0] = (float)mgn_data.raw[0] * scale_factor;
	mgn_data.mag[1] = (float)mgn_data.raw[1] * scale_factor;
	mgn_data.mag[2] = (float)mgn_data.raw[2] * scale_factor;
	
	return 0;
}

mgn_sensor_t MGN_Sensor_GetData(void)
{
	return mgn_data;
}
#endif

#ifdef QMC_DEVICE
// register addresses
#define 	MGN_REG_DATA_XL				0x00
#define 	MGN_REG_DATA_XH				0x01
#define 	MGN_REG_DATA_YL				0x02
#define 	MGN_REG_DATA_YH				0x03
#define 	MGN_REG_DATA_ZL				0x04
#define 	MGN_REG_DATA_ZH				0x05
#define 	MGN_REG_STATUS				0x06
#define 	MGN_REG_TMP_L					0x07
#define 	MGN_REG_TMP_H					0x08
#define 	MGN_REG_CONTROL1			0x09
#define 	MGN_REG_CONTROL2			0x0A
#define 	MGN_REG_SET_PERIOD		0x0B

/************* LOCAL VARIABLES ***************/
static I2C_TypeDef* mgn_I2Cx_g;
static mgn_sensor_t mgn_data_g;
static uint8_t mgn_range_g;

/************* PRIVATE FUNCTIONS ************/
uint8_t MGN_Configure(mgn_init_t* mgn_init);

uint8_t MGN_Sensor_Init(I2C_TypeDef* I2Cx, mgn_init_t* mgn_init)
{
	uint8_t error_code;
	
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	assert_param(MGN_IS_OVER_SAMPLING_X(mgn_init->over_sampling));
	assert_param(MGN_IS_OUTPUT_RATE_X(mgn_init->output_rate));
	assert_param(MGN_IS_RANGE_X(mgn_init->range));
	assert_param(MGN_IS_OP_MODE_X(mgn_init->op_mode));
	
	// update local variables
	mgn_I2Cx_g = I2Cx;
	// translate bit mask to range value in Gauss
	mgn_range_g = (mgn_init->range >> 4) * 6 + 2;
	
	// Set up timeout mechanism
  TIMEOUT_T3_USEC_Init();
	
	// check if the device is connected - no check for whoiam
	error_code = I2Cx_Is_Device_Connected(I2Cx, MGN_I2C_ADD, 0xFF, 0xFF, I2C_EVENT_WAIT);
	if(error_code){ return error_code+100;}
	
	// configure the device
	error_code = MGN_Configure(mgn_init);
	if(error_code){ return error_code+160;}
	
	// wait for 6 ms
	TIMEOUT_T3_USEC_Start();
	while(COUNTING == TIMEOUT_T3_USEC_Get_Timer_State(6000));
	
	return 0;
}

uint8_t MGN_Configure(mgn_init_t* mgn_init)
{
	uint8_t error_code, tmpreg;
	
	// configure set/reset period register (recommended by datasheet)
	tmpreg = 0x01;
	I2Cx_Send_Bytes(mgn_I2Cx_g, MGN_I2C_ADD, MGN_REG_SET_PERIOD, &tmpreg, 1, I2C_EVENT_WAIT);
	
	// Control register : over sampling, output rate, range , operation mode
	tmpreg = mgn_init->over_sampling | mgn_init->output_rate | mgn_init->range | mgn_init->op_mode;
	
	// write to Control register
	error_code = I2Cx_Send_Bytes(mgn_I2Cx_g, MGN_I2C_ADD, MGN_REG_CONTROL1, &tmpreg, 1, I2C_EVENT_WAIT);
	if(error_code) {return error_code ;}
	
	// enable rolling pointer in control register 2
	tmpreg = 0x01 << 6;
	error_code = I2Cx_Send_Bytes(mgn_I2Cx_g, MGN_I2C_ADD, MGN_REG_CONTROL2, &tmpreg, 1, I2C_EVENT_WAIT);
	
	return error_code;
}

uint8_t MGN_Sensor_Update(void)
{
	uint8_t error_code, data[6] = {0};
	
	error_code =I2Cx_Recv_Bytes(mgn_I2Cx_g, MGN_I2C_ADD, MGN_REG_DATA_XL, data, 6, I2C_EVENT_WAIT);
	if(error_code) {return error_code ;}
	
	// extract raw data
	mgn_data_g.raw[0] = ((int16_t) data[1] << 8) | data[0] ;
	mgn_data_g.raw[1] = ((int16_t) data[3] << 8) | data[2] ;
	mgn_data_g.raw[2] = ((int16_t) data[5] << 8) | data[4] ;
	
	// calculate value in Gauss
	mgn_data_g.mag[0] = (float)mgn_data_g.raw[0] * mgn_range_g /32768.0;
	mgn_data_g.mag[1] = (float)mgn_data_g.raw[1] * mgn_range_g /32768.0;
	mgn_data_g.mag[2] = (float)mgn_data_g.raw[2] * mgn_range_g /32768.0;
	
	return 0;
}

mgn_sensor_t MGN_Sensor_GetData(void)
{
	return mgn_data_g;
}

#endif
