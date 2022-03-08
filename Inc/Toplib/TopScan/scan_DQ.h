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
#define MAX_DQ_OUT 3

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
void Scan_DQ_Init(uint8_t dqNumber, uint8_t port, uint8_t pin);
void Scan_DQ_Process_Number(uint8_t dqNumber);
void Scan_DQ_Main(void);

