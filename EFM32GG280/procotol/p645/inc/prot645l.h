/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: prot645l.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description: 规约V1.0.3定义
*******************************************************************/
#ifndef PROT645L_H_
#define PROT645L_H_


#include <stdint.h>

/**********645协议相关宏定义(规约V1.0.3)**********/
#define STFD1              0x68
#define STFD1_INDEX        0
#define LEN1_INDEX         1
#define LEN2_INDEX         2
#define STFD2              0x68
#define STFD2_INDEX        3
#define CTRL_INDEX         4
#define ADDR_LEN           6
#define SCRADDR_INDEX      5
#define DSTADDR_INDEX      11
#define CRC_INDEX          259
#define ENDFIELD           0x16

#define FRAMEHEAD          4
#define MIN_LEN            13
#define MAX_LEN            255
#define MIN_PDU_LEN        3
#define MAX_DATA_LEN       239
#define	PDULEN             242   //帧数据域PDU最大长度 255-13
#define LPDUSIZE       	   261   //链路层报文的最大长度255+6
/************************************************************!
*Function: CTRL联合体
*Description: 故指通信规约中CONTROL位联合体定义(规约V1.0.3)
*************************************************************/
typedef union
{
	uint8_t CTRL;
	struct
	{
		uint8_t CC:4;       //功能码
		uint8_t SFV:1;      //后续帧标志
		uint8_t FCB:1;      //保留位
		uint8_t PRM:1;      //从站应答标志
		uint8_t DIR:1;      //传送方向
	}C;                     //b:位
}CTRL_t;
/************************************************************!
*Function: PDUFRAME联合体
*Description: 故指通信规约中PDUFRAME定义(规约V1.0.3)
*************************************************************/
typedef union
{
	uint8_t PDUFRAME[LPDUSIZE+2];
	struct
	{
		uint8_t  SF1;                    //起始字节
		uint8_t  LEN1;    		         //长度1
		uint8_t  LEN2;    		         //长度2
		uint8_t  SF2;                    //起始字节
		CTRL_t   CTRL;                   //控制域
		uint8_t  SRCADDR[ADDR_LEN];      //源地址
		uint8_t  DSTADDR[ADDR_LEN];	     //目的地址
		uint8_t  TYPE;                   //数据类型
		uint8_t  FRAMENUM;               //帧数量
		uint8_t  FRAMESEQ;               //帧序号
		uint8_t  DATA[MAX_DATA_LEN];     //数据内容
		uint8_t  CS;                    //校验码
		uint8_t  EF;                     //结束字符

		uint8_t  ISLOCK;                 //结构体占用标志
		uint8_t  PARSE;                  //数据解析标志
	}PF;                                  //B：字节
}PDUFRAME;
/**********PRM枚举**********/
typedef enum
{
	PRMSEC=0, //发起者
	PRMPRI=1, //从动者
}PRM_t;
/**********从动者功能码定义PRM=0**********/
typedef enum
{
	FCCON=0,     //确认
	FCNOCON=1,   //确认
	FCRESDATA=2, //响应帧
	FCNODATA=3,  //响应帧
}CC0_t;
/**********发起者功能码定义PRM=1**********/
typedef enum
{
	FCRESETUSE=0,    //发送/无应答
	FCRESETFAULT=1,  //发送/确认
	FCSENDDATANA=2,  //发送/无应答
	FCSENDDATA=3,    //发送/确认
	FCREQUESTD=4,    //请求/响应
}CC1_t;
/**********类型标识定义**********/
typedef enum
{
	TIMESTAMP=1,    //时标
	DBYC=2,         //遥测
	DBYXTIM=3,      //SOE
	DBWAVE=4,       //电流录波数据
	DBPARA=5,       //参数
	DBPROG=6,       //程序
	FAULTWRG=7,     //启动录波
	LED=8,          //点灯测试
	FANPAI=9,       //测试翻牌
	WAKEUP=10,      //唤醒(对采集单元有效)
	PHYID=11,       //物理地址
}TYPE_t;


#endif

