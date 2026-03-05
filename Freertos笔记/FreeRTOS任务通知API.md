# FreeRTOS 任务通知相关API和结构体完整总结

---

## 1. 任务通知概述

### 1.1 任务通知简介
**概念：**
- 每个任务都有一个32位的通知值
- 任务通知是轻量级的任务间通信机制
- 比队列、信号量、事件组更快且占用更少RAM

**优势：**
- 速度快（比队列快45%）
- RAM占用少（每个任务只需8字节）
- 可以替代二值信号量、计数信号量、事件组和邮箱

**限制：**
- 只能有一个任务接收通知
- 接收任务必须已知（不能广播）
- 只能存储一个通知值

---

### 1.2 任务通知状态
```c
// 任务通知状态枚举
typedef enum
{
    eNoAction = 0,              // 不更新通知值
    eSetBits,                   // 按位或
    eIncrement,                 // 递增
    eSetValueWithOverwrite,     // 覆盖写入
    eSetValueWithoutOverwrite   // 不覆盖写入
} eNotifyAction;
```

**通知状态：**
- 待处理（pending）：有通知等待处理
- 非待处理（not pending）：没有通知等待 

---

## 2. 任务通知发送API

### 2.1 xTaskNotify() - 发送任务通知
**函数原型：**
```c
BaseType_t xTaskNotify(
    TaskHandle_t xTaskToNotify,
    uint32_t ulValue,
    eNotifyAction eAction
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTaskToNotify | TaskHandle_t | 接收通知的任务句柄 |
| ulValue | uint32_t | 通知值 |
| eAction | eNotifyAction | 通知值动作类型 |

**返回值：**
- pdPASS：成功
- pdFAIL：失败（仅当eSetValueWithoutOverwrite且通知值未被读取时）

**作用：**向指定任务发送通知并更新通知值

**eAction参数详解：**
- **eNoAction**：只发送通知，不更新通知值
- **eSetBits**：通知值按位或操作（类似事件组）
- **eIncrement**：通知值递增（类似计数信号量）
- **eSetValueWithOverwrite**：覆盖通知值（类似邮箱）
- **eSetValueWithoutOverwrite**：仅在通知值已被读取时才更新（不覆盖通知值）

### 2.2 xTaskNotifyAndQuery() - 发送通知并保留原通知值

**函数原型：**

```c
BaseType_t xTaskNotifyAndQuery(
    TaskHandle_t xTaskToNotify,
    uint32_t ulValue,
    eNotifyAction eAction,
    uint32_t *pulPreviousNotifyValue
);
```

**参数说明：**

| 参数名                 | 类型          | 说明               |
| ---------------------- | ------------- | ------------------ |
| xTaskToNotify          | TaskHandle_t  | 接收通知的任务句柄 |
| ulValue                | uint32_t      | 通知值             |
| eAction                | eNotifyAction | 通知动作类型       |
| pulPreviousNotifyValue | uint32_t*     | 返回更新前的通知值 |

**返回值：**

- pdPASS：成功
- pdFAIL：失败

**作用：**发送通知并返回更新前的通知值

**注意：**

- 如果不需要原值，使用xTaskNotify()更高效
- pulPreviousNotifyValue可以为NULL

### 2.3 xTaskNotifyGive() - 发送通知并递增通知值

**函数原型：**
```c
BaseType_t xTaskNotifyGive(TaskHandle_t xTaskToNotify);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTaskToNotify | TaskHandle_t | 接收通知的任务句柄 |

**返回值：**总是返回pdPASS

**作用：**发送通知并递增通知值（用作轻量级计数信号量）

**等价于：**

```c
xTaskNotify(xTaskToNotify, 0, eIncrement);
```

**注意：**

- 简化版本，适用于信号量替代场景
- 速度更快，代码更简洁

### 2.4 xTaskNotifyFromISR() - 中断中发送通知

**函数原型：**
```c
BaseType_t xTaskNotifyFromISR(
    TaskHandle_t xTaskToNotify,
    uint32_t ulValue,
    eNotifyAction eAction,
    BaseType_t *pxHigherPriorityTaskWoken
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTaskToNotify | TaskHandle_t | 接收通知的任务句柄 |
| ulValue | uint32_t | 通知值 |
| eAction | eNotifyAction | 通知动作类型 |
| pxHigherPriorityTaskWoken | BaseType_t* | 高优先级任务唤醒标志 |

**返回值：**
- pdPASS：成功
- pdFAIL：失败

**作用：**在中断服务函数中发送任务通知

**注意：**
- 必须在中断中使用此版本
- 需要检查pxHigherPriorityTaskWoken并执行任务切换

### 2.5 xTaskNotifyAndQueryFromISR() - 中断中发送通知

**函数原型：**

```c
BaseType_t xTaskNotifyAndQueryFromISR(
    TaskHandle_t xTaskToNotify,
    uint32_t ulValue,
    eNotifyAction eAction,
    uint32_t *pulPreviousNotifyValue,
    BaseType_t *pxHigherPriorityTaskWoken
);
```

**参数说明：**

| 参数名                    | 类型          | 说明                 |
| ------------------------- | ------------- | -------------------- |
| xTaskToNotify             | TaskHandle_t  | 接收通知的任务句柄   |
| ulValue                   | uint32_t      | 通知值               |
| eAction                   | eNotifyAction | 通知动作类型         |
| pulPreviousNotifyValue    | uint32_t*     | 返回更新前的通知值   |
| pxHigherPriorityTaskWoken | BaseType_t*   | 高优先级任务唤醒标志 |

**返回值：**

- pdPASS：成功
- pdFAIL：失败

**作用：**在中断中发送通知并返回更新前的通知值

### 2.6 vTaskNotifyGiveFromISR() - 中断中发送通知

**函数原型：**
```c
void vTaskNotifyGiveFromISR(
    TaskHandle_t xTaskToNotify,
    BaseType_t *pxHigherPriorityTaskWoken
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTaskToNotify | TaskHandle_t | 接收通知的任务句柄 |
| pxHigherPriorityTaskWoken | BaseType_t* | 高优先级任务唤醒标志 |

**返回值：**无

**作用：**在中断中发送通知并递增通知值

**等价于：**
```c
xTaskNotifyFromISR(xTaskToNotify, 0, eIncrement, pxHigherPriorityTaskWoken);
```

## 3. 任务通知接收API

### 3.1 xTaskNotifyWait() - 等待任务通知

**函数原型：**

```c
BaseType_t xTaskNotifyWait(
    uint32_t ulBitsToClearOnEntry,
    uint32_t ulBitsToClearOnExit,
    uint32_t *pulNotificationValue,
    TickType_t xTicksToWait
);
```

**参数说明：**

| 参数名               | 类型       | 说明               |
| -------------------- | ---------- | ------------------ |
| ulBitsToClearOnEntry | uint32_t   | 进入时清除的位掩码 |
| ulBitsToClearOnExit  | uint32_t   | 退出时清除的位掩码 |
| pulNotificationValue | uint32_t*  | 返回通知值         |
| xTicksToWait         | TickType_t | 最大等待时间       |

**返回值：**

- pdTRUE：收到通知
- pdFALSE：超时

**作用：**等待任务通知并提供灵活的位清除控制

**参数详解：**

- **ulBitsToClearOnEntry**：在等待前清除指定位，确保等待的是“新事件”，而不是上次残留的旧通知
- **ulBitsToClearOnExit**：收到通知后清除指定位，避免下次等待时又被该通知唤醒
- **pulNotificationValue**：如果不为NULL，返回通知值

**使用场景：**

- 事件组替代：使用eSetBits动作发送，按位等待
- 邮箱替代：使用eSetValueWithOverwrite发送
- 复杂的通知处理场景

---

### 3.2 ulTaskNotifyTake() - 等待通知并递减通知值
**函数原型：**

```c
uint32_t ulTaskNotifyTake(
    BaseType_t xClearCountOnExit,
    TickType_t xTicksToWait
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xClearCountOnExit | BaseType_t | 退出时清零或递减 |
| xTicksToWait | TickType_t | 最大等待时间 |

**返回值：**返回通知值

**作用：**等待任务通知（用作轻量级信号量）

**参数详解：**
- **xClearCountOnExit**：
  - pdTRUE：退出时清零通知值（二值信号量）
  - pdFALSE：退出时递减通知值（计数信号量）

**注意：**
- 如果通知值为0，任务将阻塞
- 适合替代信号量的场景

## 4. 任务通知状态查询和清除API

### 4.1 xTaskNotifyStateClear() - 清除通知状态
**函数原型：**
```c
BaseType_t xTaskNotifyStateClear(TaskHandle_t xTask);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTask | TaskHandle_t | 任务句柄，NULL表示当前任务 |

**返回值：**
- pdTRUE：通知状态被清除
- pdFALSE：通知状态已经是非待处理状态

**作用：**将任务的通知状态设置为非待处理

**注意：**
- 不会改变通知值
- 用于清除待处理标志

---

### 4.2 ulTaskNotifyValueClear() - 清除通知值的位
**函数原型：**
```c
uint32_t ulTaskNotifyValueClear(
    TaskHandle_t xTask,
    uint32_t ulBitsToClear
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTask | TaskHandle_t | 任务句柄，NULL表示当前任务 |
| ulBitsToClear | uint32_t | 要清除的位掩码 |

**返回值：**清除前的通知值

**作用：**清除通知值中的指定位

**注意：**
- 不会改变通知状态（待处理/非待处理）
- 用于事件组风格的位操作

---

## 5. 使用示例和最佳实践

### 5.1 替代二值信号量示例
```c
TaskHandle_t xTaskToNotify = NULL;

// 创建任务
void vTaskCreate_example(void)
{
    xTaskCreate(
        vHandlerTask,       // 任务函数
        "Handler",          // 任务名称
        1000,               // 堆栈大小
        NULL,               // 参数
        3,                  // 优先级
        &xTaskToNotify      // 任务句柄
    );
}

// 中断服务函数
void USART1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    // 清除中断标志
    // ...
    
    // 发送通知（相当于"give"信号量）
    vTaskNotifyGiveFromISR(xTaskToNotify, &xHigherPriorityTaskWoken);
    
    // 如果需要切换任务
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// 处理任务
void vHandlerTask(void *pvParameters)
{
    while (1)
    {
        // 等待通知（相当于"take"信号量）
        // 超时时间为最大值，清零模式
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        // 处理事件
        printf("Event received!\n");
    }
}
```

### 5.2 替代计数信号量示例
```c
TaskHandle_t xTaskToNotify = NULL;

// 生产者任务
void vProducerTask(void *pvParameters)
{
    while (1)
    {
        // 生产数据
        produce_data();
        
        // 发送通知（计数递增）
        xTaskNotifyGive(xTaskToNotify);
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// 消费者任务
void vConsumerTask(void *pvParameters)
{
    uint32_t ulNotificationValue;
    
    while (1)
    {
        // 等待通知（递减模式，类似计数信号量）
        ulNotificationValue = ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        
        if (ulNotificationValue > 0)
        {
            // 处理数据
            printf("Processing %lu items\n", ulNotificationValue);
            consume_data();
        }
    }
}
```

### 5.3 替代事件组示例
```c
TaskHandle_t xTaskToNotify = NULL;

// 定义事件位
#define BIT_0   (1 << 0)
#define BIT_1   (1 << 1)
#define BIT_2   (1 << 2)

// 发送事件
void vEventSender(void)
{
    // 设置事件位（按位或操作）
    xTaskNotify(xTaskToNotify, BIT_0 | BIT_1, eSetBits);
}

// 等待事件
void vEventReceiver(void *pvParameters)
{
    uint32_t ulNotificationValue;
    const uint32_t ulExpectedBits = BIT_0 | BIT_1 | BIT_2;
    
    while (1)
    {
        // 等待通知
        xTaskNotifyWait(
            0x00,               // 进入时不清除任何位
            0xFFFFFFFF,         // 退出时清除所有位
            &ulNotificationValue,
            portMAX_DELAY
        );
        
        // 检查接收到的位
        if ((ulNotificationValue & BIT_0) != 0)
        {
            printf("Bit 0 is set\n");
        }
        
        if ((ulNotificationValue & BIT_1) != 0)
        {
            printf("Bit 1 is set\n");
        }
        
        if ((ulNotificationValue & BIT_2) != 0)
        {
            printf("Bit 2 is set\n");
        }
    }
}
```

### 5.4 替代邮箱示例
```c
TaskHandle_t xTaskToNotify = NULL;

// 发送数据（覆盖模式）
void vDataSender(void *pvParameters)
{
    uint32_t ulDataToSend = 0;
    
    while (1)
    {
        // 准备数据
        ulDataToSend++;
        
        // 发送通知值（覆盖模式，类似邮箱）
        xTaskNotify(xTaskToNotify, ulDataToSend, eSetValueWithOverwrite);
        
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// 接收数据
void vDataReceiver(void *pvParameters)
{
    uint32_t ulReceivedValue;
    
    while (1)
    {
        // 等待通知
        if (xTaskNotifyWait(
                0x00,                   // 进入时不清除位
                0xFFFFFFFF,             // 退出时清除所有位
                &ulReceivedValue,
                portMAX_DELAY) == pdTRUE)
        {
            // 处理接收到的值
            printf("Received value: %lu\n", ulReceivedValue);
        }
    }
}
```

### 5.5 不覆盖模式示例
```c
TaskHandle_t xTaskToNotify = NULL;

// 发送任务
void vSenderTask(void *pvParameters)
{
    uint32_t ulValueToSend = 100;
    BaseType_t xResult;
    
    while (1)
    {
        // 仅在通知值被读取后才发送新值
        xResult = xTaskNotify(
            xTaskToNotify,
            ulValueToSend,
            eSetValueWithoutOverwrite
        );
        
        if (xResult == pdPASS)
        {
            printf("Value sent successfully\n");
            ulValueToSend++;
        }
        else
        {
            printf("Previous value not yet processed\n");
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// 接收任务
void vReceiverTask(void *pvParameters)
{
    uint32_t ulReceivedValue;
    
    while (1)
    {
        // 等待并获取通知值
        if (xTaskNotifyWait(
                0x00,
                0xFFFFFFFF,
                &ulReceivedValue,
                portMAX_DELAY) == pdTRUE)
        {
            printf("Processing value: %lu\n", ulReceivedValue);
            
            // 模拟处理时间
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}
```

### 5.6 中断到任务的高效通信
```c
TaskHandle_t xProcessingTask = NULL;
volatile uint32_t ulISRCounter = 0;

// 中断服务函数
void TIM2_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    // 清除中断标志
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    
    // 递增计数器
    ulISRCounter++;
    
    // 发送通知
    vTaskNotifyGiveFromISR(xProcessingTask, &xHigherPriorityTaskWoken);
    
    // 请求任务切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// 处理任务
void vProcessingTask(void *pvParameters)
{
    uint32_t ulNotificationCount;
    
    while (1)
    {
        // 等待通知
        ulNotificationCount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        // 处理事件
        printf("ISR triggered %lu times, counter = %lu\n", 
               ulNotificationCount, ulISRCounter);
    }
}
```
