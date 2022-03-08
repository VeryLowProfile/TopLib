/**
  ******************************************************************************
  * @file           : scan_scan_ENC.c
  * @author         : Enrico
  * @date           : 22 dic 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "scan_ENC.h"

static uint16_t newCount;
static uint16_t prevCount;

void Encoder_Init(TIM_HandleTypeDef *htim)
{
  HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL);
}

Encoder_Status Encoder_Get_Status(TIM_HandleTypeDef *htim)
{
  newCount = (uint16_t)htim->Instance->CNT;
  if (newCount != prevCount) {
    if (newCount > prevCount) {
      prevCount = newCount;
      return Incremented;
    } else {
      prevCount = newCount;
      return Decremented;
    }
  }
  return Neutral;
}

