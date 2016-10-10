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
*Description: BCD码转化为字节
*Input: 1.bcd：BCD码
*Output: null
*Return: 字节数据
*************************************************************/
uint8_t Bcd2Byte(uint8_t bcd)
{
	uint8_t data;

	data=(uint8_t)(((bcd>>4)&&0x0F)*10+((bcd>>0)&&0x0F));

	return data;
}
/************************************************************!
*Function: Byte2Bcd
*Description: 字节转化为BCD码
*Input: 1.byte：字节数据
*Output: null
*Return: BCD码
*************************************************************/
uint8_t Byte2Bcd(uint8_t byte)
{
    uint8_t data;

    data=((byte/10)<<4)+((byte%10)<<0);

    return data;
}
/************************************************************!
*Function: Is_Valid_BCD
*Description: 判断数组是否是合法的BCD码
*Input: 1.data：BCD码数组首地址
*       2.lens：数组长度
*Output: null
*Return: BCD码是否合法
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
*Description: 将两个字符转化为一个Uint8_t,只适用于将10进制的时间格式
*             转化为Uint8_t。
*Input: data：需要转换成Uint8_t的字符的首地址
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
*Description: 得到字符的HEX值
*Input: data：需要转换成HEX值的字符
*Output: null
*Return: HEX值
*************************************************************/
uint8_t Get_Hex(uint8_t data)
{
	if(data>='0'&&data<='9')
		data=data-'0';

	if(data>='A'&&data<='F')
		data=data-'0'-7; //此处为啥-7，请看ASCII码表，谢谢

	return data;
}
/************************************************************!
*Function: Str2Uint
*Description: 将字符串转换为整数,字符数据前面可以有空格，数字必须连续，
*             之后可以有空格或结束
*Input: 1.data：字符数据首地址
*Output: null
*Return: 返回整数
*************************************************************/
uint16_t Str2Uint(uint8_t *data)
{
    uint32_t val=0;

    while(*data!=0) //首先，剔出字符数字前面的空格
    {
       if(*data==' ')
    	   data++;
       else
            break;
    }

    while(*data!=0)  //然后，对字符数据从高位到低位依次进行转换，十进制，碰到第一个非数字结束
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
*Description: 把字符串转换为整数,加上个数控制(GPS提取时间用).字符数
*             据前面可以有空格，数字必须连续，之后可以有空格或结束
*Input: 1.data：字符数据首地址
*       2.lens：字符长度
*Output: null
*Return: 返回整数
*************************************************************/
uint16_t Str2Uint_Gps(uint8_t *data,uint8_t lens)
{
    uint16_t val=0;
	uint16_t i=1; //转换个数控制，初值设成1，保证和n对应起来

    while(*data!=0)  //首先，剔出字符数字前面的空格
    {
       if(*data==' ')
    	   data++;
       else
            break;
    }

    while(*data!=0)  //然后，对字符数据从高位到低位依次进行转换，十进制，碰到第一个非数字结束
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
*Description: float转char型
*Input: 1.value：float
*       2.data：char字符数组首地址
*Output: null
*Return: 返回整数
*************************************************************/
uint8_t Float2Char(float value,uint8_t *data)
{
	uint8_t integer;
	float decimal;
	uint8_t i=0;

	char temp;

	if(value>=1) //分离整数和小数
	{
		integer=(uint8_t)value;
		decimal=value-integer;
	}
	else
	{
		integer=0;
		decimal=value-integer;
	}

	if(integer==0)  //处理整数部分
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

		for(uint8_t j=0;j<i;j++) //填充结果
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


