/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: init.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"
#include "init.h"


SemaphoreHandle_t  Sem_DataRetrans; //�����ط���־�ź���
/************************************************************!
*Function: BSP_Init
*Description: BSP��ʼ������
*Input: null
*Output: null
*Return: null
*************************************************************/
void BSP_Init(void)
{
	CHIP_Init();
	Cmu_Init();
	Gpio_Init();
	Wdog_Init();
	Uart0_Init(115200); //GPRS
	Usart0_Init(115200); //ά��
	Leuart0_Init(9600); //GPS
	Timer1_Init(); //PWM
	Letimer0_Init(); //����1S�ж�
	Adc0_Init();
	Dma_Init();
	Spi_Init(); //RADIO����
	Radio_Init();
	Radio_StartRx(Rf_Channel,0x40);
	MX25L256_FLASH_INIT();
	//MX25_CHIP_ERASE(); //Ƭ����ʱ�����30S��ע��WDOGʱ��

	//*****WANGJIANHUI*****���Դ���*****//
	Usart0_TxU8(0xFF); //�����λ��־
	Uart0_TxU8(0xFF); //�����λ��־
	Leuart0_TxU8(0xFF); //�����λ��־
	//*****WANGJIANHUI*****���Դ���*****//
}
/************************************************************!
*Function: APP_Init
*Description: ϵͳӦ�ò��ʼ������
*Input: null
*Output: null
*Return: null
*************************************************************/
void APP_Init(void)
{
	Para_Init(); //������ʼ��

	CircleQueue_Init();

	Flash_Pointer_Init(); //

	Sem_DataRetrans = xSemaphoreCreateBinary();  //��ֵ�ź��������������ͬ��

    Gpio_Int_Register(Radio_INT_Process); //��Ƶ�ж�ע�ắ��
    Radio_Tx_Cb(Radio_StartTx);
	Letimer0_Int_Register(Sys_Tick); //ϵͳʱ��ά��ע�ắ��
	Usart0_Rx_Register(Usart0_Queue_Rx); //ά���˿ڽ��ջ���ע��
	Leuart0_Rx_Register(Leuart0_Queue_Rx); //GPS���ջ���ע��

	//*****WANGJIANHUI*****���Դ���*****//
	Usart0_TxU8(0xFE); //�����λ��־
	Uart0_TxU8(0xFE); //�����λ��־
	Leuart0_TxU8(0xFE); //�����λ��־
	//*****WANGJIANHUI*****���Դ���*****//
}
/************************************************************!
*Function: Get_Sys_Time
*Description: ������С��10/32768=0.3ms����1S�жϵ�ʱ�򣬵Ȳ�������
*             ���ٿ���ʱ�꣬��ֹ�Ѿ�������CNT����û�п����룬�ֵ�ʱ�꣬
*             ����1S�жϣ����²���ʱ�����(�����1S�����)��
*Input: null
*Output: null
*Return: null
*************************************************************/
void Get_Sys_Time(void)
{
	while(LETIMER0->CNT<10);

	gSYS_TIME.T.CNT=LETIMER0->CNT;
}
/************************************************************!
*Function: Sys_Tick
*Description: LETIMER0��ʱ1S�жϻص�������ϵͳʱ��ά��������ά��ϵͳ
*             ʱ�ӵ��룬���Ӻ�Сʱ��������ʱ������GPSά����
*Input: null
*Output: null
*Return: null
*************************************************************/
void Sys_Tick(void)
{
	gSYS_TIME.T.SECOND++;

	if(gSYS_TIME.T.SECOND<60)
		return;

	gSYS_TIME.T.SECOND=0;
	gSYS_TIME.T.MINUTE++;

	if(gSYS_TIME.T.MINUTE<60)
		return;

	gSYS_TIME.T.MINUTE=0;
	gSYS_TIME.T.HOUR++;
}
/************************************************************!
*Function: SysTimer_10ms
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void SysTimer_10ms(TimerHandle_t xTimer)
{
	gSYS_INFO.TICKS_10MS++;
}
