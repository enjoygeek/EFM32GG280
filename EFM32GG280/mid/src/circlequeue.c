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
*Description: �������ζ��С�
*Input: 1.CQ�����ζ�������
*       2.depth�����ζ�����ȡ�
*Output: null
*Return: ��������״̬
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

	//memset(&CQ->QUEUE,0,CQ->depth); //��ն���

	return true;
}
/************************************************************!
*Function: ReadCircleQueue
*Description: ��ȡ���ζ������ݡ�
*Input: 1.CQ�����ζ�������
*       2.data��Ŀ�ĵ�ַ���׵�ַ��
*       3.lens����ȡ�����ݳ��ȡ�����ȡ�����ݳ��ȴ��ڻ��ζ����е�δ��
*       ȡ�����ݳ��ȣ���δ����������λ��0.
*Output: null
*Return: ��������״̬
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
*Description: �����ȡ���ζ�������(����ı价�ζ��еĶ�ָ���δ��
                           ȡ�����ݳ���)��
*Input: 1.CQ�����ζ�������
*       2.data��Ŀ�ĵ�ַ���׵�ַ��
*       3.lens����ȡ�����ݳ��ȡ�����ȡ�����ݳ��ȴ��ڻ��ζ����е�����
*       ���ȣ���δ����������λ��0��
*       4.location��Ҫ��ȡ�����ݵ���ʼλ�á�
*Output: null
*Return: ��������״̬
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

			if(newloc>=CQ->DEPTH)  //������ָ�볤�ȴ���depth��ȡ��
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
*Description: д�뻷�ζ������ݡ�
*Input: 1.CQ�����ζ�������
*       2.data��д�����ݵ��׵�ַ��
*       3.lens��д�����ݳ��ȡ���д�����ݳ����������δ���������ݳ��Ⱥ�
*       ����queuedepth������
*Output: null
*Return: ��������״̬
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
*Description: ��û��ζ���δ��ȡ�����ݳ���
*Input: 1.CQ�����ζ�������
*Output: null
*Return: ���ζ���δ��ȡ�����ݳ���
*************************************************************/
uint8_t GetCircleQueueLength(CircleQueue *CQ)
{
	return CQ->LENS;
}
/************************************************************!
*Function: ClearCircleQueue
*Description: ��ջ��ζ��С�
*Input: 1.CQ�����ζ�������
*Output: null
*Return: null
*************************************************************/
void ClearCircleQueue(CircleQueue *CQ)
{
	CQ->RPOINTER=0;
	CQ->WPOINTER=0;
	CQ->LENS=0;

	//memset(&CQ->QUEUE,0,CQ->depth); //��ն���
}
/************************************************************!
*Function: FreeCircleQueue
*Description: ���ٻ��ζ��С�
*Input: 1.CQ�����ζ�������
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
*Description: ���ζ��еĶ�ָ������ݳ���ά����
*Input: 1.CQ�����ζ�������
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
*Description: ���ζ��е�дָ������ݳ���ά����
*Input: 1.CQ�����ζ�������
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
*Description: �жϻ��ζ����Ƿ�Ϊ�ա�
*Input: 1.CQ�����ζ�������
*Output: null
*Return: ���ζ����Ƿ�Ϊ��
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
*Description: �жϻ��ζ����Ƿ�����
*Input: 1.CQ�����ζ�������
*Output: null
*Return: ���ζ����Ƿ���
*************************************************************/
uint8_t IsFullCircleQueue(CircleQueue *CQ)
{
	if(CQ->LENS==CQ->DEPTH)
		return true;
	else
		return false;
}

