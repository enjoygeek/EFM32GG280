/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: init.h
*Version: 0.1.0
*Date: 2016-09-20
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef INIT_H_
#define INIT_H_


extern SemaphoreHandle_t  Sem_DataRetrans;


void BSP_Init(void);
void APP_Init(void);
void Get_Sys_Time(void);
void Sys_Tick(void);
void SysTimer_10ms(TimerHandle_t xTimer);


#endif

