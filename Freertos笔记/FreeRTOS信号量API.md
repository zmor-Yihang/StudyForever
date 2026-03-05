# FreeRTOS 信号量相关API和结构体完整总结

---

## 1. 数据类型和结构体

### 1.1 SemaphoreHandle_t - 信号量句柄类型
**类型定义：**
```c
typedef QueueHandle_t SemaphoreHandle_t;
```

**作用：**信号量句柄类型，用于引用和管理信号量

**注意：**
- NULL表示无效信号量
- 信号量本质上是队列的特殊实现
- 句柄在信号量删除后变为无效

---

### 1.2 StaticSemaphore_t - 静态信号量控制块
**结构体定义：**
```c
typedef struct xSTATIC_SEMAPHORE
{
    StaticQueue_t xStaticQueue;
} StaticSemaphore_t;
```

**作用：**静态信号量控制块结构体，用于静态信号量创建

**注意：**
- 实际结构体内容可能因配置而异
- 用户不应直接访问内部成员
- 静态创建时必须提供此类型的缓冲区

---

## 2. 二值信号量 (Binary Semaphore)

### 2.1 vSemaphoreCreateBinary() - 宏方式创建二值信号量（已弃用）
**宏定义：**
```c
#define vSemaphoreCreateBinary(xSemaphore)
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xSemaphore | SemaphoreHandle_t | 用于存储创建的信号量句柄 |

**返回值：**无（通过参数返回）

**作用：**创建二值信号量（旧版本API）

**注意：**
- 已弃用，建议使用xSemaphoreCreateBinary()
- 创建的信号量初始状态为空（不可用）

---

### 2.2 xSemaphoreCreateBinary() - 动态创建二值信号量
**函数原型：**
```c
SemaphoreHandle_t xSemaphoreCreateBinary(void);
```

**参数说明：**无

**返回值：**成功返回信号量句柄，失败返回NULL

**作用：**动态创建一个二值信号量

**宏配置要求：**
- configSUPPORT_DYNAMIC_ALLOCATION 必须为1
- 需要足够的堆内存

**注意：**
- 创建后信号量初始状态为空（需要先Give才能Take）
- 二值信号量只有两种状态：可用（1）和不可用（0）
- 常用于任务同步和中断同步

**典型应用场景：**
- 中断与任务同步
- 任务间的简单同步
- 资源访问控制（但互斥信号量更适合）

---

### 2.3 xSemaphoreCreateBinaryStatic() - 静态创建二值信号量
**函数原型：**
```c
SemaphoreHandle_t xSemaphoreCreateBinaryStatic(
    StaticSemaphore_t *pxSemaphoreBuffer
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pxSemaphoreBuffer | StaticSemaphore_t* | 信号量控制块缓冲区 |

**返回值：**成功返回信号量句柄，失败返回NULL

**作用：**静态创建二值信号量，使用用户提供的内存缓冲区

**宏配置要求：**
- configSUPPORT_STATIC_ALLOCATION 必须为1

**注意：**
- 需要用户提供控制块缓冲区
- 创建后信号量初始状态为空
- 适用于不允许动态内存分配的系统

---

## 3. 计数型信号量 (Counting Semaphore)

### 3.1 xSemaphoreCreateCounting() - 动态创建计数型信号量
**函数原型：**
```c
SemaphoreHandle_t xSemaphoreCreateCounting(
    UBaseType_t uxMaxCount,
    UBaseType_t uxInitialCount
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| uxMaxCount | UBaseType_t | 最大计数值（必须>0） |
| uxInitialCount | UBaseType_t | 初始计数值（≤uxMaxCount） |

**返回值：**成功返回信号量句柄，失败返回NULL

**作用：**动态创建一个计数型信号量

**宏配置要求：**
- configSUPPORT_DYNAMIC_ALLOCATION 必须为1
- 需要足够的堆内存

**注意：**
- 计数值范围：0 到 uxMaxCount
- 每次Give操作计数+1（不超过最大值）
- 每次Take操作计数-1（最小为0）

**典型应用场景：**
- 事件计数
- 资源管理（如管理多个相同资源）
- 多生产者-多消费者模型

---

### 3.2 xSemaphoreCreateCountingStatic() - 静态创建计数型信号量
**函数原型：**
```c
SemaphoreHandle_t xSemaphoreCreateCountingStatic(
    UBaseType_t uxMaxCount,
    UBaseType_t uxInitialCount,
    StaticSemaphore_t *pxSemaphoreBuffer
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| uxMaxCount | UBaseType_t | 最大计数值 |
| uxInitialCount | UBaseType_t | 初始计数值 |
| pxSemaphoreBuffer | StaticSemaphore_t* | 信号量控制块缓冲区 |

**返回值：**成功返回信号量句柄，失败返回NULL

**作用：**静态创建计数型信号量，使用用户提供的内存缓冲区

**宏配置要求：**
- configSUPPORT_STATIC_ALLOCATION 必须为1

**注意：**
- 需要用户提供控制块缓冲区
- 适用于不允许动态内存分配的系统

---

## 4. 互斥信号量 (Mutex)

### 4.1 xSemaphoreCreateMutex() - 动态创建互斥信号量
**函数原型：**
```c
SemaphoreHandle_t xSemaphoreCreateMutex(void);
```

**参数说明：**无

**返回值：**成功返回互斥信号量句柄，失败返回NULL

**作用：**动态创建一个互斥信号量

**宏配置要求：**
- configSUPPORT_DYNAMIC_ALLOCATION 必须为1
- configUSE_MUTEXES 必须为1
- 需要足够的堆内存

**注意：**
- 创建后信号量初始状态为可用（已Give）
- 支持优先级继承机制
- 必须由同一任务Take和Give（递归互斥量除外）
- 不能在中断服务程序中使用

**典型应用场景：**
- 保护共享资源（临界区保护）
- 防止多任务同时访问共享变量
- 确保资源的互斥访问

---

### 4.2 xSemaphoreCreateMutexStatic() - 静态创建互斥信号量
**函数原型：**
```c
SemaphoreHandle_t xSemaphoreCreateMutexStatic(
    StaticSemaphore_t *pxMutexBuffer
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pxMutexBuffer | StaticSemaphore_t* | 互斥信号量控制块缓冲区 |

**返回值：**成功返回互斥信号量句柄，失败返回NULL

**作用：**静态创建互斥信号量，使用用户提供的内存缓冲区

**宏配置要求：**
- configSUPPORT_STATIC_ALLOCATION 必须为1
- configUSE_MUTEXES 必须为1

**注意：**
- 需要用户提供控制块缓冲区
- 创建后信号量初始状态为可用
- 适用于不允许动态内存分配的系统

---

### 4.3 xSemaphoreCreateRecursiveMutex() - 动态创建递归互斥信号量
**函数原型：**
```c
SemaphoreHandle_t xSemaphoreCreateRecursiveMutex(void);
```

**参数说明：**无

**返回值：**成功返回递归互斥信号量句柄，失败返回NULL

**作用：**动态创建一个递归互斥信号量

**宏配置要求：**
- configSUPPORT_DYNAMIC_ALLOCATION 必须为1
- configUSE_RECURSIVE_MUTEXES 必须为1
- 需要足够的堆内存

**注意：**
- 允许同一任务多次Take同一个互斥信号量
- Take和Give次数必须匹配
- 支持优先级继承
- 不能在中断服务程序中使用

**典型应用场景：**
- 递归函数中的资源保护
- 函数调用链中需要多次获取同一资源

---

### 4.4 xSemaphoreCreateRecursiveMutexStatic() - 静态创建递归互斥信号量
**函数原型：**
```c
SemaphoreHandle_t xSemaphoreCreateRecursiveMutexStatic(
    StaticSemaphore_t *pxMutexBuffer
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pxMutexBuffer | StaticSemaphore_t* | 递归互斥信号量控制块缓冲区 |

**返回值：**成功返回递归互斥信号量句柄，失败返回NULL

**作用：**静态创建递归互斥信号量，使用用户提供的内存缓冲区

**宏配置要求：**
- configSUPPORT_STATIC_ALLOCATION 必须为1
- configUSE_RECURSIVE_MUTEXES 必须为1

**注意：**
- 需要用户提供控制块缓冲区
- 适用于不允许动态内存分配的系统

---

## 5. 信号量删除操作

### 5.1 vSemaphoreDelete() - 删除信号量
**宏定义：**
```c
#define vSemaphoreDelete(xSemaphore) vQueueDelete((QueueHandle_t)(xSemaphore))
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xSemaphore | SemaphoreHandle_t | 要删除的信号量句柄 |

**返回值：**无

**作用：**删除信号量并释放相关资源

**注意：**
- 只能删除动态创建的信号量
- 删除后所有等待该信号量的任务将被唤醒
- 不要删除正在被任务使用的信号量
- 互斥信号量不应该在被持有时删除

---

## 6. 信号量获取操作（Take）

### 6.1 xSemaphoreTake() - 获取信号量
**宏定义：**
```c
#define xSemaphoreTake(xSemaphore, xBlockTime) \
        xQueueSemaphoreTake((xSemaphore), (xBlockTime))
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xSemaphore | SemaphoreHandle_t | 信号量句柄 |
| xBlockTime | TickType_t | 最大等待时间（tick数） |

**返回值：**pdTRUE表示成功获取，pdFALSE表示超时失败

**作用：**获取（占用）信号量

**注意：**
- 适用于二值信号量、计数型信号量和互斥信号量
- 不适用于递归互斥信号量（使用xSemaphoreTakeRecursive）
- 如果信号量不可用且xBlockTime>0，任务将阻塞
- xBlockTime可以设为portMAX_DELAY表示无限等待
- 互斥信号量不能在中断中使用

**使用场景：**
- 二值信号量：等待事件发生
- 计数型信号量：获取资源
- 互斥信号量：进入临界区

---

### 6.2 xSemaphoreTakeRecursive() - 递归获取互斥信号量
**宏定义：**
```c
#define xSemaphoreTakeRecursive(xMutex, xBlockTime) \
        xQueueTakeMutexRecursive((xMutex), (xBlockTime))
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xMutex | SemaphoreHandle_t | 递归互斥信号量句柄 |
| xBlockTime | TickType_t | 最大等待时间 |

**返回值：**pdTRUE表示成功获取，pdFALSE表示超时失败

**作用：**递归获取互斥信号量

**宏配置要求：**
- configUSE_RECURSIVE_MUTEXES 必须为1

**注意：**
- 只能用于递归互斥信号量
- 同一任务可以多次调用
- Take和Give次数必须匹配
- 不能在中断中使用

---

### 6.3 xSemaphoreTakeFromISR() - 中断中获取信号量（不推荐）
**函数原型：**
```c
BaseType_t xSemaphoreTakeFromISR(
    SemaphoreHandle_t xSemaphore,
    BaseType_t *pxHigherPriorityTaskWoken
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xSemaphore | SemaphoreHandle_t | 信号量句柄 |
| pxHigherPriorityTaskWoken | BaseType_t* | 是否需要上下文切换 |

**返回值：**pdTRUE表示成功，pdFALSE表示失败

**作用：**在中断服务程序中获取信号量

**注意：**
- 不推荐使用，通常在中断中应该Give而不是Take
- 不能指定阻塞时间
- 只能用于二值和计数型信号量
- 不能用于互斥信号量

---

## 7. 信号量释放操作（Give）

### 7.1 xSemaphoreGive() - 释放信号量
**宏定义：**
```c
#define xSemaphoreGive(xSemaphore) \
        xQueueGenericSend((QueueHandle_t)(xSemaphore), NULL, semGIVE_BLOCK_TIME, queueSEND_TO_BACK)
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xSemaphore | SemaphoreHandle_t | 信号量句柄 |

**返回值：**pdTRUE表示成功，pdFALSE表示失败

**作用：**释放（给出）信号量

**注意：**
- 适用于二值信号量、计数型信号量和互斥信号量
- 不适用于递归互斥信号量（使用xSemaphoreGiveRecursive）
- 二值信号量Give后状态变为可用
- 计数型信号量Give后计数+1（不超过最大值）
- 互斥信号量必须由获取它的任务释放
- 不能在中断中释放互斥信号量

**使用场景：**
- 二值信号量：通知事件发生
- 计数型信号量：释放资源
- 互斥信号量：退出临界区

---

### 7.2 xSemaphoreGiveRecursive() - 递归释放互斥信号量
**宏定义：**
```c
#define xSemaphoreGiveRecursive(xMutex) \
        xQueueGiveMutexRecursive((xMutex))
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xMutex | SemaphoreHandle_t | 递归互斥信号量句柄 |

**返回值：**pdTRUE表示成功，pdFALSE表示失败

**作用：**递归释放互斥信号量

**宏配置要求：**
- configUSE_RECURSIVE_MUTEXES 必须为1

**注意：**
- 只能用于递归互斥信号量
- 必须由获取它的任务释放
- Take和Give次数必须匹配
- 不能在中断中使用

---

### 7.3 xSemaphoreGiveFromISR() - 中断中释放信号量
**宏定义：**
```c
#define xSemaphoreGiveFromISR(xSemaphore, pxHigherPriorityTaskWoken) \
        xQueueGiveFromISR((QueueHandle_t)(xSemaphore), (pxHigherPriorityTaskWoken))
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xSemaphore | SemaphoreHandle_t | 信号量句柄 |
| pxHigherPriorityTaskWoken | BaseType_t* | 是否需要上下文切换标志 |

**返回值：**pdTRUE表示成功，pdFALSE表示失败（如队列满）

**作用：**在中断服务程序中释放信号量

**注意：**
- 常用于中断与任务同步
- 只能用于二值和计数型信号量
- 不能用于互斥信号量
- 调用后应检查pxHigherPriorityTaskWoken并调用portYIELD_FROM_ISR

**使用示例：**
```c
void vISR_Handler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    // 释放信号量
    xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
    
    // 如果需要，执行上下文切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
```

---

## 8. 信号量状态查询

### 8.1 uxSemaphoreGetCount() - 获取信号量计数值
**宏定义：**
```c
#define uxSemaphoreGetCount(xSemaphore) \
        uxQueueMessagesWaiting((QueueHandle_t)(xSemaphore))
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xSemaphore | SemaphoreHandle_t | 信号量句柄 |

**返回值：**信号量当前计数值

**作用：**获取信号量的当前计数值

**注意：**
- 二值信号量：返回0或1
- 计数型信号量：返回当前计数值（0到最大值）
- 互斥信号量：返回0（被占用）或1（可用）
- 返回值在多任务环境中可能立即失效

---

### 8.2 uxSemaphoreGetCountFromISR() - 中断中获取信号量计数值
**宏定义：**
```c
#define uxSemaphoreGetCountFromISR(xSemaphore) \
        uxQueueMessagesWaitingFromISR((QueueHandle_t)(xSemaphore))
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xSemaphore | SemaphoreHandle_t | 信号量句柄 |

**返回值：**信号量当前计数值

**作用：**在中断服务程序中获取信号量的当前计数值

**注意：**
- 可以安全地在ISR中调用
- 返回值含义与uxSemaphoreGetCount相同

---

## 9. 互斥信号量特殊功能

### 9.1 xSemaphoreGetMutexHolder() - 获取互斥信号量持有者
**函数原型：**
```c
TaskHandle_t xSemaphoreGetMutexHolder(SemaphoreHandle_t xMutex);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xMutex | SemaphoreHandle_t | 互斥信号量句柄 |

**返回值：**持有互斥信号量的任务句柄，如果未被持有则返回NULL

**作用：**获取当前持有互斥信号量的任务

**宏配置要求：**
- configUSE_MUTEXES 必须为1

**注意：**
- 只能用于互斥信号量和递归互斥信号量
- 不能用于二值和计数型信号量
- 主要用于调试和诊断

---

### 9.2 xSemaphoreGetMutexHolderFromISR() - 中断中获取互斥信号量持有者
**函数原型：**
```c
TaskHandle_t xSemaphoreGetMutexHolderFromISR(SemaphoreHandle_t xMutex);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xMutex | SemaphoreHandle_t | 互斥信号量句柄 |

**返回值：**持有互斥信号量的任务句柄

**作用：**在中断服务程序中获取当前持有互斥信号量的任务

**宏配置要求：**
- configUSE_MUTEXES 必须为1

**注意：**
- 可以安全地在ISR中调用
- 主要用于调试

---

## 10. 信号量使用注意事项和最佳实践

### 10.1 二值信号量使用注意事项
**使用场景：**
- ✅ 中断与任务同步
- ✅ 任务间的简单同步
- ❌ 不适合保护共享资源（应使用互斥信号量）

**最佳实践：**
1. 通常在中断中Give，任务中Take
2. 创建后初始状态为空，需要先Give
3. 不能用于递归锁定

---

### 10.2 计数型信号量使用注意事项
**使用场景：**
- ✅ 管理多个相同资源
- ✅ 事件计数
- ✅ 多生产者-多消费者模型

**最佳实践：**
1. 最大计数值应根据实际资源数量设置
2. 初始计数值通常设为资源的初始可用数量
3. 避免Give操作超过最大计数值

---

### 10.3 互斥信号量使用注意事项
**使用场景：**
- ✅ 保护共享资源（临界区保护）
- ✅ 防止多任务同时访问共享变量
- ❌ 不能在中断中使用

**最佳实践：**
1. 必须由同一任务Take和Give
2. 尽量缩短临界区长度
3. 避免在持有互斥信号量时调用阻塞函数
4. 利用优先级继承防止优先级反转

**优先级继承：**
- 当高优先级任务等待低优先级任务持有的互斥信号量时
- 低优先级任务的优先级暂时提升到高优先级任务的级别
- 防止优先级反转问题

---

### 10.4 递归互斥信号量使用注意事项
**使用场景：**
- ✅ 递归函数中的资源保护
- ✅ 函数调用链中需要多次获取同一资源

**最佳实践：**
1. Take和Give次数必须严格匹配
2. 只有在确实需要递归锁定时才使用
3. 普通互斥信号量性能更好，优先使用

---

## 11. 信号量类型对比总结

| 特性 | 二值信号量 | 计数型信号量 | 互斥信号量 | 递归互斥信号量 |
|------|-----------|-------------|-----------|--------------|
| 计数范围 | 0-1 | 0-最大值 | 0-1 | 0-1（每次+1） |
| 初始状态 | 空（0） | 可设置 | 满（1） | 满（1） |
| 中断中Give | ✅ 支持 | ✅ 支持 | ❌ 不支持 | ❌ 不支持 |
| 中断中Take | ⚠️ 不推荐 | ⚠️ 不推荐 | ❌ 不支持 | ❌ 不支持 |
| 优先级继承 | ❌ 不支持 | ❌ 不支持 | ✅ 支持 | ✅ 支持 |
| 递归锁定 | ❌ 不支持 | ❌ 不支持 | ❌ 不支持 | ✅ 支持 |
| 所有权检查 | ❌ 无 | ❌ 无 | ✅ 有 | ✅ 有 |
| 主要用途 | 同步 | 资源管理/计数 | 互斥访问 | 递归互斥访问 |

---

## 12. 常见配置宏

| 宏定义 | 说明 | 默认值 |
|--------|------|--------|
| configSUPPORT_DYNAMIC_ALLOCATION | 支持动态内存分配 | 1 |
| configSUPPORT_STATIC_ALLOCATION | 支持静态内存分配 | 0 |
| configUSE_MUTEXES | 使能互斥信号量 | 1 |
| configUSE_RECURSIVE_MUTEXES | 使能递归互斥信号量 | 0 |
| configUSE_COUNTING_SEMAPHORES | 使能计数型信号量 | 1 |

---

## 13. 常见错误和解决方法

### 13.1 优先级反转问题
**问题描述：**低优先级任务持有互斥信号量，中优先级任务抢占，导致高优先级任务长时间等待

**解决方法：**
- 使用互斥信号量（自动支持优先级继承）
- 不要使用二值信号量保护共享资源

---

### 13.2 死锁问题
**问题描述：**两个或多个任务互相等待对方持有的资源

**解决方法：**
- 按固定顺序获取多个互斥信号量
- 使用超时机制
- 避免嵌套锁定

---

### 13.3 忘记释放信号量
**问题描述：**任务获取信号量后忘记释放，导致其他任务永久阻塞

**解决方法：**
- 使用代码审查
- 确保所有路径都释放信号量
- 考虑使用RAII模式（C++）

---

### 13.4 在中断中使用互斥信号量
**问题描述：**在中断服务程序中使用互斥信号量导致系统崩溃

**解决方法：**
- 中断中只使用二值或计数型信号量
- 使用FromISR版本的API
- 互斥信号量只在任务中使用

---

## 14. 使用示例

### 14.1 二值信号量 - 中断与任务同步
```c
SemaphoreHandle_t xBinarySemaphore;

// 创建信号量
void vInitSemaphore(void)
{
    xBinarySemaphore = xSemaphoreCreateBinary();
    if(xBinarySemaphore == NULL) {
        // 创建失败处理
    }
}

// 中断服务程序
void vISRHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    // 释放信号量，通知任务
    xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
    
    // 如果需要上下文切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// 任务
void vTaskFunction(void *pvParameters)
{
    while(1)
    {
        // 等待信号量
        if(xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE)
        {
            // 处理中断事件
        }
    }
}
```

---

### 14.2 计数型信号量 - 资源管理
```c
#define MAX_RESOURCES 5

SemaphoreHandle_t xCountingSemaphore;

// 创建信号量
void vInitSemaphore(void)
{
    // 创建计数型信号量，最大值5，初始值5（5个资源可用）
    xCountingSemaphore = xSemaphoreCreateCounting(MAX_RESOURCES, MAX_RESOURCES);
    if(xCountingSemaphore == NULL) {
        // 创建失败处理
    }
}

// 生产者任务
void vProducerTask(void *pvParameters)
{
    while(1)
    {
        // 生产资源
        ProduceResource();
        
        // 释放信号量（增加可用资源计数）
        xSemaphoreGive(xCountingSemaphore);
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// 消费者任务
void vConsumerTask(void *pvParameters)
{
    while(1)
    {
        // 获取资源（等待资源可用）
        if(xSemaphoreTake(xCountingSemaphore, pdMS_TO_TICKS(1000)) == pdTRUE)
        {
            // 使用资源
            UseResource();
        }
        else
        {
            // 超时处理
        }
    }
}
```

---

### 14.3 互斥信号量 - 保护共享资源
```c
SemaphoreHandle_t xMutex;
int sharedVariable = 0;

// 创建互斥信号量
void vInitMutex(void)
{
    xMutex = xSemaphoreCreateMutex();
    if(xMutex == NULL) {
        // 创建失败处理
    }
}

// 任务1
void vTask1(void *pvParameters)
{
    while(1)
    {
        // 获取互斥信号量
        if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
        {
            // 临界区：访问共享变量
            sharedVariable++;
            
            // 释放互斥信号量
            xSemaphoreGive(xMutex);
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// 任务2
void vTask2(void *pvParameters)
{
    while(1)
    {
        // 获取互斥信号量
        if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
        {
            // 临界区：访问共享变量
            int temp = sharedVariable;
            vPrintNumber(temp);
            
            // 释放互斥信号量
            xSemaphoreGive(xMutex);
        }
        
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
```

---

### 14.4 递归互斥信号量 - 递归函数保护
```c
SemaphoreHandle_t xRecursiveMutex;
int recursiveCounter = 0;

// 创建递归互斥信号量
void vInitRecursiveMutex(void)
{
    xRecursiveMutex = xSemaphoreCreateRecursiveMutex();
    if(xRecursiveMutex == NULL) {
        // 创建失败处理
    }
}

// 递归函数
void vRecursiveFunction(int depth)
{
    // 获取递归互斥信号量
    if(xSemaphoreTakeRecursive(xRecursiveMutex, portMAX_DELAY) == pdTRUE)
    {
        // 访问共享资源
        recursiveCounter++;
        
        if(depth > 0)
        {
            // 递归调用（会再次获取同一个互斥信号量）
            vRecursiveFunction(depth - 1);
        }
        
        // 释放递归互斥信号量
        xSemaphoreGiveRecursive(xRecursiveMutex);
    }
}

// 任务
void vTask(void *pvParameters)
{
    while(1)
    {
        vRecursiveFunction(3);  // 递归深度为3
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

---

### 14.5 静态创建信号量示例
```c
// 静态缓冲区
StaticSemaphore_t xBinarySemaphoreBuffer;
StaticSemaphore_t xMutexBuffer;

SemaphoreHandle_t xBinarySemaphore;
SemaphoreHandle_t xMutex;

void vInitStaticSemaphores(void)
{
    // 静态创建二值信号量
    xBinarySemaphore = xSemaphoreCreateBinaryStatic(&xBinarySemaphoreBuffer);
    
    // 静态创建互斥信号量
    xMutex = xSemaphoreCreateMutexStatic(&xMutexBuffer);
    
    if(xBinarySemaphore == NULL || xMutex == NULL) {
        // 创建失败处理（通常不会失败）
    }
}
```

## 15. 现代 FreeRTOS 开发推荐组合

| 需求                       | 推荐机制                                           |
| -------------------------- | -------------------------------------------------- |
| 传递结构体/数组等数据      | **队列**                                           |
| 中断唤醒单个任务（无数据） | **任务通知**                                       |
| 多个任务等待同一组事件     | **事件标志组**                                     |
| 保护共享资源（多优先级）   | **互斥信号量**（不可替代！）                       |
| 管理 N 个相同资源          | **计数信号量**或**任务通知（计数模式）**           |
| 单任务监听多个数据队列     | **队列集**（若需数据）或**任务通知**（若只需通知） |
