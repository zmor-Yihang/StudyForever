# STM32 HAL库GPIO使用示例

本文档提供了使用STM32 HAL库进行GPIO初始化和操作的代码示例，包括输入、输出和中断功能。

## 1. GPIO输出示例

以下代码展示了如何配置GPIO引脚作为输出模式，控制LED：

```c
/* GPIO初始化函数 */
void GPIO_Output_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能GPIOC时钟 */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    /* 配置PC13引脚为输出模式 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;                /* 选择PC13引脚 */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;       /* 推挽输出模式 */
    GPIO_InitStruct.Pull = GPIO_NOPULL;               /* 无上拉/下拉 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;      /* 低速模式 */
    
    /* 初始化GPIOC */
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    /* 设置初始状态为高电平（LED熄灭，假设是低电平点亮） */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

/* LED控制函数 */
void LED_On(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}

void LED_Off(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

void LED_Toggle(void)
{
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}
```

## 2. GPIO输入示例

以下代码展示了如何配置GPIO引脚作为输入模式，读取按键状态：

```c
/* GPIO按键初始化函数 */
void GPIO_Input_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能GPIOA时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置PA0引脚为输入模式（按键） */
    GPIO_InitStruct.Pin = GPIO_PIN_0;              /* 选择PA0引脚 */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;        /* 输入模式 */
    GPIO_InitStruct.Pull = GPIO_PULLUP;            /* 上拉，按键按下接地时检测到低电平 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;   /* 低速模式 */
    
    /* 初始化GPIOA */
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* 按键读取函数 */
GPIO_PinState Key_Read(void)
{
    return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
}

/* 按键检测函数，带消抖 */
uint8_t Key_Scan(void)
{
    /* 检测是否有按键按下 */
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
    {
        /* 延时消抖 */
        HAL_Delay(20);
        if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
        {
            /* 等待按键释放 */
            while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET);
            return 1;
        }
    }
    return 0;
}
```

## 3. GPIO外部中断示例

以下代码展示了如何配置GPIO引脚为外部中断模式，实现按键中断：

```c
/* GPIO外部中断初始化函数 */
void GPIO_EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能GPIOA时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
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
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);      /* 切换LED状态 */
    }
}
```

## 4. GPIO复用功能示例

以下代码展示了如何配置GPIO引脚为复用功能模式，用于USART通信：

```c
/* GPIO复用功能初始化函数 */
void GPIO_AF_USART_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能GPIOA时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置PA9为USART1_TX */
    GPIO_InitStruct.Pin = GPIO_PIN_9;                  /* 选择PA9引脚 */
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;            /* 复用推挽输出 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;      /* 高速模式 */
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 配置PA10为USART1_RX */
    GPIO_InitStruct.Pin = GPIO_PIN_10;                 /* 选择PA10引脚 */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;            /* 输入模式 */
    GPIO_InitStruct.Pull = GPIO_PULLUP;                /* 上拉 */
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 注意：对于STM32F1系列，复用功能不需要特别配置引脚映射 */
    /* 对于STM32F4及更高系列，需要使用GPIO_InitStruct.Alternate成员配置具体的复用功能 */
}
```

## 5. GPIO锁定功能示例

以下代码展示了如何锁定GPIO引脚配置，防止意外修改：

```c
/* GPIO锁定函数 */
void GPIO_Lock_Example(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能GPIOC时钟 */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    /* 配置PC13引脚为输出模式 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    /* 锁定PC13引脚配置 */
    HAL_StatusTypeDef lock_status = HAL_GPIO_LockPin(GPIOC, GPIO_PIN_13);
    
    if(lock_status == HAL_OK)
    {
        /* 锁定成功，现在PC13引脚配置无法被修改，直到下次系统复位 */
        /* 可以添加锁定成功的处理代码 */
    }
    else
    {
        /* 锁定失败 */
        /* 可以添加锁定失败的处理代码 */
    }
}
```

# STM32 HAL库定时器使用示例

本文档提供了使用STM32 HAL库进行定时器初始化的代码示例，包括定时中断、PWM输出和脉冲计数功能。

## 1. 基本定时器定时中断示例

以下代码展示了如何配置TIM2作为基本定时器，并启用定时中断：

```c
#include "./BSP/TIMER/btim.h"

/* 定义句柄 */
TIM_HandleTypeDef g_hbtim;

void BTIM_Init()
{
    /* 开启时钟 */
    __HAL_RCC_TIM2_CLK_ENABLE();
    
    g_hbtim.Instance = TIM2;
    g_hbtim.Init.Prescaler = 7199;	/* 预分频系数，7200分频，计数频率=72MHz/7200=10kHz） */
    g_hbtim.Init.Period = 4999;			/* 自动重装载值 */   
    g_hbtim.Init.CounterMode = TIM_COUNTERMODE_UP;	/* 计数模式：向上计数 */
    g_hbtim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;   /* 允许ARR预装载 */
    HAL_TIM_Base_Init(&g_hbtim);
  
    /* 中断配置 */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    /* 启动定时器 */
    HAL_TIM_Base_Start_IT(&g_hbtim);
}

void TIM2_IRQHandler()
{
    HAL_TIM_IRQHandler(&g_hbtim);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        /* 翻转LED灯 */
        LED_Toggle(GPIOB, GPIO_PIN_14);
    }
}

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/LED.h"
#include "./BSP/TIMER/btim.h"

int main()
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);
    usart_init(115200); 
    LED_Init();
    BTIM_Init();
    while(1)
    {
        printf("Hello World\r\n");
        delay_ms(1000);
    }
}
```

## 2. 通用定时器PWM输出示例

以下代码展示了如何配置TIM3通道1输出PWM信号：

```c
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

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/TIMER/gtim.h"

/* 外部变量声明 */
extern TIM_HandleTypeDef htim3;

int main()
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);
    usart_init(115200); 
    TIM_PWM_Init();
 
    uint16_t pwmVal = 0; /* 调整PWM占空比 */
    uint8_t swap = 1; 	 /* 设置改变方向。1：占空比越来越大；0：占空比越来越小 */
 
    while (1)
    {
        delay_ms(1);
        if(swap)
        	pwmVal++;
        else
        	pwmVal--;
        if(pwmVal > 500)
        	swap = 0;
        else if(pwmVal == 0)
        	swap = 1;
        
        /* 修改比较值，修改占空比 */
        TIM_PWM_SetDuty(pwmVal);
    }
}
```

## 3. 通用定时器输入捕获模式脉冲计数

以下代码展示了如何配置TIM4作为输入捕获模式，用于脉冲计数，还可以测量脉冲宽度、占空比等

```c
/* 定时器句柄定义 */
TIM_HandleTypeDef htim4;
extern uint32_t pulse_count; /* 脉冲计数变量 */

/* 脉冲计数初始化函数 */
void PulseCounter_Init(void)
{
    /* 定时器输入捕获配置结构体 */
    TIM_IC_InitTypeDef sConfigIC = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能TIM4和GPIOB时钟 */
    __HAL_RCC_TIM4_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    /* 配置PB6为输入捕获引脚 */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* 配置定时器基本参数 */
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 0x0;                   /* 不分频 */
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 0xFFFF;                 /* 最大计数值 */
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    /* 初始化定时器 */
    HAL_TIM_IC_Init(&htim4);
    
    /* 配置输入捕获通道 */
    sConfigIC.ICPolarity = TIM_ICPOLARITY_RISING;  /* 上升沿触发 */
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI; /* 直接映射到TI1（通道一） */
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;      /* 不分频 */
    sConfigIC.ICFilter = 0;                      /* 不滤波 */
    
    /* 配置输入捕获通道1 */
	HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1);
    
    /* 配置NVIC */
    HAL_NVIC_SetPriority(TIM4_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
    
    /* 启动输入捕获并开启中断 */
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
}

/* 定时器中断服务函数 */
void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim4);
}

/* 输入捕获回调函数 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM4)
    {
        printf("pluse_count = %d \r\n",pulse_count);
		pulse_count++; /* 脉冲计数加1 */
    }
}

/* 获取脉冲计数值 */
uint32_t Get_PulseCount(void)
{
    return pulse_count;
}

/* 复位脉冲计数值 */
void Reset_PulseCount(void)
{
    printf("RESET!\r\n");
    pulse_count = 0;
}

```

## 4. 通用定时器从模式脉冲计数

以下代码展示了如何使用按键信号作为定时器的时钟源，实现手动控制定时器计数：

```c
#include "./BSP/TIMER/gtim.h"

/* 定时器句柄定义 */
TIM_HandleTypeDef htim3;

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
        /* 每捕获4个脉冲，翻转一次LED */
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
        extern uint16_t count;
        count++;
    }
}

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/TIMER/gtim.h"
#include "./BSP/LED/led.h"

/* 添加外部变量声明 */
extern TIM_HandleTypeDef htim3;
uint16_t count = 0; 
int main()
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);
    usart_init(115200); 

    Tim_SlaveMode_Init();  /* PA6为时钟源，LED在PA12 */
    LED_Init();

    while (1)
    {
        LED_Toggle(GPIOA, GPIO_PIN_7);/* 翻转PA7模拟输出脉冲 */
        printf("count = %d\r\n", count);
        delay_ms(500);
    }

    
}

```

## 5. 高级定时器编码器接口示例（*）

以下代码展示了如何配置TIM1作为编码器接口模式，用于旋转编码器计数：

```c
/* 定时器句柄定义 */
TIM_HandleTypeDef htim1;

/* 编码器接口初始化函数 */
void Encoder_Init(void)
{
    /* 定时器编码器配置结构体 */
    TIM_Encoder_InitTypeDef sConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能TIM1和GPIOA时钟 */
    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置PA8和PA9为编码器输入引脚 */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 配置定时器基本参数 */
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 0xFFFF;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    /* 配置编码器模式 */
    sConfig.EncoderMode = TIM_ENCODERMODE_TI12; /* 两个输入信号都用于计数 */
    sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC1Filter = 10;                    /* 滤波值 */
    
    sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC2Filter = 10;                    /* 滤波值 */
    
    /* 初始化编码器接口 */
    if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
    
    /* 启动编码器接口 */
    HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
}

/* 获取编码器计数值 */
int16_t Get_EncoderValue(void)
{
    return (int16_t)__HAL_TIM_GET_COUNTER(&htim1);
}

/* 设置编码器计数值 */
void Set_EncoderValue(int16_t value)
{
    __HAL_TIM_SET_COUNTER(&htim1, (uint32_t)value);
}
```

## 6. 高级定时器输出指定个数PWM波示例（*）

以下代码展示了如何利用TIM1的RepetitionCounter特性输出指定数量的PWM波：

```c
/* 定时器句柄定义 */
TIM_HandleTypeDef htim1;
volatile uint8_t pwm_complete_flag = 0; /* PWM输出完成标志 */

/* 高级定时器PWM初始化函数 */
void Advanced_PWM_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能TIM1和GPIOA时钟 */
    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置PA8为TIM1_CH1输出引脚 */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 配置定时器基本参数 */
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 72-1;                /* 预分频器，72MHz/72=1MHz的计数频率 */
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 1000-1;                 /* 自动重装载值，PWM频率=1MHz/1000=1KHz */
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;           /* 重复计数器，Generate_PWM_Pulses中设置 */
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    
    /* 初始化定时器 */
    HAL_TIM_PWM_Init(&htim1);
    
    /* 配置PWM输出通道 */
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500;                      /* 占空比50% */
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
    
    /* 配置高级定时器特有的参数 */
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig);
    
    /* 配置NVIC */
    HAL_NVIC_SetPriority(TIM1_UP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
}

/* 输出指定数量PWM波函数 */
void Generate_PWM_Pulses(uint16_t num_pulses)
{
    if(num_pulses == 0 || (__HAL_TIM_GET_COUNTER(&htim1) != 0))
        return;  /* 避免无效输入或当前正在输出 */
    
    pwm_complete_flag = 0;
    
    /* 设置重复计数器值，实际输出脉冲数为RepetitionCounter+1 */
    __HAL_TIM_SET_REPETITION_COUNTER(&htim1, num_pulses - 1);
    
    /* 启用更新中断 */
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
    
    /* 启动PWM输出 */
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

/* 定时器更新中断服务函数 */
void TIM1_UP_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim1);
}

/* 定时器更新中断回调函数 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM1)
    {
        /* 当重复计数器递减到0，最后一个周期完成时会产生更新中断 */
        if(__HAL_TIM_GET_REPETITION_COUNTER(htim) == 0)
        {
            /* 停止PWM输出 */
            HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_1);
            
            /* 禁用更新中断 */
            __HAL_TIM_DISABLE_IT(htim, TIM_IT_UPDATE);
            
            pwm_complete_flag = 1; /* 设置完成标志 */
        }
    }
}

/* 检查PWM是否输出完成 */
uint8_t Is_PWM_Complete(void)
{
    return pwm_complete_flag;
}

/* 设置PWM占空比，值范围0-1000 */
void Set_PWM_DutyCycle(uint16_t duty)
{
    if(duty > 1000) duty = 1000;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty);
}
```

# STM32 HAL库看门狗使用示例

本文档提供了使用STM32 HAL库中WWDG（窗口看门狗）和IWDG（独立看门狗）的代码示例。

## 1. IWDG使用示例

以下代码展示了如何配置和使用独立看门狗：

```c
#include "./BSP/WDG/iwdg.h"

/* 定义句柄 */
IWDG_HandleTypeDef g_hiwdg;

void IWDG_Init(void)
{
    g_hiwdg.Instance = IWDG;		
    g_hiwdg.Init.Prescaler = IWDG_PRESCALER_32;		/* 设置预分频值 */
    g_hiwdg.Init.Reload = 1249; 					/* 设置重装载值 */
    
    /* 初始化IWDG */
    HAL_IWDG_Init(&g_hiwdg);
}

/* 喂狗函数 */
void IWDG_Feed(void)
{
    HAL_IWDG_Refresh(&g_hiwdg);
}

/* iwdg.h文件 */
#ifndef __IWDG_H
#define __IWDG_H

#include "./SYSTEM/sys/sys.h"

extern IWDG_HandleTypeDef g_hiwdg;    /* IWDG的句柄 */

void IWDG_Init();       /* 溢出时间为1s */
void IWDG_Feed();

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/WDG/iwdg.h"

int main()
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);
    usart_init(115200); 
    IWDG_Init();
    printf("no\r\n");
    while(1)
    {
        delay_ms(900);
        IWDG_Feed();
        printf("yes\r\n");
    }
}
```

以下代码展示了如何配置和使用窗口看门狗：

## 2. 普通WWDG示例

```c
#include "./BSP/WDG/wwdg.h"

/* WWDG句柄定义 */
WWDG_HandleTypeDef hwwdg;

/* WWDG初始化函数 */
void WWDG_Init(void)
{
    /* 为什么wwdg需要开启时钟，iwdg不需要？ 
    ** IWDG的时钟来自内部低速RC振荡器（是物理晶振），
    ** WWDG时钟来自APB总线
    */
    __HAL_RCC_WWDG_CLK_ENABLE();
    
    /* 配置WWDG参数 */
    hwwdg.Instance = WWDG;
    hwwdg.Init.Prescaler = WWDG_PRESCALER_8;     /* 设置预分频值 */
    hwwdg.Init.Counter = 127;                    /* 设置计数器初始值（最大127），就是重装载值 */
    hwwdg.Init.Window = 80;                      /* 设置窗口值 */
    hwwdg.Init.EWIMode = WWDG_EWI_DISABLE;       /* 禁用早期唤醒中断 */
    
    /* 初始化WWDG */
	HAL_WWDG_Init(&hwwdg);
}

/* 喂狗函数 */
void WWDG_Feed()
{
    HAL_WWDG_Refresh(&hwwdg);
}

/* wwdg.h文件 */
#ifndef __WWDG_H
#define __WWDG_H

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"

extern WWDG_HandleTypeDef hwwdg;

void IWDG_Init(void);
void IWDG_Feed(void);

#endif
```

## 3. 带早期唤醒中断的WWDG示例

在复位前进入中断，保存关键数据

```c
#include "./BSP/WDG/wwdg.h"
#include "./SYSTEM/sys/sys.h"

/* WWDG句柄定义 */
WWDG_HandleTypeDef hwwdg;

void WWDG_Init_With_EWI(void)
{
    /* 开启时钟 */
    __HAL_RCC_WWDG_CLK_ENABLE();
    
    /* 配置WWDG参数 */
    hwwdg.Instance = WWDG;
    hwwdg.Init.Prescaler = WWDG_PRESCALER_8;	/* 设置预分频值为8 */
    hwwdg.Init.Window = 80;						/* 设置窗口值，喂狗必须在此窗口值以下 */
    hwwdg.Init.Counter = 127;					/* 设置初始计数器值（最大值127） */
    hwwdg.Init.EWIMode = WWDG_EWI_ENABLE;       /* 使能早期唤醒中断 */
    
    /* 初始化WWDG */
	HAL_WWDG_Init(&hwwdg);
    
    /* 配置NVIC */
    HAL_NVIC_SetPriority(WWDG_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(WWDG_IRQn);
    
    /* 结构体中已经使能中断 */
}

/* WWDG中断服务函数 */
void WWDG_IRQHandler(void)
{
    HAL_WWDG_IRQHandler(&hwwdg);
}

/* WWDG早期唤醒中断回调函数 */
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
    /* 在这里处理早期唤醒中断，执行关键数据保存等操作 */
    
    /* 打印没喂狗提示 */
    printf("No feed dog!/r/n");
}

/* 喂狗函数 */
void WWDG_Feed()
{
    HAL_WWDG_Refresh(&hwwdg);
}

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/WDG/wwdg.h"
int main()
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);
    usart_init(115200); 
    WWDG_Init();
    printf("Please feed dog!\r\n");
    while(1)
    {
        delay_ms(10);
        WWDG_Feed();
        printf("feed dog over!\r\n");
    }
}

```

# STM32 HAL库MPU使用示例（**）

本文档提供了使用STM32 HAL库配置和使用MPU(内存保护单元)的代码示例，包括区域配置、访问权限设置和错误处理。

## 1. MPU基本初始化示例

以下代码展示了如何初始化MPU并启用：

```c
/* MPU初始化函数 */
void MPU_Init(void)
{
    /* 禁用MPU */
    HAL_MPU_Disable();
    
    /* 配置MPU区域 */
    MPU_Region_InitTypeDef MPU_InitStruct = {0};
    
    /* 配置RAM区域为可读写，如果是特权级访问 */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.BaseAddress = 0x20000000; /* SRAM起始地址 */
    MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RW_URO; /* 特权读写，用户只读 */
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
    
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    
    /* 启用MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
    
    /* 确保所有指令都已完成 */
    __DSB();
    __ISB();
}
```

## 2. 多区域MPU配置示例

以下代码展示了如何配置多个MPU区域，包括保护Flash和外设区域：

```c
/* MPU多区域配置函数 */
void MPU_Config(void)
{
    /* 禁用MPU */
    HAL_MPU_Disable();
    
    MPU_Region_InitTypeDef MPU_InitStruct = {0};
    
    /* 配置Flash区域为可读，禁止写入 */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.BaseAddress = FLASH_BASE; /* Flash起始地址 */
    MPU_InitStruct.Size = MPU_REGION_SIZE_512KB; /* 根据STM32型号调整 */
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO_URO; /* 特权和用户均只读 */
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE; /* 允许执行代码 */
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    
    /* 配置SRAM区域为普通读写 */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER1;
    MPU_InitStruct.BaseAddress = SRAM_BASE; /* SRAM起始地址 */
    MPU_InitStruct.Size = MPU_REGION_SIZE_64KB; /* 根据STM32型号调整 */
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS; /* 完全访问权限 */
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE; /* 禁止执行代码 */
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
    
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    
    /* 配置外设区域 */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER2;
    MPU_InitStruct.BaseAddress = PERIPH_BASE; /* 外设起始地址 */
    MPU_InitStruct.Size = MPU_REGION_SIZE_512MB; /* 外设区域大小 */
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS; /* 完全访问权限 */
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE; /* 禁止执行代码 */
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE; /* 不使用Cache */
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
    
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    
    /* 启用MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
    
    /* 确保所有指令都已完成 */
    __DSB();
    __ISB();
}
```

## 3. 内存保护错误处理示例

以下代码展示了如何处理内存保护错误：

```c
/* 内存管理错误中断处理函数 */
void MemManage_Handler(void)
{
    /* 检查内存管理错误是否发生 */
    if(SCB->CFSR & SCB_CFSR_MEMFAULTSR_Msk)
    {
        /* 可以添加LED闪烁或其他指示 */
        while(1)
        {
            /* 点亮LED指示错误 */
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
            HAL_Delay(100);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
            HAL_Delay(100);
        }
    }
}

/* 使用MemManage故障状态寄存器信息的函数 */
void MPU_FaultInfo(void)
{
    uint32_t memfsr = (SCB->CFSR & SCB_CFSR_MEMFAULTSR_Msk) >> SCB_CFSR_MEMFAULTSR_Pos;
    
    if(memfsr & (1 << 7)) /* MMARVALID位 */
    {
        /* SCB->MMFAR包含导致故障的地址 */
        volatile uint32_t fault_address = SCB->MMFAR;
        (void)fault_address; /* 避免编译器警告，实际应用中可以使用此地址 */
    }
    
    if(memfsr & (1 << 5)) /* MLSPERR - 在浮点上下文保存期间发生故障 */
    {
        /* 浮点上下文保存错误处理 */
    }
    
    if(memfsr & (1 << 4)) /* MSTKERR - 在压栈过程中发生故障 */
    {
        /* 栈操作错误处理 */
    }
    
    if(memfsr & (1 << 3)) /* MUNSTKERR - 在出栈过程中发生故障 */
    {
        /* 栈操作错误处理 */
    }
    
    if(memfsr & (1 << 1)) /* DACCVIOL - 数据访问违规 */
    {
        /* 数据访问违规处理 */
    }
    
    if(memfsr & (1 << 0)) /* IACCVIOL - 指令访问违规 */
    {
        /* 指令访问违规处理 */
    }
}
```

## 4. 外部SRAM区域保护示例

以下代码展示了如何配置外部SRAM的MPU保护：

```c
/* 外部SRAM的MPU配置函数 */
void MPU_ExtSRAM_Config(void)
{
    /* 禁用MPU */
    HAL_MPU_Disable();
    
    MPU_Region_InitTypeDef MPU_InitStruct = {0};
    
    /* 配置外部SRAM区域 */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER3;
    MPU_InitStruct.BaseAddress = 0x68000000; /* 外部SRAM起始地址，根据实际硬件调整 */
    MPU_InitStruct.Size = MPU_REGION_SIZE_2MB; /* 外部SRAM大小，根据实际硬件调整 */
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS; /* 完全访问权限 */
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE; /* 禁止执行代码 */
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
    
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    
    /* 启用MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
    
    /* 确保所有指令都已完成 */
    __DSB();
    __ISB();
}
```

## 5. 子区域禁用示例

以下代码展示了如何使用子区域禁用功能，实现更精细的内存保护：

```c
/* 使用子区域禁用的MPU配置函数 */
void MPU_SubRegion_Config(void)
{
    /* 禁用MPU */
    HAL_MPU_Disable();
    
    MPU_Region_InitTypeDef MPU_InitStruct = {0};
    
    /* 
     * 配置SRAM区域，但禁用部分子区域
     * 每个区域被划分为8个等大小的子区域
     * SubRegionDisable是一个8位的位图，每一位对应一个子区域
     * 例如0x03表示禁用前两个子区域(0和1)
     */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER1;
    MPU_InitStruct.BaseAddress = SRAM_BASE; /* SRAM起始地址 */
    MPU_InitStruct.Size = MPU_REGION_SIZE_64KB; /* 根据STM32型号调整 */
    MPU_InitStruct.SubRegionDisable = 0xC0; /* 禁用6和7号子区域 */
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS; /* 完全访问权限 */
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
    
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    
    /* 
     * 为禁用的子区域配置不同的访问权限
     * 这里将子区域6和7配置为只读
     */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER2;
    
    /* 计算子区域6的起始地址 */
    /* 每个子区域大小 = 总区域大小 / 8 */
    /* 子区域6起始地址 = 基地址 + 6 * (总区域大小 / 8) */
    uint32_t subregion_size = (1 << (MPU_InitStruct.Size + 1)) / 8;
    MPU_InitStruct.BaseAddress = SRAM_BASE + (6 * subregion_size);
    
    MPU_InitStruct.Size = MPU_REGION_SIZE_8KB; /* 两个子区域各8KB */
    MPU_InitStruct.SubRegionDisable = 0x00; /* 不禁用任何子区域 */
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO_URO; /* 只读权限 */
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    
    /* 启用MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
    
    /* 确保所有指令都已完成 */
    __DSB();
    __ISB();
}
```

# STM32 HAL库DMA使用示例

本文档提供了使用STM32 HAL库进行DMA操作的代码示例，包括内存到内存传输和外设DMA操作。

## 1. 内存到内存DMA传输示例

以下代码展示了如何使用DMA实现内存到内存的数据传输（数组之间的数据搬运）：

```c
#include "./BSP/DMA/dma.h"

/* 缓冲区数组 */
uint32_t SrcBuffer[100] = {0};
uint32_t DstBuffer[100] = {0};

/* DMA句柄定义 */
DMA_HandleTypeDef hdma_memtomem;

/* DMA内存到内存传输初始化函数 */
void DMA_MemToMem_Init(void)
{
    /* 使能DMA时钟 */
    __HAL_RCC_DMA1_CLK_ENABLE();
    
    /* 配置DMA参数 */
    hdma_memtomem.Instance = DMA1_Channel1;                    /* 使用DMA1的通道1 */
    hdma_memtomem.Init.Direction = DMA_MEMORY_TO_MEMORY; /* 内存到内存，该模式不支持循环， */
    hdma_memtomem.Init.PeriphInc = DMA_PINC_ENABLE;      /* 源地址（外设地址）递增 */
    hdma_memtomem.Init.MemInc = DMA_MINC_ENABLE;        /* 目标地址（内存地址）递增 */
    hdma_memtomem.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; /* 源数据宽度，byte 8位，halfword 16位，word32位。搬运数据要保证数据对齐 */
    hdma_memtomem.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;    /* 目标数据宽度 */
    hdma_memtomem.Init.Mode = DMA_NORMAL;  /* 普通模式，传输完成后停止；循环模式，一直搬运数据 */
    hdma_memtomem.Init.Priority = DMA_PRIORITY_HIGH;          /* 高优先级 */
    
    /* 初始化DMA */
    HAL_DMA_Init(&hdma_memtomem);
    
    /* 配置NVIC，如果需要使用中断模式 */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    /* 启动DMA传输，带中断; 注意SrcBuffer和DstBuffer是指针类型 */
    HAL_DMA_Start_IT(&hdma_memtomem, (uint32_t) SrcBuffer, (uint32_t) DstBuffer, 100);
    
}

/* DMA中断服务函数 */
void DMA1_Channel1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_memtomem);
}

/* DMA传输完成回调函数，这个回调函数在F1系列HAL库中没有，F1系列只能在中断服务函数中写业务逻辑 */
void HAL_DMA_XferCpltCallback(DMA_HandleTypeDef *hdma)
{
    if(hdma == &hdma_memtomem)
    {
        /* DMA传输完成后的处理代码 */
		printf("DMA搬运完成！\r\n");
    }
}

/*dma.h文件*/
#ifndef __DMA_H
#define __DMA_H

#include "./SYSTEM/sys/sys.h"

#define DMA_BUFFER_SIZE 10
extern DMA_HandleTypeDef hdma;

extern uint32_t src[DMA_BUFFER_SIZE];
extern uint32_t dst[DMA_BUFFER_SIZE];

void DMA_Init(void);

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/DMA/dma.h"
#include "./BSP/LED/led.h"

int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    usart_init(115200);
    LED_Init();
    
    /* 源地址数组赋值 */
    for(int i = 0; i < 10; i++)
    {
        src[i] = i;
    }
    
    /* 打印源地址数组的值 */
    for(int i = 0; i < 10; i++)
    {
        printf("src[%d] = %d\r\n",i,src[i]);
    }
    
	/* 启动DMA搬运数据 */
    printf("DMA Start!\r\n");
    DMA_Init();
   
    /* 延时等待搬运完成 */
    delay_ms(1000);
    
    /* 打印目的数组数据 */
    for(int i = 0; i < 10; i++)
    {
        printf("dst[%d] = %d\r\n",i,dst[i]);
    }
    
    /* 指示程序正在运行 */
    while(1)
    {
        LED_Toggle(GPIOB, GPIO_PIN_14); 
        delay_ms(500);
    }       
}

```

## 2. UART与DMA结合使用示例

以下代码展示了如何使用DMA进行UART数据收发，减少CPU占用：

```c
#include "./BSP/UART/uart.h" /* 引入UART相关头文件 */

/****************************************开始*******************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if (__ARMCC_VERSION >= 6010050)            /* 判断是否使用AC6编译器 */
__asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6下声明main无参数 */

#else
/* 如果使用AC5编译器，定义__FILE结构体与半主机相关设置 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;  /* 文件句柄 */
    /* 可根据需要增加成员，通常只用printf无需其它操作 */
};

#endif

/* 不使用半主机，重定义_ttywrch函数 */
int _ttywrch(int ch)
{
    ch = ch;  /* 避免编译器警告，空实现 */
    return ch; /* 返回ch */
}

/* 定义_sys_exit函数，防止使用半主机 */
void _sys_exit(int x)
{
    x = x;  /* 避免编译器警告，空实现 */
}

/* 定义_sys_command_string函数，兼容性需要，返回NULL */
char *_sys_command_string(char *cmd, int len)
{
    return NULL; /* 不处理命令，返回NULL */
}

/* 定义标准输出文件结构体 */
FILE __stdout;

/* 重定义fputc函数，printf最终通过fputc输出到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */
    USART1->DR = (uint8_t)ch;             /* 将要发送的字符ch写入DR寄存器 */
    return ch;                            /* 返回发送的字符 */
}
/*********************************************结束*************************************/

/* 定义串口和DMA句柄 */
UART_HandleTypeDef huart1;           /* UART1句柄 */
DMA_HandleTypeDef hdma_usart1_tx;    /* USART1_TX DMA句柄 */
DMA_HandleTypeDef hdma_usart1_rx;    /* USART1_RX DMA句柄 */

/* 定义收发缓冲区 */
uint8_t txBuffer[256];               /* 发送缓冲区 */
uint8_t rxBuffer[256];               /* 接收缓冲区 */
volatile uint8_t rxCompleteFlag = 0; /* 接收完成标志 */

/* UART和DMA初始化函数 */
void UART_DMA_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};      /* 定义GPIO初始化结构体 */
    
    /* 使能外设时钟 */
    __HAL_RCC_USART1_CLK_ENABLE();               /* 使能USART1时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();                /* 使能GPIOA时钟 */
    __HAL_RCC_DMA1_CLK_ENABLE();                 /* 使能DMA1时钟 */
    
    /* 配置GPIO引脚 */
    /* 配置USART1 TX(PA9)为复用推挽输出 */
    GPIO_InitStruct.Pin = GPIO_PIN_9;            
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;      
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);      
    
    /* 配置USART1 RX(PA10)为浮空输入 */
    GPIO_InitStruct.Pin = GPIO_PIN_10;           
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;      
    GPIO_InitStruct.Pull = GPIO_NOPULL;          
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);      
    
    /* 配置UART参数 */
    huart1.Instance = USART1;                    /* 选择USART1外设 */
    huart1.Init.BaudRate = 115200;               /* 波特率115200 */
    huart1.Init.WordLength = UART_WORDLENGTH_8B; /* 8位数据位 */
    huart1.Init.StopBits = UART_STOPBITS_1;      /* 1位停止位 */
    huart1.Init.Parity = UART_PARITY_NONE;       /* 无校验 */
    huart1.Init.Mode = UART_MODE_TX_RX;          /* 发送接收模式 */
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE; /* 无硬件流控 */
    huart1.Init.OverSampling = UART_OVERSAMPLING_16; /* 16倍过采样 */
    
    /* 初始化UART */
    HAL_UART_Init(&huart1);                      /* 调用HAL库初始化UART1 */
    
    /* 配置TX DMA */
    hdma_usart1_tx.Instance = DMA1_Channel4;     /* 配置为DMA1通道4(USART1_TX) */
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;  /* 存储器到外设 */
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;      /* 外设地址不自增 */
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;          /* 内存地址自增 */
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; /* 外设按字节对齐 */
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;    /* 内存按字节对齐 */
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;                    /* 普通模式 */
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;       /* 中等优先级 */
    HAL_DMA_Init(&hdma_usart1_tx);                           /* 初始化DMA通道 */
    
    /* 将DMA与UART关联 */
    __HAL_LINKDMA(&huart1, hdmatx, hdma_usart1_tx);   /* 将TX DMA通道与UART句柄关联 */
    
    /* 配置RX DMA */
    hdma_usart1_rx.Instance = DMA1_Channel5;          /* 配置为DMA1通道5(USART1_RX) */
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY; /* 外设到存储器 */
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;     /* 外设地址不自增 */
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;         /* 内存地址自增 */
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; /* 外设按字节对齐 */
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;    /* 内存按字节对齐 */
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;                   /* 普通模式 */
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_HIGH;        /* 高优先级 */
    HAL_DMA_Init(&hdma_usart1_rx);                          /* 初始化DMA通道 */
    
    /* 将DMA与UART关联 */
    __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);   /* 将RX DMA通道与UART句柄关联 */
    
    /* 配置NVIC中断优先级及使能中断 */
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 1, 0);   /* 设置DMA1通道4中断优先级 */
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);           /* 使能DMA1通道4中断 */
    
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);   /* 设置DMA1通道5中断优先级 */
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);           /* 使能DMA1通道5中断 */
    
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);          /* 设置USART1中断优先级 */
    HAL_NVIC_EnableIRQ(USART1_IRQn);                  /* 使能USART1中断 */
    
    /*  为什么DMA方式下仍需要开启UART中断？
        原因：DMA负责数据搬运，但发送和接收的回调函数触发机制不同。
        DMA接收完成时，回调主要由DMA中断直接触发，不依赖UART中断；但DMA发送完成后，
        只有DMA把最后一个字节搬入USART后，实际数据还需要由UART外设发送出去。
        此时，HAL库需要依赖UART中断来检测"发送完成"状态（如TC标志），
        并最终调用发送完成回调函数（HAL_UART_TxCpltCallback）。
        如果不开启UART中断，发送完成回调不会被调用，影响数据链路完整性和后续数据的正确发送。
        因此，DMA模式下必须同时开启UART和DMA的相关中断，才能确保收发流程和回调机制的正常工作。
    */
}

/* 使用DMA发送数据 */
void UART_DMA_Transmit(uint8_t *data, uint16_t size)
{
    HAL_UART_Transmit_DMA(&huart1, data, size); 		/* 启动DMA方式发送 */
}

/* 使用DMA接收数据 */
void UART_DMA_Receive(uint16_t size)
{
    rxCompleteFlag = 0;                                  /* 清除接收完成标志 */
    HAL_UART_Receive_DMA(&huart1, rxBuffer, size);       /* 启动DMA方式接收 */
}

/* DMA1通道4中断服务函数（用于USART1_TX DMA） */
void DMA1_Channel4_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_tx);                /* 调用HAL库DMA TX中断处理 */
}

/* DMA1通道5中断服务函数（用于USART1_RX DMA） */
void DMA1_Channel5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_rx);                /* 调用HAL库DMA RX中断处理 */
}

/* USART1中断服务函数 */
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);                       /* 调用HAL库UART中断处理 */
}

/* 发送完成回调函数 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)                       /* 判断是否为USART1 */
    {
        /* 发送完成后的处理 */
        printf("trans over\r\n");                       /* 打印发送完成信息 */
    }
}

/* 接收完成回调函数 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)                       /* 判断是否为USART1 */
    {
        /* 接收完成后的处理 */
        rxCompleteFlag = 1;                             /* 置接收完成标志 */
        printf("recive over\r\n");                      /* 打印接收完成信息 */
    }
}

/* uart.h文件 */
#ifndef __UART_H__
#define __UART_H__

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"

/* 定义句柄 */
extern UART_HandleTypeDef huart1;

/* 定义句柄 */
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

/* 定义缓冲区 */
extern uint8_t txBuffer[256];
extern uint8_t rxBuffer[256];
extern volatile uint8_t rxCompleteFlag;

void UART_DMA_Init(void);
void UART_DMA_Transmit(uint8_t *data, uint16_t size);
void UART_DMA_Receive(uint16_t size);

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"     /* 系统时钟初始化相关头文件 */
#include "./SYSTEM/delay/delay.h" /* 延时函数头文件 */
#include "./BSP/UART/uart.h"      /* UART驱动相关头文件 */
#include "./BSP/LED/led.h"        /* LED驱动相关头文件 */

int main(void)
{
    HAL_Init(); /* 初始化HAL库 */

    sys_stm32_clock_init(RCC_PLL_MUL9);   /* 系统时钟初始化，主频72MHz */
    delay_init(72);                       /* 延时函数初始化 */
    LED_Init();                           /* LED初始化 */
    UART_DMA_Init();                      /* UART与DMA初始化 */

    /* 要发送的数据 */
    const char *msg = "Hello DMA UART!\r\n"; /* 发送字符串 */
    uint16_t msgLen = strlen(msg);           /* 字符串长度 */

    uint8_t data[16] = {0};                  /* 定义16字节数据缓冲区并初始化为0 */
    for(int i = 0; i < 16; i++)
    {
        data[i] = i * 2;                     /* 填充数组，内容为0,2,4...30 */
    } 

    /* 启动一次DMA接收，准备接收16个字节 */
    UART_DMA_Receive(16);

    while(1)
    {
        /* 发送数据，通过DMA将data数组发送出去 */
        UART_DMA_Transmit(data, 16);
		// UART_DMA_Transmit((uint8_t *)msg, msgLen);
        
        /* 主循环延时2秒，模拟做其他任务 */
        HAL_Delay(2000);

        /* 检查是否接收完成标志 */
        if(rxCompleteFlag)
        {
            rxCompleteFlag = 0;                /* 清除接收完成标志 */
            printf("Receive Data:\r\n");       /* 打印提示信息 */
            for(int i = 0; i < 16; i++)
            {
                printf("%d;", rxBuffer[i]);    /* 打印接收到的数据内容 */
            }
            LED_Toggle(GPIOB, GPIO_PIN_14);    /* 翻转LED灯，作为指示 */
            UART_DMA_Receive(16);              /* 再次启动DMA接收16字节 */
        }
    }
}
```

## 3. ADC与DMA结合使用示例

以下代码展示了如何使用DMA进行连续ADC采样，实现高速数据采集：

```cc
#include "./BSP/ADC/adc.h"
/* 定义句柄 */
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma1;

/* ADC采样数据缓冲区，数据宽度为16位 */
uint16_t ADC_Buffer[16] = {0}; 

void ADC_Init(void)
{
    /* 配置结构体定义 */
    ADC_ChannelConfTypeDef sConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /* 配置ADC时钟分频 */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    /* 开启时钟 */
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* 配置ADC参数 */
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;           /* 禁用扫描模式，单通道 */
    hadc1.Init.ContinuousConvMode = ENABLE;               /* 启用连续模式，持续转换 */
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;     /* 触发源选择，软件触发 */
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;			  /* 数据对齐模式，右对齐 */
    hadc1.Init.DiscontinuousConvMode = DISABLE;			  /* 间断模式，禁用 */
    hadc1.Init.NbrOfConversion = 1;						  /* 间断模式每次转换的通道数 */
    HAL_ADC_Init(&hadc1);

    /* 配置ADC通道参数 */
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    /* 配置GPIO引脚 */
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 配置DMA */
    hdma1.Instance = DMA1_Channel1;
    hdma1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma1.Init.MemInc = DMA_MINC_ENABLE;
    hdma1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma1.Init.Mode = DMA_CIRCULAR;
    hdma1.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma1);
    
    /* 链接DMA与ADC句柄 */
    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma1);

    /* 配置中断 */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    /* 校准ADC */
    HAL_ADCEx_Calibration_Start(&hadc1);
    
    /* 启动ADC,该函数内部调用了HAL_DMA_Start_IT函数，启动DMA传输 */
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_Buffer, 16);
}

/* DMA中断服务函数 */
void DMA1_Channel1_IRQHandler(void)
{
    /* 该函数内部会调用ADC的回调函数，已经通过__HAL_LINKDMA函数将二者句柄链接，因此可以实现 */
    HAL_DMA_IRQHandler(&hdma1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
		
    /* ADC转换完成 */
    // printf("ADC over transform!\r\n");
    // HAL_ADC_Stop_DMA(&hadc1);
	if(hadc == &hadc1)
    {
		/* 注意回调函数中不能执行耗时长的代码，由于ADC转换太快，非常容易卡死在终端中，比如回调函数中执行printf函数
        为了防止频繁进入中断，可以在回调函数中关闭ADC转换，但是这样就相当于执行一次ADC转换，ADC的连续模式就白配置了 */
                
        /* 或者使用 DMA 和 ADC 在连续模式下采样数据。每次 ADC 完成转换并通过 DMA 将数据传输到内存时，会触发一个中断，
           进而调用回调函数。在回调函数中，执行的操作应尽量简单和快速，可以通过设置标志位来通知主程序去处理数据。 */
    }
}

/* adc.h文件 */
#ifndef __ADC_H__
#define __ADC_H__ 

#include "./SYSTEM/sys/sys.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma1;
extern uint16_t ADC_Buffer[16];

void ADC_Init(void);

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/adc/adc.h"
#include "./BSP/led/led.h"
int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    usart_init(115200);    
    LED_Init();
    ADC_Init(); 
    while(1)
    {
        /* LED指示程序运行 */
        LED_Toggle(GPIOB, GPIO_PIN_14);
        delay_ms(500);
        
        /* 查看ADC转换结果 */
        for(int i = 0; i < 16; i++)
        {
            printf("ADC_Buffer[%d] = %d\r\n", i, ADC_Buffer[i]);
            printf("Voltge[%d] = %.2fV\r\n", i, ADC_Buffer[i] * (3.3 / 4096.0));
        }
    }
}
```

# STM32 HAL库ADC使用示例

本文档提供了使用STM32 HAL库进行ADC初始化和采样的代码示例，包括单通道采样、多通道扫描和中断模式。

## 1. ADC非扫描模式示例

以下代码展示了如何配置ADC进行单通道采样：

```c
#include "./BSP/ADC/adc.h"

/* 定义句柄 */
ADC_HandleTypeDef hadc1;

void ADC_Init(void)
{
    /* 定义初始化结构体 */
    ADC_ChannelConfTypeDef sConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  
    /* 配置ADC时钟分频 */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    /* 开启时钟 */
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置ADC参数 */
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;           /* 禁用扫描模式，单通道 */
    hadc1.Init.ContinuousConvMode = ENABLE;              /*  连续模式，单次转换 */
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;     /* 触发源选择，软件触发 */
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;			  /* 数据对齐模式，右对齐 */
    hadc1.Init.DiscontinuousConvMode = DISABLE;			  /* 间断模式，禁用 */
    hadc1.Init.NbrOfConversion = 1;						  /* 间断模式每次转换的通道数 */
    HAL_ADC_Init(&hadc1);
    
    /* 配置ADC通道参数 */
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    
    /* 配置GPIO引脚 */
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 校准ADC */
    HAL_ADCEx_Calibration_Start(&hadc1);
    
    /* 启动ADC  */
    HAL_ADC_Start(&hadc1);
}

uint16_t ADC_GetValue(void)
{
    /* 单次非扫描，在init函数中启动了ADC，转换完成后会自动停止，结果存放在DR寄存器中 */
    // HAL_ADC_Start(&hadc1);
    uint16_t value;
    /* 轮询等待ADC转换完成，获取ADC数据 */
    HAL_ADC_PollForConversion(&hadc1, 10);
    value = HAL_ADC_GetValue(&hadc1);
    
    // HAL_ADC_Stop(&hadc1);
    return value;
}

/* adc.h文件 */
#ifndef __ADC_H__
#define __ADC_H__ 

#include "./SYSTEM/sys/sys.h"

extern ADC_HandleTypeDef hadc1;

void ADC_Init(void);
uint16_t ADC_GetValue(void);

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/adc/adc.h"

int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    usart_init(115200);    
    ADC_Init();
    while(1)
    {
        uint16_t value = ADC_GetValue();
        float voltage = (float)value *(3.3 / 4096) ;
        printf("ADC_Value: %d\r\n", value);
        printf("Votelge_Value: %.2f\r\n", voltage);
        delay_ms(1000);  
    }        
}
```

## 2. ADC-DMA非扫描模式示例

以下代码展示了如何使用DMA进行连续ADC采样，实现高速数据采集：

```cc
#include "./BSP/ADC/adc.h"
/* 定义句柄 */
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma1;

/* ADC采样数据缓冲区，数据宽度为16位 */
uint16_t ADC_Buffer[16] = {0}; 

void ADC_Init(void)
{
    /* 配置结构体定义 */
    ADC_ChannelConfTypeDef sConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /* 配置ADC时钟分频 */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    /* 开启时钟 */
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* 配置ADC参数 */
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;           /* 禁用扫描模式，单通道 */
    hadc1.Init.ContinuousConvMode = ENABLE;               /* 启用连续模式，持续转换 */
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;     /* 触发源选择，软件触发 */
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;			  /* 数据对齐模式，右对齐 */
    hadc1.Init.DiscontinuousConvMode = DISABLE;			  /* 间断模式，禁用 */
    hadc1.Init.NbrOfConversion = 1;						  /* 间断模式每次转换的通道数 */
    HAL_ADC_Init(&hadc1);

    /* 配置ADC通道参数 */
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    /* 配置GPIO引脚 */
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 配置DMA */
    hdma1.Instance = DMA1_Channel1;
    hdma1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma1.Init.MemInc = DMA_MINC_ENABLE;
    hdma1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma1.Init.Mode = DMA_CIRCULAR;
    hdma1.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma1);
    
    /* 链接DMA与ADC句柄 */
    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma1);

    /* 配置中断 */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    /* 校准ADC */
    HAL_ADCEx_Calibration_Start(&hadc1);
    
    /* 启动ADC,该函数内部调用了HAL_DMA_Start_IT函数，启动DMA传输 */
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_Buffer, 16);
}

/* DMA中断服务函数 */
void DMA1_Channel1_IRQHandler(void)
{
    /* 该函数内部会调用ADC的回调函数，已经通过__HAL_LINKDMA函数将二者句柄链接，因此可以实现 */
    HAL_DMA_IRQHandler(&hdma1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
		
    /* ADC转换完成 */
    // printf("ADC over transform!\r\n");
    // HAL_ADC_Stop_DMA(&hadc1);
	if(hadc == &hadc1)
    {
		/* 注意回调函数中不能执行耗时长的代码，由于ADC转换太快，非常容易卡死在终端中，比如回调函数中执行printf函数
        为了防止频繁进入中断，可以在回调函数中关闭ADC转换，但是这样就相当于执行一次ADC转换，ADC的连续模式就白配置了 */
                
        /* 或者使用 DMA 和 ADC 在连续模式下采样数据。每次 ADC 完成转换并通过 DMA 将数据传输到内存时，会触发一个中断，
           进而调用回调函数。在回调函数中，执行的操作应尽量简单和快速，可以通过设置标志位来通知主程序去处理数据。 */
    }
}

/* adc.h文件 */
#ifndef __ADC_H__
#define __ADC_H__ 

#include "./SYSTEM/sys/sys.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma1;
extern uint16_t ADC_Buffer[16];

void ADC_Init(void);

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/adc/adc.h"
#include "./BSP/led/led.h"
int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    usart_init(115200);    
    LED_Init();
    ADC_Init(); 
    while(1)
    {
        /* LED指示程序运行 */
        LED_Toggle(GPIOB, GPIO_PIN_14);
        delay_ms(500);
        
        /* 查看ADC转换结果 */
        for(int i = 0; i < 16; i++)
        {
            printf("ADC_Buffer[%d] = %d\r\n", i, ADC_Buffer[i]);
            printf("Voltge[%d] = %.2fV\r\n", i, ADC_Buffer[i] * (3.3 / 4096.0));
        }
    }
}
```

## 2. ADC-DMA扫描模式示例

以下代码展示了如何配置ADC进行多通道扫描采样：

```c
#include "./BSP/ADC/adc.h"

/* ADC转换缓冲区 */
uint16_t buffer[100] = {0};

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma1;
void ADC_Init(void)
{
    /* 定义结构体 */
    ADC_ChannelConfTypeDef sConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    
    /* 配置ADC时钟分频 */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
    
	/* 开启时钟 */
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* 配置ADC参数 */
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;           /* 扫描模式，多通道 */
    hadc1.Init.ContinuousConvMode = DISABLE;              /* 连续模式，单次转换 */
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;    /* 触发源选择，软件触发 */
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;		     /* 数据对齐模式，右对齐 */
    hadc1.Init.DiscontinuousConvMode = DISABLE;		     /* 间断模式，禁用 */
    hadc1.Init.NbrOfConversion = 3;				         /* 扫描模式下的通道数量 */
    HAL_ADC_Init(&hadc1);

    /* 配置ADC通道 */
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = ADC_REGULAR_RANK_2;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = ADC_REGULAR_RANK_3;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    /* 配置GPIO */
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 配置DMA */
    hdma1.Instance = DMA1_Channel1;
    hdma1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma1.Init.MemInc = DMA_MINC_ENABLE;
    hdma1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma1.Init.Mode = DMA_CIRCULAR;
    hdma1.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma1);

    /* 链接DMA句柄到ADC句柄 */
    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma1);
	
    /* 配置中断优先级并使能 */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    /* 校准ADC */
    HAL_ADCEx_Calibration_Start(&hadc1);
    
    /* 启动ADC */
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)buffer, 6);
}

/* DMA中断服务函数 */
void DMA1_Channel1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc == &hadc1)
    {
        // printf("DMA OVER!\r\n");
    }
}

/* adc.h文件 */
#ifndef __ADC_H__
#define __ADC_H__ 

#include "./SYSTEM/sys/sys.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma1;

extern uint16_t buffer[100];

void ADC_Init(void);

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/adc/adc.h"
#include "./BSP/led/led.h"

int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    usart_init(115200);
    LED_Init();
    ADC_Init();
    while(1)
    {
        /* LED指示程序运行 */
        LED_Toggle(GPIOB, GPIO_PIN_14);
        delay_ms(5000);
        
        /* 查看ADC转换结果 */
        for(int i = 0; i < 5; i++)
        {
            printf("buffer[%d] = %d\r\n", i, buffer[i]);
            printf("Voltge[%d] = %.2fV\r\n", i, buffer[i] * (3.3 / 4096.0));
        }
    }
}

```

## 3. ADC温度传感器采样示例

以下代码展示了如何使用STM32内部温度传感器进行温度测量：

```c
#include "./BSP/ADC/adc.h"

/* 定义句柄 */
ADC_HandleTypeDef hadc1;

void ADC_Init(void)
{
    /* 定义初始化结构体 */
    ADC_ChannelConfTypeDef sConfig = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  
    /* 配置ADC时钟分频 */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    /* 开启时钟 */
    __HAL_RCC_ADC1_CLK_ENABLE();
    
    /* 配置ADC参数 */
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;           /* 禁用扫描模式，单通道 */
    hadc1.Init.ContinuousConvMode = ENABLE;               /*  连续模式，单次转换 */
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;     /* 触发源选择，软件触发 */
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;			  /* 数据对齐模式，右对齐 */
    hadc1.Init.DiscontinuousConvMode = DISABLE;			  /* 间断模式，禁用 */
    hadc1.Init.NbrOfConversion = 1;						  /* 间断模式每次转换的通道数 */
    HAL_ADC_Init(&hadc1);
    
    /* 配置ADC通道参数 */
    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;			  /* 就是ADC1通道16 */
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;    /* 温度传感器采样时间要长一点 */
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    
    /* 打开温度传感器 */
    ADC1->CR2 |= ADC_CR2_TSVREFE;
    
    /* 校准ADC */
    HAL_ADCEx_Calibration_Start(&hadc1);
    
    /* 启动ADC  */
    HAL_ADC_Start(&hadc1);
}

uint16_t ADC_GetValue(void)
{
    /* 单次非扫描，在Init函数中启动了ADC，转换完成后会自动停止，结果存放在DR寄存器中 */
    /* 轮询等待ADC转换完成，获取ADC数据 */
    HAL_ADC_PollForConversion(&hadc1, 10);
    uint16_t value = HAL_ADC_GetValue(&hadc1);
    return value;
}

/* adc.h文件 */
#ifndef __ADC_H__
#define __ADC_H__ 

#include "./SYSTEM/sys/sys.h"

extern ADC_HandleTypeDef hadc1;

void ADC_Init(void);

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/adc/adc.h"

int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    usart_init(115200);    
    ADC_Init();
    
    while(1)
    {
        uint16_t value = ADC_GetValue();
        float voltage = (float)value *(3.3 / 4096);
        float temp = ((1.43f - voltage) / 0.0043f) + 25.0f;  /* 电压值转换成温度值 */ 
        printf("ADC_Value: %d\r\n", value);
        printf("Votelge_Value: %.2f\r\n", voltage);
        printf("Temperature: %.2f\r\n", temp);   
        delay_ms(1000);  
    }        
}
```

# STM32 HAL库UART使用示例

本文档提供了使用STM32 HAL库进行UART初始化和数据传输的代码示例，包括轮询模式、中断模式和DMA模式。

## 1. UART轮询模式收发示例

以下代码展示了如何配置UART并使用轮询方式进行数据收发：

```c
#include "./BSP/UART/uart.h"

/* 定义句柄 */
UART_HandleTypeDef huart1;

void UART_Init(void)
{
    /* 定义结构体 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能时钟 */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置GPIO引脚 */
    /* USART1 TX - PA9 */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;		/* 推挽输出模式 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* USART1 RX - PA10 */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; 	/* 输入模式 */
    GPIO_InitStruct.Pull = GPIO_NOPULL;			/* 无上下拉 */
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 配置UART参数 */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;					 /* 设置波特率 */
    huart1.Init.WordLength = UART_WORDLENGTH_8B;	 /* 设置传输数据为长度 */
    huart1.Init.StopBits = UART_STOPBITS_1;		  	 /* 设置停止位长度 */
    huart1.Init.Parity = UART_PARITY_NONE;			 /* 设置奇偶校验 */
    huart1.Init.Mode = UART_MODE_TX_RX;				 /* 设置传输模式 */	
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;	 /* 硬件流控制 */
    huart1.Init.OverSampling = UART_OVERSAMPLING_16; /* 设置过采样 */
    
    /* 初始化UART */
    HAL_UART_Init(&huart1);
}

/* 发送一个字节数据 */
void UART_SendByte(uint8_t data)
{
    /* 阻塞等待直到发送完成，等待最大时间为HAL_MAX_DELAY */
    HAL_UART_Transmit(&huart1, &data, 1, HAL_MAX_DELAY);
}

/* 发送字符串数据 */
void UART_SendString(char *str)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

/* 接收一个字节数据 */
uint8_t UART_ReceiveByte(void)
{
    uint8_t data;
    /* 阻塞等待直到接收完成 */
    HAL_UART_Receive(&huart1, &data, 1, HAL_MAX_DELAY);
    return data;
}

/* uart.h文件 */
#ifndef __UART_H__
#define __UART_H__

#include "./SYSTEM/sys/sys.h"

extern UART_HandleTypeDef huart1;

void UART_SendByte(uint8_t data);
void UART_SendString(char *str);
uint8_t UART_ReceiveByte(void);

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/UART/uart.h"
#include "./BSP/LED/led.h"

int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    LED_Init();
    UART_Init();
    
    UART_SendString("UART Polling Mode Test\r\n");
    
    uint8_t receivedData;
    
    while(1)
    {
        /* 接收一个字节数据 */
        receivedData = UART_ReceiveByte();
        
        /* 发送回接收到的数据（回显） */
        UART_SendByte(receivedData);
        
        /* LED指示接收到数据 */
        LED_Toggle(GPIOC, GPIO_PIN_13);
    }
}

/* 注意：这种收发方式经常会在发送一串数据时丢失字符！
   Python/串口助手往往一口气调用 write 方法，把一整包数据瞬间写入串口缓冲，
   实际到达STM32的时候，几个字节很快堆积在串口硬件缓冲区。
   STM32的UART硬件只有极小的FIFO，根本扛不住高速爆发式的数据写入。
   STM32主循环一次只能消耗1字节，稍有处理慢、打断，后面的字节就被"挤掉"了。*/
```

## 2. UART中断模式收发示例

这段代码可以接收不定长数据，但它有一个前提条件： 数据的结束需要用\r\n（回车）作为一条数据包的结束标志。每次发送一个字节进一次中断，当检测到回车时，中断中会置接受完成标志位，主循环就能处理这一包数据。以下代码展示了如何配置UART并使用中断方式进行数据收发：

```c
#include "./BSP/UART/uart.h"

/****************************************开始*******************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if (__ARMCC_VERSION >= 6010050)            /* 判断是否使用AC6编译器 */
__asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6下声明main无参数 */

#else
/* 如果使用AC5编译器，定义__FILE结构体与半主机相关设置 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;  /* 文件句柄 */
    /* 可根据需要增加成员，通常只用printf无需其它操作 */
};

#endif

/* 不使用半主机，重定义_ttywrch函数 */
int _ttywrch(int ch)
{
    ch = ch;  /* 避免编译器警告，空实现 */
    return ch; /* 返回ch */
}

/* 定义_sys_exit函数，防止使用半主机 */
void _sys_exit(int x)
{
    x = x;  /* 避免编译器警告，空实现 */
}

/* 定义_sys_command_string函数，兼容性需要，返回NULL */
char *_sys_command_string(char *cmd, int len)
{
    return NULL; /* 不处理命令，返回NULL */
}

/* 定义标准输出文件结构体 */
FILE __stdout;

/* 重定义fputc函数，printf最终通过fputc输出到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */
    USART1->DR = (uint8_t)ch;             /* 将要发送的字符ch写入DR寄存器 */
    return ch;                            /* 返回发送的字符 */
}
/*********************************************结束*************************************/

/* 定义句柄 */
UART_HandleTypeDef huart1;

/* 定义缓冲区 */
uint8_t rxBuffer[64];   /* 接受缓冲区64字节 */
uint8_t rxData;			/* 当前接受字节缓冲区 */
volatile uint8_t rxComplete = 0; /* 接收完成标志位 */

void UART_Init(void)
{
    /* 定义结构体 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能时钟 */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置GPIO引脚 */
    /* USART1 TX - PA9 */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;		/* 推挽输出模式 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* USART1 RX - PA10 */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; 	/* 输入模式 */
    GPIO_InitStruct.Pull = GPIO_NOPULL;			/* 无上下拉 */
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 配置UART参数 */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;					 /* 设置波特率 */
    huart1.Init.WordLength = UART_WORDLENGTH_8B;	 /* 设置传输数据为长度 */
    huart1.Init.StopBits = UART_STOPBITS_1;		  	 /* 设置停止位长度 */
    huart1.Init.Parity = UART_PARITY_NONE;			 /* 设置奇偶校验 */
    huart1.Init.Mode = UART_MODE_TX_RX;				 /* 设置传输模式 */	
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;	 /* 硬件流控制 */
    huart1.Init.OverSampling = UART_OVERSAMPLING_16; /* 设置过采样 */
    
    /* 初始化UART */
    HAL_UART_Init(&huart1);

    /*开启中断*/
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    /* 启动UART */
    HAL_UART_Receive_IT(&huart1, &rxData, 1);
}

/* 发送字符串数据 */
void UART_SendString(char *str)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

/* UART中断服务函数 */
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

/* 接收完成回调函数，以 \r\n 作为一条数据的结束标志 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    /* 判断是否为 USART1 产生的中断 */
    if (huart->Instance == USART1)
    {
        static uint8_t rxIndex = 0;   /* 静态变量，记录接收数据的下标 */
        static uint8_t state = 0;     /* 状态变量：0=普通状态，1=刚收到'\r' */

        switch(state)
        {
            case 0:
                /* 普通状态，尚未遇到回车符 '\r' */
                if (rxData == '\r')
                {
                    state = 1;  /* 如果收到 '\r'，状态切换为1，等待下一个字符判断是否为 '\n' */
                }
                else
                {
                    /* 普通字符，存入缓冲区 */
                    if (rxIndex < sizeof(rxBuffer) - 1)
                        rxBuffer[rxIndex++] = rxData;
                }
                break;

            case 1:
                /* 状态1：上一次收到的是 '\r'，现在判断是否为 '\n' */
                if (rxData == '\n')
                {
                    /* 收到 '\n'，说明遇到了完整的 "\r\n"，数据接收完成 */
                    rxBuffer[rxIndex] = '\0';   /* 添加字符串结束符 */
                    rxComplete = 1;             /* 设置接收完成标志 */
                    rxIndex = 0;                /* 重置下标，准备接收下一条数据 */
                }
                else
                {
                    /* 如果不是 '\n'，则之前收到的 '\r' 视为普通数据，和当前字符一起存入缓冲区 */
                    if (rxIndex < sizeof(rxBuffer) - 2)
                    {
                        rxBuffer[rxIndex++] = '\r';    /* 存入上一个 '\r' */
                        rxBuffer[rxIndex++] = rxData;  /* 存入当前收到的字符 */
                    }
                }
                state = 0;   /* 状态回到普通状态，准备下一轮判断 */
                break;
        }

        /* 启动下一字节的中断接收，保证持续接收数据 */
        HAL_UART_Receive_IT(&huart1, &rxData, 1);
    }
}

/* usart.h文件 */
#ifndef __UART_H__
#define __UART_H__

#include "./SYSTEM/sys/sys.h"
#include "stdio.h"

extern UART_HandleTypeDef huart1;

/* 定义缓冲区 */
extern uint8_t rxBuffer[64];   /* 接受缓冲区64字节 */
extern uint8_t rxData;
extern volatile uint8_t rxComplete;

void UART_Init(void);
void UART_SendString(char *str);

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/UART/uart.h"
#include "./BSP/LED/led.h"

int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    LED_Init();
    UART_Init();
    
    UART_SendString("UART Interrupt Mode Test\r\n");
    UART_SendString("Please input command and press Enter:\r\n");
    
    while(1)
    {
        if(rxComplete)
        {
            /* 处理接收到的指令 */
            if(strcmp((char*)rxBuffer, "LED_ON") == 0)
            {
                LED_On(GPIOB, GPIO_PIN_14);
                UART_SendString("LED turned ON\r\n");
            }
            else if(strcmp((char*)rxBuffer, "LED_OFF") == 0)
            {
                LED_Off(GPIOB, GPIO_PIN_14);
                UART_SendString("LED turned OFF\r\n");
            }
            else
            {
                UART_SendString("Unknown command: ");
                UART_SendString((char*)rxBuffer);
                UART_SendString("\r\n");
            }
            
            rxComplete = 0;  /* 重置接收完成标志 */
        }
        
        /* 其他任务处理 */
        delay_ms(10);
    }
}
```

## 3. UART-DMA模式收发示例

这段代码只能接受定长数据，数据长度不够不会触发DMA中断。以下代码展示了如何配置UART和DMA，实现高效的数据传输：

```c
#include "./BSP/UART/uart.h" /* 引入UART相关头文件 */

/****************************************开始*******************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if (__ARMCC_VERSION >= 6010050)            /* 判断是否使用AC6编译器 */
__asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6下声明main无参数 */

#else
/* 如果使用AC5编译器，定义__FILE结构体与半主机相关设置 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;  /* 文件句柄 */
    /* 可根据需要增加成员，通常只用printf无需其它操作 */
};

#endif

/* 不使用半主机，重定义_ttywrch函数 */
int _ttywrch(int ch)
{
    ch = ch;  /* 避免编译器警告，空实现 */
    return ch; /* 返回ch */
}

/* 定义_sys_exit函数，防止使用半主机 */
void _sys_exit(int x)
{
    x = x;  /* 避免编译器警告，空实现 */
}

/* 定义_sys_command_string函数，兼容性需要，返回NULL */
char *_sys_command_string(char *cmd, int len)
{
    return NULL; /* 不处理命令，返回NULL */
}

/* 定义标准输出文件结构体 */
FILE __stdout;

/* 重定义fputc函数，printf最终通过fputc输出到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */
    USART1->DR = (uint8_t)ch;             /* 将要发送的字符ch写入DR寄存器 */
    return ch;                            /* 返回发送的字符 */
}
/*********************************************结束*************************************/

/* 定义串口和DMA句柄 */
UART_HandleTypeDef huart1;           /* UART1句柄 */
DMA_HandleTypeDef hdma_usart1_tx;    /* USART1_TX DMA句柄 */
DMA_HandleTypeDef hdma_usart1_rx;    /* USART1_RX DMA句柄 */

/* 定义收发缓冲区 */
uint8_t txBuffer[256];               /* 发送缓冲区 */
uint8_t rxBuffer[256];               /* 接收缓冲区 */
volatile uint8_t rxCompleteFlag = 0; /* 接收完成标志 */

/* UART和DMA初始化函数 */
void UART_DMA_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};      /* 定义GPIO初始化结构体 */
    
    /* 使能外设时钟 */
    __HAL_RCC_USART1_CLK_ENABLE();               /* 使能USART1时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();                /* 使能GPIOA时钟 */
    __HAL_RCC_DMA1_CLK_ENABLE();                 /* 使能DMA1时钟 */
    
    /* 配置GPIO引脚 */
    /* 配置USART1 TX(PA9)为复用推挽输出 */
    GPIO_InitStruct.Pin = GPIO_PIN_9;            
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;      
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);      
    
    /* 配置USART1 RX(PA10)为浮空输入 */
    GPIO_InitStruct.Pin = GPIO_PIN_10;           
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;      
    GPIO_InitStruct.Pull = GPIO_NOPULL;          
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);      
    
    /* 配置UART参数 */
    huart1.Instance = USART1;                    /* 选择USART1外设 */
    huart1.Init.BaudRate = 115200;               /* 波特率115200 */
    huart1.Init.WordLength = UART_WORDLENGTH_8B; /* 8位数据位 */
    huart1.Init.StopBits = UART_STOPBITS_1;      /* 1位停止位 */
    huart1.Init.Parity = UART_PARITY_NONE;       /* 无校验 */
    huart1.Init.Mode = UART_MODE_TX_RX;          /* 发送接收模式 */
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE; /* 无硬件流控 */
    huart1.Init.OverSampling = UART_OVERSAMPLING_16; /* 16倍过采样 */
    
    /* 初始化UART */
    HAL_UART_Init(&huart1);                      /* 调用HAL库初始化UART1 */
    
    /* 配置TX DMA */
    hdma_usart1_tx.Instance = DMA1_Channel4;     /* 配置为DMA1通道4(USART1_TX) */
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;  /* 存储器到外设 */
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;      /* 外设地址不自增 */
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;          /* 内存地址自增 */
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; /* 外设按字节对齐 */
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;    /* 内存按字节对齐 */
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;                    /* 普通模式 */
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;       /* 中等优先级 */
    HAL_DMA_Init(&hdma_usart1_tx);                           /* 初始化DMA通道 */
    
    /* 将DMA与UART关联 */
    __HAL_LINKDMA(&huart1, hdmatx, hdma_usart1_tx);   /* 将TX DMA通道与UART句柄关联 */
    
    /* 配置RX DMA */
    hdma_usart1_rx.Instance = DMA1_Channel5;          /* 配置为DMA1通道5(USART1_RX) */
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY; /* 外设到存储器 */
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;     /* 外设地址不自增 */
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;         /* 内存地址自增 */
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; /* 外设按字节对齐 */
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;    /* 内存按字节对齐 */
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;                   /* 普通模式 */
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_HIGH;        /* 高优先级 */
    HAL_DMA_Init(&hdma_usart1_rx);                          /* 初始化DMA通道 */
    
    /* 将DMA与UART关联 */
    __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);   /* 将RX DMA通道与UART句柄关联 */
    
    /* 配置NVIC中断优先级及使能中断 */
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 1, 0);   /* 设置DMA1通道4中断优先级 */
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);           /* 使能DMA1通道4中断 */
    
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);   /* 设置DMA1通道5中断优先级 */
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);           /* 使能DMA1通道5中断 */
    
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);          /* 设置USART1中断优先级 */
    HAL_NVIC_EnableIRQ(USART1_IRQn);                  /* 使能USART1中断 */
    
    /*  为什么DMA方式下仍需要开启UART中断？
        原因：DMA负责数据搬运，但发送和接收的回调函数触发机制不同。
        DMA接收完成时，回调主要由DMA中断直接触发，不依赖UART中断；但DMA发送完成后，
        只有DMA把最后一个字节搬入USART后，实际数据还需要由UART外设发送出去。
        此时，HAL库需要依赖UART中断来检测"发送完成"状态（如TC标志），
        并最终调用发送完成回调函数（HAL_UART_TxCpltCallback）。
        如果不开启UART中断，发送完成回调不会被调用，影响数据链路完整性和后续数据的正确发送。
        因此，DMA模式下必须同时开启UART和DMA的相关中断，才能确保收发流程和回调机制的正常工作。
    */
}

/* 使用DMA发送数据 */
void UART_DMA_Transmit(uint8_t *data, uint16_t size)
{
    HAL_UART_Transmit_DMA(&huart1, data, size); 		/* 启动DMA方式发送 */
}

/* 使用DMA接收数据 */
void UART_DMA_Receive(uint16_t size)
{
    rxCompleteFlag = 0;                                  /* 清除接收完成标志 */
    HAL_UART_Receive_DMA(&huart1, rxBuffer, size);       /* 启动DMA方式接收 */
}

/* DMA1通道4中断服务函数（用于USART1_TX DMA） */
void DMA1_Channel4_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_tx);                /* 调用HAL库DMA TX中断处理 */
}

/* DMA1通道5中断服务函数（用于USART1_RX DMA） */
void DMA1_Channel5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_rx);                /* 调用HAL库DMA RX中断处理 */
}

/* USART1中断服务函数 */
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);                       /* 调用HAL库UART中断处理 */
}

/* 发送完成回调函数 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)                       /* 判断是否为USART1 */
    {
        /* 发送完成后的处理 */
        printf("trans over\r\n");                       /* 打印发送完成信息 */
    }
}

/* 接收完成回调函数 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)                       /* 判断是否为USART1 */
    {
        /* 接收完成后的处理 */
        rxCompleteFlag = 1;                             /* 置接收完成标志 */
        printf("recive over\r\n");                      /* 打印接收完成信息 */
    }
}

/* uart.h文件 */
#ifndef __UART_H__
#define __UART_H__

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"

/* 定义句柄 */
extern UART_HandleTypeDef huart1;

/* 定义句柄 */
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

/* 定义缓冲区 */
extern uint8_t txBuffer[256];
extern uint8_t rxBuffer[256];
extern volatile uint8_t rxCompleteFlag;

void UART_DMA_Init(void);
void UART_DMA_Transmit(uint8_t *data, uint16_t size);
void UART_DMA_Receive(uint16_t size);

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"     /* 系统时钟初始化相关头文件 */
#include "./SYSTEM/delay/delay.h" /* 延时函数头文件 */
#include "./BSP/UART/uart.h"      /* UART驱动相关头文件 */
#include "./BSP/LED/led.h"        /* LED驱动相关头文件 */

int main(void)
{
    HAL_Init(); /* 初始化HAL库 */

    sys_stm32_clock_init(RCC_PLL_MUL9);   /* 系统时钟初始化，主频72MHz */
    delay_init(72);                       /* 延时函数初始化 */
    LED_Init();                           /* LED初始化 */
    UART_DMA_Init();                      /* UART与DMA初始化 */

    /* 要发送的数据 */
    const char *msg = "Hello DMA UART!\r\n"; /* 发送字符串 */
    uint16_t msgLen = strlen(msg);           /* 字符串长度 */

    uint8_t data[16] = {0};                  /* 定义16字节数据缓冲区并初始化为0 */
    for(int i = 0; i < 16; i++)
    {
        data[i] = i * 2;                     /* 填充数组，内容为0,2,4...30 */
    } 

    /* 启动一次DMA接收，准备接收16个字节 */
    UART_DMA_Receive(16);

    while(1)
    {
        /* 发送数据，通过DMA将data数组发送出去 */
        UART_DMA_Transmit(data, 16);
		// UART_DMA_Transmit((uint8_t *)msg, msgLen);
        
        /* 主循环延时2秒，模拟做其他任务 */
        HAL_Delay(2000);

        /* 检查是否接收完成标志 */
        if(rxCompleteFlag)
        {
            rxCompleteFlag = 0;                /* 清除接收完成标志 */
            printf("Receive Data:\r\n");       /* 打印提示信息 */
            for(int i = 0; i < 16; i++)
            {
                printf("%d;", rxBuffer[i]);    /* 打印接收到的数据内容 */
            }
            LED_Toggle(GPIOB, GPIO_PIN_14);    /* 翻转LED灯，作为指示 */
            UART_DMA_Receive(16);              /* 再次启动DMA接收16字节 */
        }
    }
}
```

## 4. UART空闲中断收发示例

​	空闲中断+DMA：不管收多少，数据间断就触发中断；普通DMA：只认死理，收够固定长度才触发中断。以下代码展示了如何使用UART空闲中断来实现不定长数据的接收：
​	**DMA循环模式的作用，是让DMA不停地从UART硬件寄存器取数据到缓冲区，极大降低数据寄存器（RDR/DR）被覆盖（丢失数据）的概率。但根本的保障还是靠主控及时处理缓冲区！**

```c
/* uart.c文件 */
#include "./BSP/UART/uart.h"    /* 引入自定义UART空闲中断相关头文件 */

/****************************************开始*******************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if (__ARMCC_VERSION >= 6010050)            /* 判断是否使用AC6编译器 */
__asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6下声明main无参数 */

#else
/* 如果使用AC5编译器，定义__FILE结构体与半主机相关设置 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;  /* 文件句柄 */
    /* 可根据需要增加成员，通常只用printf无需其它操作 */
};

#endif

/* 不使用半主机，重定义_ttywrch函数 */
int _ttywrch(int ch)
{
    ch = ch;  /* 避免编译器警告，空实现 */
    return ch; /* 返回ch */
}

/* 定义_sys_exit函数，防止使用半主机 */
void _sys_exit(int x)
{
    x = x;  /* 避免编译器警告，空实现 */
}

/* 定义_sys_command_string函数，兼容性需要，返回NULL */
char *_sys_command_string(char *cmd, int len)
{
    return NULL; /* 不处理命令，返回NULL */
}

/* 定义标准输出文件结构体 */
FILE __stdout;

/* 重定义fputc函数，printf最终通过fputc输出到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */
    USART1->DR = (uint8_t)ch;             /* 将要发送的字符ch写入DR寄存器 */
    return ch;                            /* 返回发送的字符 */
}
/*********************************************结束*************************************/

/* 定义句柄 */
UART_HandleTypeDef huart1;           /* 声明UART1句柄 */
DMA_HandleTypeDef hdma_usart1_rx;    /* 声明USART1接收DMA句柄 */

/* 接收缓冲区 */
#define RX_BUFFER_SIZE 128           /* 定义接收缓冲区大小为128字节 */
uint8_t rxBuffer[RX_BUFFER_SIZE];    /* 实际接收缓冲区 */
volatile uint16_t rxSize = 0;        /* 实际接收到的数据长度 */
volatile uint8_t rxCompleteFlag = 0; /* 接收完成标志 */

/* UART + DMA + 空闲中断初始化函数 */
void UART_IDLE_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0}; /* GPIO初始化结构体 */
    
    /* 使能USART1、GPIOA、DMA1时钟 */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    
    /* 配置USART1 TX引脚PA9为复用推挽输出模式 */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 配置USART1 RX引脚PA10为浮空输入模式 */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 初始化UART参数 */
    huart1.Instance = USART1;                        /* 指定USART1外设 */
    huart1.Init.BaudRate = 115200;                   /* 波特率115200 */
    huart1.Init.WordLength = UART_WORDLENGTH_8B;     /* 8位数据位 */
    huart1.Init.StopBits = UART_STOPBITS_1;          /* 1位停止位 */
    huart1.Init.Parity = UART_PARITY_NONE;           /* 无校验 */
    huart1.Init.Mode = UART_MODE_TX_RX;              /* 发送+接收模式 */
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;     /* 无硬件流控 */
    huart1.Init.OverSampling = UART_OVERSAMPLING_16; /* 16倍过采样 */
    HAL_UART_Init(&huart1);                          /* 初始化UART1 */
    
    /* 配置DMA参数用于USART1 RX */
    hdma_usart1_rx.Instance = DMA1_Channel5;                         /* 指定DMA1通道5 */
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;            /* 外设到内存 */
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;                /* 外设地址不自增 */
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;                    /* 内存地址自增 */
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;   /* 外设字节对齐 */
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;      /* 内存字节对齐 */
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;                         /* 循环模式 */
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_HIGH;                /* 高优先级 */
    HAL_DMA_Init(&hdma_usart1_rx);                                  /* 初始化DMA */
    
    /* 将DMA句柄与UART句柄进行关联 */
    __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);
    
    /* 配置DMA和USART1相关中断的优先级和使能 */
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);  /* 设置DMA1通道5中断优先级 */
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);          /* 使能DMA1通道5中断 */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);         /* 设置USART1中断优先级 */
    HAL_NVIC_EnableIRQ(USART1_IRQn);                 /* 使能USART1中断 */
    
    /* 启动DMA方式接收，准备接收128字节数据到rxBuffer */
    HAL_UART_Receive_DMA(&huart1, rxBuffer, RX_BUFFER_SIZE);
    
    /* 使能UART空闲中断（IDLE），用于变长包分包 */
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    /*
        使用 HAL_UART_Receive_IT 或 HAL_UART_Receive_DMA 等
        HAL库函数时，库函数内部会自动使能对应的接收/发送相关中断，
        因此用户无需再手动调用 __HAL_UART_ENABLE_IT 宏来开启这些中断；
        只有像空闲中断（IDLE）等特殊中断，需要用户显式用宏函数单独使能。
    */
}

/* 发送数据函数 */
void UART_SendData(uint8_t *data, uint16_t size)
{
    HAL_UART_Transmit(&huart1, data, size, HAL_MAX_DELAY); /* 阻塞方式发送数据 */
}

/* UART1中断服务函数 */
void USART1_IRQHandler(void)
{
    /* 判断是否为IDLE中断（空闲线） */
    if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);                       /* 清除IDLE中断标志 */
        HAL_UART_DMAStop(&huart1);                                /* 停止当前DMA接收 */
        rxSize = RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); /* 计算已接收字节数 */
        rxCompleteFlag = 1;                                       /* 标记接收完成 */
        HAL_UART_Receive_DMA(&huart1, rxBuffer, RX_BUFFER_SIZE);   /* 重新启动DMA接收 */
    }
    HAL_UART_IRQHandler(&huart1); /* 处理HAL库内部其他中断事件 */
}

/* DMA1通道5中断服务函数（USART1 RX DMA） */
void DMA1_Channel5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_rx);  /* 调用HAL库DMA RX中断处理函数 */
}

/* uart.h文件 */
#ifndef __UART_H__
#define __UART_H__

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"

/* 定义句柄 */
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;

/* 接收缓冲区 */
#define RX_BUFFER_SIZE 128

extern uint8_t rxBuffer[RX_BUFFER_SIZE];
extern volatile uint16_t rxSize;
extern volatile uint8_t rxCompleteFlag;

void UART_IDLE_Init(void);
void UART_SendData(uint8_t *data, uint16_t size);

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"     /* 系统时钟初始化头文件 */
#include "./SYSTEM/delay/delay.h" /* 延时函数头文件 */
#include "./BSP/UART/uart_idle.h" /* UART空闲中断头文件 */
#include "./BSP/LED/led.h"        /* LED控制头文件 */

int main(void)
{
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);         /* 配置系统时钟为72MHz */
    delay_init(72);                             /* 初始化延时函数 */
    LED_Init();                                 /* 初始化LED硬件 */
    UART_IDLE_Init();                           /* 初始化UART+DMA+空闲中断 */
    
    UART_SendData((uint8_t*)"UART IDLE Interrupt Test\r\n", 26); /* 发送初始化提示 */
    UART_SendData((uint8_t*)"Send any data...\r\n", 18);         /* 发送指令提示 */
    
    while(1)
    {
        if(rxCompleteFlag)  /* 检查是否接收完成 */
        {
            rxBuffer[rxSize] = '\0';                /* 添加字符串结束符，便于打印 */
            UART_SendData((uint8_t*)"Received: ", 10);      /* 回传提示 */
            UART_SendData(rxBuffer, rxSize);                /* 回传实际接收到的数据 */
            UART_SendData((uint8_t*)"\r\n", 2);             /* 换行 */
            rxCompleteFlag = 0;                     /* 清除接收完成标志 */
            LED_Toggle(GPIOC, GPIO_PIN_13);         /* 翻转LED指示灯 */
        }
        delay_ms(10);   /* 主循环延时，减小CPU占用 */
    }
}
```

## 5. USART同步通信示例（SPI模式）（*）

以下代码展示了如何配置USART工作在同步模式，用于SPI类型通信：

```c
#include "./BSP/UART/usart_sync.h"

/* 定义句柄 */
USART_HandleTypeDef husart1;

void USART_Sync_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能时钟 */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置GPIO引脚 */
    /* USART1 TX - PA9 */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* USART1 RX - PA10 */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* USART1 CK - PA8 */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* 配置USART同步模式参数 */
    husart1.Instance = USART1;
    husart1.Init.BaudRate = 1000000;               /* 波特率(时钟频率) */
    husart1.Init.WordLength = USART_WORDLENGTH_8B;
    husart1.Init.StopBits = USART_STOPBITS_1;
    husart1.Init.Parity = USART_PARITY_NONE;
    husart1.Init.Mode = USART_MODE_TX_RX;
    husart1.Init.CLKPolarity = USART_POLARITY_LOW; /* 时钟极性 */
    husart1.Init.CLKPhase = USART_PHASE_1EDGE;     /* 时钟相位 */
    husart1.Init.CLKLastBit = USART_LASTBIT_ENABLE; /* 最后一位时钟脉冲 */
    
    /* 初始化USART */
    HAL_USART_Init(&husart1);
}

/* 发送并接收一个字节 */
uint8_t USART_TransmitReceive(uint8_t data)
{
    uint8_t rxData;
    HAL_USART_TransmitReceive(&husart1, &data, &rxData, 1, HAL_MAX_DELAY);
    return rxData;
}

/* 发送并接收多个字节 */
void USART_TransmitReceiveBuffer(uint8_t *txData, uint8_t *rxData, uint16_t size)
{
    HAL_USART_TransmitReceive(&husart1, txData, rxData, size, HAL_MAX_DELAY);
}

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/UART/usart_sync.h"
#include "./BSP/LED/led.h"

int main(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    usart_init(115200);    /* 用于打印调试信息 */
    LED_Init();
    USART_Sync_Init();
    
    printf("USART Synchronous Mode Test\r\n");
    
    uint8_t txBuffer[4] = {0xA1, 0xB2, 0xC3, 0xD4};
    uint8_t rxBuffer[4] = {0};
    
    while(1)
    {
        /* 发送并接收数据 */
        USART_TransmitReceiveBuffer(txBuffer, rxBuffer, 4);
        
        /* 打印接收到的数据 */
        printf("Received: ");
        for(int i = 0; i < 4; i++)
        {
            printf("0x%02X ", rxBuffer[i]);
        }
        printf("\r\n");
        
        /* LED指示通信完成 */
        LED_Toggle(GPIOC, GPIO_PIN_13);
        
        delay_ms(1000);
    }
}
```

# STM32 HAL库I2C使用示例

本文档提供了使用STM32 HAL库进行I2C通信的代码示例，包括主机发送/接收、从机模式和访问I2C外设。

## 1. I2C软件模拟读取MPU6050

```c
/* i2c.c文件 */
#include "./BSP/I2C/i2c.h"

static void SCL_High(void)
{
    HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_SET);
    delay_us(10);
}

static void SCL_Low(void)
{
    HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_RESET);
    delay_us(10);
}
static void SDA_High(void)
{
    HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_SET);
    delay_us(10);
}

static void SDA_Low(void)
{
    HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_RESET);
    delay_us(10);
}

static uint8_t SDA_Read(void)
{
    return HAL_GPIO_ReadPin(SDA_PORT, SDA_PIN);
}

void I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pin = SCL_PIN | SDA_PIN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, SCL_PIN | SDA_PIN, GPIO_PIN_SET);
}

void I2C_Start()
{
    SCL_Low();  /* 兼容指定地址读，此时起始时序时SDA可能为0，*/
    SDA_High(); /* 因此需要再驱动时钟一次，将SDA置位 */
    SCL_High();
    SDA_Low();
}

void I2C_Stop()
{
    SCL_Low();
    SDA_Low();  /* 与start时序类似0 */
    SCL_High();
    SDA_High();
}

void I2C_SendByte(uint8_t byte)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        SCL_Low();
        HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, (GPIO_PinState) (byte & (0x80 >> i)));
        SCL_High();
    }
}

uint8_t I2C_ReceiveByte()
{
    uint8_t byte = 0;
    SCL_Low();
    SDA_High();     /* 主机放开SDA控制权,从机放数据 */
    for(uint8_t i = 0; i < 8; i++)
    {
        SCL_High();
        if(SDA_Read() == GPIO_PIN_SET)
        {
            byte |= (0x80 >> i);
        }
        SCL_Low();   
    } 
    return byte;
}

void I2C_SendAck(uint8_t ack)
{
    SCL_Low();
    if(ack == 1){
        SDA_High();
    }
    else{
        SDA_Low();
    }
    SCL_High();
}

uint8_t I2C_ReceiverAck(void)
{ 
    SCL_Low();
    SDA_High();  /* 主机放手 */
    SCL_High();
    return SDA_Read();  /* 主机读从机应答 */
}

/*i2c.h文件*/
#ifndef __I2C_H__
#define __I2C_H__

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"

#define SCL_PORT GPIOB
#define SCL_PIN  GPIO_PIN_10
#define SDA_PORT GPIOB
#define SDA_PIN  GPIO_PIN_11

void I2C_Init(void);

void I2C_Start();
void I2C_Stop();
void I2C_SendByte(uint8_t byte);
uint8_t I2C_ReceiveByte();
void I2C_SendAck(uint8_t ack);
uint8_t I2C_ReceiverAck(void);

#endif

/* mpu6050.c文件 */
#include "./MPU6050/mpu6050.h"

void MPU6050_Init(void)
{
    I2C_Init(); 
    /*MPU6050寄存器初始化，需要对照MPU6050手册的寄存器描述配置，此处仅配置了部分重要的寄存器*/
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		//电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);		//电源管理寄存器2，保持默认值0，所有轴均不待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);		//采样率分频寄存器，配置采样率
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);			//配置寄存器，配置DLPF
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);	//陀螺仪配置寄存器，选择满量程为±2000°/s
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);	//加速度计配置寄存器，选择满量程为±16g
}

void MPU6050_WriteReg(uint8_t reg, uint8_t data)
{
    I2C_Start();
    I2C_SendByte(MPU6050_ADDRESS);
    I2C_ReceiverAck();
    I2C_SendByte(reg);
    I2C_ReceiverAck();
    I2C_SendByte(data);
    I2C_ReceiverAck();
    I2C_Stop();
}

uint8_t MPU6050_ReadReg(uint8_t reg)
{
    uint8_t data = 0;

    I2C_Start();
    I2C_SendByte(MPU6050_ADDRESS);
    I2C_ReceiverAck();
    I2C_SendByte(reg);
    I2C_ReceiverAck();

    I2C_Start();
    I2C_SendByte(MPU6050_ADDRESS | 0x01);
    I2C_ReceiverAck();
    data = I2C_ReceiveByte();
    I2C_SendAck(1);          /* 主机不应答，代表接收结束*/
    I2C_Stop();
    return data;
}

void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;								//定义数据高8位和低8位的变量
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);		//读取加速度计X轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);		//读取加速度计X轴的低8位数据
	*AccX = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);		//读取加速度计Y轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);		//读取加速度计Y轴的低8位数据
	*AccY = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);		//读取加速度计Z轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);		//读取加速度计Z轴的低8位数据
	*AccZ = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);		//读取陀螺仪X轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);		//读取陀螺仪X轴的低8位数据
	*GyroX = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);		//读取陀螺仪Y轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);		//读取陀螺仪Y轴的低8位数据
	*GyroY = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);		//读取陀螺仪Z轴的高8位数据
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);		//读取陀螺仪Z轴的低8位数据
	*GyroZ = (DataH << 8) | DataL;						//数据拼接，通过输出参数返回
}

/* mpu6050.h文件 */
#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "./MPU6050/MPU6050_Reg.h"
#include "./BSP/I2C/i2c.h"
#include "./SYSTEM/usart/usart.h"

void MPU6050_Init(void);
void MPU6050_WriteReg(uint8_t reg, uint8_t data);
uint8_t MPU6050_ReadReg(uint8_t reg);
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
					int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);
#endif

/* mpu6050_REG.h文件 */
#ifndef __MPU6050_REG_H
#define __MPU6050_REG_H

#define MPU6050_ADDRESS         0xD0

#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./mpu6050/mpu6050.h"

// 读取16位数据的函数

int16_t AX, AY, AZ, GX, GY, GZ;			//定义用于存放各个数据的变量
int main(void)
{   
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    UART_Init();
    MPU6050_Init();

    printf("MPU6050 ID: 0x%x\r\n", MPU6050_ReadReg(MPU6050_WHO_AM_I));
    
    while(1)
    {   
        MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);	//获取MPU6050的数据
        printf("AccX: %d\tAccY: %d\tAccZ: %d\r\n", AX, AY, AZ);
        printf("GyroX: %d\tGyroY: %d\tGyroZ: %d\r\n", GX, GY, GZ);
        printf("\r\n");
        delay_ms(1000); 
    }
}
```

## 2. I2C硬件模拟读取MPU6050

```c
/* i2c.c文件 */
#include "./BSP/I2C/i2c.h"

I2C_HandleTypeDef hi2c2;
void I2C_Init(void)
{ 
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_I2C2_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    hi2c2.Instance = I2C2;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;  /* 7位地址模式 */
    hi2c2.Init.ClockSpeed = 100000;                       /* 通信速率，最大400k */
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE; /* 单地址模式 */
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;               /* SCL高低电平持续时间之比  */
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE; /* 禁用广播模式 */
    /* I2C主设备发送通用呼叫地址（0x00）时，I2C 总线上的所有从设备都会响应这个地址，
        这个模式通常用于对总线上的所有从设备进行广播消息或重置操作。*/
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_ENABLE;     /* 允许时钟拉伸 */
    /* 允许I2C从设备在数据传输过程中暂停（“拉伸”）时钟信号。当从设备忙于处理
       数据或等待其他操作时，它可以通过拉低时钟线来延迟主设备的时钟信号，以便有更
       多的时间来处理请求。主设备会在时钟信号被恢复后继续通信。 */
    hi2c2.Init.OwnAddress1 = 0;     /* 自身地址1 */
    hi2c2.Init.OwnAddress2 = 0;     /* 自身地址2 */
    HAL_I2C_Init(&hi2c2);

    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* 向从设备发送一个字节数据 */
void I2C_SendByte(uint16_t adr, uint8_t data)
{
    HAL_I2C_Master_Transmit(&hi2c2, adr, &data, 1, 1000);
}

/* 向从设备发送多个字节数据 */
void I2C_SendBytes(uint16_t adr, uint8_t *sendBuffre, uint8_t len)
{
    HAL_I2C_Master_Transmit(&hi2c2, adr, sendBuffre, len, 1000);
}

/* 从设备接收一个字节数据 */
void I2C_RecieveByte(uint16_t adr, uint8_t *recieveBuffer)
{
    HAL_I2C_Master_Receive(&hi2c2, adr, recieveBuffer, 1, 1000);
}

/* i2c.h文件 */
#ifndef __I2C_H__
#define __I2C_H__

#include "./SYSTEM/sys/sys.h"
#include "./MPU6050/MPU6050_Reg.h"

extern I2C_HandleTypeDef hi2c2;

void I2C_Init(void);
void I2C_SendByte(uint16_t adr, uint8_t data);
void I2C_SendBytes(uint16_t adr, uint8_t *sendBuffre, uint8_t len);
void I2C_RecieveByte(uint16_t adr, uint8_t *recieveBuffer);

#endif

/* mpu6050.c文件 */
#include "./MPU6050/mpu6050.h" 

/* 	通过I2C发送数据，这里需要将寄存器地址和要写入的数据拼接在一起，  
	因为I2C协议是基于字节传输的。首先需要发送要写入的从设备的寄存器地址，再发送数据。
	在I2C通信中，设备地址是固定的（`MPU6050_ADDRESS`），而从设备寄存器地址和数据是连续
	发送的。因此，我们需要将寄存器地址和数据打包成一个缓冲区，这样才能按顺序发送。
	
	另外，`MPU6050_ADDRESS & 0xFE` 将设备地址的最低位（即读/写位）设置为0，  
	确保进行写操作。`0xFE` 是一个掩码，其作用是清除地址的最低位，将其设置为0。
	这确保了I2C通信中，传输的是设备的写操作地址（而不是读操作地址）。	*/

/* 指定地址向MPU6050写入寄存器数据 */
void MPU6050_WriteReg(uint8_t reg, uint8_t data)
{
    uint8_t buffer[2] = {reg, data};  /* 创建一个包含寄存器地址和数据的缓冲区 */
    I2C_SendBytes(MPU6050_ADDRESS & 0xFE, buffer, 2);  /* 通过I2C发送数据，传输寄存器地址和数据 */
}

uint8_t MPU6050_ReadReg(uint8_t reg)
{
    uint8_t data;  /* 存储读取到的寄存器数据 */   
    /* 
        在I2C通信中，读取操作与写操作有些不同。
        在读操作中，设备的第一个字节数据并不是目标寄存器地址，而是从设备的当前“地址指针”读取数据。
        因此，如果我们要读取特定寄存器的数据，首先必须通过写操作指定寄存器地址。 
        这意味着，写操作的第一个字节（即`reg`）用于设置设备内部的“地址指针”指向目标寄存器。
        然后，通过执行读操作时，设备会从该地址指针位置读取数据并返回。
    */
    I2C_SendByte(MPU6050_ADDRESS, reg);  /* 通过I2C发送寄存器地址（写操作），指定要读取的寄存器 */  
    /* 
        通过I2C读取数据时，设备已经知道目标寄存器的地址指针，并会返回该寄存器的数据。
        由于之前已通过写操作指定了寄存器地址，现在设备会直接从该地址读取数据。
        这里使用 `MPU6050_ADDRESS | 0x01` 发送读取请求，并接收从该寄存器读取的数据。
    */
    I2C_RecieveByte(MPU6050_ADDRESS | 0x01, &data);  /* 发送读取请求并通过I2C接收数据 */ 
    return data;  /* 返回读取到的数据 */
}


/* MPU6050初始化函数 */
void MPU6050_Init(void)
{
    I2C_Init();  /* 初始化I2C通信 */
    /* MPU6050寄存器初始化，配置部分重要寄存器 */
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);  /* 电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪 */
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);  /* 电源管理寄存器2，保持默认值0，所有轴均不待机 */
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);  /* 采样率分频寄存器，配置采样率 */
    MPU6050_WriteReg(MPU6050_CONFIG, 0x06);  	 /* 配置寄存器，配置DLPF（数字低通滤波器） */
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18); /* 陀螺仪配置寄存器，选择满量程为±2000°/s */
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);/* 加速度计配置寄存器，选择满量程为±16g */
}

/* 获取加速度计和陀螺仪数据 */
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
                     int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
    uint8_t DataH, DataL;  /* 定义数据高8位和低8位的变量 */
    
    DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);  /* 读取加速度计X轴的高8位数据 */
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);  /* 读取加速度计X轴的低8位数据 */
    *AccX = (DataH << 8) | DataL;					/* 数据拼接，得到16位加速度计X轴的数据 */

    DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);  /* 读取加速度计Y轴的高8位数据 */
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);  /* 读取加速度计Y轴的低8位数据 */
    *AccY = (DataH << 8) | DataL;					/* 数据拼接，得到16位加速度计Y轴的数据 */

    DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);  /* 读取加速度计Z轴的高8位数据 */
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);  /* 读取加速度计Z轴的低8位数据 */
    *AccZ = (DataH << 8) | DataL;  					/* 数据拼接，得到16位加速度计Z轴的数据 */

    DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);  /* 读取陀螺仪X轴的高8位数据 */
    DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);  /* 读取陀螺仪X轴的低8位数据 */
    *GyroX = (DataH << 8) | DataL;  			   /* 数据拼接，得到16位陀螺仪X轴的数据 */

    DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);  /* 读取陀螺仪Y轴的高8位数据 */
    DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);  /* 读取陀螺仪Y轴的低8位数据 */
    *GyroY = (DataH << 8) | DataL;  			   /* 数据拼接，得到16位陀螺仪Y轴的数据 */

    DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);  /* 读取陀螺仪Z轴的高8位数据 */
    DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);  /* 读取陀螺仪Z轴的低8位数据 */
    *GyroZ = (DataH << 8) | DataL;				   /* 数据拼接，得到16位陀螺仪Z轴的数据 */
}

/* mpu6050.h文件 */
#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "./MPU6050/MPU6050_Reg.h"
#include "./BSP/I2C/i2c.h"
#include "./SYSTEM/usart/usart.h"

void MPU6050_Init(void);
void MPU6050_WriteReg(uint8_t reg, uint8_t data);
uint8_t MPU6050_ReadReg(uint8_t reg);
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
					int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);

#endif

/* mpu6050_Reg.h文件 */
#ifndef __MPU6050_REG_H
#define __MPU6050_REG_H

#define MPU6050_ADDRESS         0xD0

#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./mpu6050/mpu6050.h"

int16_t AX, AY, AZ, GX, GY, GZ;			//定义用于存放各个数据的变量
int main(void)
{   
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);   
    delay_init(72); 
    UART_Init();
    MPU6050_Init();
    printf("MPU6050 ID: 0x%x\r\n", MPU6050_ReadReg(MPU6050_WHO_AM_I));   
    while(1)
    {   
        MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);	//获取MPU6050的数据
        printf("AccX: %d\tAccY: %d\tAccZ: %d\r\n", AX, AY, AZ);
        printf("GyroX: %d\tGyroY: %d\tGyroZ: %d\r\n", GX, GY, GZ);
        printf("\r\n");
        delay_ms(1000); 
    }
}
```

# STM32 HAL库SPI使用示例

## 1. SPI软件模拟读取W25Q64

```c
/* spi.c文件 */
#include "spi.h"

/* SPI引脚控制函数 */
static void cs_low() { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); }   /* 片选信号拉低 */
static void cs_high() { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); }   /* 片选信号拉高 */
static void scl_low() { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); }  /* 时钟信号拉低 */
static void scl_high() { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); }   /* 时钟信号拉高 */
static void sda_low() { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); }  /* MOSI信号拉低 */
static void sda_high() { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET); }   /* MOSI信号拉高 */
static uint8_t sda_read() { return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6); }      /* 读取MISO信号 */

void SPI_Init()
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* PA4片选，PA5时钟，PA7输出(MOSI)，PA6输入(MISO) */
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 设置初始空闲状态 */
    cs_low();
    scl_low();
}

/*
 * SPI通信开始函数
 * 功能：拉低片选信号，开始SPI通信
 */
void SPI_Start()
{
    cs_low();  /* 拉低片选信号，选中从设备 */
}

/*
 * SPI通信结束函数
 * 功能：拉高片选信号，结束SPI通信
 */
void SPI_Stop()
{
    cs_high();  /* 拉高片选信号，释放从设备 */
} 

/*
 * SPI数据交换函数
 * 功能：通过SPI接口发送和接收一个字节的数据
 * 参数：transmitData - 要发送的字节数据
 * 返回值：接收到的字节数据
 * 说明：使用软件模拟SPI时序，MSB先行
 */
uint8_t SPI_Swap(uint8_t transmitData)
{
    uint8_t receiveData = 0;
    
    /* 逐位传输8位数据 */
    for (uint8_t i = 0; i < 8; i++) 
    {
        /* 时钟低电平时准备数据 */
        scl_low();
        /* 根据发送数据的当前位设置MOSI电平 */
        (transmitData & (0x80 >> i)) ? sda_high() : sda_low();
        
        /* 时钟高电平时读取数据 */
        scl_high();
        /* 读取MISO线上的数据并保存 */
        receiveData = (receiveData << 1) | sda_read();
    }
    return receiveData;  /* 返回接收到的数据 */
}

/* spi.h文件 */
#ifndef __SPI_H__
#define __SPI_H__

#include "./SYSTEM/sys/sys.h"

void SPI_Init();
void SPI_Start();
void SPI_Stop();
uint8_t SPI_Swap(uint8_t transmitData);

#endif

/* w25q64.c文件 */
#include "w25q64.h"

/*
 * W25Q64写使能函数
 * 功能：发送写使能指令，允许后续写操作
 */
static void W25Q64_Write_Enable(void)
{
    SPI_Start();
    SPI_Swap(W25Q64_WRITE_ENABLE);  /* 发送写使能指令0x06 */
    SPI_Stop();
}

/*
 * W25Q64等待忙状态结束
 * 功能：轮询状态寄存器，等待芯片操作完成
 */
static void W25Q64_Wait_Busy(void)
{
    SPI_Start();
    SPI_Swap(W25Q64_READ_STATUS_REGISTER_1);  /* 发送读状态寄存器指令 */
    /* 检查BUSY位(bit0)，为1表示忙，等待直到为0 */
    while ((SPI_Swap(W25Q64_DUMMY_BYTE) & 0x01) == 0x01);
    SPI_Stop();
}

/*
 * W25Q64初始化函数
 * 功能：初始化SPI接口
 */
void W25Q64_Init(void)
{
  SPI_Init();
}

/*
 * W25Q64读取ID函数
 * 功能：读取制造商ID和设备ID
 * 参数：MID - 制造商ID指针，DID - 设备ID指针
 */
void W25Q64_Read_ID(uint8_t *MID, uint16_t *DID)
{
    SPI_Start();
    SPI_Swap(W25Q64_JEDEC_ID);  /* 发送读JEDEC ID指令0x9F */
    *MID = SPI_Swap(W25Q64_DUMMY_BYTE);  /* 读取制造商ID */
    uint8_t DIDH = SPI_Swap(W25Q64_DUMMY_BYTE);  /* 读取设备ID高字节 */
    uint8_t DIDL = SPI_Swap(W25Q64_DUMMY_BYTE);  /* 读取设备ID低字节 */
    *DID = (DIDH << 8) | DIDL;  /* 组合成16位设备ID */
    SPI_Stop();
}

/*
 * W25Q64写数据函数
 * 功能：向指定地址写入数据
 * 参数：Adr - 起始地址，data - 数据缓冲区指针，len - 数据长度
 */
void W25Q64_Write(uint32_t Adr, uint8_t *data, uint16_t len)
{
    /* 这三行代码顺序不能错，擦除扇区是一个完整时序，结束之后等待就绪，就绪之后再写使能 */
    W25Q64_Sector_Erase(Adr);   /* 擦除目标扇区 */
    W25Q64_Wait_Busy();         /* 等待芯片就绪 */
    W25Q64_Write_Enable();      /* 发送写使能 */
    
    SPI_Start();
    SPI_Swap(W25Q64_PAGE_PROGRAM);  /* 发送页编程指令0x02 */
    SPI_Swap(Adr >> 16);  /* 发送地址高字节 */
    SPI_Swap(Adr >> 8);   /* 发送地址中字节 */
    SPI_Swap(Adr);        /* 发送地址低字节 */
    
    /* 写入数据 */
    for(uint16_t i = 0; i < len; i++)
    {
        SPI_Swap(*(data + i));
    }
    SPI_Stop();
}

/*
 * W25Q64读数据函数
 * 功能：从指定地址读取数据
 * 参数：Adr - 起始地址，buffer - 数据缓冲区指针，len - 数据长度
 */
void W25Q64_Read(uint32_t Adr, uint8_t *buffer, uint16_t len)
{
    W25Q64_Wait_Busy();  /* 等待芯片就绪 */
    
    SPI_Start();
    SPI_Swap(W25Q64_READ_DATA);  /* 发送读数据指令0x03 */
    SPI_Swap(Adr >> 16);  /* 发送地址高字节 */
    SPI_Swap(Adr >> 8);   /* 发送地址中字节 */
    SPI_Swap(Adr);        /* 发送地址低字节 */
    
    /* 读取数据 */
    for(uint16_t i = 0; i < len; i++)
    {
        *(buffer + i) = SPI_Swap(W25Q64_DUMMY_BYTE);
    }
    SPI_Stop();
}

/*
 * W25Q64扇区擦除函数
 * 功能：擦除指定地址所在的4KB扇区
 * 参数：Adr - 扇区内任意地址
 */
void W25Q64_Sector_Erase(uint32_t Adr)
{
    W25Q64_Wait_Busy();  /* 等待芯片就绪 */
    W25Q64_Write_Enable();  /* 发送写使能 */
    
    SPI_Start();
    SPI_Swap(W25Q64_SECTOR_ERASE_4KB);  /* 发送扇区擦除指令0x20 */
    SPI_Swap(Adr >> 16);  /* 发送地址高字节 */
    SPI_Swap(Adr >> 8);   /* 发送地址中字节 */
    SPI_Swap(Adr);        /* 发送地址低字节 */
    SPI_Stop();
}

/* w25q64.h文件 */
#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "./BSP/SPI/spi.h"
#include  "w25q64_Ins.h"

void W25Q64_Init(void);
void W25Q64_Read_ID(uint8_t *MID, uint16_t *DID);
void W25Q64_Write(uint32_t Adr, uint8_t *data, uint16_t len);
void W25Q64_Read(uint32_t Adr, uint8_t *data, uint16_t len);
void W25Q64_Sector_Erase(uint32_t Adr);

#endif

/* w25q64_Ins.h文件 */
#ifndef __W25Q64_INS_H
#define __W25Q64_INS_H

/* 写操作相关命令 */
#define W25Q64_WRITE_ENABLE							0x06	/* 写使能命令，允许后续执行擦除和编程操作 */
#define W25Q64_WRITE_DISABLE						0x04	/* 写禁止命令，禁止擦除和编程操作 */
#define W25Q64_WRITE_STATUS_REGISTER				0x01	/* 写入状态寄存器 */

/* 编程操作 */
#define W25Q64_PAGE_PROGRAM							0x02	/* 页编程命令，将数据写入指定页 */
#define W25Q64_QUAD_PAGE_PROGRAM					0x32	/* 四线页编程命令，使用4线模式快速写入数据 */

/* 擦除操作 */
#define W25Q64_SECTOR_ERASE_4KB						0x20	/* 4KB扇区擦除 */
#define W25Q64_BLOCK_ERASE_32KB						0x52	/* 32KB块擦除 */
#define W25Q64_BLOCK_ERASE_64KB						0xD8	/* 64KB块擦除 */
#define W25Q64_CHIP_ERASE							0xC7	/* 整片擦除 */

/* 状态寄存器操作 */
#define W25Q64_READ_STATUS_REGISTER_1				0x05	/* 读取状态寄存器1 */
#define W25Q64_READ_STATUS_REGISTER_2				0x35	/* 读取状态寄存器2 */

/* 电源管理 */
#define W25Q64_POWER_DOWN							0xB9	/* 掉电模式 */
#define W25Q64_RELEASE_POWER_DOWN_HPM_DEVICE_ID		0xAB	/* 退出掉电模式并读取设备ID */
#define W25Q64_HIGH_PERFORMANCE_MODE				0xA3	/* 高性能模式 */

/* 读取操作 */
#define W25Q64_READ_DATA							0x03	/* 普通读取数据 */
#define W25Q64_FAST_READ							0x0B	/* 快速读取 */
#define W25Q64_FAST_READ_DUAL_OUTPUT				0x3B	/* 双线输出快速读取 */
#define W25Q64_FAST_READ_DUAL_IO					0xBB	/* 双线I/O快速读取 */
#define W25Q64_FAST_READ_QUAD_OUTPUT				0x6B	/* 四线输出快速读取 */
#define W25Q64_FAST_READ_QUAD_IO					0xEB	/* 四线I/O快速读取 */
#define W25Q64_OCTAL_WORD_READ_QUAD_IO				0xE3	/* 八字四线I/O读取 */

/* 设备信息读取 */
#define W25Q64_MANUFACTURER_DEVICE_ID				0x90	/* 读取制造商和设备ID */
#define W25Q64_JEDEC_ID								0x9F	/* 读取JEDEC ID */
#define W25Q64_READ_UNIQUE_ID						0x4B	/* 读取唯一ID */

/* 其他操作 */
#define W25Q64_ERASE_SUSPEND						0x75	/* 暂停擦除操作 */
#define W25Q64_ERASE_RESUME							0x7A	/* 恢复擦除操作 */
#define W25Q64_CONTINUOUS_READ_MODE_RESET			0xFF	/* 重置连续读取模式 */

/* 虚拟字节 */
#define W25Q64_DUMMY_BYTE							0xFF	/* 虚拟字节，用于占位或延时 */

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./W25Q64/w25q64.h"

uint8_t MID;    /* 制造商ID存储变量 */
uint16_t DID;    /* 设备ID存储变量 */

/*
 * 主函数
 * 功能：测试W25Q64闪存芯片的读写功能
 * 1. 初始化系统时钟和外设
 * 2. 读取并显示W25Q64的ID信息
 * 3. 循环写入测试数据并读取验证
 */
int main(void)
{   
    /* 系统初始化 */
    HAL_Init();  /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);  /* 设置系统时钟为72MHz */
    delay_init(72);  /* 初始化延时函数 */
    UART_Init();  /* 初始化串口 */
    
    /* W25Q64初始化 */
    W25Q64_Init();
    
    /* 读取并显示芯片ID */
    W25Q64_Read_ID(&MID, &DID);
    printf("MID = %x\r\n",MID);  /* 显示制造商ID */
    printf("DID = %x\r\n",DID);  /* 显示设备ID */

    /* 测试数据缓冲区 */
    uint8_t buffer[4] = {0};  /* 读取缓冲区 */
    uint8_t data[4] = {0x10, 0x22, 0x3C, 0x99};  /* 测试数据 */
    
    /* 主循环：持续测试读写功能 */
    while(1)
    {   
        /* 向地址0x000000写入4字节测试数据 */
        W25Q64_Write(0x000000, data, 4);
        
        /* 从地址0x000000读取4字节数据 */
        W25Q64_Read(0x000000, buffer, 4);
        
        /* 显示读取到的数据 */
        for(uint8_t i = 0; i < 4; i++)
        {
            printf("buffer[%d] = %x\r\n",i,buffer[i]);
        }
        
        delay_ms(1000);  /* 延时1秒 */
    }
}
```

## 2. SPI硬件外设读取W25Q64

spi.c文件只写最基本的时序单元和收发函数，w25q64.c中具体实现时序拼接。

1. SPI层（底层）：

- 只实现最基本的SPI时序操作
- 提供引脚控制函数（CS控制）
- 提供基本的数据传输函数（发送/接收/交换单字节）
- 不涉及具体器件的操作逻辑

1. W25Q64层（应用层）：

- 基于SPI层的基本操作构建具体的Flash操作时序
- 实现所有W25Q64的指令集操作
- 处理地址计算、跨页写入等高级功能
- 提供友好的用户接口

```c
/* spi.c文件 */
#include "spi.h"

/* CS引脚控制函数 */
static void cs_low() { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); }   
static void cs_high() { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); }  

SPI_HandleTypeDef hspi1;  /* SPI句柄定义 */

void SPI_Init()
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();  /* 启用GPIOA和SPI1时钟 */
    
    /* PA4片选，PA5时钟，PA7输出(MOSI)，PA6输入(MISO) */
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    hspi1.Instance = SPI1;                                      /* 设置SPI实例 */
    hspi1.Init.Mode = SPI_MODE_MASTER;                          /* 设置为主模式 */
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;                /* 双线全双工 */
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;                    /* 8位数据 */
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;                  /* 时钟极性低 */
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;                      /* 时钟相位在第一个边沿 */
    hspi1.Init.NSS = SPI_NSS_SOFT;                              /* 软件管理片选线 */
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;   /* 波特率预分频 */
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;                     /* MSB(高位)先行 */
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;                     /* 禁用TI模式 */
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;     /* 禁用CRC计算 */
    hspi1.Init.CRCPolynomial = 10;                              /* CRC多项式 */
    HAL_SPI_Init(&hspi1);                                       /* 初始化SPI */

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7; /* 输出模式下仍可正常输入 */
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 初始片选信号为高电平 */
    cs_high();  
}

void SPI_Start()
{
    cs_low();   /* 拉低片选信号，选中从设备 */
}

void SPI_Stop()
{
    cs_high();  /* 拉高片选信号，释放从设备 */
} 

void SPI_Transmit(uint8_t transmitData)
{
    HAL_SPI_Transmit(&hspi1, &transmitData, 1, 1000);  /* 发送数据 */
}

uint8_t SPI_Receive()
{
    uint8_t receiveData = 0;
    HAL_SPI_Receive(&hspi1, &receiveData, 1, 1000);
    return receiveData;
}

uint8_t SPI_Swap(uint8_t transmitData)
{
    uint8_t receiveData = 0;
    HAL_SPI_TransmitReceive(&hspi1, &transmitData, &receiveData, 1, 1000);
    return receiveData;
}

/* spi.h文件 */
#ifndef __SPI_H__
#define __SPI_H__

#include "./SYSTEM/sys/sys.h"

void SPI_Init();
void SPI_Start();
void SPI_Stop();
void SPI_Transmit(uint8_t transmitData);
uint8_t SPI_Receive();
uint8_t SPI_Swap(uint8_t transmitData);

#endif

/* w25q64.c文件 */
#include "w25q64.h"

/* W25Q64写使能函数
 * 功能：发送写使能指令，允许后续写操作 */
static void W25Q64_Write_Enable(void)
{
    SPI_Start();  /* 开始SPI通信 */
    SPI_Transmit(W25Q64_WRITE_ENABLE);
    SPI_Stop();  /* 结束SPI通信 */
}

/* W25Q64等待忙状态结束
 * 功能：轮询状态寄存器，等待芯片操作完成 */
static void W25Q64_Wait_Busy(void)
{
    SPI_Start();  /* 开始SPI通信 */
    SPI_Transmit(W25Q64_READ_STATUS_REGISTER_1);  /* 发送读状态寄存器指令 */
    /* 检查BUSY位(bit0)，为1表示忙，等待直到为0 */
    while ((SPI_Receive() & 0x01) == 0x01);
    SPI_Stop();  /* 结束SPI通信 */
}

void W25Q64_Init(void)
{
    SPI_Init();
}

/* W25Q64读取ID函数
 * 功能：读取制造商ID和设备ID
 * 参数：MID - 制造商ID指针，DID - 设备ID指针 */
void W25Q64_Read_ID(uint8_t *MID, uint16_t *DID)
{
    SPI_Start();                        /* 开始SPI通信 */
    SPI_Transmit(W25Q64_JEDEC_ID);      /* 发送读JEDEC ID指令0x9F */
    *MID = SPI_Receive();               /* 读取制造商ID */
    uint8_t DIDH = SPI_Receive();       /* 读取设备ID高字节 */
    uint8_t DIDL = SPI_Receive();       /* 读取设备ID低字节 */
    *DID = (DIDH << 8) | DIDL;          /* 组合成16位设备ID */
    SPI_Stop();                         /* 结束SPI通信 */
}

/* W25Q64写数据函数
 * 功能：向指定地址写入数据
 * 参数：Adr - 起始地址，data - 数据缓冲区指针，len - 数据长度 */
void W25Q64_Write(uint32_t Adr, uint8_t *data, uint16_t len)
{
    /* 这三行代码顺序不能错，擦除扇区是一个完整时序，结束之后等待就绪，就绪之后再写使能 */
    W25Q64_Sector_Erase(Adr);  /* 擦除目标扇区 */
    W25Q64_Wait_Busy();  /* 等待芯片就绪 */
    W25Q64_Write_Enable();  /* 发送写使能 */

    SPI_Start();  /* 开始SPI通信 */
    SPI_Transmit(W25Q64_PAGE_PROGRAM);  /* 发送页编程指令0x02 */
    SPI_Transmit(Adr >> 16);  /* 发送地址高字节 */
    SPI_Transmit(Adr >> 8);   /* 发送地址中字节 */
    SPI_Transmit(Adr);        /* 发送地址低字节 */
    
    /* 写入数据 */
    for(uint16_t i = 0; i < len; i++)
    {
        SPI_Transmit(*(data + i));
    }
    SPI_Stop();  /* 结束SPI通信 */
}

/* W25Q64读数据函数
 * 功能：从指定地址读取数据
 * 参数：Adr - 起始地址，buffer - 数据缓冲区指针，len - 数据长度 */
void W25Q64_Read(uint32_t Adr, uint8_t *buffer, uint16_t len)
{
    W25Q64_Wait_Busy();  /* 等待芯片就绪 */

    SPI_Start();                /* 开始SPI通信 */
    SPI_Transmit(W25Q64_READ_DATA);  /* 发送读数据指令0x03 */
    SPI_Transmit(Adr >> 16);  /* 发送地址高字节 */
    SPI_Transmit(Adr >> 8);   /* 发送地址中字节 */
    SPI_Transmit(Adr);        /* 发送地址低字节 */
    
    /* 读取数据 */
    for(uint16_t i = 0; i < len; i++)
    {
        *(buffer + i) = SPI_Receive();
    }
    SPI_Stop();  /* 结束SPI通信 */
}

/* W25Q64扇区擦除函数
 * 功能：擦除指定地址所在的4KB扇区
 * 参数：Adr - 扇区内任意地址 */
void W25Q64_Sector_Erase(uint32_t Adr)
{
    W25Q64_Wait_Busy();  /* 等待芯片就绪 */

    SPI_Start();  /* 开始SPI通信 */
    W25Q64_Write_Enable();  /* 发送写使能 */  
    SPI_Transmit(W25Q64_SECTOR_ERASE_4KB);  /* 发送扇区擦除指令0x20 */
    SPI_Transmit(Adr >> 16);  /* 发送地址高字节 */
    SPI_Transmit(Adr >> 8);   /* 发送地址中字节 */
    SPI_Transmit(Adr);        /* 发送地址低字节 */
    SPI_Stop();  /* 结束SPI通信 */
}

/* w25q64.h文件 */
#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "./BSP/SPI/spi.h"
#include  "w25q64_Ins.h"

void W25Q64_Init(void);
void W25Q64_Read_ID(uint8_t *MID, uint16_t *DID);
void W25Q64_Write(uint32_t Adr, uint8_t *data, uint16_t len);
void W25Q64_Read(uint32_t Adr, uint8_t *data, uint16_t len);
void W25Q64_Sector_Erase(uint32_t Adr);

#endif

/* w25q64_Ins.h文件 */
#ifndef __W25Q64_INS_H
#define __W25Q64_INS_H

/* 写操作相关命令 */
#define W25Q64_WRITE_ENABLE							0x06	/* 写使能命令，允许后续执行擦除和编程操作 */
#define W25Q64_WRITE_DISABLE						0x04	/* 写禁止命令，禁止擦除和编程操作 */
#define W25Q64_WRITE_STATUS_REGISTER				0x01	/* 写入状态寄存器 */

/* 编程操作 */
#define W25Q64_PAGE_PROGRAM							0x02	/* 页编程命令，将数据写入指定页 */
#define W25Q64_QUAD_PAGE_PROGRAM					0x32	/* 四线页编程命令，使用4线模式快速写入数据 */

/* 擦除操作 */
#define W25Q64_SECTOR_ERASE_4KB						0x20	/* 4KB扇区擦除 */
#define W25Q64_BLOCK_ERASE_32KB						0x52	/* 32KB块擦除 */
#define W25Q64_BLOCK_ERASE_64KB						0xD8	/* 64KB块擦除 */
#define W25Q64_CHIP_ERASE							0xC7	/* 整片擦除 */

/* 状态寄存器操作 */
#define W25Q64_READ_STATUS_REGISTER_1				0x05	/* 读取状态寄存器1 */
#define W25Q64_READ_STATUS_REGISTER_2				0x35	/* 读取状态寄存器2 */

/* 电源管理 */
#define W25Q64_POWER_DOWN							0xB9	/* 掉电模式 */
#define W25Q64_RELEASE_POWER_DOWN_HPM_DEVICE_ID		0xAB	/* 退出掉电模式并读取设备ID */
#define W25Q64_HIGH_PERFORMANCE_MODE				0xA3	/* 高性能模式 */

/* 读取操作 */
#define W25Q64_READ_DATA							0x03	/* 普通读取数据 */
#define W25Q64_FAST_READ							0x0B	/* 快速读取 */
#define W25Q64_FAST_READ_DUAL_OUTPUT				0x3B	/* 双线输出快速读取 */
#define W25Q64_FAST_READ_DUAL_IO					0xBB	/* 双线I/O快速读取 */
#define W25Q64_FAST_READ_QUAD_OUTPUT				0x6B	/* 四线输出快速读取 */
#define W25Q64_FAST_READ_QUAD_IO					0xEB	/* 四线I/O快速读取 */
#define W25Q64_OCTAL_WORD_READ_QUAD_IO				0xE3	/* 八字四线I/O读取 */

/* 设备信息读取 */
#define W25Q64_MANUFACTURER_DEVICE_ID				0x90	/* 读取制造商和设备ID */
#define W25Q64_JEDEC_ID								0x9F	/* 读取JEDEC ID */
#define W25Q64_READ_UNIQUE_ID						0x4B	/* 读取唯一ID */

/* 其他操作 */
#define W25Q64_ERASE_SUSPEND						0x75	/* 暂停擦除操作 */
#define W25Q64_ERASE_RESUME							0x7A	/* 恢复擦除操作 */
#define W25Q64_CONTINUOUS_READ_MODE_RESET			0xFF	/* 重置连续读取模式 */

/* 虚拟字节 */
#define W25Q64_DUMMY_BYTE							0xFF	/* 虚拟字节，用于占位或延时 */

#endif

/* main文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./W25Q64/w25q64.h"

uint8_t MID;        /* 制造商ID存储变量 */
uint16_t DID;       /* 设备ID存储变量 */

int main(void)
{   
    HAL_Init(); 
    sys_stm32_clock_init(RCC_PLL_MUL9); 
    delay_init(72); 
    UART_Init(); 
    W25Q64_Init();
    
    /* 读取并显示芯片ID */
    W25Q64_Read_ID(&MID, &DID);
    printf("MID = %x\r\n",MID);  /* 显示制造商ID */
    printf("DID = %x\r\n",DID);  /* 显示设备ID */

    /* 测试数据缓冲区 */
    uint8_t buffer[4] = {0};  /* 读取缓冲区 */
    uint8_t data[4] = {0x10, 0x22, 0x3C, 0x18};  /* 测试数据 */
    
    while(1)
    {   
        /* 向地址0x000010写入4字节测试数据 */
        W25Q64_Write(0x000010, data, 4);
        
        /* 从地址0x000010读取4字节数据 */
        W25Q64_Read(0x000010, buffer, 4);
        
        /* 显示读取到的数据 */
        for(uint8_t i = 0; i < 4; i++)
        {
            printf("buffer[%d] = %x\r\n",i,buffer[i]);
        }
         
        delay_ms(1000);  /* 延时1秒 */
    }
}
```

# STM32 HAL库低功耗使用示例

• Sleep：几乎透明，注意清中断即可。
• Stop：醒来第一件事“**重配时钟** + 重新开外设”，否则stm32会默认使用内部低速时钟(HSI)。
• Standby：醒来就是“重启”，所有 SRAM/寄存器值已丢失，只能靠备份域或 Flash 恢复现场。

|  模式   |              断电区域               |                    如何进入                     |                      如何唤醒（典型源）                      |
| :-----: | :---------------------------------: | :---------------------------------------------: | :----------------------------------------------------------: |
|  Sleep  |  CPU时钟停<br>外设供电<br>SRAM供电  |              `__WFI()` / `__WFE()`              |        任意 NVIC 中断或事件（GPIO、USART、SysTick…）         |
|  Stop   | CPU时钟停<br/>外设断电<br/>SRAM供电 |  `PWR_CR1` 设 Stop 位 → `__WFI()` / `__WFE()`   | 任一 EXTI 线（GPIO WKUP 引脚）、RTC 闹钟、LPTIM、USB FS 唤醒 |
| Standby | CPU时钟停<br/>外设断电<br/>SRAM断电 | `PWR_CR1` 设 Standby 位 → `__WFI()` / `__WFE()` |    WKUP引脚上升沿、RTC 闹钟/时间戳、独立看门狗复位、NRST     |

## 1. 睡眠模式

```c
/* pwr.c文件 */
#include "./BSP/PWR/pwr.h"

static void PWR_Wakeup_Pin_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能GPIOA时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
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

void PWR_Init(void)
{ 
    /* 使能PWR时钟 */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* 初始化唤醒引脚 */
    PWR_Wakeup_Pin_Init();
}

/* pwr.h文件 */
#ifndef __PWR_H__
#define __PWR_H__

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"

void PWR_Init();

#endif

/* main文件 */
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
    LED_Init(); 
    PWR_Init();  /* 初始化PWR */
    printf("System is running...\r\n");
            
    while(1)
    {        
        LED_On(GPIOB, GPIO_PIN_14);
        delay_ms(1000); 

        printf("Entering SLeep Mode...\r\n");
        HAL_SuspendTick();				/* 防止滴答定时器中断唤醒sleep模式 */
        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI); /* 睡眠模式 */             
        HAL_ResumeTick();
        sys_stm32_clock_init(RCC_PLL_MUL9);  /* 重新配置系统时钟 */
        printf("EXIT SLeep Mode...\r\n");
    }
}
```

## 2.停止模式

```c
/* pwr.c文件 */
#include "./BSP/PWR/pwr.h"

static void PWR_Wakeup_Pin_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能GPIOA时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
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

void PWR_Init(void)
{ 
    /* 使能PWR时钟 */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* 初始化唤醒引脚 */
    PWR_Wakeup_Pin_Init();
}

/* pwr.h文件 */
#ifndef __PWR_H__
#define __PWR_H__

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"

void PWR_Init();

#endif

/* main文件 */
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
    LED_Init(); 
    PWR_Init();  /* 初始化PWR */
    printf("System is running...\r\n");
            
    while(1)
    {        
        LED_On(GPIOB, GPIO_PIN_14);
        delay_ms(1000);  /* 延时1秒 */

        printf("Entering Stop Mode...\r\n");
        HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);  /* 进入停止模式 */      

        sys_stm32_clock_init(RCC_PLL_MUL9);  /* 重新配置系统时钟 */
        printf("EXIT STOP Mode...\r\n");
    }
}
```

## 3.待机模式

```c
/* pwr.c文件 */
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

/* pwr.h文件 */
#ifndef __PWR_H__
#define __PWR_H__

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"

void PWR_Init();

#endif

/* main文件 */
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
    LED_Init(); 
    PWR_Init();  /* 初始化PWR */
    printf("System is running...\r\n");
            
    while(1)
    {        
        LED_On(GPIOB, GPIO_PIN_14);
        delay_ms(1000); 

        printf("Entering Standby Mode...\r\n");
        HAL_PWR_EnterSTANDBYMode();

        printf("EXIT Standby Mode...\r\n");  /* 这句执行不到 */
    }
}
```

```c
/******************************************************************************
 * STOP 模式调试经验小结1
 * ---------------------------------------------------------------------------
 * 现象：在 ST-Link 连接时，调用 HAL_PWR_EnterSTOPMode() 后,LED仍然闪烁，
 * 并未进入停止模式(或者进入后立即退出了)。
 *
 * 根本原因
 * 1. 调试器会置位 DBGMCU_CR.DBG_STOP（bit1），
 *    使得 HCLK/FCLK 在 STOP 模式下继续运行，
 *    MCU 无法进入STOP模式。
 * 2. 手册中“STOP 唤醒源”仅列出真正进入 STOP 后的唤醒事件；
 *    调试器属于“阻止进入 STOP”，故不会出现在该列表中。
 *
 * 验证方法
 * 1. 拔掉 ST-Link/J-Link，再上电运行，电流应降至 μA 级。
 * 2. 此时下载程序要按住复位键，点击下载后立即松手。
 * 实践建议
 * 1. 调试阶段：接受调试器阻止 STOP，断线下载/测试低功耗。
 * 2. 量产阶段：在初始化代码中关闭调试保持，确保真 STOP。
 *
 * 参考文档
 * - RMxxx DBGMCU_CR 寄存器 “DBG_STOP” 位说明
 * - STM32F10xxx参考手册_V10.pdf 29.16.1节有描述
 ******************************************************************************/

/******************************************************************************
 * STOP 模式调试经验小结2
 * ---------------------------------------------------------------------------
 *
 * 现象
 *   1. 用户代码调用 HAL_DBGMCU_DisableDBGStopMode() 无效——寄存器写不进去。
 *
 * 根本原因
 *   • DBGMCU_CR寄存器只能由调试器通过 SWD/JTAG 访问；
 *   • 用户固件在正常运行模式下对其读写被硬件屏蔽；
 *   • ST-Link 上电后自动置位 DBG_STOP，禁止 HCLK/FCLK 关闭，
 *     导致 MCU 无法进入真正的 STOP 模式。
 *
 * 验证方法
 *   1. 拔掉 ST-Link（或仅断开 SWDIO/SWCLK）；
 *   2. 重新上电运行，确认电流降至 μA 级，LED 熄灭。
 *
 * 正确做法
 *   • 调试阶段：接受“伪 STOP”，断开下载器测功耗；
 *   • 量产阶段：无需额外代码，直接上电即可真 STOP。
 ******************************************************************************/

/******************************************************************************
 * STOP 模式调试经验小结3 —— SysTick 的影响
 * ---------------------------------------------------------------------------
 * 现象
 *   • 未调用 HAL_SuspendTick() 时，LED 闪烁、电流高，看似 MCU 无法进入 STOP。
 *   • 调用 HAL_SuspendTick() 后，LED 熄灭，电流骤降，系统“正常”进入 STOP。
 *
 * 根本原因
 *   1. 调试器把 DBGMCU_CR.DBG_STOP 置 1 → MCU 实际只进入 **Sleep** 而非 **STOP**。
 *   2. 在 Sleep 模式下，SysTick 每 1 ms 触发中断，CPU 被反复唤醒，
 *      表现为“睡不稳”。
 *   3. 关闭 SysTick 中断后，CPU 不再被唤醒，
 *      虽然仍停留在 Sleep，但 **表面现象** 与真 STOP 一致，
 *      导致误以为 SysTick 阻止了 STOP。
 *
 * 验证方法
 *   1. 拔掉 ST-Link，再运行：SysTick 已停振，必须靠 EXTI/RTC 唤醒，
 *      才是真 STOP。
 *   2. 保留 ST-Link：即使关 SysTick，电流仍高，仅说明进入的是 Sleep。
 *
 * 实践建议
 *   • 调试阶段：断开下载器后再测低功耗，避免被 SysTick + 调试器双重干扰。
 *   • 量产阶段：无需手动关 SysTick，真 STOP 时计数器自动停止。
 ******************************************************************************/
```

## 4. STM32 低功耗调试笔记

### (1) Sleep 模式被 SysTick 频繁唤醒

**现象**

- 调用 `__WFI()` / `__WFE()` 后，电流仅降到 3 ~ 5 mA（与 Run 模式几乎一致）。
- 关闭 SysTick 后电流立刻掉到几百 μA。

**根因**

- SysTick 默认 1 ms 中断一次，任何中断都能唤醒 Sleep 模式。
- ISR 执行完回到 `while(1)`，看起来 MCU 一直在“假睡”。

**解决办法**

```c
/* 进 Sleep 前关闭 SysTick */
HAL_SuspendTick();          

/* 退出 Sleep 后恢复 */
HAL_ResumeTick();
```

### (2) 连接 ST-Link 时无法进入Stop / Standby模式 

**现象**

- 进入 Stop 2 后，电流仍然维持在 **1 ~ 2 mA**（手册标称 ≤ 200 µA）。  
- 拔掉 ST-Link 的 SWD 线后，电流立即骤降；重新插上后电流再次回升。

**根因**

1. **SWDIO/SWCLK 持续被调试器拉高/低**，引入额外漏电流。  
2. **DBGMCU->CR 寄存器** 默认允许 Debug 模式在 Stop/Standby 下保持部分时钟，导致 MCU 无法完全关断。
3. 芯片实际是在SLeep模式下运行，有由于systick中断唤醒，导致程序在看似正常执行。

**解决办法**

1. 硬件措施（推荐）

- **实测功耗时必须断开 ST-Link**  
  （或至少断开 **SWDIO、SWCLK、NRST** 三根线）。  
- **量产板预留调试/低功耗选择开关**  
  使用跳线帽、拨码开关或可插拔排针，便于切换“调试”与“低功耗”模式。

2. 软件措施**不行**！

```c
/* 想关闭 Debug 在 Stop/Standby 下的时钟保持功能，但发现该寄存器用户无法修改，只能有调试器改写！ */
HAL_DBGMCU_DisableDBGStopMode();
HAL_DBGMCU_DisableDBGStandbyMode();

/* 裸机写法 */
DBGMCU->CR &= ~(DBGMCU_CR_DBG_STOP | DBGMCU_CR_DBG_STANDBY);
```

# STM32 HAL库RTC使用示例

本文档提供了使用STM32 HAL库进行RTC（实时时钟）初始化和操作的代码示例，包括时间设置、闹钟功能和备份寄存器使用。

## 1. RTC 实时时钟示例

以下代码展示了如何初始化RTC并设置当前时间：

```c
/* rtc.c文件 */
#include "./BSP/RTC/rtc.h"

RTC_HandleTypeDef hrtc;

uint32_t is_first_flag = 0; // 用于标记是否第一次设置时间

static void RTC_FirstInit(void)
{
    /* 使能电源和备份时钟 */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_BKP_CLK_ENABLE();
    
    /* 使能对备份域的访问 */
    HAL_PWR_EnableBkUpAccess();
    
    /* 配置LSE时钟源 */
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET);
    
    /* 选择RTC时钟源为LSE */
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
    
    /* 使能RTC时钟 */
    __HAL_RCC_RTC_ENABLE();
    
    /* 配置RTC */
    hrtc.Instance = RTC;
    hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;  /* 自动配置1秒预分频 */
    
    HAL_RTC_Init(&hrtc);
   
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_MAGIC); // 设置标志位
}

static uint32_t RTC_Get_Seconds(void)
{
    uint32_t high;
    uint32_t low;
    
    high = RTC->CNTH;
    low = RTC->CNTL;
    
    return (high << 16) | low;   // 读取 32 位秒计数器
}

static void RTC_Get_Data_Time_With_StartTime(DataTime_Struct *dataTime,
                      uint32_t start_year, uint32_t start_month, uint32_t start_date,
                      uint32_t start_hour, uint32_t start_minute, uint32_t start_second)
{
    struct tm start_tm = {0};

    // 设置起始时间（注意 tm_year 是从 1900 开始计数的）
    start_tm.tm_year  = start_year - 1900;
    start_tm.tm_mon   = start_month - 1;     // tm_mon 是从 0 开始的
    start_tm.tm_mday  = start_date;
    start_tm.tm_hour  = start_hour;
    start_tm.tm_min   = start_minute;
    start_tm.tm_sec   = start_second;
    start_tm.tm_isdst = -1;                  // 让系统自动判断夏令时

    // 将起始时间转换为 time_t（即秒数）
    time_t start_time = mktime(&start_tm);

    // 加上 RTC 秒数偏移量
    time_t current_time = start_time + RTC_Get_Seconds();

    // 转换为目标时间结构
    struct tm *current_tm = localtime(&current_time);

    // 填充输出结构体
    dataTime->year   = current_tm->tm_year + 1900;
    dataTime->month  = current_tm->tm_mon + 1;
    dataTime->date   = current_tm->tm_mday;
    dataTime->hour   = current_tm->tm_hour;
    dataTime->minute = current_tm->tm_min;
    dataTime->second = current_tm->tm_sec;
}

void RTC_Init(void)
{
    /* 先填充句柄实例，供 BKP 读取使用 */
    hrtc.Instance = RTC;

    if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != RTC_MAGIC)
    {
        RTC_FirstInit();                // 首次/掉电后重新初始化
    }
    else
    {
        printf("RTC already configured, using existing time\r\n");
    }
}

void RTC_Get_Data_Time(DataTime_Struct *dataTime)
{
    // 使用默认起始时间 2025.8.2 23:59:00
    RTC_Get_Data_Time_With_StartTime(dataTime, 2025, 8, 2, 23, 59, 0);
}

/*rtc.h文件*/
#ifndef __RTC_H__
#define __RTC_H__

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "time.h"

#define RTC_MAGIC 0x5A5A  // 用于标记RTC是否已初始化

extern RTC_HandleTypeDef hrtc;
extern uint32_t is_first_flag; 

typedef struct {
    uint16_t year;     // 年，使用uint16_t存储四位数年份
    uint8_t month;     // 月
    uint8_t date;      // 日
    uint8_t hour;      // 时
    uint8_t minute;    // 分
    uint8_t second;    // 秒
} DataTime_Struct;

void RTC_Init(void);

void RTC_Get_Data_Time(DataTime_Struct *dataTime);

#endif

/* main文件 */
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
    RTC_Init();  // RTC初始化
    
    while(1)
    {        
        DataTime_Struct currentTime;
        RTC_Get_Data_Time(&currentTime);  // 获取当前时间
        
        printf("Current Time: %04d-%02d-%02d %02d:%02d:%02d\r\n", 
               currentTime.year, currentTime.month, currentTime.date, 
               currentTime.hour, currentTime.minute, currentTime.second);
        
        delay_ms(1000);  // 延时1秒
    }
}
```
