#include "./BSP/PWR/pwr.h"

void PWR_Wakeup_Pin_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能GPIOA时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    
    /* 配置PA0引脚为外部中断模式 */
    GPIO_InitStruct.Pin = GPIO_PIN_0;               /* 选择PA0引脚 */
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;    /* 下降沿触发中断，按键按下时触发 */
    GPIO_InitStruct.Pull = GPIO_PULLUP;             /* 上拉，按键按下接地时检测到下降沿 */
    
    /* 初始化GPIOA */
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 配置NVIC */
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);          /* 设置中断优先级 */
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);                  /* 使能EXTI0中断 */
}

/* 外部中断服务函数 */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);            /* 调用HAL库中断处理函数 */
}

/* 中断回调函数 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_0)
    {
        printf("Wakeup Pin Pressed!\n");  /* 按键按下时打印信息 */
    }
}