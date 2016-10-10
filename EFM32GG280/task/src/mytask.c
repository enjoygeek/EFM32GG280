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
*Description: �������񣬽�����ʹ��
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

		//*****WANGJIANHUI*****���Դ���*****//
		Leuart0_TxU8(0xFA); //��ӡϵͳʱ��
		Leuart0_TxU8(gSYS_TIME.T.YEAR);
		Leuart0_TxU8(gSYS_TIME.T.MONTH);
		Leuart0_TxU8(gSYS_TIME.T.DAY);
		Leuart0_TxU8(gSYS_TIME.T.HOUR);
		Leuart0_TxU8(gSYS_TIME.T.MINUTE);
		Leuart0_TxU8(gSYS_TIME.T.SECOND);
		Leuart0_TxU8((uint8_t)(LETIMER0->CNT&&0xFF));
		Leuart0_TxU8((uint8_t)((LETIMER0->CNT>>8)&&0xFF));
		//*****WANGJIANHUI*****���Դ���*****//

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
/************************************************************!
*Function: Power_Monitor
*Description: ��Դ��������������Ҫ�����Ƕ�ʱ����������Դ�ĵ�ѹ��
*             �����ص�ѹС����ֵ������PWM��磬����رճ�硣
*Input: null
*Output: null
*Return: null
*************************************************************/
void Power_Monitor(void *pvParameters)
{
	uint32_t samples[3];
	uint8_t erratashift;

	erratashift = Adc0_Errata(); //��ȡADC��У������

	while (1)
	{
		Wdog_Feed();

		//Timer1_Stop(); //������ѹǰ�ȹر�PWM����ֹ�����ĵ�ѹ���ݲ�׼ȷ

		vTaskDelay(pdMS_TO_TICKS(100)); //�ر�PWM����ʱ100MS������ѹ�ȶ�

		DMA_ActivateBasic(0,true,false,samples,(void *)((uint32_t)&(ADC0->SCANDATA)),3-1);

		ADC_Start(ADC0,adcStartScan);

		while(ADC0->STATUS&ADC_STATUS_SCANACT); //�ȴ�SCAN���

		if(erratashift) //ADC����ֵ�Ƿ���ҪУ��
		{
			for(uint8_t i=0;i<3;i++)
			{
				samples[i]<<=erratashift;
			}
		}

		gSYS_INFO.CAP_BAT=(3.31/4096)*samples[0]*6;
		gSYS_INFO.XDC_BAT=(3.31/4096)*samples[1]*6;
		gSYS_INFO.SUN_BAT=(3.31/4096)*samples[2]*6;

		//*****WANGJIANHUI*****���Դ���*****//
		//Usart0_TxU8(0xAA);
		//Usart0_TxU32(gSYS_INFO.CAP_BAT*100);
		//Usart0_TxU32(gSYS_INFO.XDC_BAT*100);
		//Usart0_TxU32(gSYS_INFO.SUN_BAT*100);
		//*****WANGJIANHUI*****���Դ���*****//

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
*Description: ���ڲɼ���Ԫ��10S�Ķ�ʱ���˺����㷨:Ϊ�˱�֤ϵͳ�жϺ���
*             �������У��������ٽ�������������жϡ�Ϊ�˷�ֹУʱ֡
*             ���ظ����͹����У�ϵͳʱ��Ŀ�����1S���жϴ�ϣ�����ʱ��
*             ���������з�������ÿ���ڷ����ظ�Уʱ֮֡ǰ���ж�CNT��
*             ��ֵ��
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

		vTaskSuspendAll(); //֮���Բ�ʹ���ٽ��������Ϊ�˱����жϣ���ʱ����ʹ��

		while(LETIMER0->CNT<20000); //ΪУʱ֡�ķ���Ԥ����20000/32768=610ms�ķ���ʱ��

		for(uint8_t i=0;i<10;i++) //�㲥Уʱ֡���˴�ֱ�ӷ�100�飬���ڸ���LDCʱ�������ÿһ֡���3.4MS������ϣ�100֡һ����ʱ340ms
	    {
			while(Radio_Flag==0); //�ȴ���һ֡Уʱ֡�������

			gSYS_TIME.T.CNT=LETIMER0->CNT;

			Set_P645_Frame(&RADIO_TX_PF,LOCAL_ADDR,BROADCAST_ADDR,0x42,0x01,1,1,gSYS_TIME.TIME,4); //0x01Уʱ֡

			Radio_StartTx(Rf_Channel,RADIO_TX_PF.PDUFRAME,26); //Уʱ֡�Ͳ�Ҫ����MID�ˣ�ֱ�ӷ��ͣ�����ʱ�����ʱ���

			Radio_Flag=0;

			//*****WANGJIANHUI*****���Դ���*****//
			//for(uint8_t m=0;m<26;m++)
				//Uart0_TxU8(RADIO_TX_PF.PDUFRAME[m]);
			//*****WANGJIANHUI*****���Դ���*****//
	    }

		xTaskResumeAll();

		vTaskDelay(pdMS_TO_TICKS(10000)); //10SУ��һ�βɼ���Ԫʱ��
	}
}
/************************************************************!
*Function: GPS_Timing
*Description: ����GPS�Ի㼯��Ԫ15���ӽ���һ��Уʱ���Ӹ�GPSģ���ϵ絽
*             Уʱ�ɹ�����40S��ʱ�䣬����ʱ��Уʱʧ����ر�GPSģ�飬
*             �ȴ���һ��Уʱ���ڼ���Уʱ��
*Input: null
*Output: null
*Return: null
*************************************************************/
void GPS_Timing(void *pvParameters)
{
	uint8_t time;

	volatile uint8_t GpsTiming=1; //GPSУʱ��־
	volatile uint8_t GpsOpen=1; //GPSģ���ϵ��־

	while (1)
	{
		Wdog_Feed();

		if(GpsTiming==1)
		{
			if(GpsOpen==1)
			{
				Gps_Power_On(); //GPS��ʼ���������ϵ�

				time=gSYS_TIME.T.SECOND; //����ʱ��ʼ

				GpsOpen=0;
			}

			if(Gps_Abstime_Process()) //������յ�GPS���ݣ���Уʱ�ɹ�
			{
				LETIMER0->CMD|=0x04; //��CNT

				GpsTiming=0;

				//*****WANGJIANHUI*****���Դ���*****//
				Leuart0_TxU8(0xFD);
				Leuart0_TxU8(gSYS_TIME.T.YEAR);
				Leuart0_TxU8(gSYS_TIME.T.MONTH);
				Leuart0_TxU8(gSYS_TIME.T.DAY);
				Leuart0_TxU8(gSYS_TIME.T.HOUR);
				Leuart0_TxU8(gSYS_TIME.T.MINUTE);
				Leuart0_TxU8(gSYS_TIME.T.SECOND);
				Leuart0_TxU8((uint8_t)(LETIMER0->CNT&&0xFF));
				Leuart0_TxU8((uint8_t)(LETIMER0->CNT>>8));
				//*****WANGJIANHUI*****���Դ���*****//
			}

	    	if(((gSYS_TIME.T.SECOND+60-time)%60)>40) //��ʱ40S��ȡ��Уʱ
	    	{
	    		GpsTiming=0;
	    	}
		}

		if(GpsTiming==0)
		{
			GpsTiming=1; //��ʱ15���Ӻ�ʼУʱ
			GpsOpen=1; //��ʱ15���Ӻ��GPSģ���ϵ�

			Gps_Power_Off();

			//vTaskDelay(pdMS_TO_TICKS(GPS_Interval*60000)); //15����У��һ�λ㼯��Ԫʱ��
		}
	}
}
/************************************************************!
*Function: P645_Comm
*Description: �ɼ���Ԫ645Э�鱨�Ĵ���ά���˿�645Э�鱨�Ĵ�������
*Input: null
*Output: null
*Return: null
*************************************************************/
void P645_Comm(void *pvParameters)
{
	while (1)
	{
		Wdog_Feed();

		P645_Parse_Process(&RADIO_RXCQ,&RADIO_RX_PF); //�ɼ���Ԫ��ά����Ƶͨ�Ŵ���
		P645_Handle_Process(&RADIO_RX_PF);
		Radio_Queue_Tx();

		P645_Parse_Process(&WEIHU_RXCQ,&USART0_RX_PF); //ά��232�˿�ͨ�Ŵ���
		P645_Handle_Process(&USART0_RX_PF);
		Usart0_Queue_Tx();

		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
/************************************************************!
*Function: P101_Comm
*Description: �����ն�101Э�鴦������
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
*Description: �����Ӻ���
*Input: null
*Output: null
*Return: null
*************************************************************/
void vApplicationIdleHook(void *pvParameters)
{

}

