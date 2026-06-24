#include "eqep.h"

//
// eqep1_init - 初始化 eQEP1 模块
//
void eqep1_init(void)
{
    // 初始化 GPIO（eQEP1 引脚映射）
    InitEQep1Gpio();
    
    // 配置解码器控制
    EQep1Regs.QDECCTL.bit.QSRC = 0;         // 正交编码模式
    EQep1Regs.QDECCTL.bit.XCR = 0;          // 2x 分辨率
    EQep1Regs.QDECCTL.bit.SWAP = 0;         // 不交换 A/B 信号
    EQep1Regs.QDECCTL.bit.IGATE = 0;        // 禁用 index 门控
    EQep1Regs.QDECCTL.bit.QAP = 0;          // QEPA 不反相
    EQep1Regs.QDECCTL.bit.QBP = 0;          // QEPB 不反相
    EQep1Regs.QDECCTL.bit.QIP = 0;          // QEPI 不反相
    EQep1Regs.QDECCTL.bit.QSP = 0;          // QEPS 不反相
    
    // 配置位置计数器控制
    EQep1Regs.QEPCTL.bit.FREE_SOFT = 2;     // 仿真模式：不受断点影响
    EQep1Regs.QEPCTL.bit.PCRM = 0;          // index 事件时复位位置计数器
    EQep1Regs.QEPCTL.bit.SEI = 0;           // 禁用软件 index 初始化
    EQep1Regs.QEPCTL.bit.IEI = 0;           // index 不初始化位置计数器
    EQep1Regs.QEPCTL.bit.SWI = 0;           // 清除软件初始化
    EQep1Regs.QEPCTL.bit.SEL = 0;           // index 发生在上升沿
    EQep1Regs.QEPCTL.bit.IEL = 1;           // 锁存 index 位置
    EQep1Regs.QEPCTL.bit.QPEN = 1;          // 使能位置计数器
    EQep1Regs.QEPCTL.bit.QCLM = 1;          // 单位时间超时时锁存
    EQep1Regs.QEPCTL.bit.UTE = 1;           // 使能单位定时器
    EQep1Regs.QEPCTL.bit.WDE = 0;           // 禁用看门狗定时器
    
    // 配置位置计数器最大值（32位无符号最大值）
    EQep1Regs.QPOSMAX = 0xFFFFFFFF;
    
    // 配置单位定时器周期（用于高速测量，100Hz 采样）
    EQep1Regs.QUPRD = EQEP1_UNIT_PERIOD;
    
    // 配置捕获控制（用于低速测量）
    EQep1Regs.QCAPCTL.bit.UPPS = 5;         // 单位位置事件预分频：1/32
    EQep1Regs.QCAPCTL.bit.CCPS = 7;         // eQEP 捕获时钟预分频：1/128
    EQep1Regs.QCAPCTL.bit.CEN = 1;          // 使能 eQEP 捕获
    
    // 清除所有中断标志
    EQep1Regs.QCLR.all = 0xFFFF;
    
    // 可选：使能中断（如需在中断中处理）
    // EQep1Regs.QEINT.bit.UTO = 1;         // 使能单位超时中断
    // EQep1Regs.QEINT.bit.IEL = 1;         // 使能 index 事件锁存中断
}

//
// eqep1_calc - 计算位置和速度
// 参数：p - 指向 EQEP_PosSpeed 结构体的指针
// 说明：此函数应周期性调用，建议在单位定时器中断或主循环中调用
//
void eqep1_calc(EQEP_PosSpeed *p)
{
    Uint32 pos32val;
    Uint16 pos16val, temp1;
    float32 newp, oldp, delta;
    
    //---------------------------------------------------------------
    // 1. 读取方向和位置
    //---------------------------------------------------------------
    p->direction = EQep1Regs.QEPSTS.bit.QDF;    // 0=CCW反转, 1=CW正转
    pos16val = (Uint16)EQep1Regs.QPOSCNT;       // 读取当前位置计数
    
    //---------------------------------------------------------------
    // 2. 计算机械角 theta_mech
    // theta_mech = (QPOSCNT / COUNTS_PER_REV) * 32767
    // 范围 0~32767 对应 0~360°
    //---------------------------------------------------------------
    p->theta_raw = pos16val + p->cal_angle;     // 加上校准偏移
    p->theta_mech = (int16)((((Uint32)p->theta_raw % EQEP_COUNTS_PER_REV) * 32767UL) / EQEP_COUNTS_PER_REV);
    
    //---------------------------------------------------------------
    // 3. 计算电气角 theta_elec
    // theta_elec = (theta_mech * pole_pairs) % 32767
    //---------------------------------------------------------------
    p->theta_elec = (p->pole_pairs * p->theta_mech) & 0x7FFF;
    
    //---------------------------------------------------------------
    // 4. 检测 Index 事件
    //---------------------------------------------------------------
    if (EQep1Regs.QFLG.bit.IEL == 1) {
        p->index_sync_flag = 0x00F0;            // 设置同步标志
        EQep1Regs.QCLR.bit.IEL = 1;             // 清除中断标志
    }
    
    //---------------------------------------------------------------
    // 5. 高速测量：使用单位定时器（100Hz 周期）
    // 通过位置差分计算速度
    //---------------------------------------------------------------
    if (EQep1Regs.QFLG.bit.UTO == 1) {          // 单位超时事件
        // 读取锁存的位置值
        pos32val = (Uint32)EQep1Regs.QPOSLAT;
        
        // 转换为标幺值 (0~1.0 代表 0~1 转)
        newp = (float32)(pos32val % EQEP_COUNTS_PER_REV) / (float32)EQEP_COUNTS_PER_REV;
        oldp = p->oldpos;
        
        // 计算位置差（考虑旋转方向和溢出）
        if (p->direction == 0) {                // 反转 CCW
            if (newp > oldp) {
                delta = -(1.0F - newp + oldp);
            } else {
                delta = newp - oldp;
            }
        } else {                                // 正转 CW
            if (newp < oldp) {
                delta = 1.0F + newp - oldp;
            } else {
                delta = newp - oldp;
            }
        }
        
        // 饱和限制
        if (delta > 1.0F) {
            p->speed_pu_fr = 1.0F;
        } else if (delta < -1.0F) {
            p->speed_pu_fr = -1.0F;
        } else {
            p->speed_pu_fr = delta;
        }
        
        // 更新旧位置
        p->oldpos = newp;
        
        // 转换为 RPM
        p->speed_rpm_fr = (int32)(p->base_rpm * p->speed_pu_fr);
        
        // 清除单位超时标志
        EQep1Regs.QCLR.bit.UTO = 1;
    }
    
    //---------------------------------------------------------------
    // 6. 低速测量：使用捕获单元
    // 通过测量 QEP 边沿时间间隔计算速度
    //---------------------------------------------------------------
    if (EQep1Regs.QEPSTS.bit.UPEVNT == 1) {     // 单位位置事件
        if (EQep1Regs.QEPSTS.bit.COEF == 0) {   // 无捕获溢出
            temp1 = (Uint16)EQep1Regs.QCPRDLAT; // 读取捕获周期
        } else {                                // 捕获溢出（速度过慢）
            temp1 = 0xFFFF;                     // 饱和
        }
        
        // 计算速度（标幺值）
        // speed_pu = (32/4000) * (SYSCLK/128) * 60 / (BaseRpm * temp1)
        // = EQEP_SPEED_SCALER / (temp1 * BaseRpm / 6000)
        if (temp1 > 0) {
            p->speed_pu_pr = (float32)EQEP_SPEED_SCALER / (float32)temp1;
        } else {
            p->speed_pu_pr = 1.0F;
        }
        
        // 饱和限制
        if (p->speed_pu_pr > 1.0F) {
            p->speed_pu_pr = 1.0F;
        }
        
        // 转换为 RPM（考虑方向）
        if (p->direction == 0) {                // 反转为负
            p->speed_rpm_pr = -(int32)(p->base_rpm * p->speed_pu_pr);
        } else {                                // 正转为正
            p->speed_rpm_pr = (int32)(p->base_rpm * p->speed_pu_pr);
        }
        
        // 清除单位位置事件和溢出标志
        EQep1Regs.QEPSTS.all = 0x88;
    }
}

//
// eqep1_reset_counter - 复位位置计数器
//
void eqep1_reset_counter(void)
{
    EQep1Regs.QEPCTL.bit.SWI = 1;   // 触发软件初始化（复位 QPOSCNT）
}

//
// eqep1_get_position - 获取当前位置计数值
// 返回：32位位置计数值
//
Uint32 eqep1_get_position(void)
{
    return EQep1Regs.QPOSCNT;
}

//
// eqep1_get_direction - 获取旋转方向
// 返回：0=反转CCW, 1=正转CW
//
int16 eqep1_get_direction(void)
{
    return EQep1Regs.QEPSTS.bit.QDF;
}