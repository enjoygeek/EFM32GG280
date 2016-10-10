/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: mytask.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef MYTASK_H_
#define MYTASK_H_


void Test_Task(void *pvParameters);
void Power_Monitor(void *pvParameters);
void Au_Timing(void *pvParameters);
void GPS_Timing(void *pvParameters);
void P645_Comm(void *pvParameters);
void P101_Comm(void *pvParameters);
void vApplicationIdleHook(void *pvParameters);


#endif

