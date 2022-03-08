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
//ADC Struct
struct ADC ADC[MAX_ADC_1_CHANNELS + MAX_ADC_2_CHANNELS];

//DMA Buffer
uint32_t ADC_1_DMA_Buffer[MAX_DMA_BUFFER_LENGHT_CHANNEL][MAX_ADC_1_CHANNELS];
uint32_t ADC_2_DMA_Buffer[MAX_DMA_BUFFER_LENGHT_CHANNEL][MAX_ADC_2_CHANNELS];

/* Functions -----------------------------------------------------------*/
void Scan_ADC_Init(uint8_t sensorNumber, uint8_t adcNumber, uint8_t adcChannel, uint8_t filterCostant, uint8_t sensorType, float linearScaling, float linearOffset)
{
	ADC[sensorNumber].Config.isUsed = YES;
	ADC[sensorNumber].Config.adcNumber = adcNumber;
	ADC[sensorNumber].Config.adcChannel = adcChannel;
	ADC[sensorNumber].Config.filterCostant = filterCostant;
	ADC[sensorNumber].Config.linearOffset = linearOffset;
	ADC[sensorNumber].Config.linearScaling = linearScaling;
	ADC[sensorNumber].Config.sensorType = sensorType;
}

void Scan_ADC_Start(ADC_HandleTypeDef *hadc1, ADC_HandleTypeDef *hadc2)
{
	//Start Sampling To DMA
	if (HAL_ADC_Start_DMA(hadc1, (uint32_t*)ADC_1_DMA_Buffer, MAX_DMA_BUFFER_LENGHT_CHANNEL * MAX_ADC_1_CHANNELS) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_ADC_Start_DMA(hadc2, (uint32_t*)ADC_2_DMA_Buffer, MAX_DMA_BUFFER_LENGHT_CHANNEL * MAX_ADC_2_CHANNELS) != HAL_OK)
	{
		Error_Handler();
	}
}

void Scan_ADC_Process_Channel(uint8_t sensorNumber)
{
	if (ADC[sensorNumber].Config.isUsed)
	{
		//Grab Sample From Ram and made a medium raw value based on the filter constant;
		int i = 0;
		ADC[sensorNumber].Status.prevCondValue = ADC[sensorNumber].Status.actualCondValue;
		ADC[sensorNumber].Status.filteredRawValue = 0;
		ADC[sensorNumber].Status.actualCondValue = 0;
		ADC[sensorNumber].Status.sampleNumber = 0;
		for (i = 0; i < MAX_DMA_BUFFER_LENGHT_CHANNEL; i = i + ADC[sensorNumber].Config.filterCostant)
		{
			if (ADC[sensorNumber].Config.adcNumber == ADC_1){
				ADC[sensorNumber].Status.filteredRawValue += ADC_1_DMA_Buffer[i][ADC[sensorNumber].Config.adcChannel];
			}
			else if (ADC[sensorNumber].Config.adcNumber == ADC_2) {
				ADC[sensorNumber].Status.filteredRawValue += ADC_2_DMA_Buffer[i][ADC[sensorNumber].Config.adcChannel];
			}
			ADC[sensorNumber].Status.sampleNumber++;
		}
		ADC[sensorNumber].Status.filteredRawValue = ADC[sensorNumber].Status.filteredRawValue / ADC[sensorNumber].Status.sampleNumber;

		//Linearization of the medium raw value
		//LINEAR
		if (ADC[sensorNumber].Config.sensorType == LINEAR)
		{
			ADC[sensorNumber].Status.actualCondValue = (ADC[sensorNumber].Status.filteredRawValue * ADC[sensorNumber].Config.linearScaling) + ADC[sensorNumber].Config.linearOffset;
		}
		//OTHERS
		else
		{
			int k, x;
			float q, m;
	 		k = ADC[sensorNumber].Status.filteredRawValue / MAX_LOOKUP_TABLE_SEGMENT;
			x = ADC[sensorNumber].Status.filteredRawValue % MAX_LOOKUP_TABLE_SEGMENT;
	 		q = ADC_LookUpTable[ADC[sensorNumber].Config.sensorType][k];
	 		m = (ADC_LookUpTable[ADC[sensorNumber].Config.sensorType][k+1] - ADC_LookUpTable[ADC[sensorNumber].Config.sensorType][k]) / MAX_LOOKUP_TABLE_SEGMENT;
	 		ADC[sensorNumber].Status.actualCondValue = m * x + q;
		}
	}
}

void Scan_ADC_Main(void)
{
	int i;
	for (i = 0; i < MAX_ADC_1_CHANNELS + MAX_ADC_2_CHANNELS; ++i)
	{
		Scan_ADC_Process_Channel(i);
	}
}

