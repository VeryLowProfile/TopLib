/*
  ******************************************************************************
  * @file           : cycleTime.h
  * @author         : Enrico
  * @date           : 27 ago 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "genericDefines.h"
#include "stm32g4xx_hal.h"

/* Defines -----------------------------------------------------------*/
#define CYCLETIME_NUMBER 1

/* Declarations -----------------------------------------------------------*/
typedef struct {

	uint32_t start = 0;
	uint32_t stop = 0;
	uint32_t elapsed = 0;

} cycleTime;

/* Prototypes -----------------------------------------------------------*/
void CycleTime_Init(uint8_t tim);
void CycleTime_Start(uint8_t tim);
void CycleTime_Stop(uint8_t tim);
void CycleTime_Clear(uint8_t tim);

