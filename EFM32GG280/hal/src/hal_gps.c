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
*Description: 得到当前的时间信息,GPS通信模式还需要与根源进行讨论
*Input: null
*Output: null
*Return: 校时成功或者失败
*************************************************************/
uint8_t Get_Abstime_Now(void)
{
	uint8_t time;

	Gps_Power_On(); //GPS初始化，引脚上电

    time=gSYS_TIME.T.SECOND;

    while(1)
    {
    	Wdog_Feed();

    	if(((gSYS_TIME.T.SECOND+60-time)%60)>40) //上电后等待40S的时间,计时超时
    	{
    		Gps_Power_Off(); //GPS关闭

    		return false;
    	}

    	if(Gps_Abstime_Process())
    	{
    		Gps_Power_Off(); //GPS关闭

    		return true;
    	}
    }

	return false;
}
/************************************************************!
*Function: Gps_Abstime_Process
*Description: 处理GPS的时间信息，解析GPS的GPRMC报文信息
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

	if(data=='$') //此处必须加此代码
	{
		state=1;
		index=0;
		return false;
	}

	switch(state)
	{
		case 1: //判断Talker ID"GPRMC"
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

		case 2: //收取帧内容
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

		case 3: //收取校验的第一个字符
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
*Description: 更新GPS的时间信息，需要解析GPS的GPRMC格式信息
*Input: 1.data：数组的首地址
*       2.lens：数组长度
*Output: null
*Return: 更新成功或失败
*************************************************************/
uint8_t Get_Gps_Abstime(uint8_t *data,uint8_t lens)
{
	if(data[17]!='A') //判断UTC是否合法，不合法返回
		return false;

    if(lens<64) //防止数组越界，需要确认最小长度是不是64(不包括$及最后三个字符)
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
*Description: 检验数组元素的异或值
*Input: 1.data：数组的首地址
*       2.lens：数组长度
*Output: null
*Return: 异或值
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

