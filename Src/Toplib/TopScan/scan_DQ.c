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
struct DQ_channel DQ[MAX_DQ_CHANNELS];

/* Body -----------------------------------------------------------*/
void Scan_DQ_Init(void)
{
	//DQ_0
	DQ[DQ_0].channelConfig.port = PORT_B;
	DQ[DQ_0].channelConfig.pin = PIN_14;

	//DQ_1
	DQ[DQ_1].channelConfig.port = PORT_B;
	DQ[DQ_1].channelConfig.pin = PIN_15;

	//DQ_2
	DQ[DQ_2].channelConfig.port = PORT_A;
	DQ[DQ_2].channelConfig.pin = PIN_5;

}

void Scan_DQ_Process_Channel(uint8_t channel)
{
	HAL_GPIO_WritePin(DQ[channel].channelConfig.port, DQ[channel].channelConfig.pin, DQ[channel].channelCommand.command);
}

void Scan_DQ_Main(void)
{
	int i;
	for (i = 0; i < MAX_DQ_CHANNELS; ++i)
	{
		Scan_DQ_Process_Channel(i);
	}
}
