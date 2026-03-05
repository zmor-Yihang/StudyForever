# FreeRTOS 内存管理相关API和结构体完整总结

---

## 1. 内存管理概述

### 1.1 内存管理简介
**概念：**
- FreeRTOS提供了多种内存管理方案（heap_1到heap_5）
- 动态内存分配用于创建任务、队列、信号量等对象
- 每种方案适用于不同的应用场景

**特点：**
- 可配置的堆大小
- 确定性的内存分配
- 可选的内存分配失败钩子函数

---

### 1.2 内存管理方案对比

| 方案 | 特点 | 是否支持释放 | 适用场景 |
|------|------|-------------|----------|
| heap_1 | 最简单，只分配不释放 | ❌ | 静态对象创建 |
| heap_2 | 支持释放，简单算法 | ✅ | 固定大小对象 |
| heap_3 | 封装标准库malloc/free | ✅ | 需要线程安全的标准库 |
| heap_4 | 支持释放，合并相邻空闲块 | ✅ | 通用场景（推荐） |
| heap_5 | heap_4扩展，支持多内存区域 | ✅ | 不连续内存区域 |

---

## 2. 数据类型和结构体

### 2.1 HeapStats_t - 堆统计信息结构体
**结构体定义：**
```c
typedef struct xHeapStats
{
    size_t xAvailableHeapSpaceInBytes;      // 当前可用堆空间（字节）
    size_t xSizeOfLargestFreeBlockInBytes;  // 最大空闲块大小（字节）
    size_t xSizeOfSmallestFreeBlockInBytes; // 最小空闲块大小（字节）
    size_t xNumberOfFreeBlocks;             // 空闲块数量
    size_t xMinimumEverFreeBytesRemaining;  // 历史最小剩余空间
    size_t xNumberOfSuccessfulAllocations;  // 成功分配次数
    size_t xNumberOfSuccessfulFrees;        // 成功释放次数
} HeapStats_t;
```

**作用：**用于存储堆内存的统计信息

**注意：**
- 仅heap_4和heap_5支持完整统计
- 需要configUSE_TRACE_FACILITY为1

---

### 2.2 HeapRegion_t - 内存区域定义结构体
**结构体定义：**
```c
typedef struct xHeapRegion
{
    uint8_t *pucStartAddress;   // 内存区域起始地址
    size_t xSizeInBytes;        // 内存区域大小（字节）
} HeapRegion_t;
```

**作用：**定义内存区域，用于heap_5

**使用示例：**
```c
const HeapRegion_t xHeapRegions[] =
{
    { (uint8_t *)0x20000000UL, 0x10000 },  // 64KB @ 0x20000000
    { (uint8_t *)0x30000000UL, 0x20000 },  // 128KB @ 0x30000000
    { NULL, 0 }                             // 结束标记
};
```

---

## 3. 内存分配和释放API

### 3.1 pvPortMalloc() - 分配内存
**函数原型：**
```c
void *pvPortMalloc(size_t xWantedSize);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| xWantedSize | size_t | 要分配的内存大小（字节） |

**返回值：**
- 成功：返回分配的内存指针
- 失败：返回NULL

**作用：**从FreeRTOS堆中分配指定大小的内存

**注意：**
- 线程安全，可在任务中调用
- 不能在中断中调用
- 分配失败会调用内存分配失败钩子（如果配置）
- 分配的内存不会自动初始化

**使用示例：**
```c
uint8_t *pBuffer;

pBuffer = (uint8_t *)pvPortMalloc(100);
if (pBuffer != NULL)
{
    // 使用内存
    memset(pBuffer, 0, 100);
    
    // 使用完毕后释放
    vPortFree(pBuffer);
}
```

---

### 3.2 vPortFree() - 释放内存
**函数原型：**
```c
void vPortFree(void *pv);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pv | void* | 要释放的内存指针 |

**返回值：**无

**作用：**释放之前分配的内存

**注意：**
- 只能释放由pvPortMalloc()分配的内存
- 不能在heap_1中使用（不支持释放）
- 释放NULL指针是安全的（无操作）
- 释放后指针应置为NULL
- 不能在中断中调用

**使用示例：**
```c
uint8_t *pBuffer;

pBuffer = (uint8_t *)pvPortMalloc(100);
if (pBuffer != NULL)
{
    // 使用内存
    
    // 释放内存
    vPortFree(pBuffer);
    pBuffer = NULL;  // 防止悬空指针
}
```

---

## 4. 堆信息查询API

### 4.1 xPortGetFreeHeapSize() - 获取当前可用堆大小
**函数原型：**
```c
size_t xPortGetFreeHeapSize(void);
```

**参数说明：**无参数

**返回值：**当前可用堆空间大小（字节）

**作用：**获取堆中当前可用的空闲空间大小

**注意：**
- 所有heap方案都支持
- 返回值是当前瞬时值
- 可能因内存碎片而无法分配返回值大小的内存

**使用示例：**
```c
size_t xFreeHeap;

xFreeHeap = xPortGetFreeHeapSize();
printf("Free heap: %u bytes\n", xFreeHeap);

if (xFreeHeap < 1024)
{
    printf("Warning: Low memory!\n");
}
```

---

### 4.2 xPortGetMinimumEverFreeHeapSize() - 获取历史最小剩余堆大小
**函数原型：**
```c
size_t xPortGetMinimumEverFreeHeapSize(void);
```

**参数说明：**无参数

**返回值：**系统运行以来的最小剩余堆空间（字节）

**作用：**获取自系统启动以来的历史最小剩余堆空间

**宏配置要求：**
- 仅heap_4和heap_5支持

**注意：**
- 用于评估堆使用情况
- 帮助优化configTOTAL_HEAP_SIZE设置
- 是系统运行期间的最坏情况

**使用示例：**
```c
size_t xMinimumEverFree;

xMinimumEverFree = xPortGetMinimumEverFreeHeapSize();
printf("Minimum ever free heap: %u bytes\n", xMinimumEverFree);

// 建议堆大小 = 当前使用量 + 安全余量
size_t xSuggestedHeapSize = (configTOTAL_HEAP_SIZE - xMinimumEverFree) + 512;
printf("Suggested heap size: %u bytes\n", xSuggestedHeapSize);
```

---

### 4.3 vPortGetHeapStats() - 获取详细堆统计信息
**函数原型：**
```c
void vPortGetHeapStats(HeapStats_t *pxHeapStats);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pxHeapStats | HeapStats_t* | 指向HeapStats_t结构体的指针 |

**返回值：**无（通过参数返回统计信息）

**作用：**获取详细的堆内存统计信息

**宏配置要求：**
- configUSE_TRACE_FACILITY 必须为1
- 仅heap_4和heap_5完全支持

**统计信息包括：**
- 当前可用空间
- 最大空闲块大小
- 最小空闲块大小
- 空闲块数量
- 历史最小剩余空间
- 分配和释放次数

**使用示例：**
```c
HeapStats_t xHeapStats;

vPortGetHeapStats(&xHeapStats);

printf("Heap Statistics:\n");
printf("  Available: %u bytes\n", xHeapStats.xAvailableHeapSpaceInBytes);
printf("  Largest free block: %u bytes\n", xHeapStats.xSizeOfLargestFreeBlockInBytes);
printf("  Smallest free block: %u bytes\n", xHeapStats.xSizeOfSmallestFreeBlockInBytes);
printf("  Free blocks: %u\n", xHeapStats.xNumberOfFreeBlocks);
printf("  Minimum ever free: %u bytes\n", xHeapStats.xMinimumEverFreeBytesRemaining);
printf("  Successful allocations: %u\n", xHeapStats.xNumberOfSuccessfulAllocations);
printf("  Successful frees: %u\n", xHeapStats.xNumberOfSuccessfulFrees);

// 计算内存碎片程度
if (xHeapStats.xAvailableHeapSpaceInBytes > 0)
{
    float fragmentation = (float)xHeapStats.xSizeOfLargestFreeBlockInBytes / 
                          (float)xHeapStats.xAvailableHeapSpaceInBytes * 100.0f;
    printf("  Fragmentation: %.1f%%\n", 100.0f - fragmentation);
}
```

---

## 5. heap_5特有API

### 5.1 vPortDefineHeapRegions() - 定义堆内存区域
**函数原型：**
```c
void vPortDefineHeapRegions(const HeapRegion_t * const pxHeapRegions);
```

**参数说明：**
| 参数名 | 类型 | 说明 |
|--------|------|------|
| pxHeapRegions | const HeapRegion_t* | 内存区域数组指针 |

**返回值：**无

**作用：**为heap_5定义多个不连续的内存区域

**注意：**
- 仅适用于heap_5
- 必须在调用任何FreeRTOS API之前调用
- 数组必须以{NULL, 0}结束
- 区域自动按地址排序

**使用示例：**
```c
// 定义多个内存区域
const HeapRegion_t xHeapRegions[] =
{
    { (uint8_t *)0x20000000UL, 0x10000 },  // SRAM1: 64KB
    { (uint8_t *)0x30000000UL, 0x20000 },  // SRAM2: 128KB
    { (uint8_t *)0x38000000UL, 0x8000  },  // SRAM3: 32KB
    { NULL, 0 }                             // 结束标记
};

int main(void)
{
    // 必须在任何FreeRTOS API调用之前初始化堆
    vPortDefineHeapRegions(xHeapRegions);
    
    // 现在可以创建任务、队列等
    xTaskCreate(...);
    
    vTaskStartScheduler();
    
    return 0;
}
```

---

## 6. 内存分配失败钩子函数

### 6.1 vApplicationMallocFailedHook() - 内存分配失败钩子
**函数原型：**
```c
void vApplicationMallocFailedHook(void);
```

**参数说明：**无参数

**返回值：**无

**作用：**内存分配失败时被调用的钩子函数

**宏配置要求：**
- configUSE_MALLOC_FAILED_HOOK 必须为1

**注意：**
- 由用户实现
- 在内存分配失败时自动调用
- 用于错误处理和调试
- 通常不应返回（系统可能无法继续运行）

**实现示例：**
```c
void vApplicationMallocFailedHook(void)
{
    // 内存分配失败处理
    printf("FATAL: Memory allocation failed!\n");
    
    // 打印堆信息
    size_t xFreeHeap = xPortGetFreeHeapSize();
    printf("Current free heap: %u bytes\n", xFreeHeap);
    
    #if (configUSE_TRACE_FACILITY == 1)
    HeapStats_t xHeapStats;
    vPortGetHeapStats(&xHeapStats);
    printf("Largest free block: %u bytes\n", 
           xHeapStats.xSizeOfLargestFreeBlockInBytes);
    #endif
    
    // 进入死循环或重启系统
    taskDISABLE_INTERRUPTS();
    while (1)
    {
        // 可以闪烁LED指示错误
    }
}
```

---

## 7. 配置选项

### 7.1 FreeRTOSConfig.h相关配置
```c
// 堆大小配置（字节）
#define configTOTAL_HEAP_SIZE                   ((size_t)(20 * 1024))

// 启用内存分配失败钩子
#define configUSE_MALLOC_FAILED_HOOK            1

// 启用堆统计功能（heap_4和heap_5）
#define configUSE_TRACE_FACILITY                1

// 动态内存分配支持
#define configSUPPORT_DYNAMIC_ALLOCATION        1

// 静态内存分配支持
#define configSUPPORT_STATIC_ALLOCATION         1

// heap_3专用：使用newlib的可重入malloc（某些平台）
#define configUSE_NEWLIB_REENTRANT              0
```

**配置说明：**
- **configTOTAL_HEAP_SIZE**：定义堆总大小（heap_1/2/4/5使用）
- **configUSE_MALLOC_FAILED_HOOK**：启用分配失败钩子
- **configUSE_TRACE_FACILITY**：启用详细统计信息
- **configSUPPORT_DYNAMIC_ALLOCATION**：启用动态分配API
- **configSUPPORT_STATIC_ALLOCATION**：启用静态分配API

---

### 7.2 堆数组定义（heap_1/2/4）
```c
// 在heap_x.c文件中
#if (configAPPLICATION_ALLOCATED_HEAP == 1)
    // 应用程序提供堆数组
    extern uint8_t ucHeap[configTOTAL_HEAP_SIZE];
#else
    // FreeRTOS内部定义堆数组
    static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
#endif
```

**自定义堆位置示例：**
```c
// 在应用程序代码中
#define configAPPLICATION_ALLOCATED_HEAP    1

// 将堆放置在特定内存区域（使用链接器脚本或编译器属性）
__attribute__((section(".ccmram")))
uint8_t ucHeap[configTOTAL_HEAP_SIZE];
```

---

## 8. 各内存管理方案详解

### 8.1 heap_1 - 最简单方案
**特点：**
- 只分配，不释放
- 确定性，无碎片
- 代码最小

**适用场景：**
- 系统启动时创建所有对象
- 对象从不删除
- 嵌入式系统中最常见的使用模式

**实现原理：**
```c
// 简化的heap_1原理
static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
static size_t xNextFreeByte = 0;

void *pvPortMalloc(size_t xWantedSize)
{
    void *pvReturn;
    
    // 字节对齐
    xWantedSize = (xWantedSize + portBYTE_ALIGNMENT - 1) & 
                  ~(portBYTE_ALIGNMENT - 1);
    
    if ((xNextFreeByte + xWantedSize) < configTOTAL_HEAP_SIZE)
    {
        pvReturn = &ucHeap[xNextFreeByte];
        xNextFreeByte += xWantedSize;
    }
    else
    {
        pvReturn = NULL;
    }
    
    return pvReturn;
}

void vPortFree(void *pv)
{
    // heap_1不支持释放
}
```

**优点：**
- 分配速度快且确定
- 无内存碎片
- 代码体积小

**缺点：**
- 不能释放内存
- 不适合动态创建/删除对象

---

### 8.2 heap_2 - 支持释放的简单方案
**特点：**
- 支持内存释放
- 使用最佳匹配算法
- 不合并相邻空闲块

**适用场景：**
- 重复分配/释放相同大小的对象
- 不需要合并碎片的场景

**实现原理：**
- 维护空闲块链表
- 分配时查找最小的足够大的块
- 释放时将块加入空闲链表
- 不会合并相邻的空闲块

**优点：**
- 支持内存释放
- 固定大小对象效率高

**缺点：**
- 容易产生内存碎片
- 不合并空闲块
- 已被heap_4取代（不推荐使用）

---

### 8.3 heap_3 - 封装标准库malloc/free
**特点：**
- 封装C标准库的malloc()和free()
- 添加线程安全保护
- 不需要configTOTAL_HEAP_SIZE

**适用场景：**
- 需要使用标准库内存管理
- 已有malloc/free实现的平台
- 需要与第三方库共享堆

**实现原理：**
```c
// 简化的heap_3原理
void *pvPortMalloc(size_t xWantedSize)
{
    void *pvReturn;
    
    vTaskSuspendAll();
    {
        pvReturn = malloc(xWantedSize);
    }
    xTaskResumeAll();
    
    return pvReturn;
}

void vPortFree(void *pv)
{
    if (pv != NULL)
    {
        vTaskSuspendAll();
        {
            free(pv);
        }
        xTaskResumeAll();
    }
}
```

**优点：**
- 使用标准库实现
- 可与其他代码共享堆
- 自动管理堆大小

**缺点：**
- 依赖C库实现
- 可能不是确定性的
- 代码体积较大

---

### 8.4 heap_4 - 推荐的通用方案
**特点：**
- 支持内存释放
- 自动合并相邻空闲块
- 使用首次适配算法
- 减少内存碎片

**适用场景：**
- 通用应用（推荐使用）
- 动态创建/删除对象
- 需要减少内存碎片

**实现原理：**
- 维护按地址排序的空闲块链表
- 分配时查找第一个足够大的块
- 释放时合并相邻空闲块
- 减少内存碎片

**数据结构：**
```c
typedef struct A_BLOCK_LINK
{
    struct A_BLOCK_LINK *pxNextFreeBlock;  // 下一个空闲块
    size_t xBlockSize;                      // 块大小
} BlockLink_t;
```

**优点：**
- 自动合并空闲块
- 减少内存碎片
- 适用于大多数场景
- 支持完整统计信息

**缺点：**
- 比heap_1复杂
- 非确定性（但可预测）

**内存布局：**
```
+----------------+
| BlockLink_t    | <- 块头
+----------------+
| User Data      | <- 用户数据
|                |
+----------------+
| BlockLink_t    | <- 下一个块头
+----------------+
```

---

### 8.5 heap_5 - 支持多内存区域
**特点：**
- heap_4的扩展版本
- 支持多个不连续的内存区域
- 自动合并相邻空闲块

**适用场景：**
- 处理器有多个RAM区域
- 需要使用不连续的内存
- 内部RAM和外部RAM混合使用

**实现原理：**
- 与heap_4相同，但支持多个堆区域
- 区域按地址排序
- 在所有区域中查找空闲块

**配置示例：**
```c
// STM32H7系列，多个SRAM区域
const HeapRegion_t xHeapRegions[] =
{
    { (uint8_t *)0x20000000UL, 128 * 1024 },  // DTCM: 128KB
    { (uint8_t *)0x24000000UL, 512 * 1024 },  // AXI SRAM: 512KB
    { (uint8_t *)0x30000000UL, 256 * 1024 },  // SRAM1: 256KB
    { (uint8_t *)0x38000000UL, 64 * 1024  },  // SRAM4: 64KB
    { NULL, 0 }
};

int main(void)
{
    // 必须首先初始化堆
    vPortDefineHeapRegions(xHeapRegions);
    
    // 然后才能使用FreeRTOS API
    xTaskCreate(...);
    vTaskStartScheduler();
}
```

**优点：**
- 充分利用所有可用RAM
- 灵活配置内存布局
- 与heap_4相同的碎片管理

**缺点：**
- 需要手动配置内存区域
- 略微复杂
