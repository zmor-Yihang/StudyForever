#include "user_MutexSemaphore.h"

/* 互斥信号量句柄 */
SemaphoreHandle_t MutexSemaphore_handle = NULL;

/**
 * @brief 初始化互斥信号量
 * 创建一个互斥信号量，并将其句柄存储在全局变量中。
 * 如果创建失败，会在控制台输出错误信息。
 *
 * @warning 重复调用此函数会导致内存泄漏
 */
void MutexSemaphore_init(void)
{
    MutexSemaphore_handle = xSemaphoreCreateMutex();
    if (MutexSemaphore_handle == NULL)
    {
        printf("Mutex Semaphore Create Failed!\n");
    }
}

/**
 * @brief 获取互斥信号量
 *
 * 尝试获取互斥信号量的所有权。如果信号量已被其他任务持有，
 * 当前任务将无限期等待直到获得信号量。
 * @warning 确保在获取信号量后及时释放，避免死锁
 */
void MutexSemaphore_take(void)
{
    if (MutexSemaphore_handle != NULL)
    {
        xSemaphoreTake(MutexSemaphore_handle, portMAX_DELAY);
    }
    else
    {
        printf("Mutex Semaphore Handle is NULL!\n");
    }
}

/**
 * @brief 释放互斥信号量
 *
 * @note 只有持有信号量的任务才能成功释放它
 * @warning 释放未持有的信号量会导致错误
 */
void MutexSemaphore_give(void)
{
    if (MutexSemaphore_handle != NULL)
    {
        if (xSemaphoreGive(MutexSemaphore_handle) != pdTRUE)
        {
            printf("Mutex Semaphore Give Failed!\n");
        }
    }
    else
    {
        printf("Mutex Semaphore Handle is NULL!\n");
    }
}

/**
 * @brief 删除互斥信号量
 *
 * 删除互斥信号量并释放相关资源。将全局句柄设置为NULL。
 */
void MutexSemaphore_delete(void)
{
    if (MutexSemaphore_handle != NULL)
    {
        vSemaphoreDelete(MutexSemaphore_handle);
        MutexSemaphore_handle = NULL;
    }
    else
    {
        printf("Mutex Semaphore Handle is NULL!\n");
    }
}

/**
 * @brief 获取当前持有互斥信号量的任务句柄
 *
 * 返回当前持有互斥信号量的任务的句柄。如果没有任务持有信号量，返回NULL。
 *
 * @return TaskHandle_t 当前持有信号量的任务句柄，如果没有任务持有则返回NULL
 * @note 此函数不会阻塞，立即返回当前状态
 */
TaskHandle_t MutexSemaphore_getMutexHolder(void)
{
    if (MutexSemaphore_handle == NULL)
    {
        return NULL;
    }
    return xSemaphoreGetMutexHolder(MutexSemaphore_handle);
}
