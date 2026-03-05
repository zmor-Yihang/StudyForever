#include "utils_delay.h"

/**
 * @brief 忙等待延时函数(不触发任务切换)
 * @param xDelayTicks 延时的滴答数
 * @note 此函数会持续占用CPU,不会让出CPU给其他任务
 *       仅在特殊场景下使用,一般应该使用 vTaskDelay()
 */
void utils_delay(TickType_t xDelayTicks)
{
    TickType_t xStart = xTaskGetTickCount();
    while ((xTaskGetTickCount() - xStart) < xDelayTicks)
    {
        // ESP32 使用 asm volatile("nop") 防止编译器优化
        asm volatile("nop"); // Xtensa 架构的空操作指令 
    }
}
