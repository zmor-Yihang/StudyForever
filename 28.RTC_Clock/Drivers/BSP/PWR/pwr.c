#include "./BSP/PWR/pwr.h"

static void PWR_Wakeup_Pin_Init(void)
{
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);  /* 使能唤醒引脚 */
}

void PWR_Init(void)
{ 
    /* 使能PWR时钟 */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* 初始化唤醒引脚 */
    PWR_Wakeup_Pin_Init();
}

