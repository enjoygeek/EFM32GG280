/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: flashstorage.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"
#include "flashstorage.h"

/************************************************************!
*Function: Flash_Pointer_Init
*Description: 意外断电重启的时候，程序需要能够在每个FLASH块接着上次写
*             入数据的位置接着写入新的数据，这时就需要查找上次写入数据
*             的位置，并将各个FLASH块的写指针指向这个位置。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Flash_Pointer_Init(void)
{
	gFLASH_POINTER.pPARA=0; //暂未用到，就是说断电不需要重新定位该指针

	gFLASH_POINTER.pPROGRAM=PROGRAM_ADDR+4; //+4:前四个字节为编程密钥保留

	gFLASH_POINTER.pYC=Find_Flash_Pointer(YC_INFO_ADDR,YC_SECTOR_NUM);

	gFLASH_POINTER.pSOE=Find_Flash_Pointer(SOE_INFO_ADDR,SOE_SECTOR_NUM);

	gFLASH_POINTER.pVOL=Find_Flash_Pointer(VOL_INFO_ADDR,VOL_SECTOR_NUM);

	gFLASH_POINTER.pCUR=Find_Flash_Pointer(CUR_INFO_ADDR,CUR_SECTOR_NUM);
}
/************************************************************!
*Function: Find_Flash_Pointer
*Description: 先了解YC,SOE,VOL,CUR值写入FLASH的算法：从每个FLASH块
*             的起始地址开始连续的写入数据，并将下一个sector数据清空，
*             为写下一个sector做准备，若下一个sector是FLASH块的最后
*             一个sector,则将写指针指向FLASH块的第一个sector,并擦除
*             第一个sector.下次写入数据的时候直接写到第一个sector里。
*
*             此函数的算法是：从FLASH块的第一个sector开始查找，找到
*             第一个为空的sector，这就证明这个空的sector的上一个sector
*             一定是上次写数据的位置，需要找出上一个sector的写指针的具
*             体位置。若上一个sector正好写满，则需要将写指针指向这个空的
*             sector的第一个字节的位置。
*Input: null
*Output: null
*Return: null
*************************************************************/
uint32_t Find_Flash_Pointer(uint32_t startaddr,uint32_t sectornum)
{
	uint8_t newdata;
	uint32_t newpointer,newaddr;

	for(uint32_t i=0;i<sectornum;i++) //从FLASH块的第一个sector开始，以sector为单位查找
	{
		newaddr=startaddr+SECTOR_LEN*i;

		MX25_QUAD_BYTE_READ4B(newaddr,&newdata,1); //读取每一个sector的第一个字节

		if(newdata==0xff) //判断读取的字节，若是0xff,则认为该sector被擦除过(或者没有被写入过数据)
		{
			if(i==0) //表明YC块的第一个sector没有数(两种情况：1.FLASH块从来没有被写过数据 2.数据写道最后一个sector,此时会将第一个sector擦除)
			{
				newpointer= YC_INFO_ADDR; //将pYC指针指到YC块的首地址
				break;
			}
			else
			{
				for(uint32_t j=0;j<SECTOR_LEN;j++) //若没有数据的sector不是YC块的第一个sector
				{
					MX25_QUAD_BYTE_READ4B(newaddr-SECTOR_LEN+j,&newdata,1); //读取没有数据的这个sector的上一个块，上一个块肯定有数据

					if(newdata==0xff) //查找没有被写入数据的位置
					{
						newpointer=newaddr-SECTOR_LEN+j;
						break;
					}

					 if(j==SECTOR_LEN) //判断出该sector已写满
						 newpointer=newaddr;  //若从sector的第一个字节查到最后一个字节全都有数据，证明该sector刚好写完，需要将指针移到下一个sector的第一个字节
				}
			}
		}
	}

	return newpointer;
}
/************************************************************!
*Function: Flash_Storage
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Flash_Storage(STORAGE_TYPE st,uint8_t *data,uint8_t lens)
{
	switch (st)
	{
		case PARA: //参数
			Set_PARA_Storage(data,lens);
			break;

		case PROGRAM: //程序
			Set_PROGRAM_Storage(data,lens);
			break;

		case YC: //遥测
			Set_INFO_Storage(YC_INFO_ADDR,gFLASH_POINTER.pYC,YC_INFO_LEN,data);
			break;

		case SOE: //遥信
			Set_INFO_Storage(SOE_INFO_ADDR,gFLASH_POINTER.pSOE,SOE_INFO_LEN,data);
			break;

		case VOL: //电压数据
			Set_INFO_Storage(VOL_INFO_ADDR,gFLASH_POINTER.pVOL,VOL_INFO_LEN,data);
			break;

		case CUR: //电流数据
			Set_INFO_Storage(CUR_INFO_ADDR,gFLASH_POINTER.pCUR,CUR_INFO_LEN,data);
			break;

			default:
				break;
	}
}
/************************************************************!
*Function: Set_PARA_Storage
*Description: 参数块的参数更新存储函数，函数逻辑为先将FLASH中的参数
*             全部读出来，然后修改其中的变量，之后擦除参数块，再将修
*             改完的参数写入FLASH。
*Input: 1.data：数组的首地址
*       2.lens：数组的长度
*Output: null
*Return: null
*************************************************************/
void Set_PARA_Storage(uint8_t *data,uint32_t lens)
{
	uint32_t sector_startno;
	//uint32_t sector_num;

	//sector_num=PARA_FLASH_LEN/SECTOR_LEN; //获得参数块的sector数量
	sector_startno=PARA_ADDR/SECTOR_LEN;  //获得参数块的起始sector号(sector号范围为0-8191)

	//for(uint32_t i=0;i<sector_num;i++)
	//{
	//	MX25_SECTOR_ERASE_4B(sector_startno+i);
	//}

	MX25_SECTOR_ERASE_4B(sector_startno);

	MX25_QUAD_BYTE_PP4B(PARA_ADDR,data,lens);
}
/************************************************************!
*Function: Get_PARA_Storage
*Description: 读取参数块中的参数
*Input: 1.data：数组的首地址
*       2.lens：数组的长度
*Output: null
*Return: 函数运行状态
*************************************************************/
uint8_t Get_PARA_Storage(uint8_t *data,uint32_t lens)
{
	if(lens==0||lens>PARA_FLASH_LEN)
		return false;

	MX25_QUAD_BYTE_READ4B(PARA_ADDR,data,lens);

	return true;
}
/************************************************************!
*Function: Set_PROGRAM_Storage
*Description: 将数组写入FLASH，此处需要考虑大端存储模式，存储前先将数
*             据转化为大端模式，然后写入FLASH，若写入的数组的长度不为
*             4的整数倍，将最后的字节凑成整数倍，写入FLASH，例如：最后
*             剩余一个字节0x12,需要将0x12转化为 0x0,0x0,0x0,0x12存入。
*Input: 1.data：数组的首地址
*       2.lens：数组的长度
*Output: null
*Return: null
*************************************************************/
void Set_PROGRAM_Storage(uint8_t *data,uint32_t lens)
{
	uint8_t len1,len2;
	uint8_t newdata[4];

	len1=lens/4;
	len2=lens%4;

    for(uint8_t i=0;i<len1;i++) //将长度为4的倍数的数据转化为大端模式存入FLASH
    {
       newdata[0]=data[4*i+3];
       newdata[1]=data[4*i+2];
       newdata[2]=data[4*i+1];
       newdata[3]=data[4*i+0];

       MX25_QUAD_BYTE_PP4B(gFLASH_POINTER.pPROGRAM,newdata,4);
       gFLASH_POINTER.pPROGRAM += 4;
    }

    if(len2!=0) //判断长度是否为4的倍数
    {
    	for(uint8_t j=0;j<(4-len2);j++)
		{
			newdata[j]=0;
		}

    	for(uint8_t k=0;k<len2;k++)
    	{
    		newdata[3-k]=data[lens-len2+k];
    	}

        MX25_QUAD_BYTE_PP4B(gFLASH_POINTER.pPROGRAM,newdata,4);
        gFLASH_POINTER.pPROGRAM += 4;
    }
}
/************************************************************!
*Function: Set_INFO_Storage
*Description: 将数据写入FLASH，判断当前指针所在的节号是否为FLASH块的最
*             后一个sector，若是，则将FLASH块的第一个sector擦除，并
*             将指针指向第一个sector的首地址，若不是，首先读取下一
*             个sector的第一个字节，若第一个字节不是0xff,则将该sector
*             擦除。
*Input: 1.data：数组的首地址
*       2.lens：数组的长度
*Output: null
*Return: null
*************************************************************/
void Set_INFO_Storage(uint32_t newflashaddr,uint32_t newpointer,uint32_t newlens,uint8_t *data)
{
	uint8_t newdata;
	uint32_t sectorno; //当前pYC指针所在的节号

	MX25_QUAD_BYTE_PP4B(newpointer,data,newlens); //将YC数据写FALSH
	newpointer+=newlens;

	sectorno=newpointer/SECTOR_LEN;//得到当前pYC指针所在的sectorno(节号)

	if((newpointer%SECTOR_LEN)==1) //为啥要判断pYC是否指向sector的第一个字节，若指向第一个字节，则说明上一个sector刚好写满
	{
		sectorno-=1;//需要将节号-1
	}

	if(sectorno==(PARA_SECTOR_NUM+PROGRAM_SECTOR_NUM+YC_SECTOR_NUM)) //判断当前pYC指针所在的sectorno是不是YC块的最后一个sector
	{
		MX25_SECTOR_ERASE_4B(newflashaddr/SECTOR_LEN); //擦除YC块的第一个sector
		newpointer=newflashaddr; //将指针重新定位到YC块的起始地址
	}
	else
	{
		MX25_QUAD_BYTE_READ4B(sectorno+1,&newdata,1); //读取当前指针所在sector的下一个sector，读取第一个字节

		if(newdata!=0xff) //判断读取的字节是否有数据，若有数据则将该sector擦除
		{
			MX25_SECTOR_ERASE_4B(sectorno+1);
		}
	}
}
/************************************************************!
*Function: Get_INFO_Storage
*Description: 将数据写入FLASH，判断当前指针所在的节号是否为FLASH块的最
*             后一个sector，若是，则将FLASH块的第一个sector擦除，并
*             将指针指向第一个sector的首地址，若不是，首先读取下一
*             个sector的第一个字节，若第一个字节不是0xff,则将该sector
*             擦除。
*Input: 1.data：数组的首地址
*       2.lens：数组的长度
*Output: null
*Return: null
*************************************************************/
void Get_INFO_Storage(STORAGE_TYPE st,uint8_t *targetdata,uint8_t *data)
{
	//此函数由于提取数据的时候时间信息格式未定，所以此函数暂未实现

}

/************************************************************!
*Function: Para_Init
*Description: 系统参数初始化，先读出参数区的第一个字节，判断是否有有效
*             参数，若有有效参数，则更新全局变量，没有有效参数，则不需
*             要更新全局变量。
*Input: 1.data：数组的首地址
*       2.lens：数组长度
*Output: null
*Return: null
*************************************************************/
uint8_t Para_Init(void)
{
	uint8_t data;
	uint8_t para[120];

	MX25_QUAD_BYTE_READ4B(PARA_ADDR,&data,1); //

	if(data==0xFF) //无有效参数，不需更新全局变量，返回
		return false;

	Get_PARA_Storage(para,120); //更新全局变量

	Refresh_Para(para,120);

	return true;
}
/************************************************************!
*Function: Refresh_Para
*Description: 更新系统变量
*Input: 1.data：数组的首地址
*       2.lens：数组长度
*Output: null
*Return: null
*************************************************************/
void Refresh_Para(uint8_t *data,uint8_t lens)
{
	for(uint8_t i=0;i<MAX_LINE_GROUP;i++) //刷新线号及相号
	{
		gLINE_INFO.LINE[i].LINENO=data[i*19];

		memcpy(&gLINE_INFO.LINE[i].AU[0],data+i*19+1,6);
		memcpy(&gLINE_INFO.LINE[i].AU[1],data+i*19+1+6,6);
		memcpy(&gLINE_INFO.LINE[i].AU[2],data+i*19+1+6+6,6);
	}

	memcpy(LOCAL_ADDR,data+6*19,6); //刷新本机地址

	//此处需要继续完善参数的添加
}

