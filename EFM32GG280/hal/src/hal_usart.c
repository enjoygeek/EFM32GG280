/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_usart.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_usart.h"

/**********USART0中断回调注册变量**********/
USART0_RX_CB Usart0RxCb=NULL;
/**********LEUART0 RX中断回调注册变量**********/
LEUART0_RX_CB Leuart0RxCb=NULL;
/**********LEUART0 TX中断回调注册变量**********/
LEUART0_TX_CB Leuart0TxCb=NULL;
/**********UART0 RX中断回调注册变量**********/
UART0_RX_CB Uart0RxCb=NULL;
/**********UART0 TX中断回调注册变量**********/
UART0_TX_CB Uart0TxCb=NULL;

/************************************************************!
*Function: USART0_Init
*Description: USART0初始化函数
*Input: baudrate：波特率
*Output: null
*Return: null
*************************************************************/
void Usart0_Init(uint32_t baudrate)
{
	CMU_ClockEnable(cmuClock_USART0,true);

	USART_InitAsync_TypeDef usart0init=USART_INITASYNC_DEFAULT;

	usart0init.baudrate=baudrate;

	USART_InitAsync(USART0,&usart0init);

	GPIO_PinModeSet(TX_WEIHU_PORT,TX_WEIHU_PIN,gpioModePushPull,1);
	GPIO_PinModeSet(RX_WEIHU_PORT,RX_WEIHU_PIN,gpioModeInputPull,1);

	USART0->ROUTE=USART_ROUTE_RXPEN|USART_ROUTE_TXPEN|USART_ROUTE_LOCATION_LOC0;

	USART_IntEnable(USART0,USART_IF_RXDATAV);
	NVIC_ClearPendingIRQ(USART0_RX_IRQn);
	NVIC_EnableIRQ(USART0_RX_IRQn);

	NVIC_SetPriority(USART0_RX_IRQn,10);
}
/************************************************************!
*Function: USART0_TxU8
*Description: USART0发送字节函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Usart0_TxU8(uint8_t data)
{
	USART_Tx(USART0,data);
}
/************************************************************!
*Function: Usart0_TxU32
*Description: USART0发送字节函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Usart0_TxU32(uint32_t data)
{
	uint16_t temp[4];

	temp[0]=(uint8_t)(((uint32_t)data>>24)&0xff);
    temp[1]=(uint8_t)(((uint32_t)data>>16)&0xff);
    temp[2]=(uint8_t)(((uint32_t)data>>8)&0xff);
    temp[3]=(uint8_t)(((uint32_t)data>>0)&0xff);

    for(uint8_t i=0;i<4;i++)
    {
    	USART_Tx(USART0,temp[i]);
    }
}
/************************************************************!
*Function: UART0_Init
*Description: UART0初始化函数
*Input: baudrate：波特率
*Output: null
*Return: null
*************************************************************/
void Uart0_Init(uint32_t baudrate)
{
	CMU_ClockEnable(cmuClock_UART0,true);

	USART_InitAsync_TypeDef uart0init=USART_INITASYNC_DEFAULT;

	uart0init.baudrate=baudrate; //!!!!!最后再加！！！否则容易造成系统卡死！！！
	//uart0init.parity=usartEvenParity;

	USART_InitAsync(UART0,&uart0init);

	GPIO_PinModeSet(TX_GPRS_PORT,TX_GPRS_PIN,gpioModePushPull,1);
	GPIO_PinModeSet(RX_GPRS_PORT,RX_GPRS_PIN,gpioModeInput,0);

	UART0->ROUTE|=UART_ROUTE_TXPEN|UART_ROUTE_RXPEN|UART_ROUTE_LOCATION_LOC0;

	//UART0->IEN=UART_IEN_TXC|UART_IEN_RXDATAV;//打开发送和接受中断

	//NVIC_ClearPendingIRQ(UART0_RX_IRQn);
	//NVIC_EnableIRQ(UART0_RX_IRQn);
	//NVIC_DisableIRQ(UART0_TX_IRQn);
}
/************************************************************!
*Function: UART0_TxU8
*Description: USART0发送字节函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Uart0_TxU8(uint8_t data)
{
	USART_Tx(UART0,data);
}
/************************************************************!
*Function: Uart0_TxU32
*Description: USART0发送字函数
*Input: data：需要发送的字数据
*Output: null
*Return: null
*************************************************************/
void Uart0_TxU32(uint32_t data)
{
	uint16_t temp[4];

	temp[0]=(uint8_t)(((uint32_t)data>>24)&0xff);
    temp[1]=(uint8_t)(((uint32_t)data>>16)&0xff);
    temp[2]=(uint8_t)(((uint32_t)data>>8)&0xff);
    temp[3]=(uint8_t)(((uint32_t)data>>0)&0xff);

    for(uint8_t i=0;i<4;i++)
    {
    	USART_Tx(UART0,temp[i]);
    }
}
/************************************************************!
*Function: Uart0_Rx_Register
*Description: UART0 RX中断注册函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Uart0_Rx_Register(UART0_RX_CB func)
{
	Uart0RxCb=func;
}
/************************************************************!
*Function: Uart0_Tx_Register
*Description: UART0 TX中断注册函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Uart0_Tx_Register(UART0_TX_CB func)
{
	Uart0TxCb=func;
}
/************************************************************!
*Function: Leuart0_Init
*Description: LEUART0初始化函数
*Input: baudrate：波特率
*Output: null
*Return: null
*************************************************************/
void Leuart0_Init(uint32_t baudrate)
{
	CMU_ClockSelectSet(cmuClock_LFB,cmuSelect_LFXO); //LFB采用外部低频晶振

	CMU_ClockEnable(cmuClock_LEUART0,true);

	LEUART_Init_TypeDef leuart0init=LEUART_INIT_DEFAULT;

	leuart0init.baudrate=baudrate;

	LEUART_Init(LEUART0,&leuart0init);

	GPIO_PinModeSet(TX_GPS_PORT,TX_GPS_PIN,gpioModePushPull,1);
	GPIO_PinModeSet(RX_GPS_PORT,RX_GPS_PIN,gpioModeInputPull,1);

	LEUART0->ROUTE=LEUART_ROUTE_TXPEN|LEUART_ROUTE_RXPEN|LEUART_ROUTE_LOCATION_LOC2;

	LEUART0->IEN=LEUART_IEN_RXDATAV|LEUART_IEN_TXC; //打开接收中断与发送完成中断

	NVIC_ClearPendingIRQ(LEUART0_IRQn);
	NVIC_EnableIRQ(LEUART0_IRQn);
}
/************************************************************!
*Function: Leuart0_TxU8
*Description: LEUART0发送字节函数
*Input: data：发送的字节
*Output: null
*Return: null
*************************************************************/
void Leuart0_TxU8(uint8_t data)
{
	LEUART_Tx(LEUART0,data);
}
/************************************************************!
*Function: Usart0_Rx_Register
*Description: USART0接收注册函数
*Input: func：需要注册的函数名
*Output: null
*Return: null
*************************************************************/
void Usart0_Rx_Register(USART0_RX_CB func)
{
	Usart0RxCb=func;
}
/************************************************************!
*Function: Leuart0_Rx_Register
*Description: Leuart0 RX中断注册函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Leuart0_Rx_Register(LEUART0_RX_CB func)
{
	Leuart0RxCb=func;
}
/************************************************************!
*Function: Leuart0_Tx_Register
*Description: Leuart0 TX中断注册函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Leuart0_Tx_Register(LEUART0_TX_CB func)
{
	Leuart0TxCb=func;
}
/************************************************************!
*Function: USART0_RX_IRQHandler
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void USART0_RX_IRQHandler(void)
{
    uint8_t data;
	USART_IntClear(USART0,USART_IF_RXDATAV);

	data=USART_Rx(USART0);

	if(Usart0RxCb!=NULL)
    {
		(*Usart0RxCb)(&data,1);
    }
}
/************************************************************!
*Function: LEUART0_IRQHandler
*Description: LEUART0中断函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void LEUART0_IRQHandler(void)
{
	if(LEUART0->IF&LEUART_IF_TXC)//发送完成中断
	{
		LEUART0->IFC=LEUART_IFC_TXC;

		if(Leuart0TxCb!=NULL)
	    {
			(*Leuart0TxCb)();
	    }
	}

	if(LEUART0->IF&LEUART_IF_RXDATAV)//接收数据中断
	{
		if(Leuart0RxCb!=NULL)
	    {
			(*Leuart0RxCb)();
	    }
	}
}
/************************************************************!
*Function: UART0_RX_IRQHandler
*Description: UART0 RX中断函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void UART0_RX_IRQHandler(void)
{
	while(!(UART0->STATUS&USART_STATUS_RXDATAV));

	if(Uart0RxCb!=NULL)
    {
		(*Uart0RxCb)();
    }
}
/************************************************************!
*Function: UART0_TX_IRQHandler
*Description: UART0 TX中断函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void UART0_TX_IRQHandler(void)
{
	UART0->IFC=UART_IFC_TXC;//清发送完成中断标志位

	if(Uart0TxCb!=NULL)
    {
		(*Uart0TxCb)();
    }
}

