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

//Adc Number
#define ADC_1 1
#define ADC_2 2

//Input Name
#define ADC_1_SENSOR_5 0
#define ADC_1_SENSOR_8 1
#define ADC_2_SENSOR_3 2
#define ADC_2_SENSOR_4 3

/* Declarations -----------------------------------------------------------*/
//Variables
struct ADC_Config
{
	uint8_t isUsed;
	uint8_t adcNumber;
	uint8_t adcChannel;
	uint8_t sensorType;
	uint8_t filterCostant;
	float linearScaling;
	float linearOffset;
};

struct ADC_Status
{
	int sampleNumber;
	int filteredRawValue;
	float prevCondValue;
	float actualCondValue;
};

struct ADC
{
	struct ADC_Config Config;
	struct ADC_Status Status;
};

/* Prototypes -----------------------------------------------------------*/
void Scan_ADC_Init(uint8_t sensorNumber, uint8_t adcNumber, uint8_t adcChannel, uint8_t filterCostant, uint8_t sensorType, float linearScaling, float linearOffset);
void Scan_ADC_Start(ADC_HandleTypeDef *hadc1, ADC_HandleTypeDef *hadc2);
void Scan_ADC_Process_Channel(uint8_t sensorNumber);
void Scan_ADC_Main(void);


