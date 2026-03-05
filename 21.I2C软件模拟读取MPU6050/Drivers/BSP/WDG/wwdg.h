#ifndef __WWDG_H
#define __WWDG_H

#include "./SYSTEM/sys/sys.h"

extern WWDG_HandleTypeDef hwwdg;


void IWDG_Init(void);

void IWDG_Feed(void);


#endif
