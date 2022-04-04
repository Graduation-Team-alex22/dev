#include "gps_sensor.h"

// ------ Private constants --------------------------------------------------
// NMEA buffer 2d array size
#define NMEA_SENTENCE_BUFFER_SIZE		20
#define NMEA_SENTENCE_MAX_CHAR			96


// ------ Private variables --------------------------------------------------
static char Rx_buffer_g[GPS_RX_BUFFER_SIZE];
static DMA_Stream_TypeDef * dma_streamx_g;
static gps_sensor_t gps_sensor_data ;
// NMEA sentences buffer
static char nmea_sentences_g[NMEA_SENTENCE_BUFFER_SIZE][NMEA_SENTENCE_MAX_CHAR] = {0};
static uint8_t nmea_pointer_g = 0;


// -- PUBLIC FUNCTIONS' IMPLEMENTATION ---------------------------
uint8_t GPS_Sensor_Init(DMA_Stream_TypeDef * DMA_streamx)
{	
   assert_param(IS_DMA_ALL_PERIPH(DMA_streamx));

   dma_streamx_g = DMA_streamx;
   
   // update device status
   gps_sensor_data.status = DEVICE_OK;
   
   // set up the NMEA parser
   nmea_parser_init();

   return 0;
}

uint16_t GPS_Sensor_Update(void)
{
	// an index to mark where to start proccessing
	static uint16_t buffer_start_index =0;
	uint16_t buffer_end_index = 0;
	static uint16_t sentence_char_index = 0;
	
   // Pre-Condition Checks [Start]
   // check if the NMEA sentence buffer is full
   if(nmea_pointer_g == NMEA_SENTENCE_BUFFER_SIZE)
   {
      nmea_pointer_g = 0;
      return ERROR_CODE_NMEA_BUFFER_FULL;
   }
   // Pre-Condition Checks [End]
   
	// get new sentences from the buffer [Start]
	// check if new data has been received
	buffer_end_index = GPS_RX_BUFFER_SIZE - DMA_GetCurrDataCounter(dma_streamx_g);
	
   while(buffer_end_index != buffer_start_index)
   {
      if(Rx_buffer_g[buffer_start_index] == '$')
      {
         // write to the beginning of the sentence
         sentence_char_index = 0;
      }
      
      nmea_sentences_g[nmea_pointer_g][sentence_char_index++] = Rx_buffer_g[buffer_start_index];
      
      if(Rx_buffer_g[buffer_start_index] == '\n')
      {
         nmea_sentences_g[nmea_pointer_g][sentence_char_index] = 0;
         nmea_pointer_g++;
         sentence_char_index = 0;
      }
      buffer_start_index++;
      
      // check boundary condition
      if( buffer_start_index == GPS_RX_BUFFER_SIZE ) buffer_start_index = 0;
   }
	// get new sentences from the buffer [End]
	
	// process new sentences in the buffer [Start]
	while(nmea_pointer_g)
   {
      nmea_sentence_parsing(nmea_sentences_g[nmea_pointer_g]);	
      nmea_pointer_g--;
   }
	// process new sentences in the buffer [End]
	
   // translate parser output [Start]
	parser_output_t o = nmea_sentence_getOutput();
	
	gps_sensor_data.utc_time = o.time;
	gps_sensor_data.num_sat = o.num_sat;
	gps_sensor_data.d3fix = o.fix;
	gps_sensor_data.DOP[0] = o.DOP[0];
	gps_sensor_data.DOP[1] = o.DOP[1];
	gps_sensor_data.DOP[2] = o.DOP[2];
	gps_sensor_data.p_gps_lla[0] = o.lat;
	gps_sensor_data.p_gps_lla[1] = o.lon;
	gps_sensor_data.p_gps_lla[2] = o.alt;
   
   
   // translate parser output [End]
      
	return 0;
}

gps_sensor_t GPS_Sensor_GetData(void)
{
	return gps_sensor_data;
}

char* GPS_Sensor_get_pBuffer(void)
{
   return Rx_buffer_g;
}
