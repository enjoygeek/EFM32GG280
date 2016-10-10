/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: p645parse.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"
#include "p645parse.h"


/**********RADIO发送函数回调注册变量**********/
RADIO_TX_CB  RadioTxCb=NULL;
/************************************************************!
*Function: P645_Parse_Process
*Description: 645协议解析函数，将645格式的数据帧从环形队列中解析出来
*             此函数的算法是：只要找到数据帧的帧头(前四个字节),就认为
*             这一数据帧的帧格式是合法的，就会继续判断CRC和ENDFIELD,
*             若这两者不正确，就会把这一数据帧丢掉。
*Input: 1.CQ：环形队列名
*       2.PF：需要解析的645结构体
*Output: null
*Return: 函数运行状态
*************************************************************/
uint8_t P645_Parse_Process(CircleQueue *CQ,PDUFRAME *PF)
{
	if(PF->PF.ISLOCK==true) //解析完一帧，并且此帧还未被处理，则不会继续下一帧解析
		return false;

	while(CQ->LENS>0)
	{
		if(PF->PF.PARSE==false) //还未开始解析数据帧
		{
			if(CQ->LENS<FRAMEHEAD) //等待队列接收完数据帧头(4字节)
				return false;

			ReadCircleQueue(CQ,PF->PDUFRAME,1); //读取！！！队列的第一个字节

			if(PF->PF.SF1!=STFD1)  //若队列的第一个字节不是0x68,继续
				continue;               //读取！！！下一个字节判断0x68,直到读取！！！到
			                            //的第一个字节是0x68为止
			VirtualReadCircleQueue(CQ,CQ->RPOINTER,PF->PDUFRAME+1,FRAMEHEAD-1);//虚拟！！！读取数据帧头

			if(!((PF->PF.SF1==PF->PF.SF2)&&(PF->PF.LEN1==PF->PF.LEN2)))
				continue;

			if((PF->PF.LEN1>=MIN_LEN)&&(PF->PF.LEN1<=MAX_LEN)) //判断长度域是否正确
			{
				PF->PF.PARSE=true; //找到数据帧头，开始解析数据
				ReadCircleQueue(CQ,PF->PDUFRAME+1,FRAMEHEAD-1); //若前四个字节是真正的数据帧头，读取！！！数据帧头
			}
		}

		if(PF->PF.PARSE==true)
		{
			if(CQ->LENS<(PF->PF.LEN1+2)) //等待整帧数据接收完
				return false;

			ReadCircleQueue(CQ,PF->PDUFRAME+FRAMEHEAD,PF->PF.LEN1); //读取长度域
			ReadCircleQueue(CQ,PF->PDUFRAME+CRC_INDEX,2); //读取校验码和结束符

			if(PF->PF.EF!=ENDFIELD)
			{
				PF->PF.PARSE=false;
					continue;
			}

			if(PF->PF.CS!=Check_Crc(PF->PDUFRAME,PF->PF.LEN1+FRAMEHEAD))
			{
				PF->PF.PARSE=false;
					continue;
			}

			PF->PF.PARSE=false; //数据解析完毕，复位
			PF->PF.ISLOCK=true; //数据帧解析完毕，将此帧上锁
		}
	}

	return false;
}
/************************************************************!
*Function: P645_Handle_Process
*Description: 645协议解析函数。此函数算法是:先判断目的地址是否为本机
*             地址，不是，则丢弃该数据帧，继续解析下一帧，是，则继续。
*             判断源地址是否为采集单元地址或维护地址，不是，则丢弃该
*             数据帧，继续解析下一帧，是，则继续判断主从者关系，进行
*             下一步处理。
*Input: 1.PF：需要解析的645结构体
*Output: null
*Return: 函数运行状态
*************************************************************/
uint8_t P645_Handle_Process(PDUFRAME *PF)
{
	uint8_t isScrAddr=0; //判断数据帧中的源地址是否合法标志

	if(PF->PF.ISLOCK==false) //未组成一个完整的数据帧，则不进行数据帧的处理
		return false;

	if(memcmp(LOCAL_ADDR,PF->PF.DSTADDR,ADDR_LEN)!=0)//确认目的地址是否为本机地址
	{
		PF->PF.ISLOCK=false; //此帧无效，继续解析下一帧
		return false;
	}

	for(uint8_t i=0;i<MAX_LINE_GROUP;i++)//判断源地址是否为汇集单元所带载的采集单元地址
	{
		if(gLINE_INFO.LINE[i].LINENO==0) //判断线号不为0,确认该线号为有效的采集单元组
			continue;

		for(uint8_t j=0;j<3;j++)
		{
			if(memcmp(gLINE_INFO.LINE[i].AU[j].ADDR,PF->PF.SRCADDR,ADDR_LEN)==0)
			{
				isScrAddr++;
				break;
			}
		}

		if(isScrAddr!=0)
			break;
	}

	if(memcmp(WEIHU_ADDR,PF->PF.SRCADDR,ADDR_LEN)==0)//确认是否为维护地址
	{
		isScrAddr++;
	}

	if(isScrAddr==0)//判断源地址是否正确
	{
		PF->PF.ISLOCK=false; //此帧无效，继续解析下一帧
		return false;
	}

	switch(PF->PF.CTRL.C.PRM) //判断主从者关系
	{
		case PRMSEC: //从动者
			App_Prmsec_Process(PF);
			break;

		case PRMPRI: //发起者
			App_Prmpri_Process(PF);
			break;

		default:
			break;
	}

	PF->PF.ISLOCK=false; //解析完此帧，继续解析下一帧

	return true;
}
/************************************************************!
*Function: App_Prmsec_Process
*Description: PRM=0，数据解析
*Input: 1.PF：需要解析的645结构体
*Output: null
*Return: null
*************************************************************/
void App_Prmsec_Process(PDUFRAME *PF) //从动者
{
	switch(PF->PF.CTRL.C.CC)
	{
		case FCCON: //确认,肯定确认
			App_Fccon_Process(PF);
			break;

		case FCNOCON: //确认,否定确认
			App_Fcnocon_Process(PF);
			break;

		case FCRESDATA: //响应帧,数据
			App_Fcresdata_Process(PF);
			break;

		case FCNODATA: //响应帧,否定认可，无数据
			App_Fcnodata_Process(PF);
			break;

		default:
			break;
	}
}
/************************************************************!
*Function: App_Prmpri_Process
*Description: PRM=1,数据解析
*Input: 1.PF：需要解析的645结构体
*Output: null
*Return: null
*************************************************************/
void App_Prmpri_Process(PDUFRAME *PF) //发起者
{
	switch(PF->PF.CTRL.C.CC)
	{
		case FCRESETUSE: //发送/无应答
			App_Fcresetuse_Process(PF);
			break;

		case FCRESETFAULT: //发送/确认
			App_Fcresetfault_Process(PF);
			break;

		case FCSENDDATANA: //发送/无应答
			App_Fcsenddatana_Process(PF);
			break;

		case FCSENDDATA: //发送/确认
			App_Fcsenddata_Process(PF);
			break;

		case FCREQUESTD: //请求/响应
			App_Fcrequestd_Process(PF);
			break;

		default:
			break;
	}
}
/************************************************************!
*Function: App_PduType_Process
*Description: 645数据类型解析函数
*Input: 1.PF：需要解析的645结构体
*Output: null
*Return: null
*************************************************************/
void App_PduType_Process(PDUFRAME *PF)
{
	switch (PF->PF.TYPE)
	{
		case TIMESTAMP: //时标
			App_Timestamp_Process(PF);
			break;

		case DBYC: //遥测
			App_Dbyc_Process(PF);
			break;

		case DBYXTIM: //SOE
			App_Dbyxtim_Process(PF);
			break;

		case DBWAVE: //录波数据
			App_Dbwave_Process(PF);
			break;

		case DBPARA: //参数
			App_Dbpara_Process(PF);
			break;

		case DBPROG: //程序
			App_Dbprog_Process(PF);
			break;

		case FAULTWRG: //启动录波
			App_Faultwrg_Process(PF);
			break;

		case LED: //点灯测试
			App_Led_Process(PF);
			break;

		case FANPAI: //测试翻牌
			break;

		case WAKEUP: //唤醒(对采集单元有效)
			break;

		case PHYID: //物理地址
			App_Phyid_Process(PF);
			break;

		default:
			break;
	}
}
/************************************************************!
*Function: App_Fccon_Process
*Description: PRM=0,确认,肯定确认ACK.采集单元不会给汇集单元发送此ACK
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Fccon_Process(PDUFRAME *PF)
{
	//此函数暂时未用到
}
/************************************************************!
*Function: App_Fcnocon_Process
*Description: PRM=0,确认,否定确认ACK.采集单元不会给汇集单元回复此ACK
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Fcnocon_Process(PDUFRAME *PF)
{
	//此函数暂时未用到
}
/************************************************************!
*Function: App_Fcresdata_Process
*Description: PRM=0,响应帧,处理数据.采集单元会给汇集单元回复录波数据
*             及时标
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Fcresdata_Process(PDUFRAME *PF)
{
	App_PduType_Process(PF); //处理数据

	xSemaphoreGive(Sem_DataRetrans); //给汇集单元同步信号进行下一步处理
}
/************************************************************!
*Function: App_Fcnodata_Process
*Description: PRM=0,响应帧,否定认可,无数据.采集单元不会给汇集单元
*             回复此帧
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Fcnodata_Process(PDUFRAME *PF)
{
	//此函数暂时未用到
}
/************************************************************!
*Function: App_Fcresetuse_Process
*Description: PRM=1,发送/无应答,复位进程,无数据域.采集单元不会给汇
*             集单元发送此ACK
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Fcresetuse_Process(PDUFRAME *PF)
{
	//此函数暂时未用到
}
/************************************************************!
*Function: App_Fcresetfault_Process
*Description: PRM=1,发送/确认,复位进程,无数据域.采集单元不会请求
*             汇集单元发送此ACK
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Fcresetfault_Process(PDUFRAME *PF)
{
	//此函数暂时未用到
}
/************************************************************!
*Function: App_Fcsenddatana_Process
*Description: PRM=1,发送/无应答,数据.用于接收采集单元YC,SOE数据
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Fcsenddatana_Process(PDUFRAME *PF)
{
	App_PduType_Process(PF);
}
/************************************************************!
*Function: App_Fcsenddata_Process
*Description: PRM=1,发送/确认,数据.采集单元不会主动请求汇集单元数据。
*             上位机会对汇集单元修改参数，上位机会给汇集单元更新程序。
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Fcsenddata_Process(PDUFRAME *PF)
{
	App_PduType_Process(PF);
}
/************************************************************!
*Function: App_Fcrequestd_Process
*Description: PRM=1,请求/响应.采集单元不会主动请求汇集单元数据。
*             上位机请求汇集单元的物理地址及请求汇集单元的参数下发此
*             命令。
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Fcrequestd_Process(PDUFRAME *PF)
{
	App_PduType_Process(PF); //处理数据
}
/************************************************************!
*Function: App_Timestamp_Process
*Description: 此函数用于接收采集单元时标，并与汇集单元进行时标比较
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Timestamp_Process(PDUFRAME *PF)
{
	uint8_t timestamp[4]; //暂存采集单元时标

	memcpy(timestamp,PF->PF.DATA,4); //拷贝时标

	//由于时标比较的结果作何处理暂时未定，此函数未实现

	//*****WANGJIANHUI*****测试代码*****//
	Uart0_TxU8(0xFC); //将采集单元的测试时标打印出来
	for(uint8_t i=0;i<4;i++)
		Uart0_TxU8(timestamp[i]);
	//*****WANGJIANHUI*****测试代码*****//

}
/************************************************************!
*Function: App_Faultwrg_Process
*Description: 此函数用于采集单元通知汇集单元启动录波。汇集单元需要将
*             采集单元的故障时间及录波数据帧数记录下来，等采集单元发
*             送遥信告知汇集单元需要开始录波时使用。汇集单元需要发送
*             启动录波命令。
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Faultwrg_Process(PDUFRAME *PF)
{
	uint8_t lineno,phase; //采集单元的线号及相号

	lineno=getLine_Info_Lineno(&gLINE_INFO,PF->PF.SRCADDR); //得到采集单元的线号
	phase=getLine_Info_Phase(&gLINE_INFO,PF->PF.SRCADDR); //得到采集单元的相号

	gFAULT_INFO.F.LINE=lineno;

	gFAULT_INFO.F.WAVE[phase].CNT=((uint16_t)PF->PF.DATA[0])<<8; //先发高八位
	gFAULT_INFO.F.WAVE[phase].CNT|=(uint16_t)PF->PF.DATA[1]; //后发低八位
	gFAULT_INFO.F.WAVE[phase].SECOND=PF->PF.DATA[2];
	gFAULT_INFO.F.WAVE[phase].MINUTE=PF->PF.DATA[3];

	gFAULT_INFO.F.WAVE[phase].HOUR=gSYS_TIME.T.HOUR;
	gFAULT_INFO.F.WAVE[phase].DAY=gSYS_TIME.T.DAY;
	gFAULT_INFO.F.WAVE[phase].MONTH=gSYS_TIME.T.MONTH;
	gFAULT_INFO.F.WAVE[phase].YEAR=gSYS_TIME.T.YEAR;

	gFAULT_INFO.F.WAVENUM=PF->PF.DATA[4]; //得到录波数据的帧数

	Set_P645_Frame(&RADIO_TX_PF,LOCAL_ADDR,BROADCAST_ADDR,0x42,0x07,1,1,PF->PF.DATA,4); //录波帧

	vTaskSuspendAll(); //之所以不使用临界代码区是为了保留系统中断

	for(uint8_t i=0;i<10;i++) //广播数据帧，此处直接发10遍，后期根据LDC时间调整
	{
		Radio_Direct_Tx(RADIO_TX_PF.PDUFRAME,FRAMEHEAD+MIN_LEN+MIN_PDU_LEN+4+2); //采集单元没有唤醒，所以不能放入MID，需要直接发送

		//*****WANGJIANHUI*****测试代码*****//
		for(uint8_t m=0;m<26;m++)
			Uart0_TxU8(RADIO_TX_PF.PDUFRAME[m]);
		//*****WANGJIANHUI*****测试代码*****//
	}

	xTaskResumeAll();
}
/************************************************************!
*Function: App_Dbyc_Process
*Description: 遥测信息存储，需要拷贝汇集单元的时标存储
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Dbyc_Process(PDUFRAME *PF)
{
	Get_Sys_Time(); //更新系统时间

	memcpy(&gYC_INFO.YCINFO,&gSYS_TIME.TIME,8); //拷贝系统时标

	memcpy(gYC_INFO.YCINFO+8,PF->PF.SRCADDR,6); //拷贝采集单元地址

	memcpy(gYC_INFO.YCINFO+14,PF->PF.DATA,8);

	Flash_Storage(YC,gYC_INFO.YCINFO,22); //遥测存储

	//*****WANGJIANHUI*****测试代码*****//
	Uart0_TxU8(0xFB); //将采集单元的遥测信息打印出来
	for(uint8_t i=0;i<22;i++)
		Uart0_TxU8(gYC_INFO.YCINFO[i]);
	//*****WANGJIANHUI*****测试代码*****//
}
/************************************************************!
*Function: App_Dbyxtim_Process
*Description: 遥信信息处理
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Dbyxtim_Process(PDUFRAME *PF)
{
	uint8_t line; //采集单元的线号
	uint8_t data[10]; //遥信信息缓存

	memcpy(data,PF->PF.DATA,4); //拷贝SOE时标,只拷贝分秒CNT
	memcpy(data+4,gSYS_TIME.TIME+4,4); //拷贝系统时标,只拷贝年月日时    //拷贝时标有问题，会被更新时间打断。。。。不一定，再想想

	memcpy(data+8,PF->PF.DATA+4,2); //拷贝SOE的类型和内容

	Set_INFO_Storage(SOE_INFO_ADDR,gFLASH_POINTER.pSOE,SOE_INFO_LEN,data);//遥测存储

	memcpy(gSOE_INFO.SOE,PF->PF.DATA,6); //拷贝SOE信息

	if(gSOE_INFO.S.TYPE!=DUANLU) //非短路故障，只是存储起来就行了,返回
		return;

	if(gSOE_INFO.S.FORMAT.F.BOXING==0) //没有波形，什么都不用干，返回
		return;

	line=getLine_Info_Line(&gLINE_INFO,PF->PF.SRCADDR); //得到采集单元的线号

	Set_P645_Frame(&RADIO_TX_PF,LOCAL_ADDR,BROADCAST_ADDR,0x40,0x0A,1,1,0,0); //唤醒帧

	vTaskSuspendAll(); //之所以不使用临界代码区是为了保留系统中断

	for(uint8_t i=0;i<10;i++) //广播唤醒帧，此处直接发10遍，后期根据LDC时间调整
	{
		Radio_Direct_Tx(RADIO_TX_PF.PDUFRAME,FRAMEHEAD+MIN_LEN+MIN_PDU_LEN+4+2); //采集单元没有唤醒，所以不能放入MID，需要直接发送

		for(uint8_t m=0;m<26;m++)
			Uart0_TxU8(RADIO_TX_PF.PDUFRAME[m]);
	}

	xTaskResumeAll();

	vTaskDelay(pdMS_TO_TICKS(10)); //延时10MS，等待采集单元唤醒

	for(uint8_t i=0;i<3;i++) //开始对于发生故障的一线召唤波形，相号轮询
	{
		for(uint8_t k=0;k<gFAULT_INFO.F.WAVENUM;k++)//数据帧帧数轮询
		{
			Set_P645_Frame(&RADIO_TX_PF,LOCAL_ADDR,gLINE_INFO.LINE[line].AU[i].ADDR,0x44,0x04,gFAULT_INFO.F.WAVENUM,k+1,0,0); //请求录波数据帧

			for(uint8_t j=0;j<5;j++) //每一帧的数据帧的重发次数
			{
				Radio_Direct_Tx(RADIO_TX_PF.PDUFRAME,22); //直接发送

				//*****WANGJIANHUI*****测试代码*****//
				for(uint8_t m=0;m<22;m++)
					Uart0_TxU8(RADIO_TX_PF.PDUFRAME[m]);

				Uart0_TxU8(0xFA);
				Uart0_TxU8(gSYS_TIME.T.MINUTE);
				Uart0_TxU8(gSYS_TIME.T.SECOND);
				Uart0_TxU8((uint8_t)(LETIMER0->CNT>>8));
				Uart0_TxU8((uint8_t)(LETIMER0->CNT&&0xFF));
				//*****WANGJIANHUI*****测试代码*****//

				if(xSemaphoreTake(Sem_DataRetrans,pdMS_TO_TICKS(2000))==true) //等待2S超时，累加重发次数
					continue;

				//*****WANGJIANHUI*****测试代码*****//
				Uart0_TxU8(0xF9);
				Uart0_TxU8(gSYS_TIME.T.MINUTE);
				Uart0_TxU8(gSYS_TIME.T.SECOND);
				Uart0_TxU8((uint8_t)(LETIMER0->CNT>>8));
				Uart0_TxU8((uint8_t)(LETIMER0->CNT&&0xFF));
				//*****WANGJIANHUI*****测试代码*****//

				//记录发送失败事件，失联的采集单元的线号及相号分别为line，i
				memset(gFAULT_INFO.FAULT,0,(3*(MAX_WAVE_LEN*2+4)+4)*sizeof(uint8_t)); //清空gFAULT_INFO
				return;
			}
		}
	}
}
/************************************************************!
*Function: App_Dbwave_Process
*Description:
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Dbwave_Process(PDUFRAME *PF)
{
	uint8_t datalens=0;
	uint8_t lineno,phase; //采集单元的线号及相号

	lineno=getLine_Info_Lineno(&gLINE_INFO,PF->PF.SRCADDR); //得到采集单元的线号
	phase=getLine_Info_Phase(&gLINE_INFO,PF->PF.SRCADDR); //得到采集单元的线号

	gFAULT_INFO.F.LINE=lineno;

	datalens=PF->PF.LEN1-MIN_LEN-MIN_PDU_LEN; //得到电压电流的数据长度

	if(PF->PF.FRAMESEQ==1) //当一相录波收到第一帧数据时
	{
		memcpy(&gFAULT_INFO.F.WAVE[phase].PHASE,PF->PF.SRCADDR,6); //拷贝采集单元地址

		gFAULT_INFO.F.POINTER=0; //清指针
	}

	for(uint8_t i=0;i<datalens;i+=2)
	{
		gFAULT_INFO.F.WAVE[phase].CUR[gFAULT_INFO.F.POINTER]=PF->PF.DATA[i+0];
		gFAULT_INFO.F.WAVE[phase].VOL[gFAULT_INFO.F.POINTER]=PF->PF.DATA[i+1];

		gFAULT_INFO.F.POINTER++;
	}

	if((PF->PF.FRAMESEQ==PF->PF.FRAMENUM)&&(phase==2)) //当ABC的最后一相C也录波完成时
	{
		if(gSOE_INFO.S.FORMAT.F.SHUNSHI==0 && gSOE_INFO.S.FORMAT.F.YONGJIU==0) //判断是否为接地故障
		{
			//故障波形判断算法，暂未定
		}
	}

	for(uint8_t m=0;m<3;m++) //故障波形存储
	{
		Set_INFO_Storage(VOL_INFO_ADDR,gFLASH_POINTER.pVOL,VOL_INFO_LEN,gFAULT_INFO.F.WAVE[m].VOL);
		Set_INFO_Storage(CUR_INFO_ADDR,gFLASH_POINTER.pCUR,CUR_INFO_LEN,gFAULT_INFO.F.WAVE[m].CUR);
	}

	memset(gFAULT_INFO.FAULT,0,(3*(MAX_WAVE_LEN*2+4)+4)*sizeof(uint8_t)); //清空gFAULT_INFO
}
/************************************************************!
*Function: App_Dbpara_Process
*Description: 参数读取及修改函数
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Dbpara_Process(PDUFRAME *PF)
{
	uint8_t data[120]; //读参数缓存

	if(PF->PF.CTRL.C.CC==FCSENDDATA) //修改参数
	{
		Set_PARA_Storage(PF->PF.DATA,PF->PF.LEN1-MIN_LEN-MIN_PDU_LEN);

		Refresh_Para(PF->PF.DATA,PF->PF.LEN1-MIN_LEN-MIN_PDU_LEN); //修改完参数需要更新参数

		Set_P645_Frame(&USART0_TX_PF,LOCAL_ADDR,PF->PF.SRCADDR,0x02,0,0,0,0,0); //ACK帧
		WriteCircleQueue(&WEIHU_TXCQ,USART0_TX_PF.PDUFRAME,19);//写入MID，等待发送

		Set_P645_Frame(&RADIO_TX_PF,LOCAL_ADDR,PF->PF.SRCADDR,0x02,0,0,0,0,0); //ACK帧
		WriteCircleQueue(&RADIO_TXCQ,RADIO_TX_PF.PDUFRAME,19);//写入MID，等待发送
		//Radio_Direct_Tx(RADIO_TX_PF.PDUFRAME,19); //直接发送
	}

	if(PF->PF.CTRL.C.CC==FCREQUESTD) //读参数
	{
		Get_PARA_Storage(data,120);

		Set_P645_Frame(&USART0_TX_PF,LOCAL_ADDR,PF->PF.SRCADDR,0x02,0x05,1,1,data,120); //参数帧
		WriteCircleQueue(&WEIHU_TXCQ,USART0_TX_PF.PDUFRAME,142);//写入MID，等待发送

		Set_P645_Frame(&RADIO_TX_PF,LOCAL_ADDR,PF->PF.SRCADDR,0x02,0x05,1,1,data,120); //参数帧
		WriteCircleQueue(&RADIO_TXCQ,RADIO_TX_PF.PDUFRAME,142);//写入MID，等待发送
		//Radio_Direct_Tx(RADIO_TX_PF.PDUFRAME,142); //直接发送
	}
}
/************************************************************!
*Function: App_Dbprog_Process
*Description: 编程函数
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Dbprog_Process(PDUFRAME *PF)
{
	uint8_t data;
	uint8_t key[4]={0x05,0xFA,0x05,0xFB};

	Set_PROGRAM_Storage(PF->PF.DATA,PF->PF.LEN1-MIN_LEN-MIN_PDU_LEN);

	if(PF->PF.FRAMESEQ==1) //当收到编程数据的第一帧
	{
		gFLASH_POINTER.pPROGRAM=PROGRAM_ADDR+4; //重新复位编程指针

		for(uint8_t i=0;i<PROGRAM_SECTOR_NUM;i++)
		{
			MX25_QUAD_BYTE_READ4B(PROGRAM_ADDR+i*SECTOR_LEN,&data,1);

			if(data!=0xFF) //该SECTOR有数据，则将该SECTOR擦除
				MX25_SECTOR_ERASE_4B((PROGRAM_ADDR+i*SECTOR_LEN)/SECTOR_LEN);
		}
	}

	if(PF->PF.FRAMESEQ==PF->PF.FRAMENUM) //当收到编程数据的最后一帧，写入编程密钥，软件复位
	{
		MX25_QUAD_BYTE_PP4B(PROGRAM_ADDR,key,4); //写入编程密钥

		SCB->AIRCR = 0x05FA0004;  //M3内核复位命令
	}
}
/************************************************************!
*Function: App_Led_Process
*Description: 汇集单元LED点灯测试
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Led_Process(PDUFRAME *PF)
{
	Led_Toggle();
}
/************************************************************!
*Function: App_Phyid_Process
*Description: 获取汇集单元物理地址
*Input: 1.PF：645结构体名
*Output: null
*Return: null
*************************************************************/
void App_Phyid_Process(PDUFRAME *PF)
{
	Set_P645_Frame(&RADIO_TX_PF,LOCAL_ADDR,PF->PF.SRCADDR,0x02,0x0b,1,1,LOCAL_ADDR,6); //参数帧
	WriteCircleQueue(&RADIO_TXCQ,RADIO_TX_PF.PDUFRAME,28);//写入MID，等待发送
	Radio_Direct_Tx(RADIO_TX_PF.PDUFRAME,28); //直接发送

	Set_P645_Frame(&USART0_TX_PF,LOCAL_ADDR,PF->PF.SRCADDR,0x02,0x0b,1,1,LOCAL_ADDR,6); //参数帧
	WriteCircleQueue(&WEIHU_TXCQ,USART0_TX_PF.PDUFRAME,28);//写入MID，等待发送
}
/************************************************************!
*Function: Set_P645_Frame
*Description: 645协议的组帧函数。现阶段有3种格式的数据帧(规约V1.0.6)
*Input: 1.PF：645结构体名
*       2.ScrAddr：源地址的首地址
*       3.DstAddr：目的地址的首地址
*       4.ctrl：控制域
*       5.pdutype：数据类型
*       6.framenum：帧数量
*       7.frameseq：帧序号
*       8.data：数据内容首地址
*       9.lens：数据内容长度
*Output: null
*Return: null
*************************************************************/
void Set_P645_Frame(PDUFRAME *PF,uint8_t *ScrAddr,uint8_t *DstAddr,uint8_t ctrl,uint8_t pdutype,uint8_t framenum,uint8_t frameseq,uint8_t *data,uint8_t lens)
{
	PF->PF.SF1=STFD1;
	PF->PF.SF2=STFD2;

	PF->PF.CTRL.CTRL=ctrl;

	memcpy(PF->PF.SRCADDR,ScrAddr,ADDR_LEN);
	memcpy(PF->PF.DSTADDR,DstAddr,ADDR_LEN);

	if(lens==0)
	{
		if(pdutype==0) //ACK帧
		{
			PF->PF.LEN1=MIN_LEN;
			PF->PF.LEN2=MIN_LEN;

			PF->PDUFRAME[FRAMEHEAD+MIN_LEN]=Check_Crc(PF->PDUFRAME,FRAMEHEAD+MIN_LEN);
			PF->PDUFRAME[FRAMEHEAD+MIN_LEN+1]=ENDFIELD;
		}
		else //非数据帧
		{
			PF->PF.LEN1=MIN_LEN+MIN_PDU_LEN;
			PF->PF.LEN2=MIN_LEN+MIN_PDU_LEN;

			PF->PF.TYPE=pdutype;
			PF->PF.FRAMENUM=framenum;
			PF->PF.FRAMESEQ=frameseq;

			PF->PDUFRAME[FRAMEHEAD+MIN_LEN+MIN_PDU_LEN]=Check_Crc(PF->PDUFRAME,FRAMEHEAD+MIN_LEN+MIN_PDU_LEN+lens);
			PF->PDUFRAME[FRAMEHEAD+MIN_LEN+MIN_PDU_LEN+1]=ENDFIELD;
		}
	}
	else //数据帧
	{
		PF->PF.LEN1=MIN_LEN+MIN_PDU_LEN+lens;
		PF->PF.LEN2=MIN_LEN+MIN_PDU_LEN+lens;

		PF->PF.TYPE=pdutype;
		PF->PF.FRAMENUM=framenum;
		PF->PF.FRAMESEQ=frameseq;

		memcpy(PF->PF.DATA,data,lens);

		PF->PDUFRAME[FRAMEHEAD+MIN_LEN+MIN_PDU_LEN+lens]=Check_Crc(PF->PDUFRAME,FRAMEHEAD+MIN_LEN+MIN_PDU_LEN+lens);
		PF->PDUFRAME[FRAMEHEAD+MIN_LEN+MIN_PDU_LEN+lens+1]=ENDFIELD;
	}
}
/************************************************************!
*Function: Set_P645_Frame1
*Description: 645协议的帧头部分组帧函数。
*Input: 1.PF：645结构体名
*       2.ScrAddr：源地址的首地址
*       3.DstAddr：目的地址的首地址
*       4.ctrl：控制域
*Output: null
*Return: null
*************************************************************/
void Set_P645_Frame1(PDUFRAME *PF,uint8_t *ScrAddr,uint8_t *DstAddr,uint8_t ctrl,uint8_t pdutype,uint8_t framenum,uint8_t frameseq,uint8_t *data,uint8_t lens)
{
	PF->PF.SF1=STFD1;

	PF->PF.LEN1=MIN_LEN+MIN_PDU_LEN+lens;
	PF->PF.LEN2=MIN_LEN+MIN_PDU_LEN+lens;

	PF->PF.SF2=STFD2;

	PF->PF.CTRL.CTRL=ctrl;

	memcpy(PF->PF.SRCADDR,ScrAddr,ADDR_LEN);
	memcpy(PF->PF.DSTADDR,DstAddr,ADDR_LEN);

	PF->PF.TYPE=pdutype;
	PF->PF.FRAMENUM=framenum;
	PF->PF.FRAMESEQ=frameseq;

	memcpy(PF->PF.DATA,data,lens);

	PF->PDUFRAME[FRAMEHEAD+MIN_LEN+MIN_PDU_LEN+lens]=Check_Crc(PF->PDUFRAME,FRAMEHEAD+MIN_LEN+lens);
	PF->PDUFRAME[FRAMEHEAD+MIN_LEN+MIN_PDU_LEN+lens+1]=ENDFIELD;
}
/************************************************************!
*Function: Radio_Tx_Func_Cb
*Description: MID的射频回调函数的注册函数
*Input: func：射频的发送函数指针
*Output: null
*Return: null
*************************************************************/
void Radio_Tx_Cb(RADIO_TX_CB func)
{
	RadioTxCb=func;
}
/************************************************************!
*Function: Radio_Mid_Tx
*Description: MID层的射频发送数据的回调函数
*Input: 1.channel：射频通道
*       2.data：数组的首地址
*       3.lens：数组长度
*Output: null
*Return: null
*************************************************************/
void Radio_Mid_Tx(uint8_t channel,uint8_t *data,uint8_t lens)
{
	if(RadioTxCb!=NULL)
    {
		(*RadioTxCb)(channel,data,lens);
    }
}
/************************************************************!
*Function: Check_Crc
*Description: 检查数组的CRC值。
*Input: 1.data：数组首地址。
*       2.lens：数组长度。
*Output: null
*Return: CRC值。
*************************************************************/
uint8_t Check_Crc(uint8_t *data,uint16_t lens)
{
	uint8_t crc=0;

	for(uint16_t i=0;i<lens;i++)
		crc+=*(data++);

	return crc;
}

