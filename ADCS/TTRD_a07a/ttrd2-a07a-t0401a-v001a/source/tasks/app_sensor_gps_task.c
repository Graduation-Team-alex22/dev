#include "stdint.h"
#include "../port/port.h"
#include "../adcs/adcs_sensors/gps_sensor.h"

#include "app_sensor_gps_task.h" 

void App_Sensor_Gps_Init(void)
{

  //----------set up GPIO pin A1 as usart4 RX pin-----------
  GPIO_InitTypeDef GPIO_InitStructure;
  // GPIOB clock enable 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  // GPIO config
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1; 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);

  GPS_Sensor_Init(UART4, DMA1_Stream2, DMA_Channel_4);
	
	REG_CONFIG_CHECKS_GPIO_Store();
	
	/*
	UART2_BUF_O_Write_String_To_Buffer("GPS TEST INIT\n");
	UART2_BUF_O_Send_All_Data();
  */	
}

uint32_t App_Sensor_Gps_Update(void)
{
	//char buff[50] = {0};
	//gps_sensor_t t;
	char buf[100] = {0};
	uint16_t sn = GPS_Sensor_Update();
	sn = GPS_Sensor_GetData(buf);
	sn = GPS_Sensor_GetData(buf);
	sn = GPS_Sensor_GetData(buf);
	
	/*
	sprintf(buff, "\nSN: %d \n", sn);
	UART2_BUF_O_Write_String_To_Buffer(buff);
	
	sn = GPS_Sensor_GetData(buf);
	t = get_struct();
	sprintf(buff, "TR: %d \n DOP:  %.2f  %.2f  %.2f\n", sn, t.DOP[0], t.DOP[1], t.DOP[2]);
	UART2_BUF_O_Write_String_To_Buffer(buff);
	sprintf(buff, "TIME: %.2f  LAT: %f  LON: %5.5f  ALT: %2.2f\n",t.utc_time, t.p_gps_lla[0], t.p_gps_lla[1], t.p_gps_lla[2]);
	UART2_BUF_O_Write_String_To_Buffer(buff);
	sprintf(buff, "FIX: %d  SatNum: %2d\n",t.d3fix, t.num_sat);
	UART2_BUF_O_Write_String_To_Buffer(buff);
	UART2_BUF_O_Write_String_To_Buffer(buf);
	
	sn = GPS_Sensor_GetData(buf);
	t = get_struct();
	sprintf(buff, "TR: %d \n DOP:  %.2f  %.2f  %.2f\n", sn, t.DOP[0], t.DOP[1], t.DOP[2]);
	UART2_BUF_O_Write_String_To_Buffer(buff);
	sprintf(buff, "TIME: %.2f  LAT: %f  LON: %5.5f  ALT: %2.2f\n",t.utc_time, t.p_gps_lla[0], t.p_gps_lla[1], t.p_gps_lla[2]);
	UART2_BUF_O_Write_String_To_Buffer(buff);
	sprintf(buff, "FIX: %d  SatNum: %2d\n",t.d3fix, t.num_sat);
	UART2_BUF_O_Write_String_To_Buffer(buff);
	UART2_BUF_O_Write_String_To_Buffer(buf);
	
	sn = GPS_Sensor_GetData(buf);
	t = get_struct();
	sprintf(buff, "TR: %d \n DOP:  %.2f  %.2f  %.2f\n", sn, t.DOP[0], t.DOP[1], t.DOP[2]);
	UART2_BUF_O_Write_String_To_Buffer(buff);
	sprintf(buff, "TIME: %.2f  LAT: %f  LON: %5.5f  ALT: %2.2f\n",t.utc_time, t.p_gps_lla[0], t.p_gps_lla[1], t.p_gps_lla[2]);
	UART2_BUF_O_Write_String_To_Buffer(buff);
	sprintf(buff, "FIX: %d  SatNum: %2d\n",t.d3fix, t.num_sat);
	UART2_BUF_O_Write_String_To_Buffer(buff);
	UART2_BUF_O_Write_String_To_Buffer(buf);
	*/
	return RETURN_NORMAL_STATE;
}
