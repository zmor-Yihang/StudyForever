#include "test_ecap.h"
#include "bsp/epwm.h"
#include "bsp/ecap.h"
#include "bsp/sci.h"
#include "bsp/key.h"

// 简单的数字转字符串函数
static void itoa_simple(Uint32 num, char *buf)
{
    char temp[12];
    int i = 0, j = 0;
    
    if(num == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    
    while(num > 0)
    {
        temp[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while(i > 0)
    {
        buf[j++] = temp[--i];
    }
    buf[j] = '\0';
}

// 浮点数转字符串函数（保留2位小数）
static void ftoa_simple(float num, char *buf, Uint16 decimals)
{
    int integer_part;
    float decimal_part;
    Uint32 decimal_value;
    Uint16 i;
    
    if(num < 0)
    {
        buf[0] = '-';
        buf++;
        num = -num;
    }
    
    integer_part = (int)num;
    decimal_part = num - (float)integer_part;
    
    itoa_simple((Uint32)integer_part, buf);
    while(*buf) buf++;
    
    *buf++ = '.';
    
    for(i = 0; i < decimals; i++)
    {
        decimal_part *= 10.0f;
    }
    decimal_value = (Uint32)(decimal_part + 0.5f);
    
    if(decimals == 2)
    {
        *buf++ = '0' + (decimal_value / 10);
        *buf++ = '0' + (decimal_value % 10);
    }
    else if(decimals == 1)
    {
        *buf++ = '0' + decimal_value;
    }
    
    *buf = '\0';
}

//
// 测试配置
// 硬件连接：用杜邦线将 GPIO0(ePWM1A) 连接到 GPIO24(eCAP1)
//
#define TEST_PWM_FREQ_HZ    5000UL      // 测试PWM频率 5KHz
#define TEST_PWM_DUTY       0.3f        // 测试PWM占空比 30%

// 占空比调整步进和范围
#define DUTY_STEP           0.05f       // 每次调整5%
#define DUTY_MIN            0.05f       // 最小5%
#define DUTY_MAX            0.95f       // 最大95%

//
// 测试流程：
// 1. ePWM1 生成指定频率和占空比的PWM信号
// 2. eCAP1 捕获PWM信号，测量周期和占空比（中断方式）
// 3. 通过串口输出测量结果
//

// 中断捕获标志和数据缓存
static volatile Uint16 g_capture_ready = 0;
static volatile Uint32 g_period_count = 0;
static volatile Uint32 g_high_time_count = 0;

// 当前占空比设定值
static float g_current_duty = TEST_PWM_DUTY;

//
// eCAP1 捕获完成回调函数
// 在中断中被调用，保存捕获数据并设置标志
//
static void ecap_capture_callback(void)
{
    // 读取捕获值
    g_period_count = ecap_get_period();
    g_high_time_count = ecap_get_high_time();
    
    // 设置数据就绪标志
    g_capture_ready = 1;
}

void test_ecap_init(void)
{
    char buf[64];
    
    // 初始化串口用于输出结果
    scia_init();
    
    // 初始化按键
    key_init();
    
    // 停止所有ePWM时基时钟
    epwm_tbclk_stop();
    
    // 初始化ePWM1生成测试信号
    epwm1_init();
    epwm1_set_freq(TEST_PWM_FREQ_HZ);
    epwm1_set_duty(g_current_duty);
    
    // 初始化eCAP1用于捕获
    ecap_init();
    
    // 使能eCAP1中断
    ecap_irq_enable(ecap_capture_callback);
    
    // 启动所有ePWM时基时钟
    epwm_tbclk_start();
    
    // 启动ePWM1输出
    epwm1_start();
    
    scia_send_str("\r\n====== eCAP PWM测量测试(中断模式) ======\r\n");
    scia_send_str("硬件连接：GPIO0(ePWM1A) --> GPIO24(eCAP1)\r\n");
    
    scia_send_str("设定参数：频率=");
    itoa_simple(TEST_PWM_FREQ_HZ, buf);
    scia_send_str(buf);
    scia_send_str(" Hz, 占空比=");
    ftoa_simple(g_current_duty * 100.0f, buf, 1);
    scia_send_str(buf);
    scia_send_str("%\r\n");
    
    scia_send_str("SYSCLK=150000000 Hz\r\n");
    scia_send_str("按键控制：SW1=增加占空比  SW2=减小占空比\r\n");
    scia_send_str("========================================\r\n\r\n");
}

void test_ecap_loop(void)
{
    static Uint32 loop_cnt = 0;
    static Uint32 last_capture_cnt = 0;
    static key_id_t last_key = KEY_NONE;
    static Uint32 delay_cnt = 0;
    Uint32 period_count;
    Uint32 high_time_count;
    float duty_cycle;
    float frequency;
    float period_us;
    float high_time_us;
    char buf[64];
    key_id_t key;
    
    // 按键扫描（每次循环都扫描，不要等待）
    key = key_scan();
    
    // 按键处理（按下时触发，释放后才能再次触发）
    if(key != KEY_NONE && last_key == KEY_NONE)
    {
        if(key == KEY_SW1)  // 增加占空比
        {
            g_current_duty += DUTY_STEP;
            if(g_current_duty > DUTY_MAX)
                g_current_duty = DUTY_MAX;
            
            epwm1_set_duty(g_current_duty);
            
            scia_send_str(">>> 占空比增加到 ");
            ftoa_simple(g_current_duty * 100.0f, buf, 1);
            scia_send_str(buf);
            scia_send_str("%\r\n\r\n");
        }
        else if(key == KEY_SW2)  // 减小占空比
        {
            g_current_duty -= DUTY_STEP;
            if(g_current_duty < DUTY_MIN)
                g_current_duty = DUTY_MIN;
            
            epwm1_set_duty(g_current_duty);
            
            scia_send_str(">>> 占空比减小到 ");
            ftoa_simple(g_current_duty * 100.0f, buf, 1);
            scia_send_str(buf);
            scia_send_str("%\r\n\r\n");
        }
    }
    last_key = key;
    
    // 短延迟后计数（用于控制显示频率）
    DELAY_MS(10);
    delay_cnt++;
    
    // 每500ms输出一次测量结果
    if(delay_cnt >= 50)  // 50 * 10ms = 500ms
    {
        delay_cnt = 0;
        loop_cnt++;
        
        // 检查是否有新的捕获数据
        if(g_capture_ready)
        {
            // 读取中断中保存的数据
            period_count = g_period_count;
            high_time_count = g_high_time_count;
            
            // 计算占空比
            if(period_count > 0)
            {
                duty_cycle = ((float)high_time_count / (float)period_count) * 100.0f;
            }
            else
            {
                duty_cycle = 0.0f;
            }
            
            // 计算实际频率和时间（SYSCLK = 150MHz）
            frequency = 150000000.0f / (float)period_count;
            period_us = (float)period_count / 150.0f;       // 转换为微秒
            high_time_us = (float)high_time_count / 150.0f; // 转换为微秒
            
            // 清除标志
            g_capture_ready = 0;
            last_capture_cnt = loop_cnt;
            
            // 输出测量结果
            scia_send_str("[");
            itoa_simple(loop_cnt, buf);
            scia_send_str(buf);
            scia_send_str("] 测量结果(中断捕获)：\r\n");
            
            scia_send_str("  周期计数    = ");
            itoa_simple(period_count, buf);
            scia_send_str(buf);
            scia_send_str("\r\n");
            
            scia_send_str("  高电平计数  = ");
            itoa_simple(high_time_count, buf);
            scia_send_str(buf);
            scia_send_str("\r\n");
            
            scia_send_str("  周期        = ");
            ftoa_simple(period_us, buf, 2);
            scia_send_str(buf);
            scia_send_str(" us\r\n");
            
            scia_send_str("  高电平时间  = ");
            ftoa_simple(high_time_us, buf, 2);
            scia_send_str(buf);
            scia_send_str(" us\r\n");
            
            scia_send_str("  频率        = ");
            ftoa_simple(frequency, buf, 2);
            scia_send_str(buf);
            scia_send_str(" Hz\r\n");
            
            scia_send_str("  占空比      = ");
            ftoa_simple(duty_cycle, buf, 2);
            scia_send_str(buf);
            scia_send_str("%\r\n");
            
            // 计算误差
            float freq_error = ((frequency - TEST_PWM_FREQ_HZ) / TEST_PWM_FREQ_HZ) * 100.0f;
            float duty_error = duty_cycle - (g_current_duty * 100.0f);
            
            scia_send_str("  频率误差    = ");
            ftoa_simple(freq_error, buf, 2);
            scia_send_str(buf);
            scia_send_str("%\r\n");
            
            scia_send_str("  占空比误差  = ");
            ftoa_simple(duty_error, buf, 2);
            scia_send_str(buf);
            scia_send_str("%\r\n\r\n");
        }
        else
        {
            // 超过2个周期未收到新数据，可能有问题
            if(loop_cnt - last_capture_cnt > 2)
            {
                scia_send_str("[");
                itoa_simple(loop_cnt, buf);
                scia_send_str(buf);
                scia_send_str("] 警告：未捕获到新的PWM信号，请检查硬件连接！\r\n\r\n");
            }
        }
    }
}