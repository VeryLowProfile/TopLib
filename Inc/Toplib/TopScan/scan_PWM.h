/**
  ******************************************************************************
  * @file           : scan_PWM.h
  * @author         : Davide
  * @date           : 21 dic 2021
  ******************************************************************************
*/

#ifndef TOPLIB_INC_GEN_PWM_H_
#define TOPLIB_INC_GEN_PWM_H_

/* Includes ------------------------------------------------------------------*/
#include "genericDefines.h"
#include "stm32g4xx_hal.h"

/* Defines -----------------------------------------------------------*/
#define MAX_PWM_NMBR 2

#define PWM_0 0
#define PWM_1 1

#define DEFAULT_TIM_CLOCK 42500000 //Hz
#define DEFAULT_PSC 0
#define DEFAULT_ARR 65535
#define DEFAULT_CCR 32767

#define MIN_DUTY 1
#define MAX_DUTY 100

#define PSC_TIM_1 0
#define PSC_TIM_2 0
#define PSC_TIM_3 0
#define PSC_TIM_4 0
#define PSC_TIM_6 0
#define PSC_TIM_7 0
#define PSC_TIM_8 0
#define PSC_TIM_15 0
#define PSC_TIM_16 0
#define PSC_TIM_17 0

#define ARR_TIM_1 0
#define ARR_TIM_2 0
#define ARR_TIM_3 0
#define ARR_TIM_4 0
#define ARR_TIM_6 0
#define ARR_TIM_7 0
#define ARR_TIM_8 0
#define ARR_TIM_15 0
#define ARR_TIM_16 0
#define ARR_TIM_17 0

#define MAX_HZ_TIM_1 1100
#define MIN_HZ_TIM_1 900
#define MAX_HZ_TIM_2 1100
#define MIN_HZ_TIM_2 900
#define MAX_HZ_TIM_3 1100
#define MIN_HZ_TIM_3 900
#define MAX_HZ_TIM_4 10000
#define MIN_HZ_TIM_4 1000
#define MAX_HZ_TIM_6 1100
#define MIN_HZ_TIM_6 900
#define MAX_HZ_TIM_7 1100
#define MIN_HZ_TIM_7 900
#define MAX_HZ_TIM_8 1100
#define MIN_HZ_TIM_8 900
#define MAX_HZ_TIM_15 1100
#define MIN_HZ_TIM_15 900
#define MAX_HZ_TIM_16 1100
#define MIN_HZ_TIM_16 900
#define MAX_HZ_TIM_17 1100
#define MIN_HZ_TIM_17 900

/* Declarations -----------------------------------------------------------*/
//Functions
void Scan_PWM_Init_Tim_PSC_ARR(TIM_HandleTypeDef *htim, int timNb);
void Scan_PWM_Update_ARR_Hz(TIM_HandleTypeDef *htim, int timNb, float freq);
void Scan_PWM_Update_ARR_Percent(TIM_HandleTypeDef *htim, int timNb, float percent);
void Scan_PWM_Update_CCR(TIM_HandleTypeDef *htim, int timNb, int channel, float duty);
void Scan_PWM_Calculate_ARR_Hz(TIM_HandleTypeDef *htim, float maxHz, float minHz, float freq);
void Scan_PWM_Calculate_ARR_Percent(TIM_HandleTypeDef *htim, float maxHz, float minHz, float percent);
void Scan_PWM_Calculate_CCR(TIM_HandleTypeDef *htim, int channel, float duty);
void Scan_PWM_Start(TIM_HandleTypeDef *htim);
void Scan_PWM_Stop(TIM_HandleTypeDef *htim);

#endif /* TOPLIB_INC_GEN_PWM_H_ */
