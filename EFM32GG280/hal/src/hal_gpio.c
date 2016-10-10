/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_gpio.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_gpio.h"


/**********GPIO中断回调注册变量**********/
GPIO_INT_CB GpioIntCb=NULL;
/************************************************************!
*Function: Gpio_Init
*Description: GPIO初始化函数，包括LED和GPS开关函数的初始化
*Input: null
*Output: null
*Return: null
*************************************************************/
void Gpio_Init(void)
{
	GPIO_PinModeSet(gpioPortD,8,gpioModePushPull,0); //LED:PD8
	GPIO_PinModeSet(gpioPortD,8,gpioModePushPull,0); //GPS使能管脚
}
/************************************************************!
*Function: Led_Toggle
*Description: LED翻转函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Led_Toggle(void)
{
	GPIO_PinOutToggle(gpioPortD,8);
}
/************************************************************!
*Function: Gps_Power_On
*Description: 使能GPS模块
*Input: null
*Output: null
*Return: null
*************************************************************/
void Gps_Power_On(void)
{
	GPIO_PinOutSet(gpioPortD,8);
}
/************************************************************!
*Function: Gps_Power_Off
*Description: 关闭GPS模块
*Input: null
*Output: null
*Return: null
*************************************************************/
void Gps_Power_Off(void)
{
	GPIO_PinOutClear(gpioPortD,8);
}
/************************************************************!
*Function: Gpio_Int_Register
*Description: GPIO中断注册函数
*Input: func：函数指针
*Output: null
*Return: null
*************************************************************/
void Gpio_Int_Register(GPIO_INT_CB func)
{
	GpioIntCb=func;
}
/************************************************************!
*Function: GPIO_ODD_IRQHandler
*Description: GPIO奇中断函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void GPIO_ODD_IRQHandler(void)
{
	uint32_t intflags=GPIO_IntGet();
	GPIO_IntClear(intflags);

	if(GpioIntCb!=NULL)
    {
		(*GpioIntCb)();
    }
}

