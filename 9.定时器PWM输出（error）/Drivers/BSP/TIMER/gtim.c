#include "./BSP/TIMER/gtim.h"


TIM_HandleTypeDef h_pwm_tim;
void GTIM_PWM_Init(void)
{
    h_pwm_tim.Instance = TIM3;
    h_pwm_tim.Init.CounterMode = TIM_COUNTERMODE_UP;
    h_pwm_tim.Init.Period = 499;
    h_pwm_tim.Init.Prescaler = 72 - 1;
    HAL_TIM_PWM_Init(&h_pwm_tim);

    TIM_OC_InitTypeDef TIM_OC_InitStruct;
    TIM_OC_InitStruct.OCMode = TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.Pulse = 249;
    TIM_OC_InitStruct.OCPolarity = TIM_OCPOLARITY_LOW;

    HAL_TIM_PWM_ConfigChannel(&h_pwm_tim, &TIM_OC_InitStruct, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&h_pwm_tim, TIM_CHANNEL_2);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_handle)
{
    if(tim_handle->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        
        __HAL_RCC_AFIO_CLK_ENABLE();
        __HAL_AFIO_REMAP_TIM3_PARTIAL();
    }
}

void GTIM_PWM_SetDuty(uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(&h_pwm_tim, TIM_CHANNEL_2, duty);
}


/*通用定时器输出比较*/

TIM_HandleTypeDef h_ic_tim;

void GTIM_IC_Init()
{
    h_ic_tim.Instance = TIM2;
    h_ic_tim.Init.CounterMode = TIM_COUNTERMODE_UP;
    h_ic_tim.Init.Period = 499;
    h_ic_tim.Init.Prescaler = 72 - 1;
    // h_ic_tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    // h_ic_tim.Init.AutoReloadPreload = TIM_AUTOMATICOUTPUT_ENABLE;
    // h_ic_tim.Init.RepetitionCounter = 0;
    HAL_TIM_OC_Init(&h_ic_tim);

    TIM_IC_InitTypeDef TIM_IC_InitStructure;
    TIM_IC_InitStructure.ICPolarity = TIM_ICPOLARITY_RISING;
    TIM_IC_InitStructure.ICSelection = TIM_ICSELECTION_DIRECTTI;
    TIM_IC_InitStructure.ICPrescaler = TIM_ICPSC_DIV1;
    TIM_IC_InitStructure.ICFilter = 0;
    HAL_TIM_IC_ConfigChannel(&h_ic_tim, &TIM_IC_InitStructure, TIM_CHANNEL_1);

    __HAL_TIM_ENABLE_IT(&h_ic_tim, TIM_IT_UPDATE);
    HAL_TIM_IC_Start_IT(&h_ic_tim, TIM_CHANNEL_1);

}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* h_ic_tim)
{
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM_IRQHandler(void)
{ 
    
}
