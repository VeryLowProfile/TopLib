/**
  ******************************************************************************
  * @file           : scan_ADC.h
  * @author         : Enrico
  * @date           : 27 ago 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "genericDefines.h"
#include "stm32g4xx_hal.h"

/* Defines -----------------------------------------------------------*/
//Configuration
#define MAX_ADC 2
#define MAX_ADC_1_CHANNELS 2
#define MAX_ADC_2_CHANNELS 2
#define MAX_DMA_BUFFER_LENGHT_CHANNEL 16
#define MAX_FILTER_COSTANT 10
#define MAX_SENSOR_TYPE 6
#define MAX_LOOKUP_TABLE_POINTS 17
#define MAX_LOOKUP_TABLE_SEGMENT 256
#define LINEAR 0			    // Linear Conversion
#define HUBBLE  1			    // Hubble Pirometer
#define TC_TYPE_K  2			// Thermocouple Type K
#define TC_TYPE_S  3			// Thermocouple Type S
#define TC_TYPE_N  4			// Thermocouple Type N
#define TC_TYPE_C  5			// Thermocouple Type C

//Input Name ADC_1
#define ADC_1_SENSOR_5 0
#define ADC_1_SENSOR_8 1

//Input Name ADC_2
#define ADC_2_SENSOR_3 0
#define ADC_2_SENSOR_4 1

/* Declarations -----------------------------------------------------------*/
//Variables
struct ADC_channelConfig
{
	uint8_t isUsed;
	uint8_t sensorType;
	uint8_t filterCostant;
	float linearScaling;
	float linearOffset;
};

struct ADC_channelStatus
{
	int sampleNumber;
	int filteredRawValue;
	float prevCondValue;
	float actualCondValue;
};

struct ADC_channel
{
	struct ADC_channelConfig channelConfig;
	struct ADC_channelStatus channelValue;
};

/* Prototypes -----------------------------------------------------------*/
void Scan_ADC_1_Init(ADC_HandleTypeDef *hadc);
void Scan_ADC_2_Init(ADC_HandleTypeDef *hadc);
void Scan_ADC_1_Process_Channel(uint8_t channel);
void Scan_ADC_2_Process_Channel(uint8_t channel);
void Scan_ADC_1_Main(void);
void Scan_ADC_2_Main(void);

