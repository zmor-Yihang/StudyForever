#include "./BSP/RTC/rtc.h"

RTC_HandleTypeDef hrtc;

/* RTC配置标志，用于标记RTC是否已经配置过 */
#define RTC_CONFIGURED_FLAG  0x5A5A

uint8_t RTC_Init(void)
{
    /* 使能相关时钟 */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_BKP_CLK_ENABLE();
    __HAL_RCC_RTC_ENABLE();
    
    /* 使能对备份域的访问 */
    HAL_PWR_EnableBkUpAccess();
    
    /* 配置RTC句柄 */
    hrtc.Instance = RTC;
    hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
    hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;

    /* 检查是否是第一次配置RTC */
    uint8_t is_first_config = (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != RTC_CONFIGURED_FLAG);
    
    if (is_first_config)
    {
        /* 配置LSE时钟源 */
        RCC_OscInitTypeDef RCC_OscInitStruct = {
            .OscillatorType = RCC_OSCILLATORTYPE_LSE,
            .LSEState = RCC_LSE_ON,
        };
        HAL_RCC_OscConfig(&RCC_OscInitStruct);
        
        /* 选择RTC时钟源为LSE */
        RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {
            .PeriphClockSelection = RCC_PERIPHCLK_RTC,
            .RTCClockSelection = RCC_RTCCLKSOURCE_LSE
        };
        HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
    }
    
    /* 初始化RTC */
    HAL_RTC_Init(&hrtc);
    
    if (is_first_config)
    {
        /* 标记RTC已配置 */
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_CONFIGURED_FLAG);
        return 1; /* 返回1表示需要设置初始时间 */
    }
    
    return 0; /* 初始化成功，RTC已配置过 */
}

void RTC_SetTime(uint8_t hour, uint8_t minute, uint8_t second)
{
    RTC_TimeTypeDef sTime = {   
        .Hours = hour,
        .Minutes = minute,
        .Seconds = second,
    };
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);   
}

void RTC_SetDate(uint8_t year, uint8_t month, uint8_t date)
{
    RTC_DateTypeDef sDate = {
        .Year = year,
        .Month = month,
        .Date = date,
    };
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

void RTC_Get_Data_Time(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{
    HAL_RTC_GetDate(&hrtc, sDate, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&hrtc, sTime, RTC_FORMAT_BIN);
}