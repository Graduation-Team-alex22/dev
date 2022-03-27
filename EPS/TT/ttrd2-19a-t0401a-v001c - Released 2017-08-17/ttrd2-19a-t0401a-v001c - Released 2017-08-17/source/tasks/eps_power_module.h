// eps_power_module.h

#include "stm32f4xx_adc.h"



#ifndef _EPS_POWER_MODULE_H_
#define _EPS_POWER_MODULE_H_
//////

typedef enum 
{
  HAL_UNLOCKED = 0x00,
  HAL_LOCKED   = 0x01  
} HAL_LockTypeDef;
typedef struct
{
  uint32_t Prescaler;         /*!< Specifies the prescaler value used to divide the TIM clock.
                                   This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF */

  uint32_t CounterMode;       /*!< Specifies the counter mode.
                                   This parameter can be a value of @ref TIM_Counter_Mode */

  uint32_t Period;            /*!< Specifies the period value to be loaded into the active
                                   Auto-Reload Register at the next update event.
                                   This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF.  */

  uint32_t ClockDivision;     /*!< Specifies the clock division.
                                   This parameter can be a value of @ref TIM_ClockDivision */

} TIM_Base_InitTypeDef;

typedef enum
{
  HAL_TIM_ACTIVE_CHANNEL_1        = 0x01,    /*!< The active channel is 1     */
  HAL_TIM_ACTIVE_CHANNEL_2        = 0x02,    /*!< The active channel is 2     */
  HAL_TIM_ACTIVE_CHANNEL_3        = 0x04,    /*!< The active channel is 3     */
  HAL_TIM_ACTIVE_CHANNEL_4        = 0x08,    /*!< The active channel is 4     */
  HAL_TIM_ACTIVE_CHANNEL_CLEARED  = 0x00     /*!< All active channels cleared */
}HAL_TIM_ActiveChannel;


//typedef struct
//{
//  __IO uint32_t CR1;          /*!< TIM control register 1,              Address offset: 0x00 */
//  __IO uint32_t CR2;          /*!< TIM control register 2,              Address offset: 0x04 */
//  __IO uint32_t SMCR;         /*!< TIM slave Mode Control register,     Address offset: 0x08 */
//  __IO uint32_t DIER;         /*!< TIM DMA/interrupt enable register,   Address offset: 0x0C */
//  __IO uint32_t SR;           /*!< TIM status register,                 Address offset: 0x10 */
//  __IO uint32_t EGR;          /*!< TIM event generation register,       Address offset: 0x14 */
//  __IO uint32_t CCMR1;        /*!< TIM capture/compare mode register 1, Address offset: 0x18 */
//  __IO uint32_t CCMR2;        /*!< TIM capture/compare mode register 2, Address offset: 0x1C */
//  __IO uint32_t CCER;         /*!< TIM capture/compare enable register, Address offset: 0x20 */
//  __IO uint32_t CNT;          /*!< TIM counter register,                Address offset: 0x24 */
//  __IO uint32_t PSC;          /*!< TIM prescaler register,              Address offset: 0x28 */
//  __IO uint32_t ARR;          /*!< TIM auto-reload register,            Address offset: 0x2C */
//  uint32_t      RESERVED12;   /*!< Reserved, 0x30                                            */    
//  __IO uint32_t CCR1;         /*!< TIM capture/compare register 1,      Address offset: 0x34 */    
//  __IO uint32_t CCR2;         /*!< TIM capture/compare register 2,      Address offset: 0x38 */    
//  __IO uint32_t CCR3;         /*!< TIM capture/compare register 3,      Address offset: 0x3C */
//  __IO uint32_t CCR4;         /*!< TIM capture/compare register 4,      Address offset: 0x40 */
//  uint32_t      RESERVED17;   /*!< Reserved, 0x44                                            */ 
//  __IO uint32_t DCR;          /*!< TIM DMA control register,            Address offset: 0x48 */
//  __IO uint32_t DMAR;         /*!< TIM DMA address for full transfer,   Address offset: 0x4C */
//  __IO uint32_t OR;           /*!< TIM option register,                 Address offset: 0x50 */
//} TIM_TypeDef;

typedef struct
{
  TIM_TypeDef              *Instance;     /*!< Register base address             */
  TIM_TimeBaseInitTypeDef    Init;          /*!< TIM Time Base required parameters */
  HAL_TIM_ActiveChannel    Channel;       /*!< Active channel                    */
  //DMA_HandleTypeDef        *hdma[7];      /*!< DMA Handlers array
                                            // This array is accessed by a @ref TIM_DMA_Handle_index */
  HAL_LockTypeDef             Lock;          /*!< Locking object                    */
 // __IO HAL_TIM_StateTypeDef   State;        /*!< TIM operation state               */
}TIM_HandleTypeDef;




typedef enum {
	POWER_MODULE_OFF,/**<  power module is turned off - no power from solar panel to batteries*/
	POWER_MODULE_ON,/**<  power module is turned on- Maximum Power Point Tracking for this solar panel*/
	POWER_MODULE_LAST_VALUE
}EPS_mppt_power_module_state;


/**
 * @brief Power module parameters.
 *
 * structure to config and control a power module
 */
typedef struct {
	EPS_mppt_power_module_state module_state; /**<  control if power module of a solar panel is on or off*/
	uint16_t voltage; /**<  average voltage at each mppt step*/
	uint16_t current; /**<  average curret at each mppt step*/
	uint32_t previous_power; /**<  average power at previous mppt step*/
	uint32_t previous_voltage; /**<  average voltage input at previous mppt step*/
	uint8_t incremennt_flag;/**<  flag for mppt algorithm must be initialized to 1*/
	uint32_t pwm_duty_cycle; /**<  duty cycle of power module pwm output*/
	//TIM_HandleTypeDef *htim_pwm;/**<  assign wich timer is assigned for this pwm output*/
	uint32_t timChannel;/**<  assign the proper timer channel assigned to module pwm output*/
	//ADC_HandleTypeDef *hadc_power_module;/**<  adc handle for voltage and current measurements for each power module*/
	uint32_t ADC_channel_current;/**<  adc channel; for current measurements for this power module*/
	uint32_t ADC_channel_voltage;/**<  adc channel for voltage measurements for this power module*/

}EPS_PowerModule;


//void EPS_PowerModule_init(EPS_PowerModule *module_X, uint32_t starting_pwm_dutycycle, TIM_HandleTypeDef *htim, uint32_t timer_channel, ADC_HandleTypeDef *hadc_power_module, uint32_t ADC_channel_current, uint32_t ADC_channel_voltage);
void EPS_update_power_module_state(EPS_PowerModule *power_module);
void EPS_PowerModule_mppt_update_pwm(EPS_PowerModule *moduleX);
void EPS_PowerModule_mppt_apply_pwm(EPS_PowerModule *moduleX);
//EPS_soft_error_status EPS_PowerModule_init_ALL(EPS_PowerModule *module_top, EPS_PowerModule *module_bottom, EPS_PowerModule *module_left, EPS_PowerModule *module_right);



#endif /* _EPS_POWER_MODULE_H_ */
