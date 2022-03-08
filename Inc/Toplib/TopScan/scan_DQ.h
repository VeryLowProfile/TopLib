/**
  ******************************************************************************
  * @file           : scan_DQ.h
  * @author         : Enrico
  * @date           : Sep 07, 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "genericDefines.h"
#include "stm32g4xx_hal.h"

/* Defines -----------------------------------------------------------*/
//Configuration
#define MAX_DQ_CHANNELS 3

//Output Name
#define DQ_0 0
#define DQ_1 1
#define DQ_2 2

/* Declarations -----------------------------------------------------------*/
//Variables
struct DQ_channelConfig
{
	GPIO_TypeDef* port;
	uint16_t pin;
};

struct DQ_channelCommand
{
	uint8_t command;
};

struct DQ_channel
{
	struct DQ_channelConfig channelConfig;
	struct DQ_channelCommand channelCommand;
};

//Functions
void Scan_DQ_Init(void);
void Scan_DQ_Process_Channel(uint8_t channel);
void Scan_DQ_Main(void);

