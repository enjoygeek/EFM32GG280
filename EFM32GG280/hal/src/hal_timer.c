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

/**********TIMER0�жϻص�ע�����**********/
TIMER0_INT_CB Timer0IntCb=NULL;
/**********LEUART0 TX�жϻص�ע�����**********/
LETIMER0_INT_CB Letimer0IntCb=NULL;
/************************************************************!
*Function: Timer0_Init
*Description: TIMER0��ʼ������,����1ms�ж�
*Input: null
*Output: null
*Return: null
*************************************************************/
void Timer0_Init(void)
{
	CMU_ClockEnable(cmuClock_TIMER0,true);

	TIMER_Init_TypeDef timer0init=TIMER_INIT_DEFAULT;

	timer0init.debugRun=true;
	timer0init.prescale=timerPrescale2,//2��Ƶ��7M
	timer0init.clkSel=timerClkSelHFPerClk,//14M

	TIMER_TopSet(TIMER0,7000);//7M��ÿ7K�δ���1ms

	TIMER_Init(TIMER0,&timer0init);

	TIMER_IntEnable(TIMER0,TIMER_IF_OF);
	NVIC_EnableIRQ(TIMER0_IRQn);
}
/************************************************************!
*Function: Timer0_Int_Register
*Description: TIMER0�ж�ע�ắ��
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
*Description: TIMER1��ʼ�������������̶�ռ�ձȵ�PWM�����
*Input: null
*Output: null
*Return: null
*************************************************************/
void Timer1_Init(void)
{
	CMU_ClockEnable(cmuClock_TIMER1,true);

	TIMER_Init_TypeDef timer1init=TIMER_INIT_DEFAULT;

	timer1init.debugRun=true;
	timer1init.prescale=timerPrescale2;   //2��Ƶ,7M
	timer1init.clkSel=timerClkSelHFPerClk;//14M

	TIMER_InitCC_TypeDef timer1initCC0=TIMER_INITCC_DEFAULT;

	timer1initCC0.edge=timerEdgeBoth;
	timer1initCC0.mode=timerCCModePWM;
	timer1initCC0.cmoa=timerOutputActionToggle;

	TIMER_InitCC(TIMER1,0,&timer1initCC0);

	TIMER_Init(TIMER1,&timer1init);

	TIMER_TopSet(TIMER1,7000000/50000); //PWMƵ���趨Ϊ50K

	TIMER_CompareBufSet(TIMER1,0,((7000000/50000)*50/100)); //ռ�ձ�50%

	TIMER1->ROUTE|=TIMER_ROUTE_CC0PEN|TIMER_ROUTE_LOCATION_LOC0;

	GPIO_PinModeSet(gpioPortC,13,gpioModePushPull,1); //PC13���PWM

	TIMER1->CMD=0b01; //0b:������,����PWM
}
/************************************************************!
*Function: Letimer0_Init
*Description: LETIMER0��ʼ��������LETIMER0ֻ�谴��32768Ƶ�ʼ򵥼���
*             ���ɣ�CNT�Ĵ���ֵ���Զ���������д������Ļ�����CMD�Ĵ�
*             ����CLEAR���
*Input: null
*Output: null
*Return: null
*************************************************************/
void Letimer0_Init(void)
{
	CMU_ClockSelectSet(cmuClock_LFA,cmuSelect_LFXO); //LETIMER0ʱ��

	CMU_ClockEnable(cmuClock_LETIMER0,true);

	LETIMER_Init_TypeDef letimer0init=LETIMER_INIT_DEFAULT;

	letimer0init.debugRun=true;
	//letimerinit.ufoa0=letimerUFOAPulse;
	letimer0init.comp0Top=true;

	LETIMER_CompareSet(LETIMER0,0,32768/1); //1Sһ���ж�
	LETIMER_RepeatSet(LETIMER0,0,1);

	LETIMER_Init(LETIMER0,&letimer0init);

	LETIMER_IntEnable(LETIMER0,LETIMER_IF_COMP0);
	NVIC_ClearPendingIRQ(LETIMER0_IRQn);
	NVIC_EnableIRQ(LETIMER0_IRQn);
	//NVIC_SetPriority(LETIMER0_IRQn,20); //����LETIMER0�ж��������ȼ�
}
/************************************************************!
*Function: Letimer0_Int_Register
*Description: LETIMER0�ж�ע�ắ��
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
*Description: TIMER1��������
*Input: null
*Output: null
*Return: null
*************************************************************/
void Timer1_Start(void)
{
	TIMER1->CMD=0b01; //0b:������
}
/************************************************************!
*Function: Timer1_Stop
*Description: TIMER1ֹͣ����
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
*Description: TIMER0�жϺ���
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
*Description: LETIMER0�жϺ���
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

