# FreeRTOS 列表与列表项详解

## 1. 列表 (List) 结构体

### List_t 结构体定义
```c
typedef struct xLIST
{
    volatile UBaseType_t uxNumberOfItems;    /* 列表中的列表项数量 */
    ListItem_t * configLIST_VOLATILE pxIndex;/* 用于遍历列表的指针，指向当前正在使用的列表项 */
    MiniListItem_t xListEnd;                 /* 列表项，包含最大可能值，用于标记列表结束 */
} List_t;
```

### 成员解释
- **uxNumberOfItems**: 记录列表中当前包含的列表项数量，不包含xListEnd
- **pxIndex**: 指向列表中的某个列表项，用于遍历列表，类似于迭代器，初始化函数会将其指向迷你列表项
- **xListEnd**: 迷你列表项，作为列表的结束标记，其xItemValue为portMAX_DELAY（最大值）

## 2. 列表项 (List Item) 结构体

### ListItem_t 结构体定义
```c
struct xLIST_ITEM
{
    configLIST_VOLATILE TickType_t xItemValue;      /* 列表项的值，用于排序 */
    struct xLIST_ITEM * configLIST_VOLATILE pxNext; /* 指向下一个列表项 */
    struct xLIST_ITEM * configLIST_VOLATILE pxPrevious;/* 指向前一个列表项 */
    void * pvOwner;                                 /* 指向包含此列表项的对象（通常是TCB） */
    struct xLIST * configLIST_VOLATILE pxContainer; /* 指向此列表项所属的列表 */
};
typedef struct xLIST_ITEM ListItem_t;
```

### 成员解释
- **xItemValue**: 列表项的值，通常用于优先级排序，值越小优先级越高
- **pxNext**: 指向下一个列表项的指针，实现双向链表
- **pxPrevious**: 指向前一个列表项的指针，实现双向链表
- **pvOwner**: 指向拥有此列表项的对象，通常是任务控制块(TCB)
- **pxContainer**: 指向此列表项所属的列表，用于检查列表项是否在某个列表中

## 3. 迷你列表项 (Mini List Item) 结构体

### MiniListItem_t 结构体定义
```c
struct xMINI_LIST_ITEM
{
    configLIST_VOLATILE TickType_t xItemValue;            /* 列表项的值 */
    struct xLIST_ITEM * configLIST_VOLATILE pxNext;       /* 指向下一个列表项 */
    struct xLIST_ITEM * configLIST_VOLATILE pxPrevious;   /* 指向前一个列表项 */
};
typedef struct xMINI_LIST_ITEM MiniListItem_t;
```

### 成员解释
- **xItemValue**: 列表项的值，在xListEnd中设置为portMAX_DELAY
- **pxNext**: 指向下一个列表项的指针
- **pxPrevious**: 指向前一个列表项的指针

## 4. 列表与列表项的关系

### 数据结构特点
- **双向环形链表**: FreeRTOS的列表实现为**双向环形链表**，列表的**内存是不连续**的，列表项之间通过指针连接
- **有序插入**: 列表项按xItemValue升序插入，值越小越靠前
- **快速访问**: 通过pxIndex可以快速遍历列表
- **内存效率**: MiniListItem_t比ListItem_t少了pvOwner和pxContainer，节省内存

### 典型应用场景
- **就绪列表**: 存储就绪状态的任务
- **阻塞列表**: 存储因等待事件而阻塞的任务
- **挂起列表**: 存储被挂起的任务
- **延时列表**: 存储处于延时状态的任务

## 5. 常用操作函数

### 列表初始化
```c
void vListInitialise( List_t * const pxList );
```

### 列表项初始化
```c
void vListInitialiseItem( ListItem_t * const pxItem );
/**
 * vListInitialiseItem()
 * ----------------------------------------------------------
 * 初始化一个列表项，使其处于“未插入任何列表”的安全状态。
 *
 * 将 pxItem->pvContainer 设为 NULL，表示该列表项当前不属于
 * 任何列表；同时将 listNEXT 和 listPREVIOUS 指向自身，形成
 * 一个空自环，防止未初始化指针导致的硬 fault。
 *
 * 必须在把列表项插入任何列表之前调用一次，通常由任务控制块
 * (TCB) 或事件控制块在创建时完成。
 *
 * 参数：
 *    pxItem - 指向待初始化的 ListItem_t 结构体
 *
 * 返回：无
 */
```

### 列表项插入
```c
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem );
void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem );
/**
 * vListInsert()
 * ----------------------------------------------------------
 * 将新列表项按 xItemValue 升序插入到列表中。
 *
 * 内部会从 xListEnd 开始沿升序方向查找，直到找到第一个
 * 大于等于 pxNewListItem->xItemValue 的节点，然后把新项
 * 插到该节点前面。
 *
 * 结果：列表始终按 xItemValue 保持升序（除 xListEnd 外）。
 * 时间：O(n) —— 需要遍历。
 *
 * 典型用途：把等待超时、周期唤醒等需要按时间排序的对象
 * 插入到就绪/延时/阻塞列表中。
 */

/**
 * vListInsertEnd()
 * ----------------------------------------------------------
 * 把新列表项直接插到 pxIndex 所指向位置的前面，然后更新
 * pxIndex 使其指向新插入的节点。
 *
 * 结果：不保证任何排序；新节点总是位于“当前遍历位置”的
 * 尾部，相当于循环链表中的“队尾”插入。
 * 时间：O(1) —— 无需遍历。
 *
 * 典型用途：快速把就绪任务追加到就绪列表尾部，实现
 * 时间片轮转调度。
 */
```

### 列表项移除
```c
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove );
```

## 6. 重要宏定义

```c
/* 检查列表是否为空 */
#define listLIST_IS_EMPTY(pxList)           ((pxList)->uxNumberOfItems == (UBaseType_t) 0)

/* 获取列表中列表项数量 */
#define listCURRENT_LIST_LENGTH(pxList)     ((pxList)->uxNumberOfItems)

/* 获取列表项的拥有者 */
#define listGET_OWNER_OF_HEAD_ENTRY(pxList) ((pxList)->pxIndex->pxNext->pvOwner)

/* 获取列表项的值 */
#define listGET_LIST_ITEM_VALUE(pxItem)     ((pxItem)->xItemValue)
```

## 7. 使用示例

### 示例1：基本列表操作
```c
#include "FreeRTOS.h"
#include "list.h"

void list_example_basic(void)
{
    List_t xMyList;
    ListItem_t xListItem1, xListItem2, xListItem3;
    
    /* 初始化列表 */
    vListInitialise(&xMyList);
    
    /* 初始化列表项 */
    vListInitialiseItem(&xListItem1);
    vListInitialiseItem(&xListItem2);
    vListInitialiseItem(&xListItem3);
    
    /* 设置列表项的值 */
    listSET_LIST_ITEM_VALUE(&xListItem1, 10);
    listSET_LIST_ITEM_VALUE(&xListItem2, 5);
    listSET_LIST_ITEM_VALUE(&xListItem3, 20);
    
    /* 插入列表项（按值排序） */
    vListInsert(&xMyList, &xListItem1);
    vListInsert(&xMyList, &xListItem2);
    vListInsert(&xMyList, &xListItem3);
    
    /* 检查列表状态 */
    printf("列表中的项目数量: %d\n", listCURRENT_LIST_LENGTH(&xMyList));
    printf("列表是否为空: %s\n", listLIST_IS_EMPTY(&xMyList) ? "是" : "否");
    
    /* 移除一个列表项 */
    uxListRemove(&xListItem2);
    printf("移除一项后，列表中的项目数量: %d\n", listCURRENT_LIST_LENGTH(&xMyList));
}
```

### 示例2：任务优先级列表模拟
```c
#include "FreeRTOS.h"
#include "list.h"

typedef struct {
    char taskName[20];
    UBaseType_t priority;
    ListItem_t xGenericListItem;
} SimpleTask_t;

void priority_list_example(void)
{
    List_t xReadyList;
    SimpleTask_t xTask1, xTask2, xTask3;
    ListItem_t *pxIterator;
    const ListItem_t *pxListEnd;
    
    /* 初始化就绪列表 */
    vListInitialise(&xReadyList);
    
    /* 初始化任务 */
    strcpy(xTask1.taskName, "Task1");
    xTask1.priority = 2;
    vListInitialiseItem(&xTask1.xGenericListItem);
    listSET_LIST_ITEM_VALUE(&xTask1.xGenericListItem, xTask1.priority);
    xTask1.xGenericListItem.pvOwner = &xTask1;
    
    strcpy(xTask2.taskName, "Task2");
    xTask2.priority = 1;
    vListInitialiseItem(&xTask2.xGenericListItem);
    listSET_LIST_ITEM_VALUE(&xTask2.xGenericListItem, xTask2.priority);
    xTask2.xGenericListItem.pvOwner = &xTask2;
    
    strcpy(xTask3.taskName, "Task3");
    xTask3.priority = 3;
    vListInitialiseItem(&xTask3.xGenericListItem);
    listSET_LIST_ITEM_VALUE(&xTask3.xGenericListItem, xTask3.priority);
    xTask3.xGenericListItem.pvOwner = &xTask3;
    
    /* 将任务添加到就绪列表 */
    vListInsert(&xReadyList, &xTask1.xGenericListItem);
    vListInsert(&xReadyList, &xTask2.xGenericListItem);
    vListInsert(&xReadyList, &xTask3.xGenericListItem);
    
    /* 遍历列表，按优先级顺序打印任务 */
    printf("就绪列表中的任务（按优先级排序）:\n");
    pxListEnd = listGET_END_MARKER(&xReadyList);
    pxIterator = listGET_HEAD_ENTRY(&xReadyList);
    
    while (pxIterator != pxListEnd) {
        SimpleTask_t *pxTask = (SimpleTask_t *)pxIterator->pvOwner;
        printf("任务名: %s, 优先级: %d\n", pxTask->taskName, pxTask->priority);
        pxIterator = listGET_NEXT(pxIterator);
    }
}
```

### 示例3：延时列表模拟
```c
#include "FreeRTOS.h"
#include "list.h"

typedef struct {
    char taskName[20];
    TickType_t delayTime;
    ListItem_t xStateListItem;
} DelayTask_t;

void delay_list_example(void)
{
    List_t xDelayedTaskList;
    DelayTask_t xTask1, xTask2, xTask3;
    TickType_t xCurrentTime = xTaskGetTickCount();
    
    /* 初始化延时列表 */
    vListInitialise(&xDelayedTaskList);
    
    /* 初始化延时任务 */
    strcpy(xTask1.taskName, "DelayTask1");
    xTask1.delayTime = xCurrentTime + 100; /* 100 ticks后唤醒 */
    vListInitialiseItem(&xTask1.xStateListItem);
    listSET_LIST_ITEM_VALUE(&xTask1.xStateListItem, xTask1.delayTime);
    xTask1.xStateListItem.pvOwner = &xTask1;
    
    strcpy(xTask2.taskName, "DelayTask2");
    xTask2.delayTime = xCurrentTime + 50;  /* 50 ticks后唤醒 */
    vListInitialiseItem(&xTask2.xStateListItem);
    listSET_LIST_ITEM_VALUE(&xTask2.xStateListItem, xTask2.delayTime);
    xTask2.xStateListItem.pvOwner = &xTask2;
    
    strcpy(xTask3.taskName, "DelayTask3");
    xTask3.delayTime = xCurrentTime + 200; /* 200 ticks后唤醒 */
    vListInitialiseItem(&xTask3.xStateListItem);
    listSET_LIST_ITEM_VALUE(&xTask3.xStateListItem, xTask3.delayTime);
    xTask3.xStateListItem.pvOwner = &xTask3;
    
    /* 将任务添加到延时列表 */
    vListInsert(&xDelayedTaskList, &xTask1.xStateListItem);
    vListInsert(&xDelayedTaskList, &xTask2.xStateListItem);
    vListInsert(&xDelayedTaskList, &xTask3.xStateListItem);
    
    /* 检查哪个任务最先到期 */
    if (!listLIST_IS_EMPTY(&xDelayedTaskList)) {
        ListItem_t *pxNextExpiry = listGET_HEAD_ENTRY(&xDelayedTaskList);
        DelayTask_t *pxTask = (DelayTask_t *)pxNextExpiry->pvOwner;
        printf("下一个到期的任务: %s, 到期时间: %d\n", 
               pxTask->taskName, pxTask->delayTime);
    }
}
```

### 示例4：列表的常用宏操作
```c
void list_macro_example(void)
{
    List_t xTestList;
    ListItem_t xItem1, xItem2;
    
    /* 初始化 */
    vListInitialise(&xTestList);
    vListInitialiseItem(&xItem1);
    vListInitialiseItem(&xItem2);
    
    /* 设置列表项值 */
    listSET_LIST_ITEM_VALUE(&xItem1, 10);
    listSET_LIST_ITEM_VALUE(&xItem2, 20);
    
    /* 插入列表项 */
    vListInsert(&xTestList, &xItem1);
    vListInsert(&xTestList, &xItem2);
    
    /* 使用宏进行操作 */
    printf("列表长度: %d\n", listCURRENT_LIST_LENGTH(&xTestList));
    printf("列表是否为空: %s\n", listLIST_IS_EMPTY(&xTestList) ? "是" : "否");
    printf("列表项1的值: %d\n", listGET_LIST_ITEM_VALUE(&xItem1));
    printf("列表项2的值: %d\n", listGET_LIST_ITEM_VALUE(&xItem2));
    
    /* 获取列表头部项目的拥有者（如果设置了的话） */
    if (!listLIST_IS_EMPTY(&xTestList)) {
        void *pvOwner = listGET_OWNER_OF_HEAD_ENTRY(&xTestList);
        printf("头部列表项的拥有者地址: %p\n", pvOwner);
    }
}
```

### 使用注意事项
1. **内存管理**: 列表和列表项的内存需要用户自行管理，FreeRTOS只负责链接操作
2. **线程安全**: 在多任务环境中操作列表时要注意临界区保护
3. **初始化**: 使用前必须先初始化列表和列表项
4. **值排序**: vListInsert()按xItemValue升序插入，vListInsertEnd()插入到末尾
5. **环形结构**: 列表是环形的，遍历时要注意结束条件
6. **pxContainer**: 列表项被插入列表后，pxContainer会自动设置为指向该列表