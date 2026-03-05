# FreeRTOS 事件标志组相关API和结构体完整总结

---

## 1. 数据类型和结构体

### 1.1 EventGroupHandle_t - 事件标志组句柄类型
**类型定义：**
```c
typedef void * EventGroupHandle_t;
```

**作用：**事件标志组句柄类型，用于引用和管理事件标志组

**注意：**
- NULL表示无效事件标志组
- 句柄在事件标志组删除后变为无效

---

### 1.2 EventBits_t - 事件位类型
**类型定义：**
```c
typedef TickType_t EventBits_t;
```

**作用：**事件位类型，用于表示事件标志组中的位

**注意：**
- 通常为24位可用（最高位保留给内核使用）
- 每一位代表一个事件状态

---

### 1.3 StaticEventGroup_t - 静态事件标志组控制块
**结构体定义：**
```c
typedef struct xSTATIC_EVENT_GROUP
{
    TickType_t xDummy1;
    StaticList_t xDummy2;
    #if( configUSE_TRACE_FACILITY == 1 )
        UBaseType_t uxDummy3;
    #endif
    #if( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) )
        uint8_t ucDummy4;
    #endif
} StaticEventGroup_t;
```

**作用：**静态事件标志组控制块结构体，用于静态事件标志组创建

**注意：**
- 实际结构体内容可能因配置而异
- 用户不应直接访问内部成员

---

## 2. 事件标志组创建和删除

### 2.1 xEventGroupCreate() - 动态创建事件标志组
**函数原型：**
```c
EventGroupHandle_t xEventGroupCreate(void);
```

**参数说明：**无参数

**返回值：**成功返回事件标志组句柄，失败返回NULL

**作用：**动态创建一个新的事件标志组

**宏配置要求：**
- configSUPPORT_DYNAMIC_ALLOCATION 必须为1
- configUSE_16_BIT_TICKS 为0时有24个可用事件位
- configUSE_16_BIT_TICKS 为1时有8个可用事件位

**注意：**
- 所有事件位初始化为0
- 内存由系统自动分配

---

### 2.2 xEventGroupCreateStatic() - 静态创建事件标志组
**函数原型：**
```c
EventGroupHandle_t xEventGroupCreateStatic(
    StaticEventGroup_t *pxEventGroupBuffer
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pxEventGroupBuffer | StaticEventGroup_t* | 事件标志组控制块缓冲区 |

**返回值：**成功返回事件标志组句柄，失败返回NULL

**作用：**静态创建事件标志组，使用用户提供的控制块缓冲区

**宏配置要求：**
- configSUPPORT_STATIC_ALLOCATION 必须为1

**注意：**
- 需要用户提供控制块缓冲区
- 控制块缓冲区必须持续有效

---

### 2.3 vEventGroupDelete() - 删除事件标志组
**函数原型：**
```c
void vEventGroupDelete(EventGroupHandle_t xEventGroup);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xEventGroup | EventGroupHandle_t | 要删除的事件标志组句柄 |

**返回值：**无

**作用：**删除事件标志组并释放相关资源

**注意：**
- 等待该事件标志组的任务将被唤醒并返回错误
- 静态创建的事件标志组不会释放内存
- 删除后句柄变为无效

---

## 3. 事件位设置操作

### 3.1 xEventGroupSetBits() - 设置事件位
**函数原型：**
```c
EventBits_t xEventGroupSetBits(
    EventGroupHandle_t xEventGroup,
    const EventBits_t uxBitsToSet
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xEventGroup | EventGroupHandle_t | 事件标志组句柄 |
| uxBitsToSet | const EventBits_t | 要设置的事件位掩码 |

**返回值：**调用函数时事件标志组的值

**作用：**设置事件标志组中的指定事件位

**注意：**
- 设置位会唤醒等待这些位的任务
- 不能在中断服务函数中使用
- 可能引起任务切换

---

### 3.2 xEventGroupSetBitsFromISR() - 中断中设置事件位
**函数原型：**
```c
BaseType_t xEventGroupSetBitsFromISR(
    EventGroupHandle_t xEventGroup,
    const EventBits_t uxBitsToSet,
    BaseType_t *pxHigherPriorityTaskWoken
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xEventGroup | EventGroupHandle_t | 事件标志组句柄 |
| uxBitsToSet | const EventBits_t | 要设置的事件位掩码 |
| pxHigherPriorityTaskWoken | BaseType_t* | 高优先级任务唤醒标志 |

**返回值：**pdPASS表示成功，pdFALSE表示失败

**作用：**在中断服务函数中设置事件位

**注意：**
- 实际设置操作会延迟到定时器任务中执行
- 如果定时器任务优先级低于被唤醒任务，可能会延迟

---

## 4. 事件位等待操作

### 4.1 xEventGroupWaitBits() - 等待事件位
**函数原型：**
```c
EventBits_t xEventGroupWaitBits(
    EventGroupHandle_t xEventGroup,
    const EventBits_t uxBitsToWaitFor,
    const BaseType_t xClearOnExit,
    const BaseType_t xWaitForAllBits,
    TickType_t xTicksToWait
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xEventGroup | EventGroupHandle_t | 事件标志组句柄 |
| uxBitsToWaitFor | const EventBits_t | 要等待的事件位掩码 |
| xClearOnExit | const BaseType_t | 退出时是否清除事件位 |
| xWaitForAllBits | const BaseType_t | 是否等待所有位都设置 |
| xTicksToWait | TickType_t | 最大等待时间 |

**返回值：**
- 满足条件时：事件标志组的值
- 超时时：当前事件标志组的值
- 错误时：0

**作用：**等待事件标志组中的指定事件位

**参数详解：**
- **xClearOnExit**：
  - pdTRUE：函数返回前清除uxBitsToWaitFor指定的位
  - pdFALSE：不清除事件位
- **xWaitForAllBits**：
  - pdTRUE：等待所有指定位都设置（AND操作）
  - pdFALSE：等待任意指定位设置（OR操作）

**注意：**
- 不能在中断服务函数中使用
- 可以设置为无限期等待（portMAX_DELAY）

---

## 5. 事件位清除和获取操作

### 5.1 xEventGroupClearBits() - 清除事件位
**函数原型：**
```c
EventBits_t xEventGroupClearBits(
    EventGroupHandle_t xEventGroup,
    const EventBits_t uxBitsToClear
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xEventGroup | EventGroupHandle_t | 事件标志组句柄 |
| uxBitsToClear | const EventBits_t | 要清除的事件位掩码 |

**返回值：**调用函数时事件标志组的值

**作用：**清除事件标志组中的指定事件位

**注意：**
- 清除位不会唤醒等待任务
- 不能在中断服务函数中使用

---

### 5.2 xEventGroupClearBitsFromISR() - 中断中清除事件位
**函数原型：**
```c
BaseType_t xEventGroupClearBitsFromISR(
    EventGroupHandle_t xEventGroup,
    const EventBits_t uxBitsToClear
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xEventGroup | EventGroupHandle_t | 事件标志组句柄 |
| uxBitsToClear | const EventBits_t | 要清除的事件位掩码 |

**返回值：**pdPASS表示成功，pdFALSE表示失败

**作用：**在中断服务函数中清除事件位

**注意：**
- 实际清除操作会延迟到定时器任务中执行

---

### 5.3 xEventGroupGetBits() - 获取事件位状态
**函数原型：**
```c
EventBits_t xEventGroupGetBits(EventGroupHandle_t xEventGroup);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xEventGroup | EventGroupHandle_t | 事件标志组句柄 |

**返回值：**当前事件标志组的值

**作用：**获取事件标志组当前的事件位状态

**注意：**
- 这是一个非阻塞操作
- 可以在任务和中断中使用

---

### 5.4 xEventGroupGetBitsFromISR() - 中断中获取事件位状态
**函数原型：**
```c
EventBits_t xEventGroupGetBitsFromISR(EventGroupHandle_t xEventGroup);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xEventGroup | EventGroupHandle_t | 事件标志组句柄 |

**返回值：**当前事件标志组的值

**作用：**在中断服务函数中获取事件标志组的状态

---

## 6. 事件位同步操作

### 6.1 xEventGroupSync() - 任务同步
**函数原型：**
```c
EventBits_t xEventGroupSync(
    EventGroupHandle_t xEventGroup,
    const EventBits_t uxBitsToSet,
    const EventBits_t uxBitsToWaitFor,
    TickType_t xTicksToWait
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xEventGroup | EventGroupHandle_t | 事件标志组句柄 |
| uxBitsToSet | const EventBits_t | 要设置的事件位掩码 |
| uxBitsToWaitFor | const EventBits_t | 要等待的事件位掩码 |
| xTicksToWait | TickType_t | 最大等待时间 |

**返回值：**
- 成功时：满足条件时的事件标志组值
- 超时时：当前事件标志组值
- 错误时：0

**作用：**实现任务同步，设置自己的位并等待其他任务的位

**执行步骤：**
1. 设置uxBitsToSet指定的事件位
2. 等待uxBitsToWaitFor指定的所有位都设置
3. 返回前清除uxBitsToWaitFor指定的位

**注意：**
- 常用于多任务同步场景
- 等待的是所有指定位（AND操作）
- 自动清除等待的位

---

## 7. 常用宏定义和常量

### 7.1 事件位掩码宏
```c
#define BIT_0     (1 << 0)   // 0x01
#define BIT_1     (1 << 1)   // 0x02
#define BIT_2     (1 << 2)   // 0x04
#define BIT_3     (1 << 3)   // 0x08
#define BIT_4     (1 << 4)   // 0x10
#define BIT_5     (1 << 5)   // 0x20
#define BIT_6     (1 << 6)   // 0x40
#define BIT_7     (1 << 7)   // 0x80
// ... 可继续定义到BIT_23
```

### 7.2 配置相关宏
```c
// 启用事件标志组功能
#define configUSE_16_BIT_TICKS                0    // 0=24位事件位，1=8位事件位
#define configSUPPORT_DYNAMIC_ALLOCATION      1    // 支持动态内存分配
#define configSUPPORT_STATIC_ALLOCATION       1    // 支持静态内存分配
```
