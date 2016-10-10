/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_cmu.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_cmu.h"

/************************************************************!
*Function: Cmu_Init
*Description: CMU初始化函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Cmu_Init(void)
{
	CMU_HFRCOBandSet(cmuHFRCOBand_14MHz); //RCO:14M，修改此处需要对应修改RTOS时钟配置

	CMU_ClockEnable(cmuClock_CORELE,true);

	CMU_ClockEnable(cmuClock_GPIO,true);
}

