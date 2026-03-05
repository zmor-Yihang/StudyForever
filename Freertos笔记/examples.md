# 队列使用示例

## 1.任务之间消息传递

在esp32-IDF中创建任务时，**优先级不能小于1**，因为app_main()函数是在main任务中，main任务由系统创建，main任务最后会调用app_main()函数，如果用户创建的任务优先级为1，会有竞争问题。

```c
/* task1.c文件 */
#include "task1.h"

/* 任务句柄 */
TaskHandle_t htask1 = NULL;

void task1(void *pvParameters)
{
    static uint8_t count = 0;
    while (1)
    {
        printf("task1 is Running!\n");
        count++;
        esp_rom_delay_us(10000); // 延时0.01秒
        xQueueSend(hqueueOfInfo, &count, 1000);
    }
}
```

```c
/* task1.h文件 */
#ifndef __TASK1_H__
#define __TASK1_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "esp_timer.h"
#include "user_queue.h"

extern TaskHandle_t htask1;

void task1(void *pvParameters);

#endif // __TASK1_H__
```

```c
/* task2.c文件 */
#include "task2.h"

TaskHandle_t htask2 = NULL;

void task2(void *pvParameters)
{
    uint8_t info;
    while (1)
    {
        printf("task2 is Running!\n");
        esp_rom_delay_us(10000); // 延时0.01秒
        xQueueReceive(hqueueOfInfo, &info, 1000);
        printf("task2 received info: %d\n", info);
        // vTaskDelay(1000);
    }
}
```

```c
/* task2.h文件 */
#ifndef __TASK2_H__
#define __TASK2_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "user_queue.h"
#include "esp_timer.h"

extern TaskHandle_t htask2;

void task2(void *pvParameters);

#endif // __TASK2_H__
```

```c
/* task3.c文件 */
#include "task3.h"

TaskHandle_t htask3 = NULL;

void task3(void *pvParameters)
{
    led_init();
    while (1)
    {
        printf("task3 is Running!---------------------\n");
        led_toggle();
        vTaskDelay(1000);
    }
}
```

```c
/* task3.h文件 */
.#ifndef __TASK3_H__
#define __TASK3_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"

extern TaskHandle_t htask3;

void task3(void *pvParameters);

#endif // __TASK3_H__
```

```c
/* user_queue.c文件 */
#include "user_queue.h"
#include "esp_log.h"

/* 消息队列句柄 */
QueueHandle_t hqueueOfInfo = NULL;

void queue_init(void)
{
    /* 创建消息队列 */
    hqueueOfInfo = xQueueCreate(10, sizeof(uint8_t));
}
```

```c
/* user_queue.h文件 */
#ifndef __USER_QUEUE_H__
#define __USER_QUEUE_H__

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t hqueueOfInfo;

void queue_init(void);

#endif  // __USER_QUEUE_H__
```

```c
/* main文件 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "user_task.h"

void app_main(void)
{
    queue_init();
    task_init();

    vTaskDelete(NULL); // 删除主任务main
}
```

## 2.中断任务之间消息传递

