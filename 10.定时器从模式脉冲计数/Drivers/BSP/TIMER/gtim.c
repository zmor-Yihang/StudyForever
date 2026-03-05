#include "./BSP/TIMER/gtim.h"

/* 定时器句柄定义 */
TIM_HandleTypeDef htim3;

/* 按键时钟源初始化函数 */
void Tim_SlaveMode_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    
    /* 使能时钟 */
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置PA7为输出翻转电平的引脚 */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 配置定时器时基单元 */
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 0;		         /* 外部时钟模式，无需设置内部时钟分频 */
    htim3.Init.Period = 4;                          /* 计数到4后产生更新事件 */
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;    /* 计数模式：向上计数 */
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;  /* 输入捕获数字滤波器时钟分频 */
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;  /* 不使用预装载 */
    HAL_TIM_IC_Init(&htim3);
    
    /* 配置为外部时钟模式1 */
    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
    sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;     /* 使用TI1作为输入 */
    sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_FALLING; /* 下降沿触发 */
    sSlaveConfig.TriggerFilter = 0xFF;              /* 设置滤波，防止抖动 */
    HAL_TIM_SlaveConfigSynchro(&htim3, &sSlaveConfig);
    
    /* 配置NVIC */
    HAL_NVIC_SetPriority(TIM3_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

    /* 使能更新中断 */
    __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);

    /* 启动定时器并开启中断 */  
    HAL_TIM_IC_Start(&htim3, TIM_CHANNEL_1);
}

/* 定时器中断服务函数 */
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
}

/* 定时器更新中断回调函数 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
    {
        /* 每按4次按键，这里会被调用一次 */
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); /* 翻转LED */
        extern uint16_t count;
        count++;
    }
}
