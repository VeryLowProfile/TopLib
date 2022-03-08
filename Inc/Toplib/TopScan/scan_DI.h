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
#define MAX_DI_INPUT 5

//Input Name
#define DI_DEVICE_1 0
#define DI_DEVICE_2 1
#define DI_DEVICE_3 2
#define DI_DEVICE_4 3
#define DI_DEVICE_5 4

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
void Scan_DI_Init(uint8_t diNumber, uint8_t port, uint8_t pin, uint8_t isDebounce, uint8_t debounceCycles);
void Scan_DI_Process_Number(uint8_t diNumber);
void Scan_DI_Main(void);

