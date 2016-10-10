/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_timer.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_timer.h"

/**********TIMER0中断回调注册变量**********/
TIMER0_INT_CB Timer0IntCb=NULL;
/**********LEUART0 TX中断回调注册变量**********/
LETIMER0_INT_CB Letimer0IntCb=NULL;
/************************************************************!
*Function: Timer0_Init
*Description: TIMER0初始化函数,产生1ms中断
*Input: null
*Output: null
*Return: null
*************************************************************/
void Timer0_Init(void)
{
	CMU_ClockEnable(cmuClock_TIMER0,true);

	TIMER_Init_TypeDef timer0init=TIMER_INIT_DEFAULT;

	timer0init.debugRun=true;
	timer0init.prescale=timerPrescale2,//2分频，7M
	timer0init.clkSel=timerClkSelHFPerClk,//14M

	TIMER_TopSet(TIMER0,7000);//7M，每7K次代表1ms

	TIMER_Init(TIMER0,&timer0init);

	TIMER_IntEnable(TIMER0,TIMER_IF_OF);
	NVIC_EnableIRQ(TIMER0_IRQn);
}
/************************************************************!
*Function: Timer0_Int_Register
*Description: TIMER0中断注册函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Timer0_Int_Register(TIMER0_INT_CB func)
{
	Timer0IntCb=NULL;
}
/************************************************************!
*Function: Timer1_Init
*Description: TIMER1初始化函数，产生固定占空比的PWM并输出
*Input: null
*Output: null
*Return: null
*************************************************************/
void Timer1_Init(void)
{
	CMU_ClockEnable(cmuClock_TIMER1,true);

	TIMER_Init_TypeDef timer1init=TIMER_INIT_DEFAULT;

	timer1init.debugRun=true;
	timer1init.prescale=timerPrescale2;   //2分频,7M
	timer1init.clkSel=timerClkSelHFPerClk;//14M

	TIMER_InitCC_TypeDef timer1initCC0=TIMER_INITCC_DEFAULT;

	timer1initCC0.edge=timerEdgeBoth;
	timer1initCC0.mode=timerCCModePWM;
	timer1initCC0.cmoa=timerOutputActionToggle;

	TIMER_InitCC(TIMER1,0,&timer1initCC0);

	TIMER_Init(TIMER1,&timer1init);

	TIMER_TopSet(TIMER1,7000000/50000); //PWM频率设定为50K

	TIMER_CompareBufSet(TIMER1,0,((7000000/50000)*50/100)); //占空比50%

	TIMER1->ROUTE|=TIMER_ROUTE_CC0PEN|TIMER_ROUTE_LOCATION_LOC0;

	GPIO_PinModeSet(gpioPortC,13,gpioModePushPull,1); //PC13输出PWM

	TIMER1->CMD=0b01; //0b:二进制,启动PWM
}
/************************************************************!
*Function: Letimer0_Init
*Description: LETIMER0初始化函数，LETIMER0只需按照32768频率简单计数
*             即可，CNT寄存器值可以读，不可以写，清零的话利用CMD寄存
*             器的CLEAR命令。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Letimer0_Init(void)
{
	CMU_ClockSelectSet(cmuClock_LFA,cmuSelect_LFXO); //LETIMER0时钟

	CMU_ClockEnable(cmuClock_LETIMER0,true);

	LETIMER_Init_TypeDef letimer0init=LETIMER_INIT_DEFAULT;

	letimer0init.debugRun=true;
	//letimerinit.ufoa0=letimerUFOAPulse;
	letimer0init.comp0Top=true;

	LETIMER_CompareSet(LETIMER0,0,32768/1); //1S一次中断
	LETIMER_RepeatSet(LETIMER0,0,1);

	LETIMER_Init(LETIMER0,&letimer0init);

	LETIMER_IntEnable(LETIMER0,LETIMER_IF_COMP0);
	NVIC_ClearPendingIRQ(LETIMER0_IRQn);
	NVIC_EnableIRQ(LETIMER0_IRQn);
	//NVIC_SetPriority(LETIMER0_IRQn,20); //设置LETIMER0中断任务优先级
}
/************************************************************!
*Function: Letimer0_Int_Register
*Description: LETIMER0中断注册函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Letimer0_Int_Register(LETIMER0_INT_CB func)
{
	Letimer0IntCb=func;
}
/************************************************************!
*Function: Timer1_Start
*Description: TIMER1启动函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Timer1_Start(void)
{
	TIMER1->CMD=0b01; //0b:二进制
}
/************************************************************!
*Function: Timer1_Stop
*Description: TIMER1停止函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Timer1_Stop(void)
{
	TIMER1->CMD=0b10;
}
/************************************************************!
*Function: TIMER0_IRQHandler
*Description: TIMER0中断函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void TIMER0_IRQHandler(void)
{
	TIMER_IntClear(TIMER0,TIMER_IF_OF);

	if(Timer0IntCb!=NULL)
    {
		(*Timer0IntCb)();
    }
}
/************************************************************!
*Function: LETIMER0_IRQHandler
*Description: LETIMER0中断函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void LETIMER0_IRQHandler(void)
{
	LETIMER_IntClear(LETIMER0,LETIMER_IF_COMP0);

	if(Letimer0IntCb!=NULL)
    {
		(*Letimer0IntCb)();
    }
}

