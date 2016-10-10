/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_wdog.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_wdog.h"

/************************************************************!
*Function: Wdog_Init
*Description: WDOG初始化函数,WDOG时间为2s.
*Input: null
*Output: null
*Return: null
*************************************************************/
void Wdog_Init(void)
{
	CMU_OscillatorEnable(cmuOsc_ULFRCO,true,true);

	WDOG_Init_TypeDef wdoginit=WDOG_INIT_DEFAULT;

	wdoginit.clkSel=wdogClkSelULFRCO; //1K频率
	wdoginit.perSel=wdogPeriod_8k; //8K时钟周期，大约8s
	wdoginit.swoscBlock=0;

	WDOG_Init(&wdoginit);

	//while(DEFAULT_WDOG->SYNCBUSY&WDOG_SYNCBUSY_CTRL);
	//WDOG_Lock();
}
/************************************************************!
*Function: Wdog_Feed
*Description: WDOG喂狗函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Wdog_Feed(void)
{
	//while(DEFAULT_WDOG->SYNCBUSY&WDOG_SYNCBUSY_CTRL);
	WDOG_Feed();
}

