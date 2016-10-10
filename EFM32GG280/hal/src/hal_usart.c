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

/**********USART0�жϻص�ע�����**********/
USART0_RX_CB Usart0RxCb=NULL;
/**********LEUART0 RX�жϻص�ע�����**********/
LEUART0_RX_CB Leuart0RxCb=NULL;
/**********LEUART0 TX�жϻص�ע�����**********/
LEUART0_TX_CB Leuart0TxCb=NULL;
/**********UART0 RX�жϻص�ע�����**********/
UART0_RX_CB Uart0RxCb=NULL;
/**********UART0 TX�жϻص�ע�����**********/
UART0_TX_CB Uart0TxCb=NULL;

/************************************************************!
*Function: USART0_Init
*Description: USART0��ʼ������
*Input: baudrate��������
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
*Description: USART0�����ֽں���
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
*Description: USART0�����ֽں���
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
*Description: UART0��ʼ������
*Input: baudrate��������
*Output: null
*Return: null
*************************************************************/
void Uart0_Init(uint32_t baudrate)
{
	CMU_ClockEnable(cmuClock_UART0,true);

	USART_InitAsync_TypeDef uart0init=USART_INITASYNC_DEFAULT;

	uart0init.baudrate=baudrate; //!!!!!����ټӣ����������������ϵͳ����������
	//uart0init.parity=usartEvenParity;

	USART_InitAsync(UART0,&uart0init);

	GPIO_PinModeSet(TX_GPRS_PORT,TX_GPRS_PIN,gpioModePushPull,1);
	GPIO_PinModeSet(RX_GPRS_PORT,RX_GPRS_PIN,gpioModeInput,0);

	UART0->ROUTE|=UART_ROUTE_TXPEN|UART_ROUTE_RXPEN|UART_ROUTE_LOCATION_LOC0;

	//UART0->IEN=UART_IEN_TXC|UART_IEN_RXDATAV;//�򿪷��ͺͽ����ж�

	//NVIC_ClearPendingIRQ(UART0_RX_IRQn);
	//NVIC_EnableIRQ(UART0_RX_IRQn);
	//NVIC_DisableIRQ(UART0_TX_IRQn);
}
/************************************************************!
*Function: UART0_TxU8
*Description: USART0�����ֽں���
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
*Description: USART0�����ֺ���
*Input: data����Ҫ���͵�������
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
*Description: UART0 RX�ж�ע�ắ��
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
*Description: UART0 TX�ж�ע�ắ��
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
*Description: LEUART0��ʼ������
*Input: baudrate��������
*Output: null
*Return: null
*************************************************************/
void Leuart0_Init(uint32_t baudrate)
{
	CMU_ClockSelectSet(cmuClock_LFB,cmuSelect_LFXO); //LFB�����ⲿ��Ƶ����

	CMU_ClockEnable(cmuClock_LEUART0,true);

	LEUART_Init_TypeDef leuart0init=LEUART_INIT_DEFAULT;

	leuart0init.baudrate=baudrate;

	LEUART_Init(LEUART0,&leuart0init);

	GPIO_PinModeSet(TX_GPS_PORT,TX_GPS_PIN,gpioModePushPull,1);
	GPIO_PinModeSet(RX_GPS_PORT,RX_GPS_PIN,gpioModeInputPull,1);

	LEUART0->ROUTE=LEUART_ROUTE_TXPEN|LEUART_ROUTE_RXPEN|LEUART_ROUTE_LOCATION_LOC2;

	LEUART0->IEN=LEUART_IEN_RXDATAV|LEUART_IEN_TXC; //�򿪽����ж��뷢������ж�

	NVIC_ClearPendingIRQ(LEUART0_IRQn);
	NVIC_EnableIRQ(LEUART0_IRQn);
}
/************************************************************!
*Function: Leuart0_TxU8
*Description: LEUART0�����ֽں���
*Input: data�����͵��ֽ�
*Output: null
*Return: null
*************************************************************/
void Leuart0_TxU8(uint8_t data)
{
	LEUART_Tx(LEUART0,data);
}
/************************************************************!
*Function: Usart0_Rx_Register
*Description: USART0����ע�ắ��
*Input: func����Ҫע��ĺ�����
*Output: null
*Return: null
*************************************************************/
void Usart0_Rx_Register(USART0_RX_CB func)
{
	Usart0RxCb=func;
}
/************************************************************!
*Function: Leuart0_Rx_Register
*Description: Leuart0 RX�ж�ע�ắ��
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
*Description: Leuart0 TX�ж�ע�ắ��
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
*Description: LEUART0�жϺ���
*Input: null
*Output: null
*Return: null
*************************************************************/
void LEUART0_IRQHandler(void)
{
	if(LEUART0->IF&LEUART_IF_TXC)//��������ж�
	{
		LEUART0->IFC=LEUART_IFC_TXC;

		if(Leuart0TxCb!=NULL)
	    {
			(*Leuart0TxCb)();
	    }
	}

	if(LEUART0->IF&LEUART_IF_RXDATAV)//���������ж�
	{
		if(Leuart0RxCb!=NULL)
	    {
			(*Leuart0RxCb)();
	    }
	}
}
/************************************************************!
*Function: UART0_RX_IRQHandler
*Description: UART0 RX�жϺ���
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
*Description: UART0 TX�жϺ���
*Input: null
*Output: null
*Return: null
*************************************************************/
void UART0_TX_IRQHandler(void)
{
	UART0->IFC=UART_IFC_TXC;//�巢������жϱ�־λ

	if(Uart0TxCb!=NULL)
    {
		(*Uart0TxCb)();
    }
}

