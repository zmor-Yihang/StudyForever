#include "key.h"

#define KEY_DEBOUNCE_CNT 3 // 消抖次数

// 3x3 按键映射表: key_map[row][col]
//              col0(GPIO48)  col1(GPIO49)  col2(GPIO50)
// row0(GPIO12)   SW1           SW2           SW3
// row1(GPIO13)   SW4           SW5           SW6
// row2(GPIO14)   SW7           SW8           SW9
static const key_id_t key_map[3][3] = {
    {KEY_SW1, KEY_SW2, KEY_SW3},
    {KEY_SW4, KEY_SW5, KEY_SW6},
    {KEY_SW7, KEY_SW8, KEY_SW9}};

// 读取行线电平(低电平有效)
static Uint16 read_row0(void)
{
    return GpioDataRegs.GPADAT.bit.GPIO12;
}
static Uint16 read_row1(void)
{
    return GpioDataRegs.GPADAT.bit.GPIO13;
}
static Uint16 read_row2(void)
{
    return GpioDataRegs.GPADAT.bit.GPIO14;
}

// 列线控制
static void col_low(Uint16 col)
{
    switch (col)
    {
    case 0:
        GpioDataRegs.GPBCLEAR.bit.GPIO48 = 1;
        break;
    case 1:
        GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;
        break;
    case 2:
        GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;
        break;
    default:
        break;
    }
}

static void col_high(Uint16 col)
{
    switch (col)
    {
    case 0:
        GpioDataRegs.GPBSET.bit.GPIO48 = 1;
        break;
    case 1:
        GpioDataRegs.GPBSET.bit.GPIO49 = 1;
        break;
    case 2:
        GpioDataRegs.GPBSET.bit.GPIO50 = 1;
        break;
    default:
        break;
    }
}

void key_init(void)
{
    EALLOW;

    // 行线 GPIO12, GPIO13, GPIO14 配置为输入，使能内部上拉
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0; // 普通 GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;  // 输入
    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;  // 使能上拉

    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO13 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0;

    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO14 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;

    // 列线 GPIO48, GPIO49, GPIO50 配置为输出，初始高电平
    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0; // 普通 GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1;  // 输出
    GpioCtrlRegs.GPBPUD.bit.GPIO48 = 1;  // 禁用上拉(输出不需要)
    GpioDataRegs.GPBSET.bit.GPIO48 = 1;  // 初始高电平

    GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO49 = 1;
    GpioDataRegs.GPBSET.bit.GPIO49 = 1;

    GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO50 = 1;
    GpioDataRegs.GPBSET.bit.GPIO50 = 1;

    EDIS;
}

key_id_t key_scan(void)
{
    static key_id_t confirmed_key = KEY_NONE;
    static key_id_t last_raw_key = KEY_NONE;
    static Uint16 debounce_cnt = 0;

    key_id_t raw_key = KEY_NONE;
    Uint16 col, row_val;
    Uint16 (*read_row[3])(void) = {read_row0, read_row1, read_row2};

    // 扫描 3 列
    for (col = 0; col < 3; col++)
    {
        col_low(col); // 拉低当前列
        DELAY_US(5);  // 等待电平稳定

        for (row_val = 0; row_val < 3; row_val++)
        {
            if (read_row[row_val]() == 0) // 低电平 = 按键按下
            {
                raw_key = key_map[row_val][col];
                break;
            }
        }

        col_high(col); // 恢复当前列为高

        if (raw_key != KEY_NONE)
            break; // 找到一个按下的键就停止扫描
    }

    // 消抖处理
    if (raw_key == last_raw_key)
    {
        if (debounce_cnt < KEY_DEBOUNCE_CNT)
            debounce_cnt++;
        if (debounce_cnt >= KEY_DEBOUNCE_CNT)
            confirmed_key = raw_key;
    }
    else
    {
        debounce_cnt = 0;
        last_raw_key = raw_key;
    }

    // 按键释放后复位确认状态
    if (raw_key == KEY_NONE)
        confirmed_key = KEY_NONE;

    return confirmed_key;
}
