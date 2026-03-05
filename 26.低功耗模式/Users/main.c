#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/PWR/pwr.h"
#include "./BSP/LED/led.h"

int main(void)
{   
    HAL_Init(); 
    sys_stm32_clock_init(RCC_PLL_MUL9); 
    delay_init(72); 
    UART_Init();
    LED_Init();  // 初始化LED
    PWR_Wakeup_Pin_Init();  // 初始化唤醒引脚
            
    while(1)
    {   
        delay_ms(10000);  // 延时10秒
        LED_On(GPIOB, GPIO_PIN_14);  // 打开LED

        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);  // 进入睡眠模式
        LED_Off(GPIOB, GPIO_PIN_14);  
    }
}