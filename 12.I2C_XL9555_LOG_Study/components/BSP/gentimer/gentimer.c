#include "gentimer.h"

/* 通用定时器全局句柄 */
gptimer_handle_t gentimer_handle = NULL;

/**
 * @brief 通用定时器中断回调函数 
 * @param timer 触发中断的定时器句柄
 * @param edata 报警事件数据指针
 * @param user_ctx 用户传递的上下文数据指针
 * @return true 如果唤醒了高优先级任务
 * @return false 如果没有唤醒高优先级任务
 */
static bool IRAM_ATTR gentimer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    led_toggle(); /* 切换LED状态 */

    return false; /* 如果唤醒了高优先级任务, 返回true */
}

/**
 * @brief 初始化通用定时器
 */
void gentimer_init(void)
{
    gptimer_config_t gentimer_config = {0};

    gentimer_config.clk_src = GPTIMER_CLK_SRC_DEFAULT; /* 时钟源, 默认 */
    gentimer_config.direction = GPTIMER_COUNT_UP;      /* 计数方向, 向上计数 */
    gentimer_config.resolution_hz = 1000000;           /* 计数器分辨率, 1MHz, 每个计数周期1us */
    gentimer_config.intr_priority = 1;                 /* 中断优先级, 1级 */
    gentimer_config.flags.intr_shared = 0;             /* 中断共享, 0: 不共享 */
    gentimer_config.flags.allow_pd = 0;                /* 允许休眠, 1: 允许 */

    gptimer_new_timer(&gentimer_config, &gentimer_handle);
}

/**
 * @brief 设置定时器报警配置
 */
void gentimer_set_alarm(void)
{
    gptimer_alarm_config_t alarm_config = {0};
    alarm_config.alarm_count = 1000000;             /* 报警值, 1秒 */
    alarm_config.flags.auto_reload_on_alarm = true; /* 报警后自动重载 */
    alarm_config.reload_count = 0;                  /* 重载值, 0表示从0开始计数 */

    gptimer_set_alarm_action(gentimer_handle, &alarm_config);
}

/**
 * @brief 注册定时器回调函数
 */
void gentimer_callback_register(void *user_data)
{
    gptimer_event_callbacks_t event_cbs = {0};
    event_cbs.on_alarm = gentimer_callback;         /* 报警回调函数 */

    gptimer_register_event_callbacks(gentimer_handle, &event_cbs, user_data);
}

/**
 * @brief 使能并启动定时器
 */
void gentimer_enable_and_start(void)
{
    gptimer_enable(gentimer_handle); /* 使能定时器 */
    gptimer_start(gentimer_handle);  /* 启动定时器 */
}