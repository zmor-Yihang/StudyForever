# FreeRTOS Tickless低功耗相关API和机制完整总结

---

## 1. Tickless低功耗概述

### 1.1 Tickless模式简介
**概念：**
- Tickless模式允许MCU在空闲时进入深度睡眠
- 停止周期性的系统时钟节拍（tick）中断
- 在需要时自动唤醒并补偿睡眠时间
- 显著降低功耗

**传统模式 vs Tickless模式：**
```
传统模式：
    运行 → 空闲 → tick中断 → 空闲 → tick中断 → ...
    功耗：较高（频繁唤醒）

Tickless模式：
    运行 → 深度睡眠（多个tick） → 唤醒 → 运行
    功耗：低（减少唤醒次数）
```

**功耗节省：**
- 减少中断处理开销
- 允许更深度的睡眠模式
- 降低平均电流消耗
- 延长电池寿命

---

### 1.2 Tickless模式类型

| 模式 | 说明 | 配置方式 |
|------|------|----------|
| 无Tickless | 标准模式，周期性tick | configUSE_TICKLESS_IDLE = 0 |
| 标准Tickless | FreeRTOS提供的实现 | configUSE_TICKLESS_IDLE = 1 |
| 扩展Tickless | 用户自定义实现 | configUSE_TICKLESS_IDLE = 2 |

---

## 2. 配置选项

### 2.1 基本配置宏
```c
// FreeRTOSConfig.h中的配置

// 启用Tickless低功耗模式
// 0 = 禁用
// 1 = 使用标准实现
// 2 = 使用用户自定义实现
#define configUSE_TICKLESS_IDLE                 1

// 进入Tickless模式的最小空闲tick数
// 如果预期空闲时间少于此值，不进入低功耗模式
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP   2

// 使用外部时钟源（可选）
#define configUSE_TICK_HOOK                     0

// 预期空闲时间计算精度（可选）
#define configPRE_SLEEP_PROCESSING(x)           /* 用户定义 */
#define configPOST_SLEEP_PROCESSING(x)          /* 用户定义 */
```

**配置说明：**
- **configUSE_TICKLESS_IDLE**：启用低功耗模式的开关
- **configEXPECTED_IDLE_TIME_BEFORE_SLEEP**：防止频繁进入/退出低功耗
- **configPRE_SLEEP_PROCESSING**：睡眠前的预处理宏
- **configPOST_SLEEP_PROCESSING**：唤醒后的后处理宏

---

### 2.2 睡眠模式配置
```c
// 定义睡眠模式的深度（用户根据MCU定义）
#define SLEEP_MODE_LIGHT        0   // 浅睡眠
#define SLEEP_MODE_DEEP         1   // 深度睡眠
#define SLEEP_MODE_STOP         2   // 停止模式

// 预睡眠处理示例
#define configPRE_SLEEP_PROCESSING(x)   \
    do {                                \
        if ((x) > 100) {                \
            enter_deep_sleep_mode();    \
        } else {                        \
            enter_light_sleep_mode();   \
        }                               \
    } while(0)

// 后睡眠处理示例
#define configPOST_SLEEP_PROCESSING(x)  \
    do {                                \
        exit_sleep_mode();              \
        reconfigure_clocks();           \
    } while(0)
```

---

## 3. Tickless模式API

### 3.1 vPortSuppressTicksAndSleep() - 抑制tick并进入睡眠
**函数原型：**
```c
void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xExpectedIdleTime | TickType_t | 预期的空闲时间（tick数） |

**返回值：**无

**作用：**停止系统tick中断，进入低功耗睡眠模式

**调用位置：**
- 由FreeRTOS内核在空闲任务中自动调用
- 用户一般不直接调用

**工作流程：**
1. 计算可以睡眠的时间
2. 配置唤醒定时器
3. 进入睡眠模式
4. 被唤醒后补偿系统时钟
5. 恢复正常运行

**注意：**
- configUSE_TICKLESS_IDLE = 2 时需要用户实现
- 必须正确处理时间补偿
- 考虑唤醒延迟

---

### 3.2 预睡眠和后睡眠处理宏

#### 3.2.1 configPRE_SLEEP_PROCESSING() - 睡眠前处理
**宏定义：**
```c
#define configPRE_SLEEP_PROCESSING(xExpectedIdleTime)
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xExpectedIdleTime | TickType_t | 预期的空闲时间 |

**作用：**在进入睡眠前执行用户自定义操作

**典型用途：**
- 关闭外设
- 降低时钟频率
- 配置唤醒源
- 选择睡眠深度

**实现示例：**
```c
#define configPRE_SLEEP_PROCESSING(xExpectedIdleTime)   \
    do {                                                \
        /* 关闭不需要的外设 */                            \
        disable_unused_peripherals();                   \
        /* 根据睡眠时间选择模式 */                        \
        if (xExpectedIdleTime > 1000) {                \
            /* 长时间睡眠，进入深度模式 */                 \
            prepare_deep_sleep();                       \
        } else {                                        \
            /* 短时间睡眠，进入浅睡眠 */                   \
            prepare_light_sleep();                      \
        }                                               \
    } while(0)
```

---

#### 3.2.2 configPOST_SLEEP_PROCESSING() - 唤醒后处理
**宏定义：**
```c
#define configPOST_SLEEP_PROCESSING(xExpectedIdleTime)
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xExpectedIdleTime | TickType_t | 预期的空闲时间（参考值） |

**作用：**在退出睡眠后执行用户自定义操作

**典型用途：**
- 重新使能外设
- 恢复时钟配置
- 清除唤醒标志
- 系统状态恢复

**实现示例：**
```c
#define configPOST_SLEEP_PROCESSING(xExpectedIdleTime)  \
    do {                                                \
        /* 恢复系统时钟 */                                \
        restore_system_clock();                         \
        /* 重新使能外设 */                                \
        enable_peripherals();                           \
        /* 清除唤醒源标志 */                              \
        clear_wakeup_flags();                           \
    } while(0)
```

---

## 4. 空闲任务钩子函数

### 4.1 vApplicationIdleHook() - 空闲任务钩子
**函数原型：**
```c
void vApplicationIdleHook(void);
```

**参数说明：**无参数

**返回值：**无

**作用：**在空闲任务的每次循环中被调用

**宏配置要求：**
- configUSE_IDLE_HOOK 必须为1

**典型用途：**
- 执行低优先级的后台任务
- 进入低功耗模式
- 喂看门狗
- 统计CPU使用率

**注意事项：**
- 不能阻塞或挂起
- 执行时间应尽可能短
- 不能调用可能阻塞的API
- Tickless模式下会减少调用频率

**实现示例：**
```c
void vApplicationIdleHook(void)
{
    // 喂看门狗
    feed_watchdog();
    
    // 执行低优先级任务
    process_background_tasks();
    
    // 在非Tickless模式下，可以手动进入低功耗
    #if (configUSE_TICKLESS_IDLE == 0)
    // 进入浅睡眠等待下一个中断
    __WFI();
    #endif
}
```

---

## 5. 时间管理相关API

### 5.1 xTaskGetTickCount() - 获取系统时钟节拍计数
**函数原型：**
```c
TickType_t xTaskGetTickCount(void);
```

**参数说明：**无参数

**返回值：**当前系统时钟节拍计数

**作用：**获取自调度器启动以来的tick计数

**注意：**
- Tickless模式下仍然准确
- 睡眠时间会自动补偿

---

### 5.2 xTaskGetTickCountFromISR() - 中断中获取tick计数
**函数原型：**
```c
TickType_t xTaskGetTickCountFromISR(void);
```

**参数说明：**无参数

**返回值：**当前系统时钟节拍计数

**作用：**在中断服务函数中获取tick计数

---

### 5.3 vTaskStepTick() - 手动推进系统时钟
**函数原型：**
```c
void vTaskStepTick(TickType_t xTicksToJump);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTicksToJump | TickType_t | 要推进的tick数 |

**返回值：**无

**作用：**手动补偿睡眠期间经过的时间

**注意：**
- 用于自定义Tickless实现
- 必须在调度器暂停时调用
- 用于时间同步

---

## 6. 标准Tickless实现原理

### 6.1 标准实现流程
```c
// 简化的标准Tickless实现流程
void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    uint32_t ulReloadValue, ulCompleteTickPeriods;
    TickType_t xModifiableIdleTime;
    
    // 1. 计算睡眠时间是否值得进入低功耗
    if (xExpectedIdleTime < configEXPECTED_IDLE_TIME_BEFORE_SLEEP)
    {
        return;  // 时间太短，不值得
    }
    
    // 2. 停止系统tick定时器
    stop_tick_interrupt();
    
    // 3. 配置唤醒定时器
    ulReloadValue = calculate_sleep_time(xExpectedIdleTime);
    configure_wakeup_timer(ulReloadValue);
    
    // 4. 再次检查是否可以睡眠（可能有新的事件）
    if (eTaskConfirmSleepModeStatus() == eAbortSleep)
    {
        // 不能睡眠，恢复tick
        restart_tick_interrupt();
        return;
    }
    
    // 5. 执行预睡眠处理
    xModifiableIdleTime = xExpectedIdleTime;
    configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
    
    // 6. 进入睡眠模式
    if (xModifiableIdleTime > 0)
    {
        __DSB();  // 数据同步屏障
        __WFI();  // 等待中断（进入睡眠）
        __ISB();  // 指令同步屏障
    }
    
    // 7. 执行后睡眠处理
    configPOST_SLEEP_PROCESSING(xExpectedIdleTime);
    
    // 8. 计算实际睡眠的时间
    ulCompleteTickPeriods = get_actual_sleep_time();
    
    // 9. 停止唤醒定时器
    stop_wakeup_timer();
    
    // 10. 恢复系统tick定时器
    restart_tick_interrupt();
    
    // 11. 补偿系统时钟
    vTaskStepTick(ulCompleteTickPeriods);
}
```

---

### 6.2 时间计算
```c
// 预期睡眠时间（tick）转换为定时器重载值
uint32_t calculate_sleep_time(TickType_t xExpectedIdleTime)
{
    uint32_t ulReloadValue;
    
    // 减去进入和退出睡眠的时间开销
    if (xExpectedIdleTime > ulTimerCountsForOneTick)
    {
        xExpectedIdleTime -= ulTimerCountsForOneTick;
    }
    
    // 转换为定时器计数值
    ulReloadValue = xExpectedIdleTime * ulTimerCountsForOneTick;
    
    // 限制最大值
    if (ulReloadValue > ulMaximumTimerValue)
    {
        ulReloadValue = ulMaximumTimerValue;
    }
    
    return ulReloadValue;
}
```

---

## 7. 自定义Tickless实现

### 7.1 实现步骤
当设置 `configUSE_TICKLESS_IDLE = 2` 时，需要用户实现 `vPortSuppressTicksAndSleep()` 函数。

**实现要点：**
1. 停止系统tick中断
2. 配置唤醒源（RTC、外部中断等）
3. 检查是否可以睡眠
4. 进入睡眠模式
5. 唤醒后计算实际睡眠时间
6. 补偿系统时钟
7. 恢复系统tick中断

---

### 7.2 STM32 RTC唤醒示例
```c
#if (configUSE_TICKLESS_IDLE == 2)

// 用户实现的Tickless函数
void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    uint32_t ulReloadValue;
    uint32_t ulCompleteTickPeriods;
    uint32_t ulSleepTime;
    eSleepModeStatus eSleepStatus;
    
    // 1. 检查最小睡眠时间
    if (xExpectedIdleTime < configEXPECTED_IDLE_TIME_BEFORE_SLEEP)
    {
        return;
    }
    
    // 2. 停止SysTick
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    
    // 3. 计算RTC唤醒时间（转换为秒）
    ulSleepTime = (xExpectedIdleTime * 1000) / configTICK_RATE_HZ;
    
    // 限制最大睡眠时间
    if (ulSleepTime > MAX_SLEEP_TIME_SEC)
    {
        ulSleepTime = MAX_SLEEP_TIME_SEC;
    }
    
    // 4. 配置RTC唤醒
    configure_rtc_wakeup(ulSleepTime);
    
    // 5. 再次确认可以睡眠
    eSleepStatus = eTaskConfirmSleepModeStatus();
    
    if (eSleepStatus == eAbortSleep)
    {
        // 取消睡眠，恢复SysTick
        disable_rtc_wakeup();
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
        return;
    }
    
    // 6. 预睡眠处理
    configPRE_SLEEP_PROCESSING(xExpectedIdleTime);
    
    // 7. 进入睡眠模式
    if (eSleepStatus == eStandardSleep)
    {
        // 标准睡眠（可被任何中断唤醒）
        __WFI();
    }
    else
    {
        // 低功耗睡眠（只能被配置的唤醒源唤醒）
        enter_stop_mode();
    }
    
    // 8. 后睡眠处理
    configPOST_SLEEP_PROCESSING(xExpectedIdleTime);
    
    // 9. 计算实际睡眠时间
    ulCompleteTickPeriods = get_rtc_elapsed_time();
    
    // 转换为tick
    ulCompleteTickPeriods = (ulCompleteTickPeriods * configTICK_RATE_HZ) / 1000;
    
    // 10. 禁用RTC唤醒
    disable_rtc_wakeup();
    
    // 11. 恢复SysTick
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    
    // 12. 补偿系统时钟
    if (ulCompleteTickPeriods > 0)
    {
        vTaskStepTick(ulCompleteTickPeriods);
    }
}

// RTC配置函数
void configure_rtc_wakeup(uint32_t seconds)
{
    // 禁用RTC唤醒
    RTC->CR &= ~RTC_CR_WUTE;
    
    // 等待写入允许
    while (!(RTC->ISR & RTC_ISR_WUTWF));
    
    // 设置唤醒计数值
    RTC->WUTR = seconds;
    
    // 选择时钟源（1Hz）
    RTC->CR &= ~RTC_CR_WUCKSEL;
    RTC->CR |= RTC_CR_WUCKSEL_2;  // ck_spre (1Hz)
    
    // 使能唤醒中断
    RTC->CR |= RTC_CR_WUTIE;
    
    // 使能RTC唤醒
    RTC->CR |= RTC_CR_WUTE;
}

#endif // configUSE_TICKLESS_IDLE == 2
```

---

### 7.3 睡眠模式确认API

#### 7.3.1 eTaskConfirmSleepModeStatus() - 确认睡眠模式状态
**函数原型：**
```c
eSleepModeStatus eTaskConfirmSleepModeStatus(void);
```

**参数说明：**无参数

**返回值：**
```c
typedef enum
{
    eAbortSleep = 0,        // 中止睡眠
    eStandardSleep,         // 标准睡眠（任何中断可唤醒）
    eNoTasksWaitingTimeout  // 无任务等待超时（深度睡眠）
} eSleepModeStatus;
```

**作用：**确认当前是否可以进入睡眠及睡眠深度

**使用场景：**
- 在进入睡眠前最后确认
- 检查是否有待处理的任务
- 决定睡眠深度

**返回值说明：**
- **eAbortSleep**：不应该睡眠（有任务就绪或待处理）
- **eStandardSleep**：可以睡眠，但可能很快被唤醒
- **eNoTasksWaitingTimeout**：可以深度睡眠（没有定时器即将到期）

---

## 8. 使用示例和最佳实践

### 8.1 基本Tickless配置示例
```c
// FreeRTOSConfig.h

// 启用标准Tickless模式
#define configUSE_TICKLESS_IDLE                 1

// 最小空闲时间（2个tick）
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP   2

// 启用空闲钩子
#define configUSE_IDLE_HOOK                     1

// 预睡眠处理
#define configPRE_SLEEP_PROCESSING(x)           \
    do {                                        \
        /* 关闭LED */                            \
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); \
    } while(0)

// 后睡眠处理
#define configPOST_SLEEP_PROCESSING(x)          \
    do {                                        \
        /* 打开LED */                            \
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET); \
    } while(0)
```

---

### 8.2 根据睡眠时间选择睡眠深度
```c
// 全局变量
volatile uint32_t ulExpectedIdleTime = 0;

// 预睡眠处理：根据睡眠时间选择模式
#define configPRE_SLEEP_PROCESSING(x)           \
    do {                                        \
        ulExpectedIdleTime = (x);               \
        if ((x) > 1000) {                       \
            /* 长时间睡眠：进入STOP模式 */         \
            prepare_stop_mode();                \
        } else if ((x) > 10) {                  \
            /* 中等时间：进入SLEEP模式 */          \
            prepare_sleep_mode();               \
        } else {                                \
            /* 短时间：使用WFI */                 \
            prepare_wfi_mode();                 \
        }                                       \
    } while(0)

void prepare_stop_mode(void)
{
    // 关闭不需要的外设
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    
    // 配置唤醒源
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    
    // 设置STOP模式调压器
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

void prepare_sleep_mode(void)
{
    // 进入SLEEP模式（保持外设时钟）
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

void prepare_wfi_mode(void)
{
    // 仅执行WFI指令
    __WFI();
}
```

---

### 8.3 外设管理示例
```c
// 外设状态保存
typedef struct
{
    uint32_t uart_state;
    uint32_t spi_state;
    uint32_t i2c_state;
    uint32_t timer_state;
} PeripheralState_t;

PeripheralState_t xPeripheralState;

// 预睡眠处理：保存并关闭外设
#define configPRE_SLEEP_PROCESSING(x)           \
    do {                                        \
        if ((x) > 100) {                        \
            save_and_disable_peripherals();     \
        }                                       \
    } while(0)

// 后睡眠处理：恢复外设
#define configPOST_SLEEP_PROCESSING(x)          \
    do {                                        \
        if (ulExpectedIdleTime > 100) {         \
            restore_peripherals();              \
        }                                       \
    } while(0)

void save_and_disable_peripherals(void)
{
    // 保存UART状态
    xPeripheralState.uart_state = USART1->CR1;
    HAL_UART_DeInit(&huart1);
    
    // 保存SPI状态
    xPeripheralState.spi_state = SPI1->CR1;
    HAL_SPI_DeInit(&hspi1);
    
    // 禁用时钟
    __HAL_RCC_USART1_CLK_DISABLE();
    __HAL_RCC_SPI1_CLK_DISABLE();
}

void restore_peripherals(void)
{
    // 使能时钟
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();
    
    // 恢复UART
    HAL_UART_Init(&huart1);
    USART1->CR1 = xPeripheralState.uart_state;
    
    // 恢复SPI
    HAL_SPI_Init(&hspi1);
    SPI1->CR1 = xPeripheralState.spi_state;
}
```

---

### 8.4 完整的低功耗应用示例
```c
// 主程序
int main(void)
{
    // 系统初始化
    HAL_Init();
    SystemClock_Config();
    
    // 外设初始化
    MX_GPIO_Init();
    MX_RTC_Init();
    MX_USART1_UART_Init();
    
    // 低功耗配置
    configure_low_power();
    
    // 创建任务
    xTaskCreate(vSensorTask, "Sensor", 128, NULL, 2, NULL);
    xTaskCreate(vCommTask, "Comm", 256, NULL, 1, NULL);
    
    // 启动调度器
    vTaskStartScheduler();
    
    while (1);
}

// 低功耗配置
void configure_low_power(void)
{
    // 使能PWR时钟
    __HAL_RCC_PWR_CLK_ENABLE();
    
    // 配置RTC作为唤醒源
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
    
    // 使能快速唤醒
    HAL_PWREx_EnableFastWakeUp();
    
    // 配置GPIO为模拟输入（降低功耗）
    configure_unused_gpio();
}

// 传感器任务（周期性采样）
void vSensorTask(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(5000);  // 5秒
    
    xLastWakeTime = xTaskGetTickCount();
    
    while (1)
    {
        // 读取传感器
        float temperature = read_temperature();
        float humidity = read_humidity();
        
        // 处理数据
        process_sensor_data(temperature, humidity);
        
        // 延时（期间可进入低功耗）
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// 通信任务（按需唤醒）
void vCommTask(void *pvParameters)
{
    uint32_t ulNotificationValue;
    
    while (1)
    {
        // 等待通知（期间系统可进入低功耗）
        ulNotificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        if (ulNotificationValue > 0)
        {
            // 发送数据
            send_data_via_uart();
        }
    }
}

// UART接收中断（唤醒系统）
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    // 通知通信任务
    vTaskNotifyGiveFromISR(xCommTaskHandle, &xHigherPriorityTaskWoken);
    
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
```

---

### 8.5 功耗测量和优化
```c
// 功耗监控任务
void vPowerMonitorTask(void *pvParameters)
{
    uint32_t ulLastTickCount = 0;
    uint32_t ulCurrentTickCount;
    uint32_t ulActiveTicks = 0;
    uint32_t ulTotalTicks;
    float fActivePercentage;
    
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10000));  // 每10秒统计一次
        
        ulCurrentTickCount = xTaskGetTickCount();
        ulTotalTicks = ulCurrentTickCount - ulLastTickCount;
        
        // 计算活动时间百分比
        fActivePercentage = (float)ulActiveTicks / (float)ulTotalTicks * 100.0f;
        
        printf("Power Statistics:\n");
        printf("  Total ticks: %lu\n", ulTotalTicks);
        printf("  Active ticks: %lu\n", ulActiveTicks);
        printf("  Active: %.2f%%\n", fActivePercentage);
        printf("  Sleep: %.2f%%\n", 100.0f - fActivePercentage);
        
        ulLastTickCount = ulCurrentTickCount;
        ulActiveTicks = 0;
    }
}

// 在空闲钩子中统计
void vApplicationIdleHook(void)
{
    static uint32_t ulIdleCount = 0;
    ulIdleCount++;
    
    // 每1000次打印一次
    if (ulIdleCount % 1000 == 0)
    {
        printf("Idle hook called %lu times\n", ulIdleCount);
    }
}
```

---

## 9. 唤醒源配置

### 9.1 常见唤醒源

| 唤醒源 | 说明 | 适用场景 |
|--------|------|----------|
| RTC | 实时时钟定时唤醒 | 周期性任务 |
| GPIO外部中断 | 按键、传感器中断 | 事件触发 |
| UART | 串口数据接收 | 通信唤醒 |
| USB | USB设备连接 | USB应用 |
| CAN | CAN总线消息 | 车载应用 |
| 看门狗 | 独立看门狗 | 系统监控 |

---

### 9.2 RTC唤醒配置示例
```c
// RTC初始化
void MX_RTC_Init(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    
    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        Error_Handler();
    }
    
    // 初始化时间
    sTime.Hours = 0;
    sTime.Minutes = 0;
    sTime.Seconds = 0;
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    
    // 初始化日期
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = 1;
    sDate.Year = 24;
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    
    // 配置唤醒定时器
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 5, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
}

// RTC唤醒中断
void RTC_WKUP_IRQHandler(void)
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    // 唤醒后的处理
    printf("RTC wakeup!\n");
}
```

---

### 9.3 GPIO外部中断唤醒
```c
// GPIO配置为外部中断
void configure_wakeup_gpio(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIO时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // 配置PA0为外部中断
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;  // 上升沿触发
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 使能并配置EXTI中断
    HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

// 外部中断处理
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if (GPIO_Pin == GPIO_PIN_0)
    {
        // 唤醒相应任务
        vTaskNotifyGiveFromISR(xTaskHandle, &xHigherPriorityTaskWoken);
    }
    
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
```
