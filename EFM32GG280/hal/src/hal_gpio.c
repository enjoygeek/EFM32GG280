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


/**********GPIO�жϻص�ע�����**********/
GPIO_INT_CB GpioIntCb=NULL;
/************************************************************!
*Function: Gpio_Init
*Description: GPIO��ʼ������������LED��GPS���غ����ĳ�ʼ��
*Input: null
*Output: null
*Return: null
*************************************************************/
void Gpio_Init(void)
{
	GPIO_PinModeSet(gpioPortD,8,gpioModePushPull,0); //LED:PD8
	GPIO_PinModeSet(gpioPortD,8,gpioModePushPull,0); //GPSʹ�ܹܽ�
}
/************************************************************!
*Function: Led_Toggle
*Description: LED��ת����
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
*Description: ʹ��GPSģ��
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
*Description: �ر�GPSģ��
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
*Description: GPIO�ж�ע�ắ��
*Input: func������ָ��
*Output: null
*Return: null
*************************************************************/
void Gpio_Int_Register(GPIO_INT_CB func)
{
	GpioIntCb=func;
}
/************************************************************!
*Function: GPIO_ODD_IRQHandler
*Description: GPIO���жϺ���
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

