/**
  ******************************************************************************
  * @file           : cycleTime.c
  * @author         : Enrico
  * @date           : Sep 07, 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "cycleTime.h"

/* Defines -----------------------------------------------------------*/

/* Declarations -----------------------------------------------------------*/
struct cycleTime cycleTimes[CYCLETIME_NUMBER];

/* Functions -----------------------------------------------------------*/
void CycleTime_Init(uint8_t tim)
{
    if (ARM_CM_DWT_CTRL != 0)
    {
        ARM_CM_DEMCR      |= 1 << 24;            // Set bit 24
        ARM_CM_DWT_CYCCNT  = 0;
        ARM_CM_DWT_CTRL   |= 1 << 0;             // Set bit 0
    }

    TopUtils_CycleTime_Clear(tim);
}

void CycleTime_Start(uint8_t tim)
{
	cycleTimes[tim].start = ARM_CM_DWT_CYCCNT;
}

void CycleTime_Stop(uint8_t tim)
{
	cycleTimes[tim].stop = ARM_CM_DWT_CYCCNT;
	cycleTimes[tim].elapsed = cycleTimes[tim].stop - cycleTimes[tim].start;
}

void CycleTime_Clear(uint8_t tim)
{
	cycleTimes[tim].start = 0;
	cycleTimes[tim].stop = 0;
	cycleTimes[tim].elapsed = 0;
}
