#include "./BSP/KEY/key.h"


void KEY_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN_x) 
{
    if (GPIOx == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }else if (GPIOx == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }else if (GPIOx == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }else if (GPIOx == GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }else if (GPIOx == GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_PIN_x;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;  /*输入模式该参数无用*/
    HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);
}


uint16_t KEY_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN_x)
{
    if (HAL_GPIO_ReadPin(GPIOx, GPIO_PIN_x) == GPIO_PIN_RESET)
    {
        HAL_Delay(10);
        if (HAL_GPIO_ReadPin(GPIOx, GPIO_PIN_x) == GPIO_PIN_RESET)
        {
            while(HAL_GPIO_ReadPin(GPIOx, GPIO_PIN_x) == GPIO_PIN_RESET);
            return 1;
        }
    }
    return 0;
}





