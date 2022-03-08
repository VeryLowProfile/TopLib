/*
  ******************************************************************************
  * @file           : rtc.h
  * @author         : Enrico
  * @date           : 27 ago 2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "genericDefines.h"
#include "stm32g4xx_hal_rtc.h"

/* Defines -----------------------------------------------------------*/

/* Declarations -----------------------------------------------------------*/
typedef struct {
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint16_t sub;
	uint8_t day;
	uint8_t months;
	uint16_t years;
	uint8_t weekDay;
} RTC_datetime;

/* Prototypes -----------------------------------------------------------*/
void Init_RTC_Time (RTC_HandleTypeDef *hrtc, uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t sub);
void Init_RTC_Date (RTC_HandleTypeDef *hrtc, uint8_t weekDay, uint8_t month, uint8_t day, uint8_t year);
RTC_datetime get_RTC_Time(RTC_HandleTypeDef *hrtc);
RTC_datetime get_RTC_Date(RTC_HandleTypeDef *hrtc);
RTC_datetime get_RTC_DateTime(RTC_HandleTypeDef *hrtc);
