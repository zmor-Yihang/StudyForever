#ifndef __IWDG_H
#define __IWDG_H

#include "./SYSTEM/sys/sys.h"

extern IWDG_HandleTypeDef g_hiwdg;    /* IWDG的句柄 */


void IWDG_Init();       /* 溢出时间为1s */
void IWDG_Feed();

#endif
