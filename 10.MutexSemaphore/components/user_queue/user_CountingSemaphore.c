#include "user_CountingSemaphore.h"

SemaphoreHandle_t CountingSemaphore_handle = NULL;

/**
 * @brief 初始化计数信号量
 *
 * 创建一个计数信号量，最大计数为2，初始计数为0，
 * 并将其句柄存储在全局变量中。
 *
 * @note 此函数应该是第一个被调用的计数信号量相关函数
 * @warning 重复调用此函数会导致内存泄漏
 */
void CountingSemaphore_init(void)
{
    /* 创建计数信号量，最大计数为2，初始计数为0 */
    CountingSemaphore_handle = xSemaphoreCreateCounting(2, 0);
    if (CountingSemaphore_handle == NULL)
    {
        printf("Counting Semaphore Create Failed!\n");
        while (1);
    }
}

/**
 * @brief 获取计数信号量
 *
 * 尝试获取计数信号量的计数。如果当前计数为0，
 * 当前任务将等待1s直到获得信号量。
 *
 * @note 此函数会阻塞当前任务，最多等待1秒
 * @warning 确保在获取信号量后及时释放，避免死锁
 */
void CountingSemaphore_take(void)
{
    if (CountingSemaphore_handle != NULL)
    {
        if (xSemaphoreTake(CountingSemaphore_handle, 1000) != pdTRUE)
        {
            printf("Counting Semaphore Take Failed!\n");
            while (1);
        }
    }
    else
    {
        printf("Counting Semaphore Handle is NULL!\n");
        while (1);
    }
}

/**
 * @brief 释放计数信号量
 *
 * 释放计数信号量，增加计数器的值。
 *
 * @note 释放操作会增加信号量的计数
 * @warning 计数不能超过最大值
 */
void CountingSemaphore_give(void)
{
    if (CountingSemaphore_handle != NULL)
    {
        if (xSemaphoreGive(CountingSemaphore_handle) != pdTRUE)
        {
            printf("Counting Semaphore Give Failed!\n");
            while (1);
        }
    }
    else
    {
        printf("Counting Semaphore Handle is NULL!\n");
        while (1);
    }
}

/**
 * @brief 删除计数信号量
 *
 * 删除计数信号量并释放相关资源。将全局句柄设置为NULL。
 *
 * @note 删除后，信号量将不再可用
 * @warning 确保没有任务仍在使用此信号量时才删除
 */
void CountingSemaphore_delete(void)
{
    if (CountingSemaphore_handle != NULL)
    {
        vSemaphoreDelete(CountingSemaphore_handle);
        CountingSemaphore_handle = NULL;
    }
    else
    {
        printf("Counting Semaphore Handle is NULL!\n");
        while (1);
    }
}

/**
 * @brief 获取计数信号量的当前计数
 *
 * 返回计数信号量的当前可用计数。
 *
 * @return UBaseType_t 当前计数，如果句柄无效则返回0
 * @note 此函数不会阻塞，立即返回当前状态
 */
UBaseType_t CountingSemaphore_getCount(void)
{
    if (CountingSemaphore_handle == NULL) {
        return 0;
    }
    return uxSemaphoreGetCount(CountingSemaphore_handle);
}