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
*Description: ���ζ��г�ʼ�����ϣ�����main�������á�
*Input:
*Output: null
*Return: null
*************************************************************/
void CircleQueue_Init(void)
{
	CreateCircleQueue(&GPS_RXCQ,1000);   //GPS���ڽ��ջ���
	CreateCircleQueue(&RADIO_RXCQ,1000); //RADIO���ջ���
	CreateCircleQueue(&RADIO_TXCQ,1000); //RADIO���ͻ���
	CreateCircleQueue(&WEIHU_RXCQ,1000); //ά���˿ڽ��ջ���
	CreateCircleQueue(&WEIHU_TXCQ,1000); //ά���˿ڷ��ͻ���
}
/************************************************************!
*Function:  Radio_INT_Process
*Description: ��Ƶ��������д�뻷�ζ��У��˺�������GPIO�ж�ע�ắ����
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_INT_Process(void)
{
	si446x_get_int_status(0u, 0u, 0u);

	if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_PH_STATUS_REP_PACKET_RX_PEND_BIT)
	{
		si446x_fifo_info(0u);    //��ȡRX_FIFO���ݳ���

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
*Description: ��Ƶ��������д�뻷�ζ��У��˺�������GPIO�ж�ע�ắ����
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_Direct_Tx(uint8_t *data, uint8_t lens)
{
	while(Radio_Flag==0); //�ȴ�RADIO���ݷ������

	Radio_StartTx(Rf_Channel,data,lens); //���RADIO BUFFER ��������

	Radio_Flag=0; //�÷��ͱ�־λ
}
/************************************************************!
*Function:  Radio_Queue_Tx
*Description: ��ѯ��Ƶ���ͻ��棬������ʱ��������Ƶ���ͺ����������ݡ�
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
*Description: ���ڽ�������д�뻷�ζ��У��˺������ڴ��ڽ���ע�ắ����
*Input: 1.data��д�����ݵ��׵�ַ��
*       2.lens��д�����ݳ��ȡ�
*Output: null
*Return: null
*************************************************************/
void Usart0_Queue_Rx(uint8_t *data,uint8_t lens)
{
	WriteCircleQueue(&WEIHU_RXCQ,data,lens);
}
/************************************************************!
*Function:  Usart0_Queue_Tx
*Description: ���ڷ�������д�뻷�ζ��У��˺������ڴ��ڽ���ע�ắ����
*Input: 1.data��д�����ݵ��׵�ַ��
*       2.lens��д�����ݳ��ȡ�
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
*Description: GPS���ڽ�������д�뻷�ζ���
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
*Description: �����ζ����е�����ͨ�����ڷ��͡�
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

