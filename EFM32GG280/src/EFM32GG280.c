/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: EFM32GG280.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description: 1.XS3:115200:  维护：维护端口
*             2.XS5:115200:  GPRS：模拟RADIO输入输出数据
*             3.XS7:9600:    GPS：输入GPS数据
*******************************************************************/
#include "include.h"


int main(void)
{
	BSP_Init();
	APP_Init();

	//xTimerStart(xTimerCreate("Timer1",(pdMS_TO_TICKS(10)),pdTRUE,(void *)0,SysTimer_10ms),0);
	xTaskCreate(Test_Task,(const char *)"task0",200,NULL,1,NULL);
	xTaskCreate(Power_Monitor,(const char *)"task1",200,NULL,1,NULL);
	//xTaskCreate(Au_Timing,(const char *)"task2",200,NULL,1,NULL);
	xTaskCreate(GPS_Timing,(const char *)"task3",200,NULL,1,NULL);
	xTaskCreate(P645_Comm,(const char *)"task4",200,NULL,1,NULL);
	xTaskCreate(P101_Comm,(const char *)"task5",200,NULL,1,NULL);

	vTaskStartScheduler();

	while(1)
	{

	}

	return 0;
}

