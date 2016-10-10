/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: circlequeue.c
*Version: 0.1.0
*Date: 2016-08-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "circlequeue.h"

/************************************************************!
*Function: CreateCircleQueue
*Description: 创建环形队列。
*Input: 1.CQ：环形队列名。
*       2.depth：环形队列深度。
*Output: null
*Return: 函数运行状态
*************************************************************/
uint8_t CreateCircleQueue(CircleQueue *CQ,uint32_t depth)
{
	if(depth==0)
		return false;

	CQ->RPOINTER=0;
	CQ->WPOINTER=0;
	CQ->LENS=0;
	CQ->DEPTH=depth;

	CQ->QUEUE=(uint8_t *)malloc(depth*sizeof(uint8_t));

	if(CQ->QUEUE==NULL)
		return false;

	//memset(&CQ->QUEUE,0,CQ->depth); //清空队列

	return true;
}
/************************************************************!
*Function: ReadCircleQueue
*Description: 读取环形队列数据。
*Input: 1.CQ：环形队列名。
*       2.data：目的地址的首地址。
*       3.lens：读取的数据长度。若读取的数据长度大于环形队列中的未读
*       取的数据长度，将未读到的数据位赋0.
*Output: null
*Return: 函数运行状态
*************************************************************/
uint8_t ReadCircleQueue(CircleQueue *CQ,uint8_t *data,uint32_t lens)
{
	if(lens==0)
		return false;

	for(uint32_t i=0;i<lens;i++)
	{
		if(CQ->LENS>0)
		{
			*(data++)=CQ->QUEUE[CQ->RPOINTER];

			ReadData_Maintain(CQ);
		}
		else
		{
			*(data++)=0;
		}
	}

	return true;
}
/************************************************************!
*Function: VirtualReadCircleQueue
*Description: 虚拟读取环形队列数据(不会改变环形队列的读指针和未读
                           取的数据长度)。
*Input: 1.CQ：环形队列名。
*       2.data：目的地址的首地址。
*       3.lens：读取的数据长度。若读取的数据长度大于环形队列中的数据
*       长度，将未读到的数据位赋0。
*       4.location：要读取的数据的起始位置。
*Output: null
*Return: 函数运行状态
*************************************************************/
uint8_t VirtualReadCircleQueue(CircleQueue *CQ,uint32_t location,uint8_t *data,uint32_t lens)
{
	uint32_t newloc;

	if(lens==0)
		return false;

	for(uint32_t i=0;i<lens;i++)
	{
		if(CQ->LENS>=i)
		{
			newloc=location+i;

			if(newloc>=CQ->DEPTH)  //若虚拟指针长度大于depth，取余
			{
				newloc=newloc%CQ->DEPTH;
			}

			*(data++)=CQ->QUEUE[newloc];
		}
		else
		{
			*(data++)=0;
		}
	}
	return true;
}
/************************************************************!
*Function: WriteCircleQueue
*Description: 写入环形队列数据。
*Input: 1.CQ：环形队列名。
*       2.data：写入数据的首地址。
*       3.lens：写入数据长度。若写入数据长度与队列中未读出的数据长度和
*       大于queuedepth，报错。
*Output: null
*Return: 函数运行状态
*************************************************************/
uint8_t WriteCircleQueue(CircleQueue *CQ,uint8_t *data,uint32_t lens)
{
	if(IsFullCircleQueue(CQ)==true)
		return false;

	if(lens>CQ->DEPTH)
		return false;

	if((lens+CQ->LENS)>CQ->DEPTH)
		return false;

	for(uint32_t i=0;i<lens;i++)
	{
		CQ->QUEUE[CQ->WPOINTER] = *(data++);

		WriteData_Maintain(CQ);
	}

	return true;
}
/************************************************************!
*Function: GetCircleQueueLength
*Description: 获得环形队列未读取的数据长度
*Input: 1.CQ：环形队列名。
*Output: null
*Return: 环形队列未读取的数据长度
*************************************************************/
uint8_t GetCircleQueueLength(CircleQueue *CQ)
{
	return CQ->LENS;
}
/************************************************************!
*Function: ClearCircleQueue
*Description: 清空环形队列。
*Input: 1.CQ：环形队列名。
*Output: null
*Return: null
*************************************************************/
void ClearCircleQueue(CircleQueue *CQ)
{
	CQ->RPOINTER=0;
	CQ->WPOINTER=0;
	CQ->LENS=0;

	//memset(&CQ->QUEUE,0,CQ->depth); //清空队列
}
/************************************************************!
*Function: FreeCircleQueue
*Description: 销毁环形队列。
*Input: 1.CQ：环形队列名。
*Output: null
*Return: null
*************************************************************/
void FreeCircleQueue(CircleQueue *CQ)
{
	CQ->RPOINTER=0;
	CQ->WPOINTER=0;
	CQ->LENS=0;
	CQ->DEPTH=0;

	free(CQ->QUEUE);
}
/************************************************************!
*Function: ReadData_Maintain
*Description: 环形队列的读指针和数据长度维护。
*Input: 1.CQ：环形队列名。
*Output: null
*Return: null
*************************************************************/
void ReadData_Maintain(CircleQueue *CQ)
{
	CQ->LENS=(CQ->LENS-1)%CQ->DEPTH;

	CQ->RPOINTER = CQ->RPOINTER + 1;

	if(CQ->RPOINTER==CQ->DEPTH)
	{
		CQ->RPOINTER=0;
	}
}
/************************************************************!
*Function: WriteData_Maintain
*Description: 环形队列的写指针和数据长度维护。
*Input: 1.CQ：环形队列名。
*Output: null
*Return: null
*************************************************************/
void WriteData_Maintain(CircleQueue *CQ)
{
	CQ->LENS=(CQ->LENS+1)%(CQ->DEPTH+1);

	CQ->WPOINTER = CQ->WPOINTER + 1;

	if(CQ->WPOINTER==CQ->DEPTH)
	{
		CQ->WPOINTER=0;
	}
}
/************************************************************!
*Function: IsEmptyCircleQueue
*Description: 判断环形队列是否为空。
*Input: 1.CQ：环形队列名。
*Output: null
*Return: 环形队列是否为空
*************************************************************/
uint8_t IsEmptyCircleQueue(CircleQueue *CQ)
{
	if(CQ->LENS==0)
		return true;
	else
		return false;
}
/************************************************************!
*Function: IsFullCircleQueue
*Description: 判断环形队列是否满。
*Input: 1.CQ：环形队列名。
*Output: null
*Return: 环形队列是否满
*************************************************************/
uint8_t IsFullCircleQueue(CircleQueue *CQ)
{
	if(CQ->LENS==CQ->DEPTH)
		return true;
	else
		return false;
}

