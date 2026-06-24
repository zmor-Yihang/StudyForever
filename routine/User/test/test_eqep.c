#include "test_eqep.h"
#include "../bsp/eqep.h"
#include "../bsp/sci.h"
#include <stdio.h>

//
// ePWM1 模拟编码器配置（5kHz = 300rpm）
//
#if (CPU_FRQ_150MHZ)
    #define EPWM1_SIM_CLK   150000000UL
#endif
#if (CPU_FRQ_100MHZ)
    #define EPWM1_SIM_CLK   100000000UL
#endif

#define EPWM1_SIM_FREQ  5000UL      // 5kHz 模拟编码器频率 = 300rpm
#define EPWM1_SIM_PERIOD ((Uint16)(EPWM1_SIM_CLK / (2UL * EPWM1_SIM_FREQ)))

//
// 全局变量：位置速度结构体
//
static EQEP_PosSpeed qep_data = EQEP_POSSPEED_DEFAULTS;
static Uint16 sim_qep_counter = 0;  // 用于生成 Index 信号

//
// epwm1_sim_encoder_init - 配置 ePWM1 模拟正交编码器信号
// 功能：ePWM1A 和 ePWM1B 产生相位相差 90° 的方波，模拟 A/B 相编码器
//
static void epwm1_sim_encoder_init(void)
{
    // 初始化 GPIO
    InitEPwm1Gpio();
    
    //
    // 时基配置：向上计数模式，无分频
    // 使用上计数模式更容易精确控制 A/B 相位差
    //
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;      // 向上计数
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;
    EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;
    EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm1Regs.TBCTR = 0;
    
    // 周期值翻倍（因为只向上计数，不像中心对齐会往返）
    EPwm1Regs.TBPRD = EPWM1_SIM_PERIOD * 2;
    
    // 比较值配置：影子模式
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    
    //
    // A 相：50% 占空比
    // 计数到 CMPA 时翻转，产生方波
    //
    EPwm1Regs.CMPA.half.CMPA = EPWM1_SIM_PERIOD;    // 50% 位置
    
    //
    // B 相：相位滞后 90°（CMPB = CMPA + 周期/4）
    // 90° 相位差 = 周期的 1/4
    //
    EPwm1Regs.CMPB = EPWM1_SIM_PERIOD + (EPWM1_SIM_PERIOD / 2);  // 75% 位置
    
    //
    // 动作限定：生成正交信号
    //
    // ePWM1A（eQEP A 相）：
    //   - 计数到 0 时置高
    //   - 计数到 CMPA 时置低
    //   产生 50% 占空比方波
    EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;          // 计数 = 0 → 置高
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;        // 计数 = CMPA → 置低
    
    // ePWM1B（eQEP B 相）：
    //   - 计数到 0 时置高
    //   - 计数到 CMPB 时置低
    //   相对 A 相滞后 90°（正转方向）
    EPwm1Regs.AQCTLB.bit.ZRO = AQ_SET;          // 计数 = 0 → 置高
    EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;        // 计数 = CMPB → 置低
    
    // 禁用死区和斩波
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;
    EPwm1Regs.PCCTL.bit.CHPEN = CHP_DISABLE;
    
    // 禁用中断（不需要中断方式）
    EPwm1Regs.ETSEL.bit.INTEN = 0;
}

//
// test_eqep_sim_init - 初始化 eQEP 模拟编码器测试
// 说明：配置 ePWM1 生成正交信号，配置 GPIO4 输出 Index 信号
//
void test_eqep_sim_init(void)
{
    char msg[256];
    
    // 停止所有 ePWM 时基
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;
    
    // 初始化 eQEP1
    eqep1_init();
    
    // 初始化 ePWM1（模拟编码器信号）
    epwm1_sim_encoder_init();
    
    // 配置 GPIO4 作为 Index 信号输出
    EALLOW;
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;      // GPIO4 设为输出
    GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;    // 初始为低电平
    EDIS;
    
    // 启动所有 ePWM 时基
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
    
    // 初始化串口
    scia_init();
    scia_send_str("\r\n=== eQEP Simulated Encoder Test (Debug Mode) ===\r\n");
    scia_send_str("ePWM1 generates 5kHz quadrature signals (300 rpm)\r\n");
    scia_send_str("Hardware Connection Required:\r\n");
    scia_send_str("  GPIO0 (ePWM1A) -> GPIO20 (eQEP1A)\r\n");
    scia_send_str("  GPIO1 (ePWM1B) -> GPIO21 (eQEP1B)\r\n");
    scia_send_str("  GPIO4 (Index)  -> GPIO23 (eQEP1I) [Optional]\r\n\r\n");
    
    // 输出 ePWM1 配置信息
    sprintf(msg, "ePWM1 Config:\r\n");
    scia_send_str(msg);
    sprintf(msg, "  TBPRD = %u (Period)\r\n", EPwm1Regs.TBPRD);
    scia_send_str(msg);
    sprintf(msg, "  CMPA  = %u (50%% duty)\r\n", EPwm1Regs.CMPA.half.CMPA);
    scia_send_str(msg);
    sprintf(msg, "  CMPB  = %u (50%% duty)\r\n", EPwm1Regs.CMPB);
    scia_send_str(msg);
    sprintf(msg, "  TBCTL = 0x%04X\r\n", EPwm1Regs.TBCTL.all);
    scia_send_str(msg);
    sprintf(msg, "  AQCTLA = 0x%04X (A phase)\r\n", EPwm1Regs.AQCTLA.all);
    scia_send_str(msg);
    sprintf(msg, "  AQCTLB = 0x%04X (B phase)\r\n\r\n", EPwm1Regs.AQCTLB.all);
    scia_send_str(msg);
    
    // 输出 eQEP1 配置信息
    sprintf(msg, "eQEP1 Config:\r\n");
    scia_send_str(msg);
    sprintf(msg, "  QUPRD  = %lu (Unit timer)\r\n", EQep1Regs.QUPRD);
    scia_send_str(msg);
    sprintf(msg, "  QDECCTL = 0x%04X\r\n", EQep1Regs.QDECCTL.all);
    scia_send_str(msg);
    sprintf(msg, "  QEPCTL  = 0x%04X\r\n", EQep1Regs.QEPCTL.all);
    scia_send_str(msg);
    sprintf(msg, "  QCAPCTL = 0x%04X\r\n\r\n", EQep1Regs.QCAPCTL.all);
    scia_send_str(msg);
    
    scia_send_str("Starting test...\r\n\r\n");
}

//
// test_eqep_sim_loop - eQEP 测试循环（调试模式）
// 说明：周期性计算位置和速度，输出详细调试信息
//
void test_eqep_sim_loop(void)
{
    char msg[256];
    Uint16 i;
    static Uint16 loop_count = 0;
    
    // 计算位置和速度
    eqep1_calc(&qep_data);
    
    // 生成 Index 信号（每 4000 个计数 = 1 转）
    sim_qep_counter++;
    if (sim_qep_counter >= 20) {    // 每 20 次循环 = 4 秒生成一次（模拟 1 转）
        EALLOW;
        GpioDataRegs.GPASET.bit.GPIO4 = 1;      // Index 脉冲高电平
        for (i = 0; i < 1000; i++) { }          // 短暂延时
        GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;    // Index 恢复低电平
        EDIS;
        sim_qep_counter = 0;
    }
    
    loop_count++;
    
    // 每 5 次循环输出详细信息
    if (loop_count % 5 == 0) {
        scia_send_str("\r\n--- Detailed Debug Info ---\r\n");
        
        // eQEP 寄存器状态
        sprintf(msg, "QPOSCNT  = %lu (Position)\r\n", EQep1Regs.QPOSCNT);
        scia_send_str(msg);
        sprintf(msg, "QPOSLAT  = %lu (Latched Pos)\r\n", EQep1Regs.QPOSLAT);
        scia_send_str(msg);
        sprintf(msg, "QCPRDLAT = %u (Capture Period)\r\n", EQep1Regs.QCPRDLAT);
        scia_send_str(msg);
        sprintf(msg, "QEPSTS   = 0x%04X (Status)\r\n", EQep1Regs.QEPSTS.all);
        scia_send_str(msg);
        sprintf(msg, "  QDF=%u COEF=%u UPEVNT=%u\r\n", 
                EQep1Regs.QEPSTS.bit.QDF,
                EQep1Regs.QEPSTS.bit.COEF,
                EQep1Regs.QEPSTS.bit.UPEVNT);
        scia_send_str(msg);
        sprintf(msg, "QFLG     = 0x%04X (Flags: UTO=%u IEL=%u)\r\n\r\n",
                EQep1Regs.QFLG.all,
                EQep1Regs.QFLG.bit.UTO,
                EQep1Regs.QFLG.bit.IEL);
        scia_send_str(msg);
        
        // 计算结果
        sprintf(msg, "Speed_FR = %ld rpm (oldpos=%.4f)\r\n", 
                qep_data.speed_rpm_fr,
                qep_data.oldpos);
        scia_send_str(msg);
        sprintf(msg, "Speed_PR = %ld rpm (pu=%.4f)\r\n", 
                qep_data.speed_rpm_pr,
                qep_data.speed_pu_pr);
        scia_send_str(msg);
        sprintf(msg, "Theta    = 0x%04X (%.1f deg)\r\n",
                qep_data.theta_mech,
                (float32)qep_data.theta_mech * 360.0F / 32768.0F);
        scia_send_str(msg);
        sprintf(msg, "Dir      = %s (%u)\r\n",
                (qep_data.direction == 1) ? "CW" : "CCW",
                qep_data.direction);
        scia_send_str(msg);
        sprintf(msg, "Index    = 0x%02X\r\n", qep_data.index_sync_flag);
        scia_send_str(msg);
        
        // ePWM 状态
        sprintf(msg, "\r\nePWM1 TBCTR = %u (Current Counter)\r\n", EPwm1Regs.TBCTR);
        scia_send_str(msg);
        
        scia_send_str("---------------------------\r\n\r\n");
    } else {
        // 简洁输出
        sprintf(msg, 
                "Pos: %lu | Speed_FR: %ld rpm | Speed_PR: %ld rpm | "
                "Theta: 0x%04X | Dir: %s | Idx: 0x%02X\r\n",
                eqep1_get_position(),
                qep_data.speed_rpm_fr,
                qep_data.speed_rpm_pr,
                qep_data.theta_mech,
                (qep_data.direction == 1) ? "CW" : "CCW",
                qep_data.index_sync_flag);
        scia_send_str(msg);
    }
}

//
// test_eqep_basic_init - 基础测试初始化（无模拟信号）
//
void test_eqep_basic_init(void)
{
    eqep1_init();
    scia_init();
    scia_send_str("\r\n=== eQEP Basic Test ===\r\n");
    scia_send_str("Connect real encoder to eQEP1 pins...\r\n\r\n");
}

//
// test_eqep_basic_loop - 基础测试循环
//
void test_eqep_basic_loop(void)
{
    char msg[256];
    
    eqep1_calc(&qep_data);
    
    sprintf(msg, 
            "Pos: %lu | Speed_FR: %ld rpm | Speed_PR: %ld rpm | "
            "Dir: %s | Theta: 0x%04X\r\n",
            eqep1_get_position(),
            qep_data.speed_rpm_fr,
            qep_data.speed_rpm_pr,
            (qep_data.direction == 1) ? "CW" : "CCW",
            qep_data.theta_mech);
    scia_send_str(msg);
}