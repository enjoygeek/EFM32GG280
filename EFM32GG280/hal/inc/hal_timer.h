/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_timer.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_TIMER_H_
#define HAL_TIMER_H_


#include <stddef.h>
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_letimer.h"

/**********函数指针回调声明**********/
typedef void(*TIMER0_INT_CB)(void);
/**********函数指针回调声明**********/
typedef void(*LETIMER0_INT_CB)(void);


void Timer0_Init(void);
void Timer0_Int_Register(TIMER0_INT_CB func);
void Timer1_Init(void);
void Letimer0_Init(void);
void Letimer0_Int_Register(LETIMER0_INT_CB func);
void Timer1_Start(void);
void Timer1_Stop(void);
void TIMER0_IRQHandler(void);
void LETIMER0_IRQHandler(void);


#endif

