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
*Description: ����ϵ�������ʱ�򣬳�����Ҫ�ܹ���ÿ��FLASH������ϴ�д
*             �����ݵ�λ�ý���д���µ����ݣ���ʱ����Ҫ�����ϴ�д������
*             ��λ�ã���������FLASH���дָ��ָ�����λ�á�
*Input: null
*Output: null
*Return: null
*************************************************************/
void Flash_Pointer_Init(void)
{
	gFLASH_POINTER.pPARA=0; //��δ�õ�������˵�ϵ粻��Ҫ���¶�λ��ָ��

	gFLASH_POINTER.pPROGRAM=PROGRAM_ADDR+4; //+4:ǰ�ĸ��ֽ�Ϊ�����Կ����

	gFLASH_POINTER.pYC=Find_Flash_Pointer(YC_INFO_ADDR,YC_SECTOR_NUM);

	gFLASH_POINTER.pSOE=Find_Flash_Pointer(SOE_INFO_ADDR,SOE_SECTOR_NUM);

	gFLASH_POINTER.pVOL=Find_Flash_Pointer(VOL_INFO_ADDR,VOL_SECTOR_NUM);

	gFLASH_POINTER.pCUR=Find_Flash_Pointer(CUR_INFO_ADDR,CUR_SECTOR_NUM);
}
/************************************************************!
*Function: Find_Flash_Pointer
*Description: ���˽�YC,SOE,VOL,CURֵд��FLASH���㷨����ÿ��FLASH��
*             ����ʼ��ַ��ʼ������д�����ݣ�������һ��sector������գ�
*             Ϊд��һ��sector��׼��������һ��sector��FLASH������
*             һ��sector,��дָ��ָ��FLASH��ĵ�һ��sector,������
*             ��һ��sector.�´�д�����ݵ�ʱ��ֱ��д����һ��sector�
*
*             �˺������㷨�ǣ���FLASH��ĵ�һ��sector��ʼ���ң��ҵ�
*             ��һ��Ϊ�յ�sector�����֤������յ�sector����һ��sector
*             һ�����ϴ�д���ݵ�λ�ã���Ҫ�ҳ���һ��sector��дָ��ľ�
*             ��λ�á�����һ��sector����д��������Ҫ��дָ��ָ������յ�
*             sector�ĵ�һ���ֽڵ�λ�á�
*Input: null
*Output: null
*Return: null
*************************************************************/
uint32_t Find_Flash_Pointer(uint32_t startaddr,uint32_t sectornum)
{
	uint8_t newdata;
	uint32_t newpointer,newaddr;

	for(uint32_t i=0;i<sectornum;i++) //��FLASH��ĵ�һ��sector��ʼ����sectorΪ��λ����
	{
		newaddr=startaddr+SECTOR_LEN*i;

		MX25_QUAD_BYTE_READ4B(newaddr,&newdata,1); //��ȡÿһ��sector�ĵ�һ���ֽ�

		if(newdata==0xff) //�ж϶�ȡ���ֽڣ�����0xff,����Ϊ��sector��������(����û�б�д�������)
		{
			if(i==0) //����YC��ĵ�һ��sectorû����(���������1.FLASH�����û�б�д������ 2.����д�����һ��sector,��ʱ�Ὣ��һ��sector����)
			{
				newpointer= YC_INFO_ADDR; //��pYCָ��ָ��YC����׵�ַ
				break;
			}
			else
			{
				for(uint32_t j=0;j<SECTOR_LEN;j++) //��û�����ݵ�sector����YC��ĵ�һ��sector
				{
					MX25_QUAD_BYTE_READ4B(newaddr-SECTOR_LEN+j,&newdata,1); //��ȡû�����ݵ����sector����һ���飬��һ����϶�������

					if(newdata==0xff) //����û�б�д�����ݵ�λ��
					{
						newpointer=newaddr-SECTOR_LEN+j;
						break;
					}

					 if(j==SECTOR_LEN) //�жϳ���sector��д��
						 newpointer=newaddr;  //����sector�ĵ�һ���ֽڲ鵽���һ���ֽ�ȫ�������ݣ�֤����sector�պ�д�꣬��Ҫ��ָ���Ƶ���һ��sector�ĵ�һ���ֽ�
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
		case PARA: //����
			Set_PARA_Storage(data,lens);
			break;

		case PROGRAM: //����
			Set_PROGRAM_Storage(data,lens);
			break;

		case YC: //ң��
			Set_INFO_Storage(YC_INFO_ADDR,gFLASH_POINTER.pYC,YC_INFO_LEN,data);
			break;

		case SOE: //ң��
			Set_INFO_Storage(SOE_INFO_ADDR,gFLASH_POINTER.pSOE,SOE_INFO_LEN,data);
			break;

		case VOL: //��ѹ����
			Set_INFO_Storage(VOL_INFO_ADDR,gFLASH_POINTER.pVOL,VOL_INFO_LEN,data);
			break;

		case CUR: //��������
			Set_INFO_Storage(CUR_INFO_ADDR,gFLASH_POINTER.pCUR,CUR_INFO_LEN,data);
			break;

			default:
				break;
	}
}
/************************************************************!
*Function: Set_PARA_Storage
*Description: ������Ĳ������´洢�����������߼�Ϊ�Ƚ�FLASH�еĲ���
*             ȫ����������Ȼ���޸����еı�����֮����������飬�ٽ���
*             ����Ĳ���д��FLASH��
*Input: 1.data��������׵�ַ
*       2.lens������ĳ���
*Output: null
*Return: null
*************************************************************/
void Set_PARA_Storage(uint8_t *data,uint32_t lens)
{
	uint32_t sector_startno;
	//uint32_t sector_num;

	//sector_num=PARA_FLASH_LEN/SECTOR_LEN; //��ò������sector����
	sector_startno=PARA_ADDR/SECTOR_LEN;  //��ò��������ʼsector��(sector�ŷ�ΧΪ0-8191)

	//for(uint32_t i=0;i<sector_num;i++)
	//{
	//	MX25_SECTOR_ERASE_4B(sector_startno+i);
	//}

	MX25_SECTOR_ERASE_4B(sector_startno);

	MX25_QUAD_BYTE_PP4B(PARA_ADDR,data,lens);
}
/************************************************************!
*Function: Get_PARA_Storage
*Description: ��ȡ�������еĲ���
*Input: 1.data��������׵�ַ
*       2.lens������ĳ���
*Output: null
*Return: ��������״̬
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
*Description: ������д��FLASH���˴���Ҫ���Ǵ�˴洢ģʽ���洢ǰ�Ƚ���
*             ��ת��Ϊ���ģʽ��Ȼ��д��FLASH����д�������ĳ��Ȳ�Ϊ
*             4�����������������ֽڴճ���������д��FLASH�����磺���
*             ʣ��һ���ֽ�0x12,��Ҫ��0x12ת��Ϊ 0x0,0x0,0x0,0x12���롣
*Input: 1.data��������׵�ַ
*       2.lens������ĳ���
*Output: null
*Return: null
*************************************************************/
void Set_PROGRAM_Storage(uint8_t *data,uint32_t lens)
{
	uint8_t len1,len2;
	uint8_t newdata[4];

	len1=lens/4;
	len2=lens%4;

    for(uint8_t i=0;i<len1;i++) //������Ϊ4�ı���������ת��Ϊ���ģʽ����FLASH
    {
       newdata[0]=data[4*i+3];
       newdata[1]=data[4*i+2];
       newdata[2]=data[4*i+1];
       newdata[3]=data[4*i+0];

       MX25_QUAD_BYTE_PP4B(gFLASH_POINTER.pPROGRAM,newdata,4);
       gFLASH_POINTER.pPROGRAM += 4;
    }

    if(len2!=0) //�жϳ����Ƿ�Ϊ4�ı���
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
*Description: ������д��FLASH���жϵ�ǰָ�����ڵĽں��Ƿ�ΪFLASH�����
*             ��һ��sector�����ǣ���FLASH��ĵ�һ��sector��������
*             ��ָ��ָ���һ��sector���׵�ַ�������ǣ����ȶ�ȡ��һ
*             ��sector�ĵ�һ���ֽڣ�����һ���ֽڲ���0xff,�򽫸�sector
*             ������
*Input: 1.data��������׵�ַ
*       2.lens������ĳ���
*Output: null
*Return: null
*************************************************************/
void Set_INFO_Storage(uint32_t newflashaddr,uint32_t newpointer,uint32_t newlens,uint8_t *data)
{
	uint8_t newdata;
	uint32_t sectorno; //��ǰpYCָ�����ڵĽں�

	MX25_QUAD_BYTE_PP4B(newpointer,data,newlens); //��YC����дFALSH
	newpointer+=newlens;

	sectorno=newpointer/SECTOR_LEN;//�õ���ǰpYCָ�����ڵ�sectorno(�ں�)

	if((newpointer%SECTOR_LEN)==1) //ΪɶҪ�ж�pYC�Ƿ�ָ��sector�ĵ�һ���ֽڣ���ָ���һ���ֽڣ���˵����һ��sector�պ�д��
	{
		sectorno-=1;//��Ҫ���ں�-1
	}

	if(sectorno==(PARA_SECTOR_NUM+PROGRAM_SECTOR_NUM+YC_SECTOR_NUM)) //�жϵ�ǰpYCָ�����ڵ�sectorno�ǲ���YC������һ��sector
	{
		MX25_SECTOR_ERASE_4B(newflashaddr/SECTOR_LEN); //����YC��ĵ�һ��sector
		newpointer=newflashaddr; //��ָ�����¶�λ��YC�����ʼ��ַ
	}
	else
	{
		MX25_QUAD_BYTE_READ4B(sectorno+1,&newdata,1); //��ȡ��ǰָ������sector����һ��sector����ȡ��һ���ֽ�

		if(newdata!=0xff) //�ж϶�ȡ���ֽ��Ƿ������ݣ����������򽫸�sector����
		{
			MX25_SECTOR_ERASE_4B(sectorno+1);
		}
	}
}
/************************************************************!
*Function: Get_INFO_Storage
*Description: ������д��FLASH���жϵ�ǰָ�����ڵĽں��Ƿ�ΪFLASH�����
*             ��һ��sector�����ǣ���FLASH��ĵ�һ��sector��������
*             ��ָ��ָ���һ��sector���׵�ַ�������ǣ����ȶ�ȡ��һ
*             ��sector�ĵ�һ���ֽڣ�����һ���ֽڲ���0xff,�򽫸�sector
*             ������
*Input: 1.data��������׵�ַ
*       2.lens������ĳ���
*Output: null
*Return: null
*************************************************************/
void Get_INFO_Storage(STORAGE_TYPE st,uint8_t *targetdata,uint8_t *data)
{
	//�˺���������ȡ���ݵ�ʱ��ʱ����Ϣ��ʽδ�������Դ˺�����δʵ��

}

/************************************************************!
*Function: Para_Init
*Description: ϵͳ������ʼ�����ȶ����������ĵ�һ���ֽڣ��ж��Ƿ�����Ч
*             ������������Ч�����������ȫ�ֱ�����û����Ч����������
*             Ҫ����ȫ�ֱ�����
*Input: 1.data��������׵�ַ
*       2.lens�����鳤��
*Output: null
*Return: null
*************************************************************/
uint8_t Para_Init(void)
{
	uint8_t data;
	uint8_t para[120];

	MX25_QUAD_BYTE_READ4B(PARA_ADDR,&data,1); //

	if(data==0xFF) //����Ч�������������ȫ�ֱ���������
		return false;

	Get_PARA_Storage(para,120); //����ȫ�ֱ���

	Refresh_Para(para,120);

	return true;
}
/************************************************************!
*Function: Refresh_Para
*Description: ����ϵͳ����
*Input: 1.data��������׵�ַ
*       2.lens�����鳤��
*Output: null
*Return: null
*************************************************************/
void Refresh_Para(uint8_t *data,uint8_t lens)
{
	for(uint8_t i=0;i<MAX_LINE_GROUP;i++) //ˢ���ߺż����
	{
		gLINE_INFO.LINE[i].LINENO=data[i*19];

		memcpy(&gLINE_INFO.LINE[i].AU[0],data+i*19+1,6);
		memcpy(&gLINE_INFO.LINE[i].AU[1],data+i*19+1+6,6);
		memcpy(&gLINE_INFO.LINE[i].AU[2],data+i*19+1+6+6,6);
	}

	memcpy(LOCAL_ADDR,data+6*19,6); //ˢ�±�����ַ

	//�˴���Ҫ�������Ʋ��������
}

