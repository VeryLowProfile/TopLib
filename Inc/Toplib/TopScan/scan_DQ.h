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
#define REL_0 0
#define REL_1 1
#define REL_2 2
#define REL_3 3
#define REL_4 4
#define REL_5 5
#define REL_6 6
#define REL_7 7
#define REL_8 8
#define REL_9 9
#define REL_10 10
#define REL_11 11
#define BUZZER 12
#define LOCK 13


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
void Scan_DQ_Init(uint8_t dqNumber, GPIO_TypeDef* port, uint8_t pin);
void Scan_DQ_Process_Number(uint8_t dqNumber);
void Scan_DQ_Main(void);

