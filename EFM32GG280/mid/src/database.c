/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: database.c
*Version: 0.1.0
*Date: 2016-09-20
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "database.h"

/************************************************************!
*Function: getLine_Info_Line
*Description: ���ݲɼ���Ԫ��ַ�õ����������ߵ���������
*Input: 1.LI���ߺ���Ϣ�ṹ��
*       2.addr���ɼ���Ԫ��ַ
*Output: null
*Return: �ɼ���Ԫ�������ߵ���������
*************************************************************/
uint8_t getLine_Info_Line(LINE_INFO *LI,uint8_t *addr)
{
	for(uint8_t i=0;i<MAX_LINE_GROUP;i++)
	{
		if(LI->LINE[i].LINENO==0) //�ж��ߺŲ�Ϊ0,ȷ�ϸ��ߺ�Ϊ��Ч�Ĳɼ���Ԫ��
			continue;

		for(uint8_t j=0;j<3;j++)
		{
			if(memcmp(addr,LI->LINE[i].AU[j].ADDR,AU_ADDR_LEN)==0)
			{
				return i;
			}
		}
	}

	return 0xFF;
}
/************************************************************!
*Function: getLine_Info_Lineno
*Description: ���ݲɼ���Ԫ��ַ�õ����������ߵ��ߺ�
*Input: 1.LI���ߺ���Ϣ�ṹ��
*       2.addr���ɼ���Ԫ��ַ
*Output: null
*Return: �ɼ���Ԫ�������ߵ��ߺ�
*************************************************************/
uint8_t getLine_Info_Lineno(LINE_INFO *LI,uint8_t *addr)
{
	for(uint8_t i=0;i<MAX_LINE_GROUP;i++)
	{
		if(LI->LINE[i].LINENO==0) //�ж��ߺŲ�Ϊ0,ȷ�ϸ��ߺ�Ϊ��Ч�Ĳɼ���Ԫ��
			continue;

		for(uint8_t j=0;j<3;j++)
		{
			if(memcmp(addr,LI->LINE[i].AU[j].ADDR,AU_ADDR_LEN)==0)
			{
				return LI->LINE[i].LINENO;
			}
		}
	}

	return 0x00;
}
/************************************************************!
*Function: getLine_Info_Phase
*Description: ���ݲɼ���Ԫ��ַ�õ������
*Input: 1.LI���ߺ���Ϣ�ṹ��
*       2.addr���ɼ���Ԫ��ַ
*Output: null
*Return: �ɼ���Ԫ���������
*************************************************************/
uint8_t getLine_Info_Phase(LINE_INFO *LI,uint8_t *addr)
{
	for(uint8_t i=0;i<MAX_LINE_GROUP;i++)
	{
		if(LI->LINE[i].LINENO==0) //�ж��ߺŲ�Ϊ0,ȷ�ϸ��ߺ�Ϊ��Ч�Ĳɼ���Ԫ��
			continue;

		for(uint8_t j=0;j<3;j++)
		{
			if(memcmp(addr,LI->LINE[i].AU[j].ADDR,AU_ADDR_LEN)==0)
			{
				return j;
			}
		}
	}

	return 0xFF;
}
/************************************************************!
*Function: setLine_Info
*Description:
*Input: 1.LI���ߺ���Ϣ�ṹ��
*       2.line���ߺ�
*       3.a���ɼ���ԪA��ַ�׵�ַ
*       4.b���ɼ���ԪB��ַ�׵�ַ
*       5.c���ɼ���ԪC��ַ�׵�ַ
*Output: null
*Return: ��������״̬
*************************************************************/
uint8_t setLine_Info(LINE_INFO *LI,uint8_t line,uint8_t *a,uint8_t *b,uint8_t *c)
{
	for(uint8_t i=0;i<MAX_LINE_GROUP;i++)
	{
		if(LI->LINE[i].LINENO==line) //�˴�Ϊ�˱�֤�趨�����ߺ�û�и�֮ǰ���е��ߺ��ظ�
			return false;
	}

	for(uint8_t j=0;j<MAX_LINE_GROUP;j++)
	{
		if(LI->LINE[j].LINENO==0) //�ж��ߺ�Ϊ0,���ߺ�Ϊ�գ����Բ����µĲɼ���Ԫ��
		{
			LI->LINE[j].LINENO=line;

			memcpy(LI->LINE[j].AU[0].ADDR,a,AU_ADDR_LEN);
			memcpy(LI->LINE[j].AU[1].ADDR,b,AU_ADDR_LEN);
			memcpy(LI->LINE[j].AU[2].ADDR,c,AU_ADDR_LEN);

			return true;
		}
	}

	return false;
}
/************************************************************!
*Function: delLine_Info
*Description: ɾ��ָ���ߺŵĲɼ���Ԫ��
*Input: 1.LI���ߺ���Ϣ�ṹ��
*       2.line���ߺ�
*Output: null
*Return: ��������״̬
*************************************************************/
uint8_t delLine_Info(LINE_INFO *LI,uint8_t line)
{
	if(line==0) //��Ҫɾ�����ߺ�Ϊ0����ɾ���ɹ�(WANGJIANHUI�߼�)
		return true;

	for(uint8_t i=0;i<MAX_LINE_GROUP;i++)
	{
		if(LI->LINE[i].LINENO==line) //�����ߺ�
		{
			LI->LINE[i].LINENO=0;

			memset(LI->LINE[i].AU[0].ADDR,0,6*sizeof(uint8_t));
			memset(LI->LINE[i].AU[1].ADDR,0,6*sizeof(uint8_t));
			memset(LI->LINE[i].AU[2].ADDR,0,6*sizeof(uint8_t));

			return true;
		}
	}

	return false; //�������е�����
}
/************************************************************!
*Function: serAU_Info
*Description: ������Ч���ߺ���Ŀ��
*Input: 1.LI���ߺ���Ϣ�ṹ��
*Output: null
*Return: ��Ч���ߺ���Ŀ��
*************************************************************/
uint8_t serAU_Info(LINE_INFO *LI)
{
	uint8_t num=0;

	for(uint8_t i=0;i<MAX_LINE_GROUP;i++)
	{
		if(LI->LINE[i].LINENO!=0) //
		{
			num++;
		}
	}

	return num;
}

