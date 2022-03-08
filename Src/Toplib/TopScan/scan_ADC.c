/*
  ******************************************************************************
  * @file           : scan_ADC.c
  * @author         : Enrico
  * @date           : Aug 27, 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "scan_ADC.h"

/* Declarations -----------------------------------------------------------*/
float ADC_LookUpTable[MAX_SENSOR_TYPE][MAX_LOOKUP_TABLE_POINTS] = {{0 ,100 ,200 ,300 ,400 ,500 ,600 ,700 ,800 ,900 ,1000 ,1100 ,1200 ,1300 ,1400 ,1500 ,1600 },		/* lineare */
																	{800 ,1001,1091,1134,1163,1191,1220,1246,1265,1277,1285 ,1294 ,1309 ,1339 ,1376 ,1403 ,1430 },	    /* hubble */
																	{0 ,128 ,249 ,364 ,480 ,594 ,710 ,825 ,945 ,1072,1199 ,1321 ,1443 ,1565 ,1687 ,1809 ,1931 },		/* type_K (G = 52.7) */
																	{0 ,167 ,300 ,420 ,542 ,656 ,767 ,873 ,976 ,1076,1174 ,1271 ,1367 ,1463 ,1560 ,1658 ,1760 },		/* type_S (G = 220)*/
																	{0 ,165 ,311 ,443 ,569 ,693 ,818 ,943 ,1065,1198,1340 ,2000 ,2000 ,2000 ,2000 ,2000 ,2000 },		/* type_N (G = 52.7)*/
																	{0 ,124 ,232 ,334 ,431 ,525 ,619 ,713 ,809 ,903 ,1001 ,1103, 1207 ,1316 ,1428 ,1546 ,1670 }};	    /* type_C (G = 140 )*/
struct ADC_channel ADC_1[MAX_ADC_1_CHANNELS];
struct ADC_channel ADC_2[MAX_ADC_2_CHANNELS];

uint32_t ADC_1_DMA_Buffer[MAX_DMA_BUFFER_LENGHT_CHANNEL][MAX_ADC_1_CHANNELS];
uint32_t ADC_2_DMA_Buffer[MAX_DMA_BUFFER_LENGHT_CHANNEL][MAX_ADC_2_CHANNELS];

/* Functions -----------------------------------------------------------*/
void Scan_ADC_1_Init(ADC_HandleTypeDef *hadc)
{
	//ADC1------------------------------------------

	//CHANNEL_5
	ADC_1[ADC_1_SENSOR_5].channelConfig.isUsed = YES;
	ADC_1[ADC_1_SENSOR_5].channelConfig.filterCostant = 1;
	ADC_1[ADC_1_SENSOR_5].channelConfig.linearOffset = 0;
	ADC_1[ADC_1_SENSOR_5].channelConfig.linearScaling = 0.0244140625;
	ADC_1[ADC_1_SENSOR_5].channelConfig.sensorType = LINEAR;

	//CHANNEL_8
	ADC_1[ADC_1_SENSOR_8].channelConfig.isUsed = YES;
	ADC_1[ADC_1_SENSOR_8].channelConfig.filterCostant = 1;
	ADC_1[ADC_1_SENSOR_8].channelConfig.linearOffset = 0;
	ADC_1[ADC_1_SENSOR_8].channelConfig.linearScaling = 0.0244140625;
	ADC_1[ADC_1_SENSOR_8].channelConfig.sensorType = LINEAR;

	//Start Sampling To DMA
	if (HAL_ADC_Start_DMA(hadc, (uint32_t*)ADC_1_DMA_Buffer, MAX_DMA_BUFFER_LENGHT_CHANNEL * MAX_ADC_1_CHANNELS) != HAL_OK)
	{
		Error_Handler();
	}
}

void Scan_ADC_2_Init(ADC_HandleTypeDef *hadc)
{
	//ADC2------------------------------------------

	//CHANNEL_3
	ADC_2[ADC_2_SENSOR_3].channelConfig.isUsed = YES;
	ADC_2[ADC_2_SENSOR_3].channelConfig.filterCostant = 1;
	ADC_2[ADC_2_SENSOR_3].channelConfig.linearOffset = 0;
	ADC_2[ADC_2_SENSOR_3].channelConfig.linearScaling = 0.0244140625;
	ADC_2[ADC_2_SENSOR_3].channelConfig.sensorType = LINEAR;

	//CHANNEL_4
	ADC_2[ADC_2_SENSOR_4].channelConfig.isUsed = YES;
	ADC_2[ADC_2_SENSOR_4].channelConfig.filterCostant = 1;
	ADC_2[ADC_2_SENSOR_4].channelConfig.linearOffset = 0;
	ADC_2[ADC_2_SENSOR_4].channelConfig.linearScaling = 0.0244140625;
	ADC_2[ADC_2_SENSOR_4].channelConfig.sensorType = LINEAR;

	//Start Sampling To DMA
	if (HAL_ADC_Start_DMA(hadc, (uint32_t*)ADC_2_DMA_Buffer, MAX_DMA_BUFFER_LENGHT_CHANNEL * MAX_ADC_2_CHANNELS) != HAL_OK)
	{
		Error_Handler();
	}
}

void Scan_ADC_1_Process_Channel(uint8_t channel)
{
	if (ADC_1[channel].channelConfig.isUsed)
	{
		//Grab Sample From Ram and made a medium raw value based on the filter constant;
		int i = 0;
		ADC_1[channel].channelValue.prevCondValue = ADC_1[channel].channelValue.actualCondValue;
		ADC_1[channel].channelValue.filteredRawValue = 0;
		ADC_1[channel].channelValue.actualCondValue = 0;
		ADC_1[channel].channelValue.sampleNumber = 0;
		for (i = 0; i < MAX_DMA_BUFFER_LENGHT_CHANNEL; i = i + ADC_1[channel].channelConfig.filterCostant)
		{
			ADC_1[channel].channelValue.filteredRawValue += ADC_1_DMA_Buffer[i][channel];
			ADC_1[channel].channelValue.sampleNumber++;
		}
		ADC_1[channel].channelValue.filteredRawValue = ADC_1[channel].channelValue.filteredRawValue / ADC_1[channel].channelValue.sampleNumber;

		//Linearization of the medium raw value
		//LINEAR
		if (ADC_1[channel].channelConfig.sensorType == LINEAR)
		{
			ADC_1[channel].channelValue.actualCondValue = (ADC_1[channel].channelValue.filteredRawValue * ADC_1[channel].channelConfig.linearScaling) + ADC_1[channel].channelConfig.linearOffset;
		}
		//OTHERS
		else
		{
			int k, x;
			float q, m;
	 		k = ADC_1[channel].channelValue.filteredRawValue / MAX_LOOKUP_TABLE_SEGMENT;
			x = ADC_1[channel].channelValue.filteredRawValue % MAX_LOOKUP_TABLE_SEGMENT;
	 		q = ADC_LookUpTable[ADC_1[channel].channelConfig.sensorType][k];
	 		m = (ADC_LookUpTable[ADC_1[channel].channelConfig.sensorType][k+1] - ADC_LookUpTable[ADC_1[channel].channelConfig.sensorType][k]) / MAX_LOOKUP_TABLE_SEGMENT;
	 		ADC_1[channel].channelValue.actualCondValue = m * x + q;
		}
	}
}

void Scan_ADC_2_Process_Channel(uint8_t channel)
{
	if (ADC_2[channel].channelConfig.isUsed)
	{
		//Grab Sample From Ram and made a medium raw value based on the filter constant;
		int i = 0;
		ADC_2[channel].channelValue.prevCondValue = ADC_2[channel].channelValue.actualCondValue;
		ADC_2[channel].channelValue.filteredRawValue = 0;
		ADC_2[channel].channelValue.actualCondValue = 0;
		ADC_2[channel].channelValue.sampleNumber = 0;
		for (i = 0; i < MAX_DMA_BUFFER_LENGHT_CHANNEL; i = i + ADC_2[channel].channelConfig.filterCostant)
		{
			ADC_2[channel].channelValue.filteredRawValue += ADC_2_DMA_Buffer[i][channel];
			ADC_2[channel].channelValue.sampleNumber++;
		}
		ADC_2[channel].channelValue.filteredRawValue = ADC_2[channel].channelValue.filteredRawValue / ADC_2[channel].channelValue.sampleNumber;

		//Linearization of the medium raw value
		//LINEAR
		if (ADC_2[channel].channelConfig.sensorType == LINEAR)
		{
			ADC_2[channel].channelValue.actualCondValue = (ADC_2[channel].channelValue.filteredRawValue * ADC_2[channel].channelConfig.linearScaling) + ADC_2[channel].channelConfig.linearOffset;
		}
		//OTHERS
		else
		{
			int k, x;
			float q, m;
	 		k = ADC_2[channel].channelValue.filteredRawValue / MAX_LOOKUP_TABLE_SEGMENT;
			x = ADC_2[channel].channelValue.filteredRawValue % MAX_LOOKUP_TABLE_SEGMENT;
	 		q = ADC_LookUpTable[ADC_2[channel].channelConfig.sensorType][k];
	 		m = (ADC_LookUpTable[ADC_2[channel].channelConfig.sensorType][k+1] - ADC_LookUpTable[ADC_2[channel].channelConfig.sensorType][k]) / MAX_LOOKUP_TABLE_SEGMENT;
	 		ADC_2[channel].channelValue.actualCondValue = m * x + q;
		}
	}
}

void Scan_ADC_1_Main(void)
{
	int i;
	for (i = 0; i < MAX_ADC_1_CHANNELS; ++i)
	{
		Scan_ADC_1_Process_Channel(i);
	}
}

void Scan_ADC_2_Main(void)
{
	int i;
	for (i = 0; i < MAX_ADC_2_CHANNELS; ++i)
	{
		Scan_ADC_2_Process_Channel(i);
	}
}
