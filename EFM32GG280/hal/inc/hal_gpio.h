/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_gpio.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_GPIO_H_
#define HAL_GPIO_H_


#include <stddef.h>
#include "em_gpio.h"


/**********函数指针回调声明**********/
typedef void(*GPIO_INT_CB)(void);


void Gpio_Init(void);
void Led_Toggle(void);
void Gps_Power_On(void);
void Gps_Power_Off(void);
void Gpio_Int_Register(GPIO_INT_CB func);
void GPIO_ODD_IRQHandler(void);


#endif

