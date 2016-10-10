/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: mytask.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"
#include "mytask.h"

/************************************************************!
*Function: Test_Task
*Description: 测试任务，仅测试使用
*Input: null
*Output: null
*Return: null
*************************************************************/
void Test_Task(void *pvParameters)
{
	while (1)
	{
		Wdog_Feed();

		Led_Toggle();

		//*****WANGJIANHUI*****测试代码*****//
		Leuart0_TxU8(0xFA); //打印系统时间
		Leuart0_TxU8(gSYS_TIME.T.YEAR);
		Leuart0_TxU8(gSYS_TIME.T.MONTH);
		Leuart0_TxU8(gSYS_TIME.T.DAY);
		Leuart0_TxU8(gSYS_TIME.T.HOUR);
		Leuart0_TxU8(gSYS_TIME.T.MINUTE);
		Leuart0_TxU8(gSYS_TIME.T.SECOND);
		Leuart0_TxU8((uint8_t)(LETIMER0->CNT&&0xFF));
		Leuart0_TxU8((uint8_t)((LETIMER0->CNT>>8)&&0xFF));
		//*****WANGJIANHUI*****测试代码*****//

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
/************************************************************!
*Function: Power_Monitor
*Description: 电源检测任务，任务的主要功能是定时测量三个电源的电压，
*             若蓄电池电压小于阈值，则开启PWM充电，否则关闭充电。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Power_Monitor(void *pvParameters)
{
	uint32_t samples[3];
	uint8_t erratashift;

	erratashift = Adc0_Errata(); //读取ADC的校正数据

	while (1)
	{
		Wdog_Feed();

		//Timer1_Stop(); //测量电压前先关闭PWM，防止测量的电压数据不准确

		vTaskDelay(pdMS_TO_TICKS(100)); //关闭PWM后延时100MS，待电压稳定

		DMA_ActivateBasic(0,true,false,samples,(void *)((uint32_t)&(ADC0->SCANDATA)),3-1);

		ADC_Start(ADC0,adcStartScan);

		while(ADC0->STATUS&ADC_STATUS_SCANACT); //等待SCAN完毕

		if(erratashift) //ADC采样值是否需要校正
		{
			for(uint8_t i=0;i<3;i++)
			{
				samples[i]<<=erratashift;
			}
		}

		gSYS_INFO.CAP_BAT=(3.31/4096)*samples[0]*6;
		gSYS_INFO.XDC_BAT=(3.31/4096)*samples[1]*6;
		gSYS_INFO.SUN_BAT=(3.31/4096)*samples[2]*6;

		//*****WANGJIANHUI*****测试代码*****//
		//Usart0_TxU8(0xAA);
		//Usart0_TxU32(gSYS_INFO.CAP_BAT*100);
		//Usart0_TxU32(gSYS_INFO.XDC_BAT*100);
		//Usart0_TxU32(gSYS_INFO.SUN_BAT*100);
		//*****WANGJIANHUI*****测试代码*****//

		if((gSYS_INFO.XDC_BAT<12)&&(gSYS_INFO.SUN_BAT>=17))
		{
			//Timer1_Start();
		}
		else
		{
			//Timer1_Stop();
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
/************************************************************!
*Function: Au_Timing
*Description: 用于采集单元的10S的对时。此函数算法:为了保证系统中断函数
*             正常运行，不进入临界代码区，保留中断。为了防止校时帧
*             的重复发送过程中，系统时标的拷贝被1S的中断打断，导致时标
*             拷贝过程中发生错误，每次在发送重复校时帧之前，判断CNT计
*             数值。
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Au_Timing(void *pvParameters)
{
	while (1)
	{
		Wdog_Feed();

		vTaskSuspendAll(); //之所以不使用临界代码区是为了保留中断，暂时这样使用

		while(LETIMER0->CNT<20000); //为校时帧的发送预留出20000/32768=610ms的发送时间

		for(uint8_t i=0;i<10;i++) //广播校时帧，此处直接发100遍，后期根据LDC时间调整，每一帧大概3.4MS发送完毕，100帧一共耗时340ms
	    {
			while(Radio_Flag==0); //等待上一帧校时帧发送完毕

			gSYS_TIME.T.CNT=LETIMER0->CNT;

			Set_P645_Frame(&RADIO_TX_PF,LOCAL_ADDR,BROADCAST_ADDR,0x42,0x01,1,1,gSYS_TIME.TIME,4); //0x01校时帧

			Radio_StartTx(Rf_Channel,RADIO_TX_PF.PDUFRAME,26); //校时帧就不要放入MID了，直接发送，减少时标的延时误差

			Radio_Flag=0;

			//*****WANGJIANHUI*****测试代码*****//
			//for(uint8_t m=0;m<26;m++)
				//Uart0_TxU8(RADIO_TX_PF.PDUFRAME[m]);
			//*****WANGJIANHUI*****测试代码*****//
	    }

		xTaskResumeAll();

		vTaskDelay(pdMS_TO_TICKS(10000)); //10S校正一次采集单元时间
	}
}
/************************************************************!
*Function: GPS_Timing
*Description: 用于GPS对汇集单元15分钟进行一次校时。从给GPS模块上电到
*             校时成功设置40S的时间，超出时间校时失败则关闭GPS模块，
*             等待下一个校时周期继续校时。
*Input: null
*Output: null
*Return: null
*************************************************************/
void GPS_Timing(void *pvParameters)
{
	uint8_t time;

	volatile uint8_t GpsTiming=1; //GPS校时标志
	volatile uint8_t GpsOpen=1; //GPS模块上电标志

	while (1)
	{
		Wdog_Feed();

		if(GpsTiming==1)
		{
			if(GpsOpen==1)
			{
				Gps_Power_On(); //GPS初始化，引脚上电

				time=gSYS_TIME.T.SECOND; //倒计时开始

				GpsOpen=0;
			}

			if(Gps_Abstime_Process()) //如果接收到GPS数据，且校时成功
			{
				LETIMER0->CMD|=0x04; //清CNT

				GpsTiming=0;

				//*****WANGJIANHUI*****测试代码*****//
				Leuart0_TxU8(0xFD);
				Leuart0_TxU8(gSYS_TIME.T.YEAR);
				Leuart0_TxU8(gSYS_TIME.T.MONTH);
				Leuart0_TxU8(gSYS_TIME.T.DAY);
				Leuart0_TxU8(gSYS_TIME.T.HOUR);
				Leuart0_TxU8(gSYS_TIME.T.MINUTE);
				Leuart0_TxU8(gSYS_TIME.T.SECOND);
				Leuart0_TxU8((uint8_t)(LETIMER0->CNT&&0xFF));
				Leuart0_TxU8((uint8_t)(LETIMER0->CNT>>8));
				//*****WANGJIANHUI*****测试代码*****//
			}

	    	if(((gSYS_TIME.T.SECOND+60-time)%60)>40) //超时40S则取消校时
	    	{
	    		GpsTiming=0;
	    	}
		}

		if(GpsTiming==0)
		{
			GpsTiming=1; //延时15分钟后开始校时
			GpsOpen=1; //延时15分钟后给GPS模块上电

			Gps_Power_Off();

			//vTaskDelay(pdMS_TO_TICKS(GPS_Interval*60000)); //15分钟校正一次汇集单元时间
		}
	}
}
/************************************************************!
*Function: P645_Comm
*Description: 采集单元645协议报文处理及维护端口645协议报文处理函数。
*Input: null
*Output: null
*Return: null
*************************************************************/
void P645_Comm(void *pvParameters)
{
	while (1)
	{
		Wdog_Feed();

		P645_Parse_Process(&RADIO_RXCQ,&RADIO_RX_PF); //采集单元和维护射频通信处理
		P645_Handle_Process(&RADIO_RX_PF);
		Radio_Queue_Tx();

		P645_Parse_Process(&WEIHU_RXCQ,&USART0_RX_PF); //维护232端口通信处理
		P645_Handle_Process(&USART0_RX_PF);
		Usart0_Queue_Tx();

		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
/************************************************************!
*Function: P101_Comm
*Description: 负控终端101协议处理函数。
*Input: null
*Output: null
*Return: null
*************************************************************/
void P101_Comm(void *pvParameters)
{
	while (1)
	{
		Wdog_Feed();


	}
}
/************************************************************!
*Function: vApplicationIdleHook
*Description: 任务钩子函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void vApplicationIdleHook(void *pvParameters)
{

}

