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
#define MAX_DI_INPUT 16

//Input Name
#define SETTING_1 0
#define SETTING_2 1
#define PHASE_AL 2
#define CHILLER_AL 3
#define IN_0 4
#define IN_1 5
#define IN_2 6
#define IN_3 7
#define IN_5 8
#define IN_6 9
#define IN_7 10
#define IN_8 11
#define IN_9 12
#define IN_10 13
#define IN_11 14
#define IGBT_T 15


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
void Scan_DI_Init(uint8_t diNumber, GPIO_TypeDef* port, uint16_t pin, uint8_t isDebounce, uint8_t debounceCycles);
void Scan_DI_Process_Number(uint8_t diNumber);
void Scan_DI_Main(void);

