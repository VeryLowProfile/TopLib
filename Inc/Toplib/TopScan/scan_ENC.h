/**
  ******************************************************************************
  * @file           : scan_ENC.h
  * @author         : Enrico
  * @date           : Sep 07, 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
#include "stdint.h"
#include "stdbool.h"

/* Defines -----------------------------------------------------------*/
#ifndef ENCODER_TIM
#define ENCODER_TIM                            htim2
#endif

#ifndef ENCODER_GPIO_PORT
#define ENCODER_GPIO_PORT                      GPIOA
#endif
#ifndef ENCODER_GPIO_CH1
#define ENCODER_GPIO_CH1                       GPIO_PIN_0
#endif

#ifndef ENCODER_GPIO_CH2
#define ENCODER_GPIO_CH2                       GPIO_PIN_1
#endif

/* Declarations -----------------------------------------------------------*/
//Variables
extern TIM_HandleTypeDef htim2;

typedef enum
{
  Incremented = 1,
  Decremented = -1,
  Neutral = 0,
} Encoder_Status;

//Functions
void Encoder_Init(TIM_HandleTypeDef *htim);
Encoder_Status Encoder_Get_Status(TIM_HandleTypeDef *htim);

