#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/RTC/rtc.h"

int main(void)
{   
    HAL_Init(); 
    sys_stm32_clock_init(RCC_PLL_MUL9); 
    delay_init(72); 
    UART_Init();
    
    uint8_t rtc_status = RTC_Init();  // RTC初始化
    
    // 只在第一次启动时设置初始时间和日期
    if (rtc_status == 2)
    {
        RTC_SetDate(25, 8, 1);    // 年-月-日 (2025-08-01)
        RTC_SetTime(12, 0, 0);    // 时:分:秒 (12:00:00)
        printf("RTC initialized with default time\r\n");
    }
    else if (rtc_status == 0)
    {
        printf("RTC already configured, using existing time\r\n");
    }
    else
    {
        printf("RTC initialization failed\r\n");
    }
    
    while(1)
    {        
        RTC_TimeTypeDef sTime;
        RTC_DateTypeDef sDate;

        // 获取当前时间和日期
        RTC_Get_Data_Time(&sDate, &sTime);

        // 这里可以添加代码来处理获取的时间和日期，例如打印到串口
        printf("Current Date: %02d-%02d-%02d\r\nTime: %02d:%02d:%02d\r\n", 
               sDate.Year, sDate.Month, sDate.Date, 
               sTime.Hours, sTime.Minutes, sTime.Seconds);

        delay_ms(1000);  // 延时1秒
    }
}