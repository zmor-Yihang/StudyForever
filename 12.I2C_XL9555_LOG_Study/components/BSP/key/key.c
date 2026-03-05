#include "key.h"

/* 按键中断标志 */
uint8_t FlagOfButtonPressed = 0;

/**
 * @brief GPIO中断服务程序, 仅设置中断标志位
 * 
 * @param arg 
 */
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    xQueueSend(queueOfInfo_handle, &FlagOfButtonPressed, 1000);      /* 发送按键事件到消息队列 */
}

/**
 * @brief 按键初始化
 * @note  配置KEY0引脚为输入模式, 上拉, 下降沿中断，
 * 
 */
void key_init(void)
{
    gpio_config_t io_conf = {0};

    io_conf.pin_bit_mask = (1ULL << KEY_BOOT);    // 配置KEY0引脚
    io_conf.mode = GPIO_MODE_INPUT;               // 设置为输入模式
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;      // 启用上拉
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // 禁用下拉
    io_conf.intr_type = GPIO_INTR_NEGEDGE;        // 启用中断
    gpio_config(&io_conf);                        // 初始化GPIO

    gpio_install_isr_service(0);                  // 安装GPIO中断服务, 相当于使能NVIC [HAL_NVIC_EnableIRQ(EXTI0_IRQn)]

    gpio_isr_handler_add(KEY_BOOT, gpio_isr_handler, (void *)KEY_BOOT); // 注册中断处理函数, 触发中断时跳转到gpio_isr_handler
}

/**
 * @brief 获取按键状态（带防抖）
 * @note  软件防抖，避免按键抖动造成的误触发
 * @return uint8_t 按键状态, 0表示按下, 1表示未按下
 */
uint8_t key_get_state(void)
{
    static uint8_t last_state = 1;           // 上次按键状态，初始为高电平（未按下）
    static uint32_t last_change_time = 0;    // 上次状态改变的时间
    const uint32_t DEBOUNCE_DELAY = 50;      // 防抖延时 50ms
    
    uint8_t current_state = gpio_get_level(KEY_BOOT);  // 读取当前按键状态
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;  // 获取当前时间（毫秒）
    
    // 如果状态发生变化
    if (current_state != last_state) 
    {
        last_change_time = current_time;     // 记录状态改变时间
        last_state = current_state;          // 更新状态
    }
    
    // 如果状态稳定超过防抖时间，且为按下状态
    if ((current_time - last_change_time) > DEBOUNCE_DELAY && current_state == 0) 
    {
        return 0;  // 按键按下
    }
    
    return 1;  // 按键未按下或在防抖期间
}
