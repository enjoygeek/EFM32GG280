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


/**********RADIO���ͺ����ص�ע�����**********/
RADIO_TX_CB  RadioTxCb=NULL;
/************************************************************!
*Function: P645_Parse_Process
*Description: 645Э�������������645��ʽ������֡�ӻ��ζ����н�������
*             �˺������㷨�ǣ�ֻҪ�ҵ�����֡��֡ͷ(ǰ�ĸ��ֽ�),����Ϊ
*             ��һ����֡��֡��ʽ�ǺϷ��ģ��ͻ�����ж�CRC��ENDFIELD,
*             �������߲���ȷ���ͻ����һ����֡������
*Input: 1.CQ�����ζ�����
*       2.PF����Ҫ������645�ṹ��
*Output: null
*Return: ��������״̬
*************************************************************/
uint8_t P645_Parse_Process(CircleQueue *CQ,PDUFRAME *PF)
{
	if(PF->PF.ISLOCK==true) //������һ֡�����Ҵ�֡��δ�������򲻻������һ֡����
		return false;

	while(CQ->LENS>0)
	{
		if(PF->PF.PARSE==false) //��δ��ʼ��������֡
		{
			if(CQ->LENS<FRAMEHEAD) //�ȴ����н���������֡ͷ(4�ֽ�)
				return false;

			ReadCircleQueue(CQ,PF->PDUFRAME,1); //��ȡ���������еĵ�һ���ֽ�

			if(PF->PF.SF1!=STFD1)  //�����еĵ�һ���ֽڲ���0x68,����
				continue;               //��ȡ��������һ���ֽ��ж�0x68,ֱ����ȡ��������
			                            //�ĵ�һ���ֽ���0x68Ϊֹ
			VirtualReadCircleQueue(CQ,CQ->RPOINTER,PF->PDUFRAME+1,FRAMEHEAD-1);//���⣡������ȡ����֡ͷ

			if(!((PF->PF.SF1==PF->PF.SF2)&&(PF->PF.LEN1==PF->PF.LEN2)))
				continue;

			if((PF->PF.LEN1>=MIN_LEN)&&(PF->PF.LEN1<=MAX_LEN)) //�жϳ������Ƿ���ȷ
			{
				PF->PF.PARSE=true; //�ҵ�����֡ͷ����ʼ��������
				ReadCircleQueue(CQ,PF->PDUFRAME+1,FRAMEHEAD-1); //��ǰ�ĸ��ֽ�������������֡ͷ����ȡ����������֡ͷ
			}
		}

		if(PF->PF.PARSE==true)
		{
			if(CQ->LENS<(PF->PF.LEN1+2)) //�ȴ���֡���ݽ�����
				return false;

			ReadCircleQueue(CQ,PF->PDUFRAME+FRAMEHEAD,PF->PF.LEN1); //��ȡ������
			ReadCircleQueue(CQ,PF->PDUFRAME+CRC_INDEX,2); //��ȡУ����ͽ�����

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

			PF->PF.PARSE=false; //���ݽ�����ϣ���λ
			PF->PF.ISLOCK=true; //����֡������ϣ�����֡����
		}
	}

	return false;
}
/************************************************************!
*Function: P645_Handle_Process
*Description: 645Э������������˺����㷨��:���ж�Ŀ�ĵ�ַ�Ƿ�Ϊ����
*             ��ַ�����ǣ�����������֡������������һ֡���ǣ��������
*             �ж�Դ��ַ�Ƿ�Ϊ�ɼ���Ԫ��ַ��ά����ַ�����ǣ�������
*             ����֡������������һ֡���ǣ�������ж������߹�ϵ������
*             ��һ������
*Input: 1.PF����Ҫ������645�ṹ��
*Output: null
*Return: ��������״̬
*************************************************************/
uint8_t P645_Handle_Process(PDUFRAME *PF)
{
	uint8_t isScrAddr=0; //�ж�����֡�е�Դ��ַ�Ƿ�Ϸ���־

	if(PF->PF.ISLOCK==false) //δ���һ������������֡���򲻽�������֡�Ĵ���
		return false;

	if(memcmp(LOCAL_ADDR,PF->PF.DSTADDR,ADDR_LEN)!=0)//ȷ��Ŀ�ĵ�ַ�Ƿ�Ϊ������ַ
	{
		PF->PF.ISLOCK=false; //��֡��Ч������������һ֡
		return false;
	}

	for(uint8_t i=0;i<MAX_LINE_GROUP;i++)//�ж�Դ��ַ�Ƿ�Ϊ�㼯��Ԫ�����صĲɼ���Ԫ��ַ
	{
		if(gLINE_INFO.LINE[i].LINENO==0) //�ж��ߺŲ�Ϊ0,ȷ�ϸ��ߺ�Ϊ��Ч�Ĳɼ���Ԫ��
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

	if(memcmp(WEIHU_ADDR,PF->PF.SRCADDR,ADDR_LEN)==0)//ȷ���Ƿ�Ϊά����ַ
	{
		isScrAddr++;
	}

	if(isScrAddr==0)//�ж�Դ��ַ�Ƿ���ȷ
	{
		PF->PF.ISLOCK=false; //��֡��Ч������������һ֡
		return false;
	}

	switch(PF->PF.CTRL.C.PRM) //�ж������߹�ϵ
	{
		case PRMSEC: //�Ӷ���
			App_Prmsec_Process(PF);
			break;

		case PRMPRI: //������
			App_Prmpri_Process(PF);
			break;

		default:
			break;
	}

	PF->PF.ISLOCK=false; //�������֡������������һ֡

	return true;
}
/************************************************************!
*Function: App_Prmsec_Process
*Description: PRM=0�����ݽ���
*Input: 1.PF����Ҫ������645�ṹ��
*Output: null
*Return: null
*************************************************************/
void App_Prmsec_Process(PDUFRAME *PF) //�Ӷ���
{
	switch(PF->PF.CTRL.C.CC)
	{
		case FCCON: //ȷ��,�϶�ȷ��
			App_Fccon_Process(PF);
			break;

		case FCNOCON: //ȷ��,��ȷ��
			App_Fcnocon_Process(PF);
			break;

		case FCRESDATA: //��Ӧ֡,����
			App_Fcresdata_Process(PF);
			break;

		case FCNODATA: //��Ӧ֡,���Ͽɣ�������
			App_Fcnodata_Process(PF);
			break;

		default:
			break;
	}
}
/************************************************************!
*Function: App_Prmpri_Process
*Description: PRM=1,���ݽ���
*Input: 1.PF����Ҫ������645�ṹ��
*Output: null
*Return: null
*************************************************************/
void App_Prmpri_Process(PDUFRAME *PF) //������
{
	switch(PF->PF.CTRL.C.CC)
	{
		case FCRESETUSE: //����/��Ӧ��
			App_Fcresetuse_Process(PF);
			break;

		case FCRESETFAULT: //����/ȷ��
			App_Fcresetfault_Process(PF);
			break;

		case FCSENDDATANA: //����/��Ӧ��
			App_Fcsenddatana_Process(PF);
			break;

		case FCSENDDATA: //����/ȷ��
			App_Fcsenddata_Process(PF);
			break;

		case FCREQUESTD: //����/��Ӧ
			App_Fcrequestd_Process(PF);
			break;

		default:
			break;
	}
}
/************************************************************!
*Function: App_PduType_Process
*Description: 645�������ͽ�������
*Input: 1.PF����Ҫ������645�ṹ��
*Output: null
*Return: null
*************************************************************/
void App_PduType_Process(PDUFRAME *PF)
{
	switch (PF->PF.TYPE)
	{
		case TIMESTAMP: //ʱ��
			App_Timestamp_Process(PF);
			break;

		case DBYC: //ң��
			App_Dbyc_Process(PF);
			break;

		case DBYXTIM: //SOE
			App_Dbyxtim_Process(PF);
			break;

		case DBWAVE: //¼������
			App_Dbwave_Process(PF);
			break;

		case DBPARA: //����
			App_Dbpara_Process(PF);
			break;

		case DBPROG: //����
			App_Dbprog_Process(PF);
			break;

		case FAULTWRG: //����¼��
			App_Faultwrg_Process(PF);
			break;

		case LED: //��Ʋ���
			App_Led_Process(PF);
			break;

		case FANPAI: //���Է���
			break;

		case WAKEUP: //����(�Բɼ���Ԫ��Ч)
			break;

		case PHYID: //�����ַ
			App_Phyid_Process(PF);
			break;

		default:
			break;
	}
}
/************************************************************!
*Function: App_Fccon_Process
*Description: PRM=0,ȷ��,�϶�ȷ��ACK.�ɼ���Ԫ������㼯��Ԫ���ʹ�ACK
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Fccon_Process(PDUFRAME *PF)
{
	//�˺�����ʱδ�õ�
}
/************************************************************!
*Function: App_Fcnocon_Process
*Description: PRM=0,ȷ��,��ȷ��ACK.�ɼ���Ԫ������㼯��Ԫ�ظ���ACK
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Fcnocon_Process(PDUFRAME *PF)
{
	//�˺�����ʱδ�õ�
}
/************************************************************!
*Function: App_Fcresdata_Process
*Description: PRM=0,��Ӧ֡,��������.�ɼ���Ԫ����㼯��Ԫ�ظ�¼������
*             ��ʱ��
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Fcresdata_Process(PDUFRAME *PF)
{
	App_PduType_Process(PF); //��������

	xSemaphoreGive(Sem_DataRetrans); //���㼯��Ԫͬ���źŽ�����һ������
}
/************************************************************!
*Function: App_Fcnodata_Process
*Description: PRM=0,��Ӧ֡,���Ͽ�,������.�ɼ���Ԫ������㼯��Ԫ
*             �ظ���֡
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Fcnodata_Process(PDUFRAME *PF)
{
	//�˺�����ʱδ�õ�
}
/************************************************************!
*Function: App_Fcresetuse_Process
*Description: PRM=1,����/��Ӧ��,��λ����,��������.�ɼ���Ԫ�������
*             ����Ԫ���ʹ�ACK
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Fcresetuse_Process(PDUFRAME *PF)
{
	//�˺�����ʱδ�õ�
}
/************************************************************!
*Function: App_Fcresetfault_Process
*Description: PRM=1,����/ȷ��,��λ����,��������.�ɼ���Ԫ��������
*             �㼯��Ԫ���ʹ�ACK
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Fcresetfault_Process(PDUFRAME *PF)
{
	//�˺�����ʱδ�õ�
}
/************************************************************!
*Function: App_Fcsenddatana_Process
*Description: PRM=1,����/��Ӧ��,����.���ڽ��ղɼ���ԪYC,SOE����
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Fcsenddatana_Process(PDUFRAME *PF)
{
	App_PduType_Process(PF);
}
/************************************************************!
*Function: App_Fcsenddata_Process
*Description: PRM=1,����/ȷ��,����.�ɼ���Ԫ������������㼯��Ԫ���ݡ�
*             ��λ����Ի㼯��Ԫ�޸Ĳ�������λ������㼯��Ԫ���³���
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Fcsenddata_Process(PDUFRAME *PF)
{
	App_PduType_Process(PF);
}
/************************************************************!
*Function: App_Fcrequestd_Process
*Description: PRM=1,����/��Ӧ.�ɼ���Ԫ������������㼯��Ԫ���ݡ�
*             ��λ������㼯��Ԫ�������ַ������㼯��Ԫ�Ĳ����·���
*             ���
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Fcrequestd_Process(PDUFRAME *PF)
{
	App_PduType_Process(PF); //��������
}
/************************************************************!
*Function: App_Timestamp_Process
*Description: �˺������ڽ��ղɼ���Ԫʱ�꣬����㼯��Ԫ����ʱ��Ƚ�
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Timestamp_Process(PDUFRAME *PF)
{
	uint8_t timestamp[4]; //�ݴ�ɼ���Ԫʱ��

	memcpy(timestamp,PF->PF.DATA,4); //����ʱ��

	//����ʱ��ȽϵĽ�����δ�����ʱδ�����˺���δʵ��

	//*****WANGJIANHUI*****���Դ���*****//
	Uart0_TxU8(0xFC); //���ɼ���Ԫ�Ĳ���ʱ���ӡ����
	for(uint8_t i=0;i<4;i++)
		Uart0_TxU8(timestamp[i]);
	//*****WANGJIANHUI*****���Դ���*****//

}
/************************************************************!
*Function: App_Faultwrg_Process
*Description: �˺������ڲɼ���Ԫ֪ͨ�㼯��Ԫ����¼�����㼯��Ԫ��Ҫ��
*             �ɼ���Ԫ�Ĺ���ʱ�估¼������֡����¼�������Ȳɼ���Ԫ��
*             ��ң�Ÿ�֪�㼯��Ԫ��Ҫ��ʼ¼��ʱʹ�á��㼯��Ԫ��Ҫ����
*             ����¼�����
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Faultwrg_Process(PDUFRAME *PF)
{
	uint8_t lineno,phase; //�ɼ���Ԫ���ߺż����

	lineno=getLine_Info_Lineno(&gLINE_INFO,PF->PF.SRCADDR); //�õ��ɼ���Ԫ���ߺ�
	phase=getLine_Info_Phase(&gLINE_INFO,PF->PF.SRCADDR); //�õ��ɼ���Ԫ�����

	gFAULT_INFO.F.LINE=lineno;

	gFAULT_INFO.F.WAVE[phase].CNT=((uint16_t)PF->PF.DATA[0])<<8; //�ȷ��߰�λ
	gFAULT_INFO.F.WAVE[phase].CNT|=(uint16_t)PF->PF.DATA[1]; //�󷢵Ͱ�λ
	gFAULT_INFO.F.WAVE[phase].SECOND=PF->PF.DATA[2];
	gFAULT_INFO.F.WAVE[phase].MINUTE=PF->PF.DATA[3];

	gFAULT_INFO.F.WAVE[phase].HOUR=gSYS_TIME.T.HOUR;
	gFAULT_INFO.F.WAVE[phase].DAY=gSYS_TIME.T.DAY;
	gFAULT_INFO.F.WAVE[phase].MONTH=gSYS_TIME.T.MONTH;
	gFAULT_INFO.F.WAVE[phase].YEAR=gSYS_TIME.T.YEAR;

	gFAULT_INFO.F.WAVENUM=PF->PF.DATA[4]; //�õ�¼�����ݵ�֡��

	Set_P645_Frame(&RADIO_TX_PF,LOCAL_ADDR,BROADCAST_ADDR,0x42,0x07,1,1,PF->PF.DATA,4); //¼��֡

	vTaskSuspendAll(); //֮���Բ�ʹ���ٽ��������Ϊ�˱���ϵͳ�ж�

	for(uint8_t i=0;i<10;i++) //�㲥����֡���˴�ֱ�ӷ�10�飬���ڸ���LDCʱ�����
	{
		Radio_Direct_Tx(RADIO_TX_PF.PDUFRAME,FRAMEHEAD+MIN_LEN+MIN_PDU_LEN+4+2); //�ɼ���Ԫû�л��ѣ����Բ��ܷ���MID����Ҫֱ�ӷ���

		//*****WANGJIANHUI*****���Դ���*****//
		for(uint8_t m=0;m<26;m++)
			Uart0_TxU8(RADIO_TX_PF.PDUFRAME[m]);
		//*****WANGJIANHUI*****���Դ���*****//
	}

	xTaskResumeAll();
}
/************************************************************!
*Function: App_Dbyc_Process
*Description: ң����Ϣ�洢����Ҫ�����㼯��Ԫ��ʱ��洢
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Dbyc_Process(PDUFRAME *PF)
{
	Get_Sys_Time(); //����ϵͳʱ��

	memcpy(&gYC_INFO.YCINFO,&gSYS_TIME.TIME,8); //����ϵͳʱ��

	memcpy(gYC_INFO.YCINFO+8,PF->PF.SRCADDR,6); //�����ɼ���Ԫ��ַ

	memcpy(gYC_INFO.YCINFO+14,PF->PF.DATA,8);

	Flash_Storage(YC,gYC_INFO.YCINFO,22); //ң��洢

	//*****WANGJIANHUI*****���Դ���*****//
	Uart0_TxU8(0xFB); //���ɼ���Ԫ��ң����Ϣ��ӡ����
	for(uint8_t i=0;i<22;i++)
		Uart0_TxU8(gYC_INFO.YCINFO[i]);
	//*****WANGJIANHUI*****���Դ���*****//
}
/************************************************************!
*Function: App_Dbyxtim_Process
*Description: ң����Ϣ����
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Dbyxtim_Process(PDUFRAME *PF)
{
	uint8_t line; //�ɼ���Ԫ���ߺ�
	uint8_t data[10]; //ң����Ϣ����

	memcpy(data,PF->PF.DATA,4); //����SOEʱ��,ֻ��������CNT
	memcpy(data+4,gSYS_TIME.TIME+4,4); //����ϵͳʱ��,ֻ����������ʱ    //����ʱ�������⣬�ᱻ����ʱ���ϡ���������һ����������

	memcpy(data+8,PF->PF.DATA+4,2); //����SOE�����ͺ�����

	Set_INFO_Storage(SOE_INFO_ADDR,gFLASH_POINTER.pSOE,SOE_INFO_LEN,data);//ң��洢

	memcpy(gSOE_INFO.SOE,PF->PF.DATA,6); //����SOE��Ϣ

	if(gSOE_INFO.S.TYPE!=DUANLU) //�Ƕ�·���ϣ�ֻ�Ǵ洢����������,����
		return;

	if(gSOE_INFO.S.FORMAT.F.BOXING==0) //û�в��Σ�ʲô�����øɣ�����
		return;

	line=getLine_Info_Line(&gLINE_INFO,PF->PF.SRCADDR); //�õ��ɼ���Ԫ���ߺ�

	Set_P645_Frame(&RADIO_TX_PF,LOCAL_ADDR,BROADCAST_ADDR,0x40,0x0A,1,1,0,0); //����֡

	vTaskSuspendAll(); //֮���Բ�ʹ���ٽ��������Ϊ�˱���ϵͳ�ж�

	for(uint8_t i=0;i<10;i++) //�㲥����֡���˴�ֱ�ӷ�10�飬���ڸ���LDCʱ�����
	{
		Radio_Direct_Tx(RADIO_TX_PF.PDUFRAME,FRAMEHEAD+MIN_LEN+MIN_PDU_LEN+4+2); //�ɼ���Ԫû�л��ѣ����Բ��ܷ���MID����Ҫֱ�ӷ���

		for(uint8_t m=0;m<26;m++)
			Uart0_TxU8(RADIO_TX_PF.PDUFRAME[m]);
	}

	xTaskResumeAll();

	vTaskDelay(pdMS_TO_TICKS(10)); //��ʱ10MS���ȴ��ɼ���Ԫ����

	for(uint8_t i=0;i<3;i++) //��ʼ���ڷ������ϵ�һ���ٻ����Σ������ѯ
	{
		for(uint8_t k=0;k<gFAULT_INFO.F.WAVENUM;k++)//����֡֡����ѯ
		{
			Set_P645_Frame(&RADIO_TX_PF,LOCAL_ADDR,gLINE_INFO.LINE[line].AU[i].ADDR,0x44,0x04,gFAULT_INFO.F.WAVENUM,k+1,0,0); //����¼������֡

			for(uint8_t j=0;j<5;j++) //ÿһ֡������֡���ط�����
			{
				Radio_Direct_Tx(RADIO_TX_PF.PDUFRAME,22); //ֱ�ӷ���

				//*****WANGJIANHUI*****���Դ���*****//
				for(uint8_t m=0;m<22;m++)
					Uart0_TxU8(RADIO_TX_PF.PDUFRAME[m]);

				Uart0_TxU8(0xFA);
				Uart0_TxU8(gSYS_TIME.T.MINUTE);
				Uart0_TxU8(gSYS_TIME.T.SECOND);
				Uart0_TxU8((uint8_t)(LETIMER0->CNT>>8));
				Uart0_TxU8((uint8_t)(LETIMER0->CNT&&0xFF));
				//*****WANGJIANHUI*****���Դ���*****//

				if(xSemaphoreTake(Sem_DataRetrans,pdMS_TO_TICKS(2000))==true) //�ȴ�2S��ʱ���ۼ��ط�����
					continue;

				//*****WANGJIANHUI*****���Դ���*****//
				Uart0_TxU8(0xF9);
				Uart0_TxU8(gSYS_TIME.T.MINUTE);
				Uart0_TxU8(gSYS_TIME.T.SECOND);
				Uart0_TxU8((uint8_t)(LETIMER0->CNT>>8));
				Uart0_TxU8((uint8_t)(LETIMER0->CNT&&0xFF));
				//*****WANGJIANHUI*****���Դ���*****//

				//��¼����ʧ���¼���ʧ���Ĳɼ���Ԫ���ߺż���ŷֱ�Ϊline��i
				memset(gFAULT_INFO.FAULT,0,(3*(MAX_WAVE_LEN*2+4)+4)*sizeof(uint8_t)); //���gFAULT_INFO
				return;
			}
		}
	}
}
/************************************************************!
*Function: App_Dbwave_Process
*Description:
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Dbwave_Process(PDUFRAME *PF)
{
	uint8_t datalens=0;
	uint8_t lineno,phase; //�ɼ���Ԫ���ߺż����

	lineno=getLine_Info_Lineno(&gLINE_INFO,PF->PF.SRCADDR); //�õ��ɼ���Ԫ���ߺ�
	phase=getLine_Info_Phase(&gLINE_INFO,PF->PF.SRCADDR); //�õ��ɼ���Ԫ���ߺ�

	gFAULT_INFO.F.LINE=lineno;

	datalens=PF->PF.LEN1-MIN_LEN-MIN_PDU_LEN; //�õ���ѹ���������ݳ���

	if(PF->PF.FRAMESEQ==1) //��һ��¼���յ���һ֡����ʱ
	{
		memcpy(&gFAULT_INFO.F.WAVE[phase].PHASE,PF->PF.SRCADDR,6); //�����ɼ���Ԫ��ַ

		gFAULT_INFO.F.POINTER=0; //��ָ��
	}

	for(uint8_t i=0;i<datalens;i+=2)
	{
		gFAULT_INFO.F.WAVE[phase].CUR[gFAULT_INFO.F.POINTER]=PF->PF.DATA[i+0];
		gFAULT_INFO.F.WAVE[phase].VOL[gFAULT_INFO.F.POINTER]=PF->PF.DATA[i+1];

		gFAULT_INFO.F.POINTER++;
	}

	if((PF->PF.FRAMESEQ==PF->PF.FRAMENUM)&&(phase==2)) //��ABC�����һ��CҲ¼�����ʱ
	{
		if(gSOE_INFO.S.FORMAT.F.SHUNSHI==0 && gSOE_INFO.S.FORMAT.F.YONGJIU==0) //�ж��Ƿ�Ϊ�ӵع���
		{
			//���ϲ����ж��㷨����δ��
		}
	}

	for(uint8_t m=0;m<3;m++) //���ϲ��δ洢
	{
		Set_INFO_Storage(VOL_INFO_ADDR,gFLASH_POINTER.pVOL,VOL_INFO_LEN,gFAULT_INFO.F.WAVE[m].VOL);
		Set_INFO_Storage(CUR_INFO_ADDR,gFLASH_POINTER.pCUR,CUR_INFO_LEN,gFAULT_INFO.F.WAVE[m].CUR);
	}

	memset(gFAULT_INFO.FAULT,0,(3*(MAX_WAVE_LEN*2+4)+4)*sizeof(uint8_t)); //���gFAULT_INFO
}
/************************************************************!
*Function: App_Dbpara_Process
*Description: ������ȡ���޸ĺ���
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Dbpara_Process(PDUFRAME *PF)
{
	uint8_t data[120]; //����������

	if(PF->PF.CTRL.C.CC==FCSENDDATA) //�޸Ĳ���
	{
		Set_PARA_Storage(PF->PF.DATA,PF->PF.LEN1-MIN_LEN-MIN_PDU_LEN);

		Refresh_Para(PF->PF.DATA,PF->PF.LEN1-MIN_LEN-MIN_PDU_LEN); //�޸��������Ҫ���²���

		Set_P645_Frame(&USART0_TX_PF,LOCAL_ADDR,PF->PF.SRCADDR,0x02,0,0,0,0,0); //ACK֡
		WriteCircleQueue(&WEIHU_TXCQ,USART0_TX_PF.PDUFRAME,19);//д��MID���ȴ�����

		Set_P645_Frame(&RADIO_TX_PF,LOCAL_ADDR,PF->PF.SRCADDR,0x02,0,0,0,0,0); //ACK֡
		WriteCircleQueue(&RADIO_TXCQ,RADIO_TX_PF.PDUFRAME,19);//д��MID���ȴ�����
		//Radio_Direct_Tx(RADIO_TX_PF.PDUFRAME,19); //ֱ�ӷ���
	}

	if(PF->PF.CTRL.C.CC==FCREQUESTD) //������
	{
		Get_PARA_Storage(data,120);

		Set_P645_Frame(&USART0_TX_PF,LOCAL_ADDR,PF->PF.SRCADDR,0x02,0x05,1,1,data,120); //����֡
		WriteCircleQueue(&WEIHU_TXCQ,USART0_TX_PF.PDUFRAME,142);//д��MID���ȴ�����

		Set_P645_Frame(&RADIO_TX_PF,LOCAL_ADDR,PF->PF.SRCADDR,0x02,0x05,1,1,data,120); //����֡
		WriteCircleQueue(&RADIO_TXCQ,RADIO_TX_PF.PDUFRAME,142);//д��MID���ȴ�����
		//Radio_Direct_Tx(RADIO_TX_PF.PDUFRAME,142); //ֱ�ӷ���
	}
}
/************************************************************!
*Function: App_Dbprog_Process
*Description: ��̺���
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Dbprog_Process(PDUFRAME *PF)
{
	uint8_t data;
	uint8_t key[4]={0x05,0xFA,0x05,0xFB};

	Set_PROGRAM_Storage(PF->PF.DATA,PF->PF.LEN1-MIN_LEN-MIN_PDU_LEN);

	if(PF->PF.FRAMESEQ==1) //���յ�������ݵĵ�һ֡
	{
		gFLASH_POINTER.pPROGRAM=PROGRAM_ADDR+4; //���¸�λ���ָ��

		for(uint8_t i=0;i<PROGRAM_SECTOR_NUM;i++)
		{
			MX25_QUAD_BYTE_READ4B(PROGRAM_ADDR+i*SECTOR_LEN,&data,1);

			if(data!=0xFF) //��SECTOR�����ݣ��򽫸�SECTOR����
				MX25_SECTOR_ERASE_4B((PROGRAM_ADDR+i*SECTOR_LEN)/SECTOR_LEN);
		}
	}

	if(PF->PF.FRAMESEQ==PF->PF.FRAMENUM) //���յ�������ݵ����һ֡��д������Կ�������λ
	{
		MX25_QUAD_BYTE_PP4B(PROGRAM_ADDR,key,4); //д������Կ

		SCB->AIRCR = 0x05FA0004;  //M3�ں˸�λ����
	}
}
/************************************************************!
*Function: App_Led_Process
*Description: �㼯��ԪLED��Ʋ���
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Led_Process(PDUFRAME *PF)
{
	Led_Toggle();
}
/************************************************************!
*Function: App_Phyid_Process
*Description: ��ȡ�㼯��Ԫ�����ַ
*Input: 1.PF��645�ṹ����
*Output: null
*Return: null
*************************************************************/
void App_Phyid_Process(PDUFRAME *PF)
{
	Set_P645_Frame(&RADIO_TX_PF,LOCAL_ADDR,PF->PF.SRCADDR,0x02,0x0b,1,1,LOCAL_ADDR,6); //����֡
	WriteCircleQueue(&RADIO_TXCQ,RADIO_TX_PF.PDUFRAME,28);//д��MID���ȴ�����
	Radio_Direct_Tx(RADIO_TX_PF.PDUFRAME,28); //ֱ�ӷ���

	Set_P645_Frame(&USART0_TX_PF,LOCAL_ADDR,PF->PF.SRCADDR,0x02,0x0b,1,1,LOCAL_ADDR,6); //����֡
	WriteCircleQueue(&WEIHU_TXCQ,USART0_TX_PF.PDUFRAME,28);//д��MID���ȴ�����
}
/************************************************************!
*Function: Set_P645_Frame
*Description: 645Э�����֡�������ֽ׶���3�ָ�ʽ������֡(��ԼV1.0.6)
*Input: 1.PF��645�ṹ����
*       2.ScrAddr��Դ��ַ���׵�ַ
*       3.DstAddr��Ŀ�ĵ�ַ���׵�ַ
*       4.ctrl��������
*       5.pdutype����������
*       6.framenum��֡����
*       7.frameseq��֡���
*       8.data�����������׵�ַ
*       9.lens���������ݳ���
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
		if(pdutype==0) //ACK֡
		{
			PF->PF.LEN1=MIN_LEN;
			PF->PF.LEN2=MIN_LEN;

			PF->PDUFRAME[FRAMEHEAD+MIN_LEN]=Check_Crc(PF->PDUFRAME,FRAMEHEAD+MIN_LEN);
			PF->PDUFRAME[FRAMEHEAD+MIN_LEN+1]=ENDFIELD;
		}
		else //������֡
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
	else //����֡
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
*Description: 645Э���֡ͷ������֡������
*Input: 1.PF��645�ṹ����
*       2.ScrAddr��Դ��ַ���׵�ַ
*       3.DstAddr��Ŀ�ĵ�ַ���׵�ַ
*       4.ctrl��������
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
*Description: MID����Ƶ�ص�������ע�ắ��
*Input: func����Ƶ�ķ��ͺ���ָ��
*Output: null
*Return: null
*************************************************************/
void Radio_Tx_Cb(RADIO_TX_CB func)
{
	RadioTxCb=func;
}
/************************************************************!
*Function: Radio_Mid_Tx
*Description: MID�����Ƶ�������ݵĻص�����
*Input: 1.channel����Ƶͨ��
*       2.data��������׵�ַ
*       3.lens�����鳤��
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
*Description: ��������CRCֵ��
*Input: 1.data�������׵�ַ��
*       2.lens�����鳤�ȡ�
*Output: null
*Return: CRCֵ��
*************************************************************/
uint8_t Check_Crc(uint8_t *data,uint16_t lens)
{
	uint8_t crc=0;

	for(uint16_t i=0;i<lens;i++)
		crc+=*(data++);

	return crc;
}

