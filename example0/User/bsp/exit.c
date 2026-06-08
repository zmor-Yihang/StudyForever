#include "exit.h"

// 28335有8个外部中断，XINT1,XINT2,XINT3,XINT4,XINT5,XINT6,XINT7和 XNMI
// XNMI为不可屏蔽中断，其他为可屏蔽中断
// GPIO0～GPIO31 （刚好对应GPA组）可以作为外部中断的触发源
// GPIO32～GPIO87 不可作为外部中断的触发源


void exit_init(void)
{

}