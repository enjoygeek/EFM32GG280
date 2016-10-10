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
*Description: 根据采集单元地址得到其所属的线的数组索引
*Input: 1.LI：线号信息结构体
*       2.addr：采集单元地址
*Output: null
*Return: 采集单元所属的线的数组索引
*************************************************************/
uint8_t getLine_Info_Line(LINE_INFO *LI,uint8_t *addr)
{
	for(uint8_t i=0;i<MAX_LINE_GROUP;i++)
	{
		if(LI->LINE[i].LINENO==0) //判断线号不为0,确认该线号为有效的采集单元组
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
*Description: 根据采集单元地址得到其所属的线的线号
*Input: 1.LI：线号信息结构体
*       2.addr：采集单元地址
*Output: null
*Return: 采集单元所属的线的线号
*************************************************************/
uint8_t getLine_Info_Lineno(LINE_INFO *LI,uint8_t *addr)
{
	for(uint8_t i=0;i<MAX_LINE_GROUP;i++)
	{
		if(LI->LINE[i].LINENO==0) //判断线号不为0,确认该线号为有效的采集单元组
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
*Description: 根据采集单元地址得到其相号
*Input: 1.LI：线号信息结构体
*       2.addr：采集单元地址
*Output: null
*Return: 采集单元所属的相号
*************************************************************/
uint8_t getLine_Info_Phase(LINE_INFO *LI,uint8_t *addr)
{
	for(uint8_t i=0;i<MAX_LINE_GROUP;i++)
	{
		if(LI->LINE[i].LINENO==0) //判断线号不为0,确认该线号为有效的采集单元组
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
*Input: 1.LI：线号信息结构体
*       2.line：线号
*       3.a：采集单元A地址首地址
*       4.b：采集单元B地址首地址
*       5.c：采集单元C地址首地址
*Output: null
*Return: 函数运行状态
*************************************************************/
uint8_t setLine_Info(LINE_INFO *LI,uint8_t line,uint8_t *a,uint8_t *b,uint8_t *c)
{
	for(uint8_t i=0;i<MAX_LINE_GROUP;i++)
	{
		if(LI->LINE[i].LINENO==line) //此处为了保证设定的新线号没有跟之前已有的线号重复
			return false;
	}

	for(uint8_t j=0;j<MAX_LINE_GROUP;j++)
	{
		if(LI->LINE[j].LINENO==0) //判断线号为0,该线号为空，可以插入新的采集单元组
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
*Description: 删除指定线号的采集单元组
*Input: 1.LI：线号信息结构体
*       2.line：线号
*Output: null
*Return: 函数运行状态
*************************************************************/
uint8_t delLine_Info(LINE_INFO *LI,uint8_t line)
{
	if(line==0) //需要删除的线号为0，则删除成功(WANGJIANHUI逻辑)
		return true;

	for(uint8_t i=0;i<MAX_LINE_GROUP;i++)
	{
		if(LI->LINE[i].LINENO==line) //查找线号
		{
			LI->LINE[i].LINENO=0;

			memset(LI->LINE[i].AU[0].ADDR,0,6*sizeof(uint8_t));
			memset(LI->LINE[i].AU[1].ADDR,0,6*sizeof(uint8_t));
			memset(LI->LINE[i].AU[2].ADDR,0,6*sizeof(uint8_t));

			return true;
		}
	}

	return false; //不会运行到这里
}
/************************************************************!
*Function: serAU_Info
*Description: 查找有效的线号数目。
*Input: 1.LI：线号信息结构体
*Output: null
*Return: 有效的线号数目。
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

