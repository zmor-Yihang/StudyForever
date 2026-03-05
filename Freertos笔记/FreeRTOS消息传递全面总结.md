好的，针对您提炼出的核心口诀，我将为每一项提供具体的、贴近实际项目的使用场景。

---

### 1. 传数据 -> 队列

**场景：多传感器数据采集与显示系统**

*   **描述**：
    *   **任务A（传感器采集任务）**：周期性地读取温度传感器和湿度传感器的值，并将它们封装成一个结构体 `struct { float temp; float humi; } sensor_data_t;`。
    *   **任务B（显示任务）**：负责将数据刷新到OLED屏幕上。
*   **为什么使用队列**：
    *   **传递数据本身**：任务A产生的 `sensor_data_t` 结构体需要完整地传递给任务B。
    *   **生产者-消费者模型**：任务A是生产者，任务B是消费者。队列充当一个安全的缓冲区，即使任务B因为屏幕刷新较慢而暂时无法处理数据，任务A采集的新数据也能在队列中暂存，不会丢失。
    *   **解耦**：两个任务互不关心对方的存在，只与队列交互，降低了系统耦合度。
*   **伪代码示例**：
    ```c
    // 创建队列，可容纳10个sensor_data_t结构体
    QueueHandle_t xSensorQueue = xQueueCreate(10, sizeof(sensor_data_t));
    
    // 传感器任务
    void vSensorTask(void *pvParameters) {
        sensor_data_t data;
        while(1) {
            data.temp = read_temperature();
            data.humi = read_humidity();
            // 将数据发送到队列，如果队列满则等待10个Tick
            xQueueSend(xSensorQueue, &data, pdMS_TO_TICKS(10));
            vTaskDelay(pdMS_TO_TICKS(1000)); // 每秒采集一次
        }
    }
    
    // 显示任务
    void vDisplayTask(void *pvParameters) {
        sensor_data_t received_data;
        while(1) {
            // 从队列接收数据，如果队列空则永久等待
            if(xQueueReceive(xSensorQueue, &received_data, portMAX_DELAY)) {
                oled_show_temp(received_data.temp);
                oled_show_humi(received_data.humi);
            }
        }
    }
    ```

---

### 2. 一对一通知 -> 任务通知

**场景：中断接收数据并通知处理任务**

*   **描述**：
    *   一个UART串口中断服务程序（ISR）接收到一帧完整的数据。
    *   一个**特定的**数据处理任务 `vDataProcessTask` 需要被唤醒来处理这帧数据。
*   **为什么使用任务通知**：
    *   **高效**：这是最核心的原因。任务通知直接操作目标任务的控制块，无需经过中间的队列或信号量对象，速度最快，内存开销最小。
    *   **一对一**：中断源明确知道要通知哪个任务。
    *   **可以携带数据**：可以将接收数据的缓冲区指针作为“值”传递给任务。
*   **伪代码示例**：
    ```c
    TaskHandle_t xDataProcessTaskHandle; // 数据处理任务的句柄
    
    // 数据处理任务
    void vDataProcessTask(void *pvParameters) {
        void *pReceivedData;
        while(1) {
            // 等待通知，并获取传递过来的“值”（即指针）
            if(xTaskNotifyWait(0, ULONG_MAX, (uint32_t*)&pReceivedData, portMAX_DELAY)) {
                // 收到通知，pReceivedData就是数据地址，开始处理
                process_data(pReceivedData);
            }
        }
    }
    
    // UART中断服务程序
    void USART_IRQHandler(void) {
        if(/* 接收中断 */) {
            void *pData = get_received_data_buffer(); // 获取数据缓冲区
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
            // 直接通知目标任务，并将数据指针作为通知值发送
            vTaskNotifyGiveFromISR(xDataProcessTaskHandle, pData, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // 如果需要，执行任务切换
        }
    }
    ```

---

### 3. 保护资源 -> 互斥量

**场景：多个任务共享同一个SPI总线**

*   **描述**：
    *   任务A（存储任务）需要SPI总线来读写SD卡。
    *   任务B（通信任务）需要SPI总线与LoRa模块通信。
    *   SPI总线是共享资源，不能同时被两个任务操作。
*   **为什么使用互斥量**：
    *   **互斥访问**：互斥量就像一个钥匙，谁拿到了钥匙（获取了互斥量）谁才能使用SPI总线。
    *   **解决优先级反转**：互斥量具有“优先级继承”机制。如果低优先级任务A占着SPI总线，而高优先级任务B在等待，系统会临时将任务A的优先级提升到与任务B相同，让其尽快执行完释放总线，从而最大限度地减少高优先级任务的等待时间。这是它优于简单的“关中断”或“调度器锁”的地方。
*   **伪代码示例**：
    ```c
    SemaphoreHandle_t xSPIMutex;
    
    void main(void) {
        xSPIMutex = xSemaphoreCreateMutex(); // 创建互斥量
        // ... 创建其他任务
    }
    
    void vSDCardTask(void *pvParameters) {
        while(1) {
            // ... 其他逻辑
            if(xSemaphoreTake(xSPIMutex, portMAX_DELAY)) { // 获取互斥量
                spi_write_sdcard(...); // 安全地使用SPI
                xSemaphoreGive(xSPIMutex); // 释放互斥量
            }
        }
    }
    
    void vLoRaTask(void *pvParameters) {
        while(1) {
            // ... 其他逻辑
            if(xSemaphoreTake(xSPIMutex, portMAX_DELAY)) { // 获取互斥量
                spi_write_lora(...); // 安全地使用SPI
                xSemaphoreGive(xSPIMutex); // 释放互斥量
            }
        }
    }
    ```

---

### 4. 等多事件 -> 事件标志组

**场景：系统启动自检任务**

*   **描述**：
    *   一个系统启动任务 `vStartupTask`，必须在以下三个事件**都**发生后，才能继续执行，启动主应用程序：
        1.  **事件位0**：网络连接成功（由网络任务设置）。
        2.  **事件位1**：文件系统挂载成功（由文件系统任务设置）。
        3.  **事件位2**：关键传感器校准完成（由传感器任务设置）。
*   **为什么使用事件标志组**：
    *   **等待多个条件**：任务可以等待一个32位变量中的**任意位（或）** 或**所有位（与）** 被设置。
    *   **事件源分散**：三个事件由三个不同的任务（或中断）触发，但它们最终需要汇集到一个任务中进行统一决策。
    *   **不传递数据**：任务只关心“网络是否已连接”这个状态，而不需要知道连接的IP地址等具体信息（如果需要具体信息，则应使用队列）。
*   **伪代码示例**：
    ```c
    EventGroupHandle_t xSystemEvents;
    
    #define NETWORK_CONNECTED_BIT (1 << 0)
    #define FS_MOUNTED_BIT        (1 << 1)
    #define SENSOR_CALIB_BIT      (1 << 2)
    
    void vStartupTask(void *pvParameters) {
        // 等待所有事件位都被设置
        EventBits_t uxBits = xEventGroupWaitBits(
            xSystemEvents,           // 事件组句柄
            NETWORK_CONNECTED_BIT | FS_MOUNTED_BIT | SENSOR_CALIB_BIT, // 等待的位
            pdTRUE,                  // 退出后清除这些位
            pdTRUE,                  // 需要等待ALL位都置位
            portMAX_DELAY            // 永久等待
        );
    
        // 当所有事件都就绪后，继续执行主流程
        start_main_application();
    }
    
    // 网络任务连接成功后的回调
    void network_connected_callback(void) {
        xEventGroupSetBits(xSystemEvents, NETWORK_CONNECTED_BIT);
    }
    
    // 文件系统任务挂载成功后的回调
    void fs_mounted_callback(void) {
        xEventGroupSetBits(xSystemEvents, FS_MOUNTED_BIT);
    }
    ```

您说得对！**计数型信号量**是一个非常重要的同步机制，我之前确实遗漏了它。现在为您补充计数型信号量的详细说明和应用场景。

---

### 6. 资源计数 -> 计数型信号量

**核心作用**：**管理一组数量有限的相同资源**。它不像二进制信号量只有0/1两种状态，而是可以记录多个可用资源的数量。

**数据传递**：不传递任何数据，只记录可用资源的数量。

**场景一：内存池管理**

**描述**：
- 系统有一个固定大小的内存块池，比如包含10个内存块，每个块1KB。
- 多个任务（如网络任务、文件任务等）需要动态申请和释放这些内存块。

**为什么使用计数型信号量**：
- **跟踪可用资源数量**：信号量的计数值直接表示当前可用的内存块数量。
- **阻塞申请**：当任务申请内存时，如果计数值>0（表示有可用内存块），则立即获取成功并将计数值减1；如果计数值=0（表示内存已用完），则任务进入阻塞状态，直到有其他任务释放内存。
- **安全释放**：当任务释放内存块时，信号量计数值加1，并唤醒可能正在等待的任务。

**伪代码示例**：
```c
// 假设有10个内存块
#define MEMORY_BLOCK_COUNT 10
void *memory_pool[MEMORY_BLOCK_COUNT];
SemaphoreHandle_t xMemorySemaphore;

// 初始化内存池和计数信号量
void memory_init(void) {
    // 创建计数信号量，初始值为10，最大值为10
    xMemorySemaphore = xSemaphoreCreateCounting(MEMORY_BLOCK_COUNT, MEMORY_BLOCK_COUNT);
    
    // 实际分配内存块...
    for(int i = 0; i < MEMORY_BLOCK_COUNT; i++) {
        memory_pool[i] = pvPortMalloc(1024); // 每个块1KB
    }
}

// 申请内存块
void* allocate_memory(void) {
    // 等待可用的内存块
    if(xSemaphoreTake(xMemorySemaphore, portMAX_DELAY) == pdTRUE) {
        // 找到并返回一个空闲的内存块
        for(int i = 0; i < MEMORY_BLOCK_COUNT; i++) {
            if(memory_pool[i] != NULL) {
                void* block = memory_pool[i];
                memory_pool[i] = NULL; // 标记为已使用
                return block;
            }
        }
    }
    return NULL;
}

// 释放内存块
void free_memory(void* block) {
    // 将内存块放回池中
    for(int i = 0; i < MEMORY_BLOCK_COUNT; i++) {
        if(memory_pool[i] == NULL) {
            memory_pool[i] = block;
            // 释放信号量，表示一个内存块可用
            xSemaphoreGive(xMemorySemaphore);
            break;
        }
    }
}
```

**场景二：连接池管理（如TCP连接）**

**描述**：
- 嵌入式设备作为服务器，最多同时处理5个TCP客户端连接。
- 需要管理这些连接资源，防止超过系统承载能力。

**为什么使用计数型信号量**：
- **限制并发连接数**：信号量初始值为5，表示最多5个并发连接。
- **优雅的拒绝机制**：当新的连接请求到达时，如果信号量计数值>0，则接受连接；如果=0，可以让客户端等待或直接拒绝。
- **连接释放管理**：当连接关闭时，信号量计数值加1，允许新的连接建立。

**伪代码示例**：
```c
SemaphoreHandle_t xConnectionSemaphore;

void server_init(void) {
    // 最多5个并发连接
    xConnectionSemaphore = xSemaphoreCreateCounting(5, 5);
}

void connection_handler_task(void *pvParameters) {
    while(1) {
        // 等待可用的连接槽位，最多等待10秒
        if(xSemaphoreTake(xConnectionSemaphore, pdMS_TO_TICKS(10000)) == pdTRUE) {
            // 接受新的TCP连接
            int new_socket = accept_connection();
            if(new_socket >= 0) {
                // 创建任务处理这个连接
                xTaskCreate(process_client, "Client", 1024, (void*)new_socket, 2, NULL);
            } else {
                // 接受失败，归还信号量
                xSemaphoreGive(xConnectionSemaphore);
            }
        }
    }
}

void process_client(void *pvParameters) {
    int client_socket = (int)pvParameters;
    
    // 处理客户端请求...
    handle_client_requests(client_socket);
    
    // 关闭连接
    close_connection(client_socket);
    
    // 释放连接槽位
    xSemaphoreGive(xConnectionSemaphore);
    
    vTaskDelete(NULL); // 删除自己
}
```
