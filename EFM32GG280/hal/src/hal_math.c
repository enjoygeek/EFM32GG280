/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_math.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_math.h"

/************************************************************!
*Function: Bcd2Byte
*Description: BCD��ת��Ϊ�ֽ�
*Input: 1.bcd��BCD��
*Output: null
*Return: �ֽ�����
*************************************************************/
uint8_t Bcd2Byte(uint8_t bcd)
{
	uint8_t data;

	data=(uint8_t)(((bcd>>4)&&0x0F)*10+((bcd>>0)&&0x0F));

	return data;
}
/************************************************************!
*Function: Byte2Bcd
*Description: �ֽ�ת��ΪBCD��
*Input: 1.byte���ֽ�����
*Output: null
*Return: BCD��
*************************************************************/
uint8_t Byte2Bcd(uint8_t byte)
{
    uint8_t data;

    data=((byte/10)<<4)+((byte%10)<<0);

    return data;
}
/************************************************************!
*Function: Is_Valid_BCD
*Description: �ж������Ƿ��ǺϷ���BCD��
*Input: 1.data��BCD�������׵�ַ
*       2.lens�����鳤��
*Output: null
*Return: BCD���Ƿ�Ϸ�
*************************************************************/
uint8_t Is_Valid_BCD(uint8_t *data,uint8_t lens)
{
   for(uint8_t i=0;i<lens;i++)
   {
      if((data[i]&0x0F)>0x09)
    	  return false;

      if((data[i]&0xF0)>0x90)
    	  return false;
   }

   return true;
}
/************************************************************!
*Function: Char2Uint8_t
*Description: �������ַ�ת��Ϊһ��Uint8_t,ֻ�����ڽ�10���Ƶ�ʱ���ʽ
*             ת��ΪUint8_t��
*Input: data����Ҫת����Uint8_t���ַ����׵�ַ
*Output: null
*Return: uint8_t
*************************************************************/
uint8_t Char2Uint8_t(uint8_t *data)
{
	uint8_t result;

	result=Get_Hex(data[0])*10+Get_Hex(data[1]);

	return result;
}
/************************************************************!
*Function: Get_Hex
*Description: �õ��ַ���HEXֵ
*Input: data����Ҫת����HEXֵ���ַ�
*Output: null
*Return: HEXֵ
*************************************************************/
uint8_t Get_Hex(uint8_t data)
{
	if(data>='0'&&data<='9')
		data=data-'0';

	if(data>='A'&&data<='F')
		data=data-'0'-7; //�˴�Ϊɶ-7���뿴ASCII���лл

	return data;
}
/************************************************************!
*Function: Str2Uint
*Description: ���ַ���ת��Ϊ����,�ַ�����ǰ������пո����ֱ���������
*             ֮������пո�����
*Input: 1.data���ַ������׵�ַ
*Output: null
*Return: ��������
*************************************************************/
uint16_t Str2Uint(uint8_t *data)
{
    uint32_t val=0;

    while(*data!=0) //���ȣ��޳��ַ�����ǰ��Ŀո�
    {
       if(*data==' ')
    	   data++;
       else
            break;
    }

    while(*data!=0)  //Ȼ�󣬶��ַ����ݴӸ�λ����λ���ν���ת����ʮ���ƣ�������һ�������ֽ���
    {
        if((*data>='0')&&(*data<='9'))
        {
            val=val*10+((*data)-'0');
        }
        else
        {
           break;
        }

        data++;
    }

    return val;
}
/************************************************************!
*Function: Str2Uint_Gps
*Description: ���ַ���ת��Ϊ����,���ϸ�������(GPS��ȡʱ����).�ַ���
*             ��ǰ������пո����ֱ���������֮������пո�����
*Input: 1.data���ַ������׵�ַ
*       2.lens���ַ�����
*Output: null
*Return: ��������
*************************************************************/
uint16_t Str2Uint_Gps(uint8_t *data,uint8_t lens)
{
    uint16_t val=0;
	uint16_t i=1; //ת���������ƣ���ֵ���1����֤��n��Ӧ����

    while(*data!=0)  //���ȣ��޳��ַ�����ǰ��Ŀո�
    {
       if(*data==' ')
    	   data++;
       else
            break;
    }

    while(*data!=0)  //Ȼ�󣬶��ַ����ݴӸ�λ����λ���ν���ת����ʮ���ƣ�������һ�������ֽ���
    {
    	if(i<=lens)
            val=val*10+((*data)-'0');
        else
           break;

        data++;
        i++;
    }

    return val;
}
/************************************************************!
*Function: Float2Char
*Description: floatתchar��
*Input: 1.value��float
*       2.data��char�ַ������׵�ַ
*Output: null
*Return: ��������
*************************************************************/
uint8_t Float2Char(float value,uint8_t *data)
{
	uint8_t integer;
	float decimal;
	uint8_t i=0;

	char temp;

	if(value>=1) //����������С��
	{
		integer=(uint8_t)value;
		decimal=value-integer;
	}
	else
	{
		integer=0;
		decimal=value-integer;
	}

	if(integer==0)  //������������
	{
		data[0]='0';
		data[1]='.';
		i=1;
	}
	else
	{
		while(integer > 0)
		{
			data[i]=integer%10+'0';
			decimal/=10;
			i++;
		}

		i--;

		for(uint8_t j=0;j<i;j++) //�����
		{
			temp=data[j];
			data[j]=data[i - j];
			data[i-j]=temp;
		}

		i++;
		data[i] = '.';

	}

	i++;

	data[i++]=(uint32_t)(decimal*10)%10+'0';
	data[i++]=(uint32_t)(decimal*100)%10+'0';
	data[i++]=(uint32_t)(decimal*1000)%10+'0';
	data[i++]=(uint32_t)(decimal*10000)%10+'0';
	data[i]='\0';

	 return i;
}


