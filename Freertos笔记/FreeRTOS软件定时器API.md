# FreeRTOS 软件定时器相关API和结构体完整总结

---

## 1. 数据类型和结构体

### 1.1 TimerHandle_t - 定时器句柄类型
**类型定义：**
```c
typedef void * TimerHandle_t;
```

**作用：**定时器句柄类型，用于引用和管理软件定时器

**注意：**
- NULL表示无效定时器
- 句柄在定时器删除后变为无效

---

### 1.2 TimerCallbackFunction_t - 定时器回调函数类型
**类型定义：**
```c
typedef void (*TimerCallbackFunction_t)(TimerHandle_t xTimer);
```

**作用：**定时器回调函数的函数指针类型

**函数原型：**
```c
void vTimerCallback(TimerHandle_t xTimer)
{
    // 定时器到期时执行的代码
}
```

**注意：**
- 回调函数在定时器服务任务上下文中执行
- 回调函数应尽快返回，不应阻塞
- 可以在回调中调用FreeRTOS API

---

### 1.3 StaticTimer_t - 静态定时器控制块
**结构体定义：**
```c
typedef struct xSTATIC_TIMER
{
    void *pvDummy1;
    StaticListItem_t xDummy2;
    TickType_t xDummy3;
    void *pvDummy4;
    UBaseType_t uxDummy5;
    #if ( configUSE_TRACE_FACILITY == 1 )
        UBaseType_t uxDummy6;
    #endif
    #if( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )
        uint8_t ucDummy7;
    #endif
} StaticTimer_t;
```

**作用：**静态定时器控制块结构体，用于静态定时器创建

**注意：**
- 实际结构体内容可能因配置而异
- 用户不应直接访问内部成员

---

## 2. 定时器类型

### 2.1 单次定时器（One-shot Timer）
**特点：**
- 定时器启动后只执行一次回调函数
- 回调执行后定时器自动进入休眠状态
- 需要手动重启才能再次运行

**使用场景：**
- 延时执行某个操作
- 超时检测
- 单次事件触发

---

### 2.2 周期定时器（Auto-reload Timer）
**特点：**
- 定时器启动后周期性执行回调函数
- 回调执行后自动重新装载并继续运行
- 除非手动停止，否则一直运行

**使用场景：**
- 周期性采样
- 定期状态检查
- LED闪烁等周期性任务

---

## 3. 定时器创建和删除

### 3.1 xTimerCreate() - 动态创建定时器
**函数原型：**
```c
TimerHandle_t xTimerCreate(
    const char * const pcTimerName,
    const TickType_t xTimerPeriodInTicks,
    const UBaseType_t uxAutoReload,
    void * const pvTimerID,
    TimerCallbackFunction_t pxCallbackFunction
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pcTimerName | const char* | 定时器名称（用于调试） |
| xTimerPeriodInTicks | TickType_t | 定时器周期（时钟节拍数） |
| uxAutoReload | UBaseType_t | pdTRUE=周期定时器，pdFALSE=单次定时器 |
| pvTimerID | void* | 定时器ID（用户自定义标识） |
| pxCallbackFunction | TimerCallbackFunction_t | 回调函数指针 |

**返回值：**成功返回定时器句柄，失败返回NULL

**作用：**动态创建一个新的软件定时器

**宏配置要求：**
- configUSE_TIMERS 必须为1
- configSUPPORT_DYNAMIC_ALLOCATION 必须为1

**注意：**
- 创建后定时器处于休眠状态
- 需要调用xTimerStart()启动
- 定时器周期使用pdMS_TO_TICKS()宏转换

---

### 3.2 xTimerCreateStatic() - 静态创建定时器
**函数原型：**
```c
TimerHandle_t xTimerCreateStatic(
    const char * const pcTimerName,
    const TickType_t xTimerPeriodInTicks,
    const UBaseType_t uxAutoReload,
    void * const pvTimerID,
    TimerCallbackFunction_t pxCallbackFunction,
    StaticTimer_t *pxTimerBuffer
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pcTimerName | const char* | 定时器名称 |
| xTimerPeriodInTicks | TickType_t | 定时器周期 |
| uxAutoReload | UBaseType_t | pdTRUE=周期，pdFALSE=单次 |
| pvTimerID | void* | 定时器ID |
| pxCallbackFunction | TimerCallbackFunction_t | 回调函数指针 |
| pxTimerBuffer | StaticTimer_t* | 定时器控制块缓冲区 |

**返回值：**成功返回定时器句柄，失败返回NULL

**作用：**静态创建定时器，使用用户提供的控制块缓冲区

**宏配置要求：**
- configUSE_TIMERS 必须为1
- configSUPPORT_STATIC_ALLOCATION 必须为1

**注意：**
- 需要用户提供StaticTimer_t类型的缓冲区
- 缓冲区必须持续有效

---

### 3.3 xTimerDelete() - 删除定时器
**函数原型：**
```c
BaseType_t xTimerDelete(
    TimerHandle_t xTimer,
    TickType_t xTicksToWait
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 要删除的定时器句柄 |
| xTicksToWait | TickType_t | 命令队列满时的等待时间 |

**返回值：**
- pdPASS：命令成功发送到定时器命令队列
- pdFAIL：命令发送失败

**作用：**删除软件定时器并释放相关资源

**注意：**
- 删除命令通过队列发送到定时器服务任务
- 如果定时器正在运行，会先停止再删除
- 静态创建的定时器不会释放内存

---

## 4. 定时器控制操作

### 4.1 xTimerStart() - 启动定时器
**函数原型：**
```c
BaseType_t xTimerStart(
    TimerHandle_t xTimer,
    TickType_t xTicksToWait
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 定时器句柄 |
| xTicksToWait | TickType_t | 命令队列满时的等待时间 |

**返回值：**
- pdPASS：命令成功发送
- pdFAIL：命令发送失败

**作用：**启动定时器或重启已停止的定时器

**注意：**
- 如果定时器已经运行，此函数会重置定时器
- 定时器从调用此函数时开始计时

---

### 4.2 xTimerStartFromISR() - 中断中启动定时器
**函数原型：**
```c
BaseType_t xTimerStartFromISR(
    TimerHandle_t xTimer,
    BaseType_t *pxHigherPriorityTaskWoken
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 定时器句柄 |
| pxHigherPriorityTaskWoken | BaseType_t* | 高优先级任务唤醒标志 |

**返回值：**
- pdPASS：命令成功发送
- pdFAIL：命令发送失败

**作用：**在中断服务函数中启动定时器

**注意：**
- 必须在中断中使用此版本
- 需要检查pxHigherPriorityTaskWoken并执行任务切换

---

### 4.3 xTimerStop() - 停止定时器
**函数原型：**
```c
BaseType_t xTimerStop(
    TimerHandle_t xTimer,
    TickType_t xTicksToWait
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 定时器句柄 |
| xTicksToWait | TickType_t | 命令队列满时的等待时间 |

**返回值：**
- pdPASS：命令成功发送
- pdFAIL：命令发送失败

**作用：**停止正在运行的定时器

**注意：**
- 定时器停止后不会触发回调
- 可以使用xTimerStart()重新启动

---

### 4.4 xTimerStopFromISR() - 中断中停止定时器
**函数原型：**
```c
BaseType_t xTimerStopFromISR(
    TimerHandle_t xTimer,
    BaseType_t *pxHigherPriorityTaskWoken
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 定时器句柄 |
| pxHigherPriorityTaskWoken | BaseType_t* | 高优先级任务唤醒标志 |

**返回值：**
- pdPASS：命令成功发送
- pdFAIL：命令发送失败

**作用：**在中断服务函数中停止定时器

---

### 4.5 xTimerReset() - 重置定时器
**函数原型：**
```c
BaseType_t xTimerReset(
    TimerHandle_t xTimer,
    TickType_t xTicksToWait
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 定时器句柄 |
| xTicksToWait | TickType_t | 命令队列满时的等待时间 |

**返回值：**
- pdPASS：命令成功发送
- pdFAIL：命令发送失败

**作用：**重置定时器的计时周期

**注意：**
- 如果定时器已运行，重新开始计时
- 如果定时器已停止，启动定时器
- 常用于看门狗定时器场景

---

### 4.6 xTimerResetFromISR() - 中断中重置定时器
**函数原型：**
```c
BaseType_t xTimerResetFromISR(
    TimerHandle_t xTimer,
    BaseType_t *pxHigherPriorityTaskWoken
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 定时器句柄 |
| pxHigherPriorityTaskWoken | BaseType_t* | 高优先级任务唤醒标志 |

**返回值：**
- pdPASS：命令成功发送
- pdFAIL：命令发送失败

**作用：**在中断服务函数中重置定时器

---

### 4.7 xTimerChangePeriod() - 改变定时器周期
**函数原型：**
```c
BaseType_t xTimerChangePeriod(
    TimerHandle_t xTimer,
    TickType_t xNewPeriod,
    TickType_t xTicksToWait
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 定时器句柄 |
| xNewPeriod | TickType_t | 新的定时器周期 |
| xTicksToWait | TickType_t | 命令队列满时的等待时间 |

**返回值：**
- pdPASS：命令成功发送
- pdFAIL：命令发送失败

**作用：**改变定时器的周期时间

**注意：**
- 如果定时器正在运行，改变周期后会重新开始计时
- 如果定时器已停止，改变周期后会启动定时器

---

### 4.8 xTimerChangePeriodFromISR() - 中断中改变定时器周期
**函数原型：**
```c
BaseType_t xTimerChangePeriodFromISR(
    TimerHandle_t xTimer,
    TickType_t xNewPeriod,
    BaseType_t *pxHigherPriorityTaskWoken
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 定时器句柄 |
| xNewPeriod | TickType_t | 新的定时器周期 |
| pxHigherPriorityTaskWoken | BaseType_t* | 高优先级任务唤醒标志 |

**返回值：**
- pdPASS：命令成功发送
- pdFAIL：命令发送失败

**作用：**在中断服务函数中改变定时器周期

---

## 5. 定时器查询操作

### 5.1 xTimerIsTimerActive() - 查询定时器是否激活
**函数原型：**
```c
BaseType_t xTimerIsTimerActive(TimerHandle_t xTimer);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 定时器句柄 |

**返回值：**
- pdTRUE：定时器处于活动状态
- pdFALSE：定时器处于休眠状态

**作用：**查询定时器是否处于活动（运行）状态

**注意：**
- 活动状态表示定时器正在计时
- 休眠状态表示定时器已停止或未启动

---

### 5.2 pvTimerGetTimerID() - 获取定时器ID
**函数原型：**
```c
void *pvTimerGetTimerID(const TimerHandle_t xTimer);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 定时器句柄 |

**返回值：**返回定时器的ID（void*类型）

**作用：**获取定时器创建时设置的ID

**使用场景：**
- 区分使用同一回调函数的不同定时器
- 存储与定时器相关的应用数据指针

---

### 5.3 vTimerSetTimerID() - 设置定时器ID
**函数原型：**
```c
void vTimerSetTimerID(
    TimerHandle_t xTimer,
    void *pvNewID
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 定时器句柄 |
| pvNewID | void* | 新的定时器ID |

**返回值：**无

**作用：**设置定时器的ID

**注意：**
- 可以在运行时动态改变定时器ID
- 不影响定时器的运行状态

---

### 5.4 xTimerGetPeriod() - 获取定时器周期
**函数原型：**
```c
TickType_t xTimerGetPeriod(TimerHandle_t xTimer);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 定时器句柄 |

**返回值：**返回定时器周期（时钟节拍数）

**作用：**获取定时器的周期时间

---

### 5.5 xTimerGetExpiryTime() - 获取定时器到期时间
**函数原型：**
```c
TickType_t xTimerGetExpiryTime(TimerHandle_t xTimer);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 定时器句柄 |

**返回值：**返回定时器将到期的时间（时钟节拍绝对值）

**作用：**获取定时器下次到期的绝对时间

**注意：**
- 返回值是系统启动以来的时钟节拍数
- 只对活动的定时器有意义

---

### 5.6 pcTimerGetName() - 获取定时器名称
**函数原型：**
```c
const char *pcTimerGetName(TimerHandle_t xTimer);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimer | TimerHandle_t | 定时器句柄 |

**返回值：**返回定时器名称字符串指针

**作用：**获取定时器创建时设置的名称

**使用场景：**
- 调试和诊断
- 日志记录

---

## 6. 定时器守护任务配置

### 6.1 定时器服务任务（Timer Daemon Task）
**说明：**
- FreeRTOS使用一个专用任务来管理所有软件定时器
- 这个任务称为定时器服务任务或定时器守护任务
- 所有定时器回调函数都在此任务的上下文中执行

**特点：**
- 自动创建（当configUSE_TIMERS为1时）
- 通过命令队列接收定时器命令
- 管理所有定时器的状态和到期

---

### 6.2 定时器相关配置宏
```c
// FreeRTOSConfig.h中的配置

// 启用软件定时器功能
#define configUSE_TIMERS                        1

// 定时器服务任务优先级（建议设置为最高优先级）
#define configTIMER_TASK_PRIORITY               (configMAX_PRIORITIES - 1)

// 定时器命令队列长度
#define configTIMER_QUEUE_LENGTH                10

// 定时器服务任务堆栈大小
#define configTIMER_TASK_STACK_DEPTH            configMINIMAL_STACK_SIZE
```

**配置说明：**
- **configUSE_TIMERS**：必须设置为1才能使用定时器
- **configTIMER_TASK_PRIORITY**：定时器任务优先级，影响回调执行的及时性
- **configTIMER_QUEUE_LENGTH**：命令队列长度，影响可挂起的命令数
- **configTIMER_TASK_STACK_DEPTH**：定时器任务堆栈大小，需考虑回调函数的堆栈需求
