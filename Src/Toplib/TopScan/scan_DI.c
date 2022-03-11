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
struct DI_channel DI[MAX_DI_INPUT];

/* Body -----------------------------------------------------------*/
void Scan_DI_Init(uint8_t diNumber, GPIO_TypeDef* port, uint16_t pin, uint8_t isDebounce, uint8_t debounceCycles)
{
	DI[diNumber].channelConfig.port = port;
	DI[diNumber].channelConfig.pin = pin;
	DI[diNumber].channelConfig.isDebounce = isDebounce;
	DI[diNumber].channelConfig.debounceCycles = debounceCycles;
}

void Scan_DI_Process_Number(uint8_t diNumber)
{
	if (DI[diNumber].channelConfig.isDebounce)
	{
		if (DI[diNumber].channelStatus.status)
		{
			if (!HAL_GPIO_ReadPin(DI[diNumber].channelConfig.port, DI[diNumber].channelConfig.pin))
			{
				if (DI[diNumber].channelAux.debounceCounter < DI[diNumber].channelConfig.debounceCycles)
				{
					DI[diNumber].channelAux.debounceCounter++;
				}
				else
				{
					DI[diNumber].channelAux.debounceCounter = 0;
					DI[diNumber].channelStatus.status = OFF;
				}
			}
		}
		else
		{
			if (HAL_GPIO_ReadPin(DI[diNumber].channelConfig.port, DI[diNumber].channelConfig.pin))
			{
				if (DI[diNumber].channelAux.debounceCounter < DI[diNumber].channelConfig.debounceCycles)
				{
					DI[diNumber].channelAux.debounceCounter++;
				}
				else
				{
					DI[diNumber].channelAux.debounceCounter = 0;
					DI[diNumber].channelStatus.status = ON;
				}
			}
		}
	}
	else
	{
		DI[diNumber].channelStatus.status = HAL_GPIO_ReadPin(DI[diNumber].channelConfig.port, DI[diNumber].channelConfig.pin);
	}

	//Edges
	DI[diNumber].channelStatus.rStatus = !DI[diNumber].channelStatus.prevStatus && HAL_GPIO_ReadPin(DI[diNumber].channelConfig.port, DI[diNumber].channelConfig.pin);
	DI[diNumber].channelStatus.fStatus = DI[diNumber].channelStatus.prevStatus && !HAL_GPIO_ReadPin(DI[diNumber].channelConfig.port, DI[diNumber].channelConfig.pin);
	DI[diNumber].channelStatus.prevStatus = HAL_GPIO_ReadPin(DI[diNumber].channelConfig.port, DI[diNumber].channelConfig.pin);
}

void Scan_DI_Main(void)
{
	int i;
	for (i = 0; i < MAX_DI_INPUT; ++i)
	{
		Scan_DI_Process_Number(i);
	}
}
