/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: circlequeue.h
*Version: 0.1.0
*Date: 2016-08-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef CIRCLEQUEUE_H_
#define CIRCLEQUEUE_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
/************************************************************!
*Function: ���ζ��нṹ��
*Description:
*************************************************************/
typedef struct
{
    volatile uint32_t  RPOINTER;   //��ָ��
    volatile uint32_t  WPOINTER;   //дָ��
    volatile uint32_t  LENS;	   //��д����У���δ���������ݸ���
    volatile uint32_t  DEPTH;      //���ζ������
             uint8_t   *QUEUE;     //���ζ�������ָ��
}CircleQueue;


uint8_t CreateCircleQueue(CircleQueue *CQ,uint32_t queuedepth);
uint8_t ReadCircleQueue(CircleQueue *CQ,uint8_t *data,uint32_t lens);
uint8_t VirtualReadCircleQueue(CircleQueue *CQ,uint32_t location,uint8_t *data,uint32_t lens);
uint8_t WriteCircleQueue(CircleQueue *CQ,uint8_t *data,uint32_t lens);
uint8_t GetCircleQueueLength(CircleQueue *CQ);
void ClearCircleQueue(CircleQueue *CQ);
void FreeCircleQueue(CircleQueue *CQ);

void ReadData_Maintain(CircleQueue *CQ);
void WriteData_Maintain(CircleQueue *CQ);
uint8_t IsEmptyCircleQueue(CircleQueue *CQ);
uint8_t IsFullCircleQueue(CircleQueue *CQ);


#endif

