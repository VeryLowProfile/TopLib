/**
  ******************************************************************************
  * @file           : rtc.c
  * @author         : Enrico
  * @date           : Sep 07, 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* Defines -----------------------------------------------------------*/

/* Declarations -----------------------------------------------------------*/

/* Functions -----------------------------------------------------------*/
void Init_RTC_Time (RTC_HandleTypeDef *hrtc, uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t sub)
{
	RTC_TimeTypeDef sTime = {0};

	sTime.Hours = hours;
	sTime.Minutes = minutes;
	sTime.Seconds = seconds;
	sTime.SubSeconds = sub;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;

	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}

void Init_RTC_Date (RTC_HandleTypeDef *hrtc, uint8_t weekDay, uint8_t month, uint8_t day, uint8_t year)
{
	RTC_DateTypeDef sDate = {0};

	sDate.WeekDay = weekDay;
	sDate.Month = month;
	sDate.Date = day;
	sDate.Year = year;

	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F0); // backup register for date; 0x32F0 is a random value that is used as a label
}

RTC_datetime get_RTC_Time(RTC_HandleTypeDef *hrtc)
{

	RTC_TimeTypeDef time;
	RTC_datetime gTime;

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);

	gTime.hours = time.Hours;
	gTime.minutes = time.Minutes;
	gTime.seconds = time.Seconds;
	gTime.sub = time.SubSeconds;

	return gTime;

}

RTC_datetime get_RTC_Date(RTC_HandleTypeDef *hrtc)
{

	RTC_DateTypeDef date;
	RTC_datetime gDate;

	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	gDate.day = date.Date;
	gDate.months = date.Month;
	gDate.years = date.Year + 2000;   //adding 2000 for visualization
	gDate.weekDay = date.WeekDay;

	return gDate;

}

RTC_datetime get_RTC_DateTime(RTC_HandleTypeDef *hrtc)
{

	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	RTC_datetime gDateTime;

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	gDateTime.hours = time.Hours;
	gDateTime.minutes = time.Minutes;
	gDateTime.seconds = time.Seconds;
	gDateTime.sub = time.SubSeconds;
	gDateTime.day = date.Date;
	gDateTime.months = date.Month;
	gDateTime.years = date.Year + 2000;   //adding 2000 for visualization
	gDateTime.weekDay = date.WeekDay;

	return gDateTime;
}
