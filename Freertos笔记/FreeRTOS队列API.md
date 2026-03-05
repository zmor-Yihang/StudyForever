# FreeRTOS 队列相关API和结构体完整总结

---

## 1. 数据类型和结构体

### 1.1 QueueHandle_t - 队列句柄类型
**类型定义：**
```c
typedef void * QueueHandle_t;
```

**作用：**队列句柄类型，用于引用和管理队列

**注意：**
- NULL表示无效队列
- 句柄在队列删除后变为无效

---

### 1.2 QueueSetHandle_t - 队列集句柄类型
**类型定义：**
```c
typedef void * QueueSetHandle_t;
```

**作用：**队列集句柄类型，用于管理多个队列和信号量

**注意：**
- 用于同时等待多个队列和信号量
- 需要configUSE_QUEUE_SETS为1

---

### 1.3 QueueSetMemberHandle_t - 队列集成员句柄
**类型定义：**
```c
typedef void * QueueSetMemberHandle_t;
```

**作用：**队列集成员句柄，可以是队列或信号量

---

### 1.4 StaticQueue_t - 静态队列控制块
**结构体定义：**
```c
typedef struct xSTATIC_QUEUE
{
    void *pvDummy1[3];
    union
    {
        void *pvDummy2;
        UBaseType_t uxDummy2;
    } u;
    StaticListItem_t xDummy3[2];
    UBaseType_t uxDummy4[3];
    uint8_t ucDummy5[2];
    uint8_t ucDummy6;
    #if ( configUSE_MUTEXES == 1 )
        uint8_t ucDummy7;
    #endif
} StaticQueue_t;
```

**作用：**静态队列控制块结构体，用于静态队列创建

**注意：**
- 实际结构体内容可能因配置而异
- 用户不应直接访问内部成员

---

## 2. 队列创建和删除

### 2.1 xQueueCreate() - 动态创建队列
**函数原型：**
```c
QueueHandle_t xQueueCreate(
    UBaseType_t uxQueueLength,
    UBaseType_t uxItemSize
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| uxQueueLength | UBaseType_t | 队列长度（能存储的消息数量） |
| uxItemSize | UBaseType_t | 每个消息的大小（字节） |

**返回值：**成功返回队列句柄，失败返回NULL

**作用：**动态创建一个新队列

**宏配置要求：**
- configSUPPORT_DYNAMIC_ALLOCATION 必须为1
- 需要足够的堆内存

**注意：**
- 队列可存储任意类型的数据
- 内存由系统自动分配

---

### 2.2 xQueueCreateStatic() - 静态创建队列
**函数原型：**
```c
QueueHandle_t xQueueCreateStatic(
    UBaseType_t uxQueueLength,
    UBaseType_t uxItemSize,
    uint8_t *pucQueueStorageBuffer,
    StaticQueue_t *pxQueueBuffer
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| uxQueueLength | UBaseType_t | 队列长度 |
| uxItemSize | UBaseType_t | 每个消息的大小 |
| pucQueueStorageBuffer | uint8_t* | 队列存储缓冲区 |
| pxQueueBuffer | StaticQueue_t* | 队列控制块缓冲区 |

**返回值：**成功返回队列句柄，失败返回NULL

**作用：**静态创建队列，使用用户提供的内存缓冲区

**宏配置要求：**
- configSUPPORT_STATIC_ALLOCATION 必须为1

**注意：**
- 需要用户提供存储和控制块缓冲区
- 缓冲区大小 = uxQueueLength * uxItemSize

---

### 2.3 vQueueDelete() - 删除队列
**函数原型：**
```c
void vQueueDelete(QueueHandle_t xQueue);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueue | QueueHandle_t | 要删除的队列句柄 |

**返回值：**无

**作用：**删除队列并释放相关资源

**注意：**
- 只能删除动态创建的队列
- 删除后所有等待该队列的任务将被唤醒

---

## 3. 队列发送操作

### 3.1 xQueueSend() - 发送消息到队列尾部
**函数原型：**
```c
BaseType_t xQueueSend(
    QueueHandle_t xQueue,
    const void *pvItemToQueue,
    TickType_t xTicksToWait
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueue | QueueHandle_t | 队列句柄 |
| pvItemToQueue | const void* | 要发送的消息指针 |
| xTicksToWait | TickType_t | 最大等待时间 |

**返回值：**pdTRUE表示成功，errQUEUE_FULL表示队列满

**作用：**将消息发送到队列尾部

**注意：**
- 等同于xQueueSendToBack()
- 如果队列满且xTicksToWait>0，任务将阻塞

---

### 3.2 xQueueSendToBack() - 发送消息到队列尾部
**函数原型：**
```c
BaseType_t xQueueSendToBack(
    QueueHandle_t xQueue,
    const void *pvItemToQueue,
    TickType_t xTicksToWait
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueue | QueueHandle_t | 队列句柄 |
| pvItemToQueue | const void* | 要发送的消息指针 |
| xTicksToWait | TickType_t | 最大等待时间 |

**返回值：**pdTRUE表示成功，errQUEUE_FULL表示队列满

**作用：**将消息发送到队列尾部（FIFO方式）

---

### 3.3 xQueueSendToFront() - 发送消息到队列头部
**函数原型：**
```c
BaseType_t xQueueSendToFront(
    QueueHandle_t xQueue,
    const void *pvItemToQueue,
    TickType_t xTicksToWait
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueue | QueueHandle_t | 队列句柄 |
| pvItemToQueue | const void* | 要发送的消息指针 |
| xTicksToWait | TickType_t | 最大等待时间 |

**返回值：**pdTRUE表示成功，errQUEUE_FULL表示队列满

**作用：**将消息发送到队列头部（LIFO方式）

**注意：**
- 实现优先级消息处理
- 打破FIFO顺序

---

### 3.4 xQueueOverwrite() - 覆盖发送
**函数原型：**
```c
BaseType_t xQueueOverwrite(
    QueueHandle_t xQueue,
    const void *pvItemToQueue
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueue | QueueHandle_t | 队列句柄 |
| pvItemToQueue | const void* | 要发送的消息指针 |

**返回值：**总是返回pdPASS

**作用：**发送消息，如果队列满则覆盖最旧的消息

**注意：**
- 队列长度必须为1
- 永远不会阻塞
- 适用于最新值覆盖场景

---

### 3.5 中断服务函数发送API

#### 3.5.1 xQueueSendFromISR() - 中断中发送到尾部
**函数原型：**
```c
BaseType_t xQueueSendFromISR(
    QueueHandle_t xQueue,
    const void *pvItemToQueue,
    BaseType_t *pxHigherPriorityTaskWoken
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueue | QueueHandle_t | 队列句柄 |
| pvItemToQueue | const void* | 要发送的消息指针 |
| pxHigherPriorityTaskWoken | BaseType_t* | 高优先级任务唤醒标志 |

**返回值：**pdTRUE表示成功，errQUEUE_FULL表示队列满

**作用：**在中断服务函数中发送消息到队列尾部

---

#### 3.5.2 xQueueSendToBackFromISR() - 中断中发送到尾部
**函数原型：**
```c
BaseType_t xQueueSendToBackFromISR(
    QueueHandle_t xQueue,
    const void *pvItemToQueue,
    BaseType_t *pxHigherPriorityTaskWoken
);
```

**作用：**在中断服务函数中发送消息到队列尾部，与xQueueSendFromISR()等同

---

#### 3.5.3 xQueueSendToFrontFromISR() - 中断中发送到头部
**函数原型：**
```c
BaseType_t xQueueSendToFrontFromISR(
    QueueHandle_t xQueue,
    const void *pvItemToQueue,
    BaseType_t *pxHigherPriorityTaskWoken
);
```

**作用：**在中断服务函数中发送消息到队列头部

---

#### 3.5.4 xQueueOverwriteFromISR() - 中断中覆盖发送
**函数原型：**
```c
BaseType_t xQueueOverwriteFromISR(
    QueueHandle_t xQueue,
    const void *pvItemToQueue,
    BaseType_t *pxHigherPriorityTaskWoken
);
```

**作用：**在中断服务函数中覆盖发送消息

---

## 4. 队列接收操作

### 4.1 xQueueReceive() - 接收并删除消息
**函数原型：**
```c
BaseType_t xQueueReceive(
    QueueHandle_t xQueue,
    void *pvBuffer,
    TickType_t xTicksToWait
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueue | QueueHandle_t | 队列句柄 |
| pvBuffer | void* | 接收缓冲区指针 |
| xTicksToWait | TickType_t | 最大等待时间 |

**返回值：**pdTRUE表示成功，pdFALSE表示超时

**作用：**从队列头部接收消息并从队列中删除

**注意：**
- 如果队列空且xTicksToWait>0，任务将阻塞
- FIFO顺序接收

---

### 4.2 xQueuePeek() - 接收但不删除消息
**函数原型：**
```c
BaseType_t xQueuePeek(
    QueueHandle_t xQueue,
    void *pvBuffer,
    TickType_t xTicksToWait
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueue | QueueHandle_t | 队列句柄 |
| pvBuffer | void* | 接收缓冲区指针 |
| xTicksToWait | TickType_t | 最大等待时间 |

**返回值：**pdTRUE表示成功，pdFALSE表示超时

**作用：**从队列头部接收消息但不从队列中删除

**注意：**
- 消息仍保留在队列中
- 可多次读取同一消息

---

### 4.3 中断服务函数接收API

#### 4.3.1 xQueueReceiveFromISR() - 中断中接收消息
**函数原型：**
```c
BaseType_t xQueueReceiveFromISR(
    QueueHandle_t xQueue,
    void *pvBuffer,
    BaseType_t *pxHigherPriorityTaskWoken
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueue | QueueHandle_t | 队列句柄 |
| pvBuffer | void* | 接收缓冲区指针 |
| pxHigherPriorityTaskWoken | BaseType_t* | 高优先级任务唤醒标志 |

**返回值：**pdTRUE表示成功，pdFALSE表示队列空

**作用：**在中断服务函数中接收消息

**注意：**
- 不会阻塞，立即返回
- 必须检查返回值

---

#### 4.3.2 xQueuePeekFromISR() - 中断中查看消息
**函数原型：**
```c
BaseType_t xQueuePeekFromISR(
    QueueHandle_t xQueue,
    void *pvBuffer
);
```

**作用：**在中断服务函数中查看消息但不删除

---

## 5. 队列状态查询

### 5.1 uxQueueMessagesWaiting() - 获取队列中消息数量
**函数原型：**
```c
UBaseType_t uxQueueMessagesWaiting(const QueueHandle_t xQueue);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueue | const QueueHandle_t | 队列句柄 |

**返回值：**队列中当前的消息数量

**作用：**获取队列中等待处理的消息数量

---

### 5.2 uxQueueSpacesAvailable() - 获取队列剩余空间
**函数原型：**
```c
UBaseType_t uxQueueSpacesAvailable(const QueueHandle_t xQueue);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueue | const QueueHandle_t | 队列句柄 |

**返回值：**队列中剩余的空间数量

**作用：**获取队列还能存储多少消息

---

### 5.3 xQueueIsQueueEmptyFromISR() - 中断中检查队列是否为空
**函数原型：**
```c
BaseType_t xQueueIsQueueEmptyFromISR(const QueueHandle_t xQueue);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueue | const QueueHandle_t | 队列句柄 |

**返回值：**pdTRUE表示队列为空，pdFALSE表示队列非空

**作用：**在中断服务函数中检查队列是否为空

---

### 5.4 xQueueIsQueueFullFromISR() - 中断中检查队列是否已满
**函数原型：**
```c
BaseType_t xQueueIsQueueFullFromISR(const QueueHandle_t xQueue);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueue | const QueueHandle_t | 队列句柄 |

**返回值：**pdTRUE表示队列已满，pdFALSE表示队列未满

**作用：**在中断服务函数中检查队列是否已满

---

## 6. 队列集操作

### 6.1 xQueueCreateSet() - 创建队列集
**函数原型：**
```c
QueueSetHandle_t xQueueCreateSet(const UBaseType_t uxEventQueueLength);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| uxEventQueueLength | const UBaseType_t | 队列集中可包含的队列和信号量总数 |

**返回值：**成功返回队列集句柄，失败返回NULL

**作用：**创建一个队列集，用于同时等待多个队列和信号量

**宏配置要求：**
- configUSE_QUEUE_SETS 必须为1

---

### 6.2 xQueueAddToSet() - 添加队列到队列集
**函数原型：**
```c
BaseType_t xQueueAddToSet(
    QueueSetMemberHandle_t xQueueOrSemaphore,
    QueueSetHandle_t xQueueSet
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueueOrSemaphore | QueueSetMemberHandle_t | 要添加的队列或信号量句柄 |
| xQueueSet | QueueSetHandle_t | 队列集句柄 |

**返回值：**pdPASS表示成功，pdFAIL表示失败

**作用：**将队列或信号量添加到队列集中

**注意：**
- 队列必须为空才能添加到队列集
- 队列一旦添加到队列集，就不能直接读取

---

### 6.3 xQueueRemoveFromSet() - 从队列集中移除队列
**函数原型：**
```c
BaseType_t xQueueRemoveFromSet(
    QueueSetMemberHandle_t xQueueOrSemaphore,
    QueueSetHandle_t xQueueSet
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueueOrSemaphore | QueueSetMemberHandle_t | 要移除的队列或信号量句柄 |
| xQueueSet | QueueSetHandle_t | 队列集句柄 |

**返回值：**pdPASS表示成功，pdFAIL表示失败

**作用：**从队列集中移除队列或信号量

---

### 6.4 xQueueSelectFromSet() - 从队列集中选择就绪的队列
**函数原型：**
```c
QueueSetMemberHandle_t xQueueSelectFromSet(
    QueueSetHandle_t xQueueSet,
    TickType_t xTicksToWait
);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueueSet | QueueSetHandle_t | 队列集句柄 |
| xTicksToWait | TickType_t | 最大等待时间 |

**返回值：**返回有数据可读的队列或信号量句柄，超时返回NULL

**作用：**等待队列集中任意队列或信号量变为就绪状态

**注意：**
- 返回的句柄需要进一步读取具体数据
- 可同时等待多个通信对象

---

### 6.5 xQueueSelectFromSetFromISR() - 中断中从队列集选择
**函数原型：**
```c
QueueSetMemberHandle_t xQueueSelectFromSetFromISR(QueueSetHandle_t xQueueSet);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueueSet | QueueSetHandle_t | 队列集句柄 |

**返回值：**返回有数据可读的队列或信号量句柄，无数据返回NULL

**作用：**在中断服务函数中从队列集选择就绪的队列

---

## 7. 队列重置

### 7.1 xQueueReset() - 重置队列
**函数原型：**
```c
BaseType_t xQueueReset(QueueHandle_t xQueue);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xQueue | QueueHandle_t | 要重置的队列句柄 |

**返回值：**总是返回pdPASS

**作用：**清空队列中的所有消息，重置为初始状态

**注意：**
- 所有等待该队列的任务将被唤醒
- 队列长度和消息大小不变

---

## 8. 特殊宏定义

### 8.1 队列等待时间宏
```c
#define portMAX_DELAY        (TickType_t)0xffffffffUL  // 最大延时
#define portTICK_PERIOD_MS   (1000/configTICK_RATE_HZ) // 节拍周期(ms)
```

### 8.2 队列操作结果宏
```c
#define pdTRUE               1       // 操作成功
#define pdFALSE              0       // 操作失败
#define pdPASS               pdTRUE  // 操作通过
#define errQUEUE_EMPTY       0       // 队列为空
#define errQUEUE_FULL        0       // 队列已满
```
