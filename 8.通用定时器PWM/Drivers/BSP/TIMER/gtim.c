#include "./BSP/TIMER/gtim.h"

/* 定义句柄 */
TIM_HandleTypeDef htim3;

void TIM_PWM_Init(void)
{
    /* 开启时钟 */
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
   
    /* 定义结构体 */
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_OC_InitTypeDef TIM_OC_InitStruct;
 
    /* 配置时基单元 */
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 72 - 1;
    htim3.Init.Period = 499;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_PWM_Init(&htim3);

    /* 配置PWM输出通道 */
    TIM_OC_InitStruct.OCMode = TIM_OCMODE_PWM1;			/* PWM模式1 */
    TIM_OC_InitStruct.Pulse = 249;						/* 比较值，决定占空比 */
    TIM_OC_InitStruct.OCPolarity = TIM_OCPOLARITY_LOW;	/* 输出极性为低电平有效 */
    HAL_TIM_PWM_ConfigChannel(&htim3, &TIM_OC_InitStruct, TIM_CHANNEL_2);
	
    /* 配置TIM3_CH2复用的GPIO */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 启动定时器 */
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

/* 设置CCR寄存器，调节PWM占空比 */
void TIM_PWM_SetDuty(uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, duty);
}