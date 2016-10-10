/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: mid.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"
#include "mid.h"

/************************************************************!
*Function: CircleQueue_Init
*Description: 环形队列初始化集合，方便main函数调用。
*Input:
*Output: null
*Return: null
*************************************************************/
void CircleQueue_Init(void)
{
	CreateCircleQueue(&GPS_RXCQ,1000);   //GPS串口接收缓存
	CreateCircleQueue(&RADIO_RXCQ,1000); //RADIO接收缓存
	CreateCircleQueue(&RADIO_TXCQ,1000); //RADIO发送缓存
	CreateCircleQueue(&WEIHU_RXCQ,1000); //维护端口接收缓存
	CreateCircleQueue(&WEIHU_TXCQ,1000); //维护端口发送缓存
}
/************************************************************!
*Function:  Radio_INT_Process
*Description: 射频接收数据写入环形队列，此函数用于GPIO中断注册函数。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_INT_Process(void)
{
	si446x_get_int_status(0u, 0u, 0u);

	if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_PH_STATUS_REP_PACKET_RX_PEND_BIT)
	{
		si446x_fifo_info(0u);    //读取RX_FIFO数据长度

		RadioPkt.lens=Si446xCmd.FIFO_INFO.RX_FIFO_COUNT;

		si446x_read_rx_fifo(RadioPkt.lens,&RadioPkt.data[0]);

		WriteCircleQueue(&RADIO_RXCQ,RadioPkt.data,RadioPkt.lens);
	}

	if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_PH_STATUS_REP_PACKET_SENT_PEND_BIT)
	{
		Radio_StartRx(Rf_Channel,0x40);

		Radio_Flag=1;
	}
}

/************************************************************!
*Function:  Radio_Direct_Tx
*Description: 射频发送数据写入环形队列，此函数用于GPIO中断注册函数。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_Direct_Tx(uint8_t *data, uint8_t lens)
{
	while(Radio_Flag==0); //等待RADIO数据发送完毕

	Radio_StartTx(Rf_Channel,data,lens); //填充RADIO BUFFER 启动发送

	Radio_Flag=0; //置发送标志位
}
/************************************************************!
*Function:  Radio_Queue_Tx
*Description: 查询射频发送缓存，有数据时，调用射频发送函数发送数据。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_Queue_Tx(void)
{
	uint8_t data;
	uint8_t index=0;
	uint8_t buf[64];

	while(RADIO_TXCQ.LENS>0)
	{
		ReadCircleQueue(&RADIO_TXCQ,&data,1);
		buf[index++]=data;

		if(RADIO_TXCQ.LENS==0||index==64)
		{
			while(Radio_Flag==0);

			Radio_StartTx(Rf_Channel,buf,index);

			Radio_Flag=0;
			index=0;
		}
	}
}
/************************************************************!
*Function:  Usart0_Queue_Rx
*Description: 串口接收数据写入环形队列，此函数用于串口接收注册函数。
*Input: 1.data：写入数据的首地址。
*       2.lens：写入数据长度。
*Output: null
*Return: null
*************************************************************/
void Usart0_Queue_Rx(uint8_t *data,uint8_t lens)
{
	WriteCircleQueue(&WEIHU_RXCQ,data,lens);
}
/************************************************************!
*Function:  Usart0_Queue_Tx
*Description: 串口发送数据写入环形队列，此函数用于串口接收注册函数。
*Input: 1.data：写入数据的首地址。
*       2.lens：写入数据长度。
*Output: null
*Return: null
*************************************************************/
void Usart0_Queue_Tx(void)
{
	uint8_t data;

	while(WEIHU_TXCQ.LENS>0)
	{
		ReadCircleQueue(&WEIHU_TXCQ,&data,1);

		Usart0_TxU8(data);
	}
}
/************************************************************!
*Function:  Leuart0_Queue_Rx
*Description: GPS串口接收数据写入环形队列
*Input: null
*Output: null
*Return: null
*************************************************************/
void Leuart0_Queue_Rx(void)
{
	uint8_t data;

	data=(uint8_t)LEUART0->RXDATA;

	WriteCircleQueue(&GPS_RXCQ,&data,1);
}
/************************************************************!
*Function:  Leuart0_Queue_Tx
*Description: 将环形队列中的数据通过串口发送。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Leuart0_Queue_Tx(void)
{
	uint8_t data;

	if(GPS_RXCQ.LENS>0)
	{
		ReadCircleQueue(&GPS_RXCQ,&data,1);

		LEUART_Tx(LEUART0,data);
	}
}

