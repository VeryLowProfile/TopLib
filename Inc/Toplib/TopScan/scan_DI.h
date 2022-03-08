/**
  ******************************************************************************
  * @file           : scan_DI.h
  * @author         : Enrico
  * @date           : Sep 07, 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "genericDefines.h"
#include "stm32g4xx_hal.h"

/* Defines -----------------------------------------------------------*/
//Generic

//Configuration
#define MAX_DI_CHANNELS 3

//Input Name
#define DI_SWITCH 0

/* Declarations -----------------------------------------------------------*/
//Variables
struct DI_channelConfig
{
	GPIO_TypeDef* port;
	uint16_t pin;
	uint8_t isDebounce;
	uint32_t debounceCycles;
};

struct DI_channelAux
{
	uint32_t debounceCounter;
};

struct DI_channelStatus
{
	uint8_t prevStatus;
	uint8_t status;
	uint8_t rStatus;
	uint8_t fStatus;
};

struct DI_channel
{
	struct DI_channelConfig channelConfig;
	struct DI_channelAux channelAux;
	struct DI_channelStatus channelStatus;
};

//Functions
void Scan_DI_Init(void);
void Scan_DI_Process_Channel(uint8_t channel);
void Scan_DI_Main(void);

