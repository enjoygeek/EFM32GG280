/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_wdog.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_WDOG_H_
#define HAL_WDOG_H_

#include "em_cmu.h"
#include "em_wdog.h"


void Wdog_Init(void);
void Wdog_Feed(void);


#endif

