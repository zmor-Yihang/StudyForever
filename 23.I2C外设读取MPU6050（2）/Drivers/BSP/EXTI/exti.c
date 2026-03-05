#include "./BSP/EXTI/exti.h"
#include "./SYSTEM/delay/delay.h"
void EXTI_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = GPIO_PIN_12,
        .Mode = GPIO_MODE_IT_FALLING,
        .Pull = GPIO_PULLUP,
    };
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);  // 为什么还要再清除一次中断标志位？
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_12)
    {
        delay_ms(10);             /*中断里面延迟如何避免*/
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET)
        {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
        }
    }
}
