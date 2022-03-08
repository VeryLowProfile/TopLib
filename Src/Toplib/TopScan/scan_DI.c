/**
  ******************************************************************************
  * @file           : scan_DI.c
  * @author         : Enrico
  * @date           : Sep 07, 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "scan_DI.h"

/* Defines -----------------------------------------------------------*/

/* Declarations -----------------------------------------------------------*/
struct DI_channel DI[MAX_DI_CHANNELS];

/* Body -----------------------------------------------------------*/
void Scan_DI_Init(void)
{
	//DI_0
	DI[DI_SWITCH].channelConfig.port = PORT_B;
	DI[DI_SWITCH].channelConfig.pin = PIN_6;
	DI[DI_SWITCH].channelConfig.isDebounce = NO;
	DI[DI_SWITCH].channelConfig.debounceCycles = 0;
}

void Scan_DI_Process_Channel(uint8_t channel)
{
	if (DI[channel].channelConfig.isDebounce)
	{
		if (DI[channel].channelStatus.status)
		{
			if (!HAL_GPIO_ReadPin(DI[channel].channelConfig.port, DI[channel].channelConfig.pin))
			{
				if (DI[channel].channelAux.debounceCounter < DI[channel].channelConfig.debounceCycles)
				{
					DI[channel].channelAux.debounceCounter++;
				}
				else
				{
					DI[channel].channelAux.debounceCounter = 0;
					DI[channel].channelStatus.status = OFF;
				}
			}
		}
		else
		{
			if (HAL_GPIO_ReadPin(DI[channel].channelConfig.port, DI[channel].channelConfig.pin))
			{
				if (DI[channel].channelAux.debounceCounter < DI[channel].channelConfig.debounceCycles)
				{
					DI[channel].channelAux.debounceCounter++;
				}
				else
				{
					DI[channel].channelAux.debounceCounter = 0;
					DI[channel].channelStatus.status = ON;
				}
			}
		}
	}
	else
	{
		DI[channel].channelStatus.status = HAL_GPIO_ReadPin(DI[channel].channelConfig.port, DI[channel].channelConfig.pin);
	}

	//Edges
	DI[channel].channelStatus.rStatus = !DI[channel].channelStatus.prevStatus && HAL_GPIO_ReadPin(DI[channel].channelConfig.port, DI[channel].channelConfig.pin);
	DI[channel].channelStatus.fStatus = DI[channel].channelStatus.prevStatus && !HAL_GPIO_ReadPin(DI[channel].channelConfig.port, DI[channel].channelConfig.pin);
	DI[channel].channelStatus.prevStatus = HAL_GPIO_ReadPin(DI[channel].channelConfig.port, DI[channel].channelConfig.pin);
}

void Scan_DI_Main(void)
{
	int i;
	for (i = 0; i < MAX_DI_CHANNELS; ++i)
	{
		Scan_DI_Process_Channel(i);
	}
}
