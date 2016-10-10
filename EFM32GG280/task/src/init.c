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


SemaphoreHandle_t  Sem_DataRetrans; //数据重发标志信号量
/************************************************************!
*Function: BSP_Init
*Description: BSP初始化函数
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
	Usart0_Init(115200); //维护
	Leuart0_Init(9600); //GPS
	Timer1_Init(); //PWM
	Letimer0_Init(); //产生1S中断
	Adc0_Init();
	Dma_Init();
	Spi_Init(); //RADIO驱动
	Radio_Init();
	Radio_StartRx(Rf_Channel,0x40);
	MX25L256_FLASH_INIT();
	//MX25_CHIP_ERASE(); //片擦除时间大于30S，注意WDOG时间

	//*****WANGJIANHUI*****测试代码*****//
	Usart0_TxU8(0xFF); //软件复位标志
	Uart0_TxU8(0xFF); //软件复位标志
	Leuart0_TxU8(0xFF); //软件复位标志
	//*****WANGJIANHUI*****测试代码*****//
}
/************************************************************!
*Function: APP_Init
*Description: 系统应用层初始化函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void APP_Init(void)
{
	Para_Init(); //参数初始化

	CircleQueue_Init();

	Flash_Pointer_Init(); //

	Sem_DataRetrans = xSemaphoreCreateBinary();  //二值信号量，用于任务的同步

    Gpio_Int_Register(Radio_INT_Process); //射频中断注册函数
    Radio_Tx_Cb(Radio_StartTx);
	Letimer0_Int_Register(Sys_Tick); //系统时钟维护注册函数
	Usart0_Rx_Register(Usart0_Queue_Rx); //维护端口接收缓存注册
	Leuart0_Rx_Register(Leuart0_Queue_Rx); //GPS接收缓存注册

	//*****WANGJIANHUI*****测试代码*****//
	Usart0_TxU8(0xFE); //软件复位标志
	Uart0_TxU8(0xFE); //软件复位标志
	Leuart0_TxU8(0xFE); //软件复位标志
	//*****WANGJIANHUI*****测试代码*****//
}
/************************************************************!
*Function: Get_Sys_Time
*Description: 当还有小于10/32768=0.3ms产生1S中断的时候，等产生完中
*             断再拷贝时标，防止已经拷贝了CNT，还没有拷贝秒，分等时标，
*             产生1S中断，导致产生时标误差(会产生1S的误差)。
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
*Description: LETIMER0定时1S中断回调函数，系统时钟维护函数，维护系统
*             时钟的秒，分钟和小时，年月日时分秒由GPS维护。
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
