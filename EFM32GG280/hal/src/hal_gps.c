/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_gpio.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_math.h"
#include "hal_gps.h"


/************************************************************!
*Function: Get_Abstime_Now
*Description: �õ���ǰ��ʱ����Ϣ,GPSͨ��ģʽ����Ҫ���Դ��������
*Input: null
*Output: null
*Return: Уʱ�ɹ�����ʧ��
*************************************************************/
uint8_t Get_Abstime_Now(void)
{
	uint8_t time;

	Gps_Power_On(); //GPS��ʼ���������ϵ�

    time=gSYS_TIME.T.SECOND;

    while(1)
    {
    	Wdog_Feed();

    	if(((gSYS_TIME.T.SECOND+60-time)%60)>40) //�ϵ��ȴ�40S��ʱ��,��ʱ��ʱ
    	{
    		Gps_Power_Off(); //GPS�ر�

    		return false;
    	}

    	if(Gps_Abstime_Process())
    	{
    		Gps_Power_Off(); //GPS�ر�

    		return true;
    	}
    }

	return false;
}
/************************************************************!
*Function: Gps_Abstime_Process
*Description: ����GPS��ʱ����Ϣ������GPS��GPRMC������Ϣ
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t Gps_Abstime_Process(void)
{
	uint8_t data=0;

	static uint8_t crc=0;
	static uint8_t index=0;
	static uint8_t state=0;
	static uint8_t GPS_BUF[80];

	if(GPS_RXCQ.LENS==0)
		return false;

	ReadCircleQueue(&GPS_RXCQ,&data,1);

	if(data=='$') //�˴�����Ӵ˴���
	{
		state=1;
		index=0;
		return false;
	}

	switch(state)
	{
		case 1: //�ж�Talker ID"GPRMC"
			GPS_BUF[index++]=data;

			if(index<5)
				return false;

			if(memcmp(GPS_BUF,"GPRMC",5)!=0)
			{
				state=0;
				index=0;
			}
			else
				state++;
			break;

		case 2: //��ȡ֡����
			if(data!='*')
			{
				GPS_BUF[index++]=data;
				if(index>=68)
				{
					state=0;
					index=0;
				}
			}
			else
				state++;
			break;

		case 3: //��ȡУ��ĵ�һ���ַ�
            crc=Get_Hex(data)<<4;
            state++;
			break;

		case 4:
			crc+=Get_Hex(data);

			if(crc==Check_Gps_Crc(GPS_BUF,index))
				return Get_Gps_Abstime(GPS_BUF,index);
			break;

		 default:
			 state=0;
			 break;
	}

	return false;
}
/************************************************************!
*Function: Get_Gps_Abstime
*Description: ����GPS��ʱ����Ϣ����Ҫ����GPS��GPRMC��ʽ��Ϣ
*Input: 1.data��������׵�ַ
*       2.lens�����鳤��
*Output: null
*Return: ���³ɹ���ʧ��
*************************************************************/
uint8_t Get_Gps_Abstime(uint8_t *data,uint8_t lens)
{
	if(data[17]!='A') //�ж�UTC�Ƿ�Ϸ������Ϸ�����
		return false;

    if(lens<64) //��ֹ����Խ�磬��Ҫȷ����С�����ǲ���64(������$����������ַ�)
    	return false;

	gSYS_TIME.T.HOUR=Char2Uint8_t(data+6);
	gSYS_TIME.T.MINUTE=Char2Uint8_t(data+8);
	gSYS_TIME.T.SECOND=Char2Uint8_t(data+10);
	gSYS_TIME.T.DAY=Char2Uint8_t(data+54);
	gSYS_TIME.T.MONTH=Char2Uint8_t(data+56);
	gSYS_TIME.T.YEAR=Char2Uint8_t(data+58);

	return true;
}
/************************************************************!
*Function: Check_Gps_Crc
*Description: ��������Ԫ�ص����ֵ
*Input: 1.data��������׵�ַ
*       2.lens�����鳤��
*Output: null
*Return: ���ֵ
*************************************************************/
uint8_t Check_Gps_Crc(uint8_t *data,uint8_t lens)
{
	uint8_t crc;

	crc=data[0];

    for(uint8_t i=1;i<lens;i++)
    {
    	crc^=data[i];
    }

    return crc;
}

