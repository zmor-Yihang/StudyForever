#ifndef __RTC_H
#define __RTC_H

#include "./SYSTEM/sys/sys.h"

extern RTC_HandleTypeDef hrtc;

uint8_t RTC_Init(void);

void RTC_SetTime(uint8_t hour, uint8_t minute, uint8_t second);

void RTC_SetDate(uint8_t year, uint8_t month, uint8_t date);

void RTC_Get_Data_Time(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);

#endif