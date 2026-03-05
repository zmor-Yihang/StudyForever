#include "pwm.h"

/**
 * @brief 初始化 LEDC 定时器与通道以实现 PWM 输出。
 */
void pwm_init(void)
{
    ledc_timer_config_t pwm_timer_config = {0};           /* 定义PWM定时器配置结构体*/
    pwm_timer_config.timer_num = LEDC_TIMER_0;            /* 使用定时器0 */
    pwm_timer_config.clk_cfg = LEDC_AUTO_CLK;             /* 自动选择时钟 */
    pwm_timer_config.duty_resolution = LEDC_TIMER_13_BIT; /* 设置PWM分辨率13位 */
    pwm_timer_config.freq_hz = 5000;                      /* 设置PWM频率5k */
    pwm_timer_config.speed_mode = LEDC_LOW_SPEED_MODE;    /* 低速模式 */
    pwm_timer_config.deconfigure = false;                 /* 取消定时器配置 */

    ledc_timer_config(&pwm_timer_config); /* 应用定时器配置 */

    ledc_channel_config_t pwm_channel_config = {0};      /* 定义PWM通道配置结构体并初始化为0 */
    pwm_channel_config.timer_sel = LEDC_TIMER_0;         /* 选择定时器0 */
    pwm_channel_config.channel = LEDC_CHANNEL_0;         /* 选择通道0 */
    pwm_channel_config.gpio_num = LED_PIN;               /* 设置GPIO引脚 */
    pwm_channel_config.speed_mode = LEDC_LOW_SPEED_MODE; /* 低速模式 */
    pwm_channel_config.duty = 0;                         /* 设置初始占空比为0 */
    pwm_channel_config.flags.output_invert = 1;          /* 输出反相 */
    pwm_channel_config.hpoint = 0;                       /* 设置高电平起始点, 设置多个PWM的相位偏移 */
    pwm_channel_config.intr_type = LEDC_INTR_DISABLE;    /* 禁用中断 */

    ledc_channel_config(&pwm_channel_config); /* 应用通道配置 */

    ledc_fade_func_install(0); /* 安装渐变功能，参数0表示不使用中断 */
}

/**
 * @brief 以阻塞方式执行 LED 呼吸灯渐变效果。
 */
void pwm_fade(void)
{
    while (1)
    {
        /* 从暗到亮渐变 */
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 8000, 2000, LEDC_FADE_WAIT_DONE);

        /* 从亮到暗渐变 */
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 2000, LEDC_FADE_WAIT_DONE);
    }
}