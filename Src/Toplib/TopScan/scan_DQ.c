/**
  ******************************************************************************
  * @file           : scan_DQ.c
  * @author         : Enrico
  * @date           : Sep 07, 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "scan_DQ.h"

/* Defines -----------------------------------------------------------*/

/* Declarations -----------------------------------------------------------*/
struct DQ_channel DQ[MAX_DQ_OUT];

/* Body -----------------------------------------------------------*/
void Scan_DQ_Init(uint8_t dqNumber, GPIO_TypeDef* port, uint8_t pin)
{
	DQ[dqNumber].channelConfig.port = port;
	DQ[dqNumber].channelConfig.pin = pin;
}

void Scan_DQ_Process_Number(uint8_t dqNumber)
{
	HAL_GPIO_WritePin(DQ[dqNumber].channelConfig.port, DQ[dqNumber].channelConfig.pin, DQ[dqNumber].channelCommand.command);
}

void Scan_DQ_Main(void)
{
	int i;
	for (i = 0; i < MAX_DQ_OUT; ++i)
	{
		Scan_DQ_Process_Number(i);
	}
}
