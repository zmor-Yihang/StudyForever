# FreeRTOS 任务相关API和结构体完整总结

---

## 1. 数据类型和结构体

### 1.1 TaskHandle_t - 任务句柄类型
**类型定义：**
```c
typedef void * TaskHandle_t;
```

**作用：**任务句柄类型，用于引用和管理任务

**注意：**
- NULL表示当前任务
- 句柄在任务删除后变为无效

---

### 1.2 TaskFunction_t - 任务函数类型
**类型定义：**
```c
typedef void (*TaskFunction_t)(void *pvParameters);
```

**作用：**任务函数指针类型定义

**任务函数要求：**
- 必须是无限循环结构
- 或显式调用vTaskDelete(NULL)终止
- 参数通过pvParameters传递

---

### 1.3 StaticTask_t - 静态任务控制块
**结构体定义：**
```c
typedef struct xSTATIC_TCB
{
    void *pxDummy1[2];
    void *pxDummy2[3];
    #if ( portUSING_MPU_WRAPPERS == 1 )
        xMPU_SETTINGS xDummy3;
    #endif
    void *pxDummy4[3];
    uint32_t ulDummy5[2];
    void *pxDummy6;
    uint8_t ucDummy7;
    #if ( configUSE_MUTEXES == 1 )
        uint8_t ucDummy8[2];
    #endif
    uint32_t ulDummy9[2];
    uint32_t ulDummy10[2];
    #if ( configUSE_TASK_NOTIFICATIONS == 1 )
        uint32_t ulDummy11;
        uint8_t ucDummy12[3];
    #endif
    uint8_t ucDummy13;
    #if ( configGENERATE_RUN_TIME_STATS == 1 )
        uint32_t ulDummy14;
    #endif
} StaticTask_t;
```

**作用：**静态任务控制块结构体，用于静态任务创建

**注意：**
- 实际结构体内容可能因配置而异
- 用户不应直接访问内部成员

---

### 1.4 TaskParameters_t - 受限任务参数结构体
**结构体定义：**
```c
typedef struct xTASK_PARAMETERS
{
    TaskFunction_t pvTaskCode;
    const char *pcName;
    uint16_t usStackDepth;
    void *pvParameters;
    UBaseType_t uxPriority;
    portSTACK_TYPE *puxStackBuffer;
    MemoryRegion_t xRegions[portNUM_CONFIGURABLE_REGIONS];
} TaskParameters_t;
```

**作用：**用于xTaskCreateRestricted()函数的任务参数结构体

**注意：**
- 仅用于受MPU保护的任务创建
- 需要定义内存区域访问权限

---

### 1.5 StackType_t - 堆栈元素类型
**类型定义：**
```c
typedef uint32_t StackType_t;
```

**作用：**堆栈元素类型定义，用于任务堆栈

**注意：**
- 具体类型可能因处理器架构而异
- 堆栈大小以该类型为单位计算

---

### 1.6 TaskStatus_t - 任务状态结构体
**结构体定义：**
```c
typedef struct xTASK_STATUS
{
    TaskHandle_t xHandle;              /* 任务句柄 */
    const char *pcTaskName;             /* 任务名称 */
    UBaseType_t xTaskNumber;            /* 任务编号 */
    eTaskState eCurrentState;           /* 当前状态 */
    UBaseType_t uxCurrentPriority;      /* 当前优先级 */
    UBaseType_t uxBasePriority;         /* 基础优先级 */
    uint32_t ulRunTimeCounter;         /* 运行时间计数器 */
    StackType_t *pxStackBase;          /* 堆栈基址 */
    configSTACK_DEPTH_TYPE usStackHighWaterMark; /* 堆栈高水位标记 */
} TaskStatus_t;
```

**作用：**用于存储任务的完整状态信息

**注意：**
- 在uxTaskGetSystemState()和vTaskGetInfo()中使用
- 包含了任务的所有关键信息
- 可用于任务监控和调试

---

## 2. 任务创建和删除

### 2.1 xTaskCreate() - 动态创建任务
**函数原型：**
```c
BaseType_t xTaskCreate(
    TaskFunction_t pvTaskCode,
    const char * const pcName,
    const configSTACK_DEPTH_TYPE usStackDepth,
    void *pvParameters,
    UBaseType_t uxPriority,
    TaskHandle_t *pxCreatedTask
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pvTaskCode | TaskFunction_t | 任务函数指针 |
| pcName | const char* | 任务名称 |
| usStackDepth | const configSTACK_DEPTH_TYPE | 堆栈深度 |
| pvParameters | void* | 任务参数 |
| uxPriority | UBaseType_t | 任务优先级 |
| pxCreatedTask | TaskHandle_t* | 返回的任务句柄 |

**返回值：**pdPASS表示成功，errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY表示内存不足

**作用：**动态创建一个新任务并加入就绪列表

**宏配置要求：**
- configSUPPORT_DYNAMIC_ALLOCATION 必须为1
- 需要足够的堆内存

**注意：**
- 任务函数必须是无限循环或显式删除
- 返回的任务句柄可用于后续管理

---

### 2.2 xTaskCreateStatic() - 静态创建任务
**函数原型：**
```c
TaskHandle_t xTaskCreateStatic(
    TaskFunction_t pvTaskCode,
    const char * const pcName,
    const uint32_t ulStackDepth,
    void *pvParameters,
    UBaseType_t uxPriority,
    StackType_t * const puxStackBuffer,
    StaticTask_t * const pxTaskBuffer
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pvTaskCode | TaskFunction_t | 任务函数指针 |
| pcName | const char* | 任务名称 |
| ulStackDepth | const uint32_t | 堆栈深度 |
| pvParameters | void* | 任务参数 |
| uxPriority | UBaseType_t | 任务优先级 |
| puxStackBuffer | StackType_t* | 堆栈缓冲区 |
| pxTaskBuffer | StaticTask_t* | 任务控制块缓冲区 |

**返回值：**成功返回任务句柄，失败返回NULL

**作用：**静态创建任务，使用用户提供的内存缓冲区

**宏配置要求：**

- configSUPPORT_STATIC_ALLOCATION 必须为1

**注意：**
- 需要用户提供堆栈和TCB缓冲区
- 适用于内存受限或确定性要求高的场景

---

### 2.3 xTaskCreateRestricted() - 创建受保护任务
**函数原型：**
```c
BaseType_t xTaskCreateRestricted(
    const TaskParameters_t * const pxTaskDefinition,
    TaskHandle_t *pxCreatedTask
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pxTaskDefinition | const TaskParameters_t* | 任务参数结构体 |
| pxCreatedTask | TaskHandle_t* | 返回的任务句柄 |

**返回值：**pdPASS表示成功，其他值表示失败

**作用：**创建受MPU保护的任务

**宏配置要求：**
- 需要MPU支持
- 需要定义内存区域权限

**注意：**
- 仅用于支持MPU的系统
- 提供硬件级别的内存保护

---

### 2.4 vTaskDelete() - 删除任务
**函数原型：**
```c
void vTaskDelete(TaskHandle_t xTaskToDelete);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTaskToDelete | TaskHandle_t | 要删除的任务句柄，NULL表示删除当前任务 |

**返回值：**无

**作用：**删除指定任务并释放相关资源

**注意：**
- 被删除任务的内存由空闲任务清理
- 删除当前任务后不会返回

---

## 3. 任务控制

### 3.1 vTaskSuspend() - 挂起任务
**函数原型：**
```c
void vTaskSuspend(TaskHandle_t xTaskToSuspend);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTaskToSuspend | TaskHandle_t | 要挂起的任务句柄，NULL表示挂起当前任务 |

**返回值：**无

**作用：**挂起指定任务，使其不再参与调度

**注意：**
- 任务将保持挂起状态直到被显式恢复
- 多次挂起只需一次恢复

---

### 3.2 vTaskResume() - 恢复任务
**函数原型：**
```c
void vTaskResume(TaskHandle_t xTaskToResume);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTaskToResume | TaskHandle_t | 要恢复的任务句柄 |

**返回值：**无

**作用：**恢复被挂起的任务

**注意：**
- 不能在中断服务函数中使用
- 恢复后任务进入就绪状态

---

### 3.3 xTaskResumeFromISR() - 中断中恢复任务
**函数原型：**
```c
BaseType_t xTaskResumeFromISR(TaskHandle_t xTaskToResume);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTaskToResume | TaskHandle_t | 要恢复的任务句柄 |

**返回值：**pdTRUE表示需要任务切换，pdFALSE表示不需要

**作用：**在中断服务函数中恢复被挂起的任务

**注意：**
- 专用于中断上下文
- 返回值为切换标志

---

### 3.4 vTaskSuspendAll() - 挂起所有任务

**函数原型：**

```c
void vTaskSuspendAll(void);
```

**参数说明：**无

**返回值：**无

**作用：**挂起所有任务，停止任务调度

**注意：**

- 不会挂起中断
- 可以嵌套调用

---

### 3.5 xTaskResumeAll() - 恢复所有任务

**函数原型：**

```c
BaseType_t xTaskResumeAll(void);
```

**参数说明：**无

**返回值：**pdTRUE表示调度已恢复，pdFALSE表示仍有嵌套挂起

**作用：**恢复所有任务的调度

**注意：**

- 必须与vTaskSuspendAll配对使用
- 嵌套调用需要相同次数的恢复

### 3.6 vTaskPrioritySet() - 设置优先级

**函数原型：**
```c
void vTaskPrioritySet(TaskHandle_t xTask, UBaseType_t uxNewPriority);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTask | TaskHandle_t | 任务句柄，NULL表示当前任务 |
| uxNewPriority | UBaseType_t | 新优先级 |

**返回值：**无

**作用：**设置任务优先级

**注意：**
- 优先级数值越大表示优先级越高
- 设置后可能立即触发任务切换

---

### 3.7 uxTaskPriorityGet() - 获取优先级
**函数原型：**
```c
UBaseType_t uxTaskPriorityGet(const TaskHandle_t xTask);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTask | const TaskHandle_t | 任务句柄，NULL表示当前任务 |

**返回值：**当前优先级

**作用：**获取任务当前优先级

---

<<<<<<< HEAD
- 
=======
### 3.6 vTaskSuspendAll() - 挂起调度器
**函数原型：**

```c
void vTaskSuspendAll(void);
```

**参数说明：**无

**返回值：**无

**作用：**停止任务调度

**注意：**

- 不会挂起中断
- 可以嵌套调用

---

### 3.7 xTaskResumeAll() - 恢复调度器
**函数原型：**
```c
BaseType_t xTaskResumeAll(void);
```

**参数说明：**无

**返回值：**pdTRUE表示调度已恢复，pdFALSE表示仍有嵌套挂起

**作用：**恢复任务调度

**注意：**
- 必须与vTaskSuspendAll配对使用
- 嵌套调用需要相同次数的恢复
>>>>>>> 7262803be5f440501a0ef8db856b0cc2f8a8a18e

---

## 4. 任务延时

### 4.1 vTaskDelay() - 相对延时
**函数原型：**
```c
void vTaskDelay(const TickType_t xTicksToDelay);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTicksToDelay | const TickType_t | 延时的时钟节拍数 |

**返回值：**无

**作用：**使当前任务延时指定的时钟节拍数

**注意：**
- 相对延时，从调用时刻开始计算
- 延时期间任务处于阻塞状态

---

### 4.2 vTaskDelayUntil() - 绝对延时
**函数原型：**
```c
void vTaskDelayUntil(TickType_t *pxPreviousWakeTime, const TickType_t xTimeIncrement);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pxPreviousWakeTime | TickType_t* | 指向保存上次唤醒时间的变量 |
| xTimeIncrement | const TickType_t | 时间增量，即周期 |

**返回值：**无

**作用：**实现精确的周期性延时

**特点：**
- 绝对延时，保持精确的周期
- 不受任务执行时间影响
- 适用于需要固定周期的任务

---

### 4.3 pdMS_TO_TICKS() - 毫秒转节拍
**函数原型：**
```c
#define pdMS_TO_TICKS(xTimeInMs)
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTimeInMs | uint32_t | 毫秒时间 |

**返回值：**对应的时钟节拍数

**作用：**将毫秒时间转换为时钟节拍数

---

## 5. 任务状态查询

### 5.1 uxTaskPriorityGet() - 获取任务优先级
**函数原型：**
```c
UBaseType_t uxTaskPriorityGet(const TaskHandle_t xTask);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTask | const TaskHandle_t | 任务句柄，NULL表示获取当前任务优先级 |

**返回值：**任务的当前优先级数值

**作用：**获取指定任务的优先级

**注意：**
- 优先级数值越大表示优先级越高
- 可用于运行时动态调整任务优先级

---

### 5.2 vTaskPrioritySet() - 设置任务优先级
**函数原型：**
```c
void vTaskPrioritySet(TaskHandle_t xTask, UBaseType_t uxNewPriority);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTask | TaskHandle_t | 任务句柄，NULL表示设置当前任务优先级 |
| uxNewPriority | UBaseType_t | 新的优先级数值 |

**返回值：**无

**作用：**动态设置任务的优先级

**注意：**
- 优先级数值范围：0 到 (configMAX_PRIORITIES-1)
- 设置后可能立即触发任务切换
- 可用于实现动态优先级调度

---

### 5.3 uxTaskGetNumberOfTasks() - 获取系统中任务的数量
**函数原型：**
```c
UBaseType_t uxTaskGetNumberOfTasks(void);
```

**参数说明：**无

**返回值：**当前系统中存在的任务总数

**作用：**获取系统中当前存在的任务数量

**注意：**
- 包括就绪、阻塞、挂起等各种状态的任务
- 不包括已删除但尚未清理的任务

---

### 5.4 uxTaskGetSystemState() - 获取所有任务状态信息
**函数原型：**
```c
UBaseType_t uxTaskGetSystemState(
    TaskStatus_t *pxTaskStatusArray,
    const UBaseType_t uxArraySize,
    uint32_t *pulTotalRunTime
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pxTaskStatusArray | TaskStatus_t* | 任务状态数组缓冲区 |
| uxArraySize | const UBaseType_t | 数组缓冲区大小 |
| pulTotalRunTime | uint32_t* | 返回系统总运行时间 |

**返回值：**实际获取到的任务数量

**作用：**一次性获取系统中所有任务的详细状态信息

**宏配置要求：**
- configUSE_TRACE_FACILITY 必须为1

**注意：**
- 需要预先分配足够大的TaskStatus_t数组
- 可用于实现自定义的任务监控功能

---

### 5.5 vTaskGetInfo() - 获取指定单个任务信息
**函数原型：**
```c
void vTaskGetInfo(
    TaskHandle_t xTask,
    TaskStatus_t *pxTaskStatus,
    BaseType_t xGetFreeStackSpace,
    eTaskState eState
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTask | TaskHandle_t | 要查询的任务句柄 |
| pxTaskStatus | TaskStatus_t* | 返回任务状态信息的缓冲区 |
| xGetFreeStackSpace | BaseType_t | 是否获取堆栈剩余空间：pdTRUE-获取，pdFALSE-不获取 |
| eState | eTaskState | 传入任务状态可提高效率，传入eInvalid-自动获取 |

**返回值：**无

**作用：**获取指定单个任务的详细信息

**宏配置要求：**
- configUSE_TRACE_FACILITY 必须为1

---

### 5.6 xTaskGetCurrentTaskHandle() - 获取当前任务句柄
**函数原型：**
```c
TaskHandle_t xTaskGetCurrentTaskHandle(void);
```

**参数说明：**无

**返回值：**当前正在运行的任务的任务句柄

**作用：**获取当前正在执行的任务的句柄

**注意：**
- 在中断服务函数中使用时，返回的是被中断的任务句柄
- 常用于调试和运行时任务识别

---

### 5.7 xTaskGetHandle() - 根据任务名获取任务句柄
**函数原型：**
```c
TaskHandle_t xTaskGetHandle(const char *pcNameToQuery);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pcNameToQuery | const char* | 要查询的任务名称字符串 |

**返回值：**找到的任务句柄，如果未找到返回NULL

**作用：**通过任务名称字符串获取对应的任务句柄

**注意：**
- 任务名称区分大小写
- 如果多个任务同名，返回最先找到的任务句柄
- 任务名称为创建任务时指定的pcName参数

---

### 5.8 uxTaskGetStackHighWaterMark() - 堆栈监控
**函数原型：**
```c
UBaseType_t uxTaskGetStackHighWaterMark(TaskHandle_t xTask);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTask | TaskHandle_t | 任务句柄，NULL表示当前任务 |

**返回值：**
剩余堆栈空间的最小值（单位：字）

**作用：**获取任务堆栈的历史最小剩余空间

**注意：**
- 返回值越小表示堆栈使用越接近极限
- 建议保持至少20-50字的余量
- 可用于堆栈溢出预警

---

### 5.9 eTaskGetState() - 获取任务状态
**函数原型：**
```c
eTaskState eTaskGetState(TaskHandle_t xTask);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xTask | TaskHandle_t | 任务句柄 |

**返回值：**任务状态枚举值
- eRunning = 0：运行状态
- eReady = 1：就绪状态
- eBlocked = 2：阻塞状态
- eSuspended = 3：挂起状态
- eDeleted = 4：删除状态
- eInvalid = 5：无效状态

**作用：**获取指定任务的当前状态

---

### 5.10 vTaskList() - 任务列表
**函数原型：**
```c
void vTaskList(char *pcWriteBuffer);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pcWriteBuffer | char* | 输出缓冲区指针，需要足够大的缓冲区 |

**返回值：**无

**作用：**以表格形式获取系统中所有任务的列表信息

**输出格式：**
```
任务名		状态	优先级	堆栈	任务号
Task1		R	1	200	1
Task2		B	2	150	2
```

**状态说明：**
- 'R'：就绪(Ready)
- 'B'：阻塞(Blocked)
- 'S'：挂起(Suspended)
- 'D'：删除(Deleted)

**宏配置要求：**
- configUSE_TRACE_FACILITY 必须为1

---

### 5.11 vTaskGetRunTimeStats() - 运行时间统计
**函数原型：**
```c
void vTaskGetRunTimeStats(char *pcWriteBuffer);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pcWriteBuffer | char* | 输出缓冲区指针，需要足够大的缓冲区 |

**返回值：**无

**作用：**获取各任务的运行时间统计信息

**输出格式：**
```
任务名		运行时间	百分比
Task1		1000		45.5%
Task2		500		22.7%
IDLE		700		31.8%
```

**宏配置要求：**
- configGENERATE_RUN_TIME_STATS 必须为1
- 需要实现portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
- 需要定义portGET_RUN_TIME_COUNTER_VALUE()

**注意：**
- 需要高精度定时器支持
- 可用于性能分析和任务负载监控

---

## 6. 使用示例

### 6.1 获取所有任务信息示例
```c
// 获取系统中所有任务的状态信息
void printAllTaskInfo(void)
{
    UBaseType_t uxArraySize, uxIndex;
    TaskStatus_t *pxTaskStatusArray;
    uint32_t ulTotalRunTime;
    
    // 获取任务数量
    uxArraySize = uxTaskGetNumberOfTasks();
    
    // 分配内存
    pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));
    
    if(pxTaskStatusArray != NULL)
    {
        // 获取所有任务状态
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);
        
        printf("任务名称\t状态\t优先级\t堆栈剩余\n");
        for(uxIndex = 0; uxIndex < uxArraySize; uxIndex++)
        {
            printf("%s\t%d\t%d\t%d\n",
                   pxTaskStatusArray[uxIndex].pcTaskName,
                   pxTaskStatusArray[uxIndex].eCurrentState,
                   pxTaskStatusArray[uxIndex].uxCurrentPriority,
                   pxTaskStatusArray[uxIndex].usStackHighWaterMark);
        }
        
        vPortFree(pxTaskStatusArray);
    }
}
```

### 6.2 任务监控和调试示例
```c
// 监控特定任务的堆栈使用情况
void monitorTaskStack(const char *pcTaskName)
{
    TaskHandle_t xTask;
    UBaseType_t uxHighWaterMark;
    
    // 通过名称获取任务句柄
    xTask = xTaskGetHandle(pcTaskName);
    
    if(xTask != NULL)
    {
        // 获取堆栈高水位标记
        uxHighWaterMark = uxTaskGetStackHighWaterMark(xTask);
        
        printf("任务 %s 的堆栈最小剩余：%d 字\n", pcTaskName, uxHighWaterMark);
        
        if(uxHighWaterMark < 50)
        {
            printf("警告：任务 %s 堆栈即将溢出！\n", pcTaskName);
        }
    }
}

// 获取任务运行时间统计
void printRuntimeStats(void)
{
    char pcWriteBuffer[1024];
    
    vTaskGetRunTimeStats(pcWriteBuffer);
    printf("任务运行时间统计：\n%s\n", pcWriteBuffer);
}
```

### 6.3 动态优先级调整示例
```c
// 根据CPU使用率动态调整任务优先级
void dynamicPriorityAdjustment(void)
{
    TaskHandle_t xTask;
    UBaseType_t uxCurrentPriority;
    
    // 获取当前任务句柄
    xTask = xTaskGetCurrentTaskHandle();
    
    // 获取当前优先级
    uxCurrentPriority = uxTaskPriorityGet(xTask);
    
    // 根据某些条件调整优先级
    if(someCondition)
    {
        vTaskPrioritySet(xTask, uxCurrentPriority + 1);
    }
    else
    {
        vTaskPrioritySet(xTask, uxCurrentPriority - 1);
    }
}
```

---
