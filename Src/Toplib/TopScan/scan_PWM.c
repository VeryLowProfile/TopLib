/**
  ******************************************************************************
  * @file           : Scan_PWM.c
  * @author         : Davide
  * @date           : 21 dic 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Scan_PWM.h"

/* Body -----------------------------------------------------------*/
void Scan_PWM_Init_Tim_Default(TIM_HandleTypeDef *htim)
{
	htim->Instance->PSC = 0;
	htim->Instance->ARR = DEFAULT_ARR;
	htim->Instance->CCR1 = DEFAULT_CCR;
	htim->Instance->CCR2 = DEFAULT_CCR;
	htim->Instance->CCR3 = DEFAULT_CCR;
	htim->Instance->CCR4 = DEFAULT_CCR;
	htim->Instance->CCR5 = DEFAULT_CCR;
	htim->Instance->CCR6 = DEFAULT_CCR;
}

void Scan_PWM_Init_Tim_PSC_ARR(TIM_HandleTypeDef *htim, int timNb)
{
	Scan_PWM_Init_Tim_Default(htim);

	switch(timNb)
	{
		case 1:
			htim->Instance->PSC = PSC_TIM_1;
			htim->Instance->ARR = ARR_TIM_1;
			break;
		case 2:
			htim->Instance->PSC = PSC_TIM_2;
			htim->Instance->ARR = ARR_TIM_2;
			break;
		case 3:
			htim->Instance->PSC = PSC_TIM_3;
			htim->Instance->ARR = ARR_TIM_3;
			break;
		case 4:
			htim->Instance->PSC = PSC_TIM_4;
			htim->Instance->ARR = ARR_TIM_4;
			break;
		case 6:
			htim->Instance->PSC = PSC_TIM_6;
			htim->Instance->ARR = ARR_TIM_6;
			break;
		case 7:
			htim->Instance->PSC = PSC_TIM_7;
			htim->Instance->ARR = ARR_TIM_7;
			break;
		case 8:
			htim->Instance->PSC = PSC_TIM_8;
			htim->Instance->ARR = ARR_TIM_8;
			break;
		case 15:
			htim->Instance->PSC = PSC_TIM_15;
			htim->Instance->ARR = ARR_TIM_15;
			break;
		case 16:
			htim->Instance->PSC = PSC_TIM_16;
			htim->Instance->ARR = ARR_TIM_16;
			break;
		case 17:
			htim->Instance->PSC = PSC_TIM_17;
			htim->Instance->ARR = ARR_TIM_17;
			break;

		default:
			htim->Instance->PSC = 0;
			htim->Instance->ARR = DEFAULT_ARR;
    }
}

void Scan_PWM_Update_ARR_Hz(TIM_HandleTypeDef *htim, int timNb, float freq)
{
	switch(timNb)
	{
		case 1:
			Scan_PWM_Calculate_ARR_Hz(htim, MAX_HZ_TIM_1, MIN_HZ_TIM_1, freq);
			break;
		case 2:
			Scan_PWM_Calculate_ARR_Hz(htim, MAX_HZ_TIM_2, MIN_HZ_TIM_2, freq);
			break;
		case 3:
			Scan_PWM_Calculate_ARR_Hz(htim, MAX_HZ_TIM_3, MIN_HZ_TIM_3, freq);
			break;
		case 4:
			Scan_PWM_Calculate_ARR_Hz(htim, MAX_HZ_TIM_4, MIN_HZ_TIM_4, freq);
			break;
		case 6:
			Scan_PWM_Calculate_ARR_Hz(htim, MAX_HZ_TIM_6, MIN_HZ_TIM_6, freq);
			break;
		case 7:
			Scan_PWM_Calculate_ARR_Hz(htim, MAX_HZ_TIM_7, MIN_HZ_TIM_7, freq);
			break;
		case 8:
			Scan_PWM_Calculate_ARR_Hz(htim, MAX_HZ_TIM_8, MIN_HZ_TIM_8, freq);
			break;
		case 15:
			Scan_PWM_Calculate_ARR_Hz(htim, MAX_HZ_TIM_15, MIN_HZ_TIM_15, freq);
			break;
		case 16:
			Scan_PWM_Calculate_ARR_Hz(htim, MAX_HZ_TIM_16, MIN_HZ_TIM_16, freq);
			break;
		case 17:
			Scan_PWM_Calculate_ARR_Hz(htim, MAX_HZ_TIM_17, MIN_HZ_TIM_17, freq);
			break;
    }
}

void Scan_PWM_Update_ARR_Percent(TIM_HandleTypeDef *htim, int timNb, float percent)
{
	switch(timNb)
	{
		case 1:
			Scan_PWM_Calculate_ARR_Percent(htim, MAX_HZ_TIM_1, MIN_HZ_TIM_1, percent);
			break;
		case 2:
			Scan_PWM_Calculate_ARR_Percent(htim, MAX_HZ_TIM_2, MIN_HZ_TIM_2, percent);
			break;
		case 3:
			Scan_PWM_Calculate_ARR_Percent(htim, MAX_HZ_TIM_3, MIN_HZ_TIM_3, percent);
			break;
		case 4:
			Scan_PWM_Calculate_ARR_Percent(htim, MAX_HZ_TIM_4, MIN_HZ_TIM_4, percent);
			break;
		case 6:
			Scan_PWM_Calculate_ARR_Percent(htim, MAX_HZ_TIM_6, MIN_HZ_TIM_6, percent);
			break;
		case 7:
			Scan_PWM_Calculate_ARR_Percent(htim, MAX_HZ_TIM_7, MIN_HZ_TIM_7, percent);
			break;
		case 8:
			Scan_PWM_Calculate_ARR_Percent(htim, MAX_HZ_TIM_8, MIN_HZ_TIM_8, percent);
			break;
		case 15:
			Scan_PWM_Calculate_ARR_Percent(htim, MAX_HZ_TIM_15, MIN_HZ_TIM_15, percent);
			break;
		case 16:
			Scan_PWM_Calculate_ARR_Percent(htim, MAX_HZ_TIM_16, MIN_HZ_TIM_16, percent);
			break;
		case 17:
			Scan_PWM_Calculate_ARR_Percent(htim, MAX_HZ_TIM_17, MIN_HZ_TIM_17, percent);
			break;
    }
}

void Scan_PWM_Update_CCR(TIM_HandleTypeDef *htim, int timNb, int channel, float duty)
{
	switch(timNb)
	{
		case 1:
			Scan_PWM_Calculate_CCR(htim, channel, duty);
			break;
		case 2:
			Scan_PWM_Calculate_CCR(htim, channel, duty);
			break;
		case 3:
			Scan_PWM_Calculate_CCR(htim, channel, duty);
			break;
		case 4:
			Scan_PWM_Calculate_CCR(htim, channel, duty);
			break;
		case 6:
			Scan_PWM_Calculate_CCR(htim, channel, duty);
			break;
		case 7:
			Scan_PWM_Calculate_CCR(htim, channel, duty);
			break;
		case 8:
			Scan_PWM_Calculate_CCR(htim, channel, duty);
			break;
		case 15:
			Scan_PWM_Calculate_CCR(htim, channel, duty);
			break;
		case 16:
			Scan_PWM_Calculate_CCR(htim, channel, duty);
			break;
		case 17:
			Scan_PWM_Calculate_CCR(htim, channel, duty);
			break;
    }
}

void Scan_PWM_Start(TIM_HandleTypeDef *htim)
{
	HAL_TIM_PWM_Start(htim, htim->Channel);
}

void Scan_PWM_Stop(TIM_HandleTypeDef *htim)
{
	HAL_TIM_PWM_Stop(htim, htim->Channel);
}

void Scan_PWM_Calculate_ARR_Hz(TIM_HandleTypeDef *htim, float maxHz, float minHz, float freq)
{
	float newFreq = freq;
	float actualDuty1 = (htim->Instance->CCR1 * 100)/htim->Instance->ARR;
	float actualDuty2 = (htim->Instance->CCR2 * 100)/htim->Instance->ARR;
	float actualDuty3 = (htim->Instance->CCR3 * 100)/htim->Instance->ARR;
	float actualDuty4 = (htim->Instance->CCR4 * 100)/htim->Instance->ARR;
	float actualDuty5 = (htim->Instance->CCR5 * 100)/htim->Instance->ARR;
	float actualDuty6 = (htim->Instance->CCR6 * 100)/htim->Instance->ARR;


	if (newFreq > maxHz)
	{
		newFreq = maxHz;
	}
	else if (newFreq < minHz)
	{
		newFreq = minHz;
	}
	else
	{
		htim->Instance->ARR = DEFAULT_TIM_CLOCK/newFreq;
		Scan_PWM_Calculate_CCR(htim, 1, actualDuty1);
		Scan_PWM_Calculate_CCR(htim, 2, actualDuty2);
		Scan_PWM_Calculate_CCR(htim, 3, actualDuty3);
		Scan_PWM_Calculate_CCR(htim, 4, actualDuty4);
		Scan_PWM_Calculate_CCR(htim, 5, actualDuty5);
		Scan_PWM_Calculate_CCR(htim, 6, actualDuty6);
	}
}

void Scan_PWM_Calculate_ARR_Percent(TIM_HandleTypeDef *htim, float maxHz, float minHz, float percent)
{
	float newPercent = percent;

	if (newPercent > MAX_DUTY)
	{
		newPercent = MAX_DUTY;
	}
	else if (newPercent < MIN_DUTY)
	{
		newPercent = MIN_DUTY;
	}

	float newFreq = (((maxHz - minHz)/100) * newPercent) + minHz;
	float actualDuty1 = (htim->Instance->CCR1 * 100)/htim->Instance->ARR;
	float actualDuty2 = (htim->Instance->CCR2 * 100)/htim->Instance->ARR;
	float actualDuty3 = (htim->Instance->CCR3 * 100)/htim->Instance->ARR;
	float actualDuty4 = (htim->Instance->CCR4 * 100)/htim->Instance->ARR;
	float actualDuty5 = (htim->Instance->CCR5 * 100)/htim->Instance->ARR;
	float actualDuty6 = (htim->Instance->CCR6 * 100)/htim->Instance->ARR;

	htim->Instance->ARR = DEFAULT_TIM_CLOCK/newFreq;
	Scan_PWM_Calculate_CCR(htim, 1, actualDuty1);
	Scan_PWM_Calculate_CCR(htim, 2, actualDuty2);
	Scan_PWM_Calculate_CCR(htim, 3, actualDuty3);
	Scan_PWM_Calculate_CCR(htim, 4, actualDuty4);
	Scan_PWM_Calculate_CCR(htim, 5, actualDuty5);
	Scan_PWM_Calculate_CCR(htim, 6, actualDuty6);
}

void Scan_PWM_Calculate_CCR(TIM_HandleTypeDef *htim, int channel, float duty)
{
	float newDuty = duty;

	if (newDuty > MAX_DUTY)
	{
		newDuty = MAX_DUTY;
	}
	else if (newDuty < MIN_DUTY)
	{
		newDuty = MIN_DUTY;
	}

	switch(channel)
	{
		case 1:
			htim->Instance->CCR1 = (newDuty / 100) * htim->Instance->ARR;
			break;
		case 2:
			htim->Instance->CCR2 = (newDuty / 100) * htim->Instance->ARR;
			break;
		case 3:
			htim->Instance->CCR3 = (newDuty / 100) * htim->Instance->ARR;
			break;
		case 4:
			htim->Instance->CCR4 = (newDuty / 100) * htim->Instance->ARR;
			break;
		case 5:
			htim->Instance->CCR5 = (newDuty / 100) * htim->Instance->ARR;
			break;
		case 6:
			htim->Instance->CCR6 = (newDuty / 100) * htim->Instance->ARR;
			break;
	}
}



