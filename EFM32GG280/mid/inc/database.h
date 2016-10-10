/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: database.h
*Version: 0.1.0
*Date: 2016-09-20
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef DATABASE_H_
#define DATABASE_H_


#include <stdint.h>
#include <string.h>
#include <stdbool.h>


/**********汇集单元带载的最大线数(采集单元组数)**********/
#define MAX_LINE_GROUP         6
/**********采集单元地址长度**********/
#define AU_ADDR_LEN            6
/**********录波数据帧的最大帧数**********/
#define MAX_WAVE_FRAME_NUM     100
/**********采集单元录播数据的最大长度(字节)**********/
#define MAX_WAVE_LEN           3200

/************************************************************!
*Function: AU_INFO结构体
*Description:采集单元信息，此信息可拓展
*************************************************************/
typedef struct
{
	uint8_t  ADDR[AU_ADDR_LEN]; //采集单元地址
	                            //可拓展采集单元的其他信息
}AU_INFO;
/************************************************************!
*Function: AU_GROUP结构体
*Description:汇集单元需要维护的采集单元组信息
*************************************************************/
typedef struct
{
	uint8_t  LINENO; //采集单元组所属线号，线号为0代表该线为空线，无采集单元组
	AU_INFO  AU[3]; //采集单元信息
}LINE_GROUP;
/************************************************************!
*Function: LINE_INFO结构体
*Description:线信息
*************************************************************/
typedef struct
{
	LINE_GROUP LINE[MAX_LINE_GROUP];
}LINE_INFO;

/************************************************************!
*Function: Sys_Time结构体
*Description:
*************************************************************/
typedef union
{
	uint8_t TIME[8];
	struct
	{
		uint16_t  CNT;
		uint8_t   SECOND;
		uint8_t   MINUTE;
		uint8_t   HOUR;
		uint8_t   DAY;
		uint8_t   MONTH;
		uint8_t   YEAR;
	}T;
}SYS_TIME;
/************************************************************!
*Function: GPS_Info结构体
*Description:
*************************************************************/
typedef struct
{
    int8_t   NS_LOC;        //南/北纬
	int8_t   EW_LOC;        //东/西经
    uint16_t LATITUDE;      //纬度（整）
    uint16_t LATITUDE_S;    //纬度（秒）
    uint16_t LONGITUDE;     //经度（整）
    uint16_t LONGITUDE_S;   //经度（秒）
    uint8_t  ABSTIME_HH;
    uint8_t  ABSTIME_MM;
    uint8_t  ABSTIME_SS;
    uint16_t ABSTIME_MS;
}GPS_INFO;
/************************************************************!
*Function: T645结构体中CTRL位BIT定义
*Description: 参考DLT_645_2007通信协议定义的控制域中的位定义
*************************************************************/
typedef struct
{
	float    SUN_BAT; //太阳能电池电压
	float    CAP_BAT; //超级电容电池电压
	float    XDC_BAT; //蓄电池电池电压
	uint32_t TICKS;      //心跳，需维持
	uint32_t TICKS_10MS; //10ms心跳
	GPS_INFO GPS;
	uint8_t  RF_Timing;
	uint8_t  GPS_Timing;
	uint8_t  rs232_report_type;
	uint8_t  ertu_devid[6];
}SYS_INFO;
/************************************************************!
*Function: FAULT_WAVE结构体
*Description:采集单元组故障波形
*************************************************************/
typedef struct
{
	uint16_t    CNT;
	uint8_t     SECOND;
	uint8_t     MINUTE;
	uint8_t     HOUR;
	uint8_t     DAY;
	uint8_t     MONTH;
	uint8_t     YEAR;

	uint8_t     PHASE[6];

	uint8_t     VOL[MAX_WAVE_LEN];
	uint8_t     CUR[MAX_WAVE_LEN];
}FAULT_WAVE;
/************************************************************!
*Function: FAULT_INFO结构体
*Description:采集单元故障信息
*************************************************************/
typedef union
{
	uint8_t FAULT[3*(MAX_WAVE_LEN*2+4)+4];
	struct
	{
		uint8_t     LINE;
		uint8_t     WAVENUM;
		uint16_t    POINTER;
		FAULT_WAVE  WAVE[3];
	}F;
}FAULT_INFO;
/************************************************************!
*Function: YC_INFO结构体
*Description:
*************************************************************/
typedef union
{
	uint8_t YCDATA[8];
	struct
	{
		uint16_t	I;  //电流
		uint16_t	U;  //电场
		uint16_t	T;  //线温
		uint16_t	B;  //电池电压
	}Y;                 //B:字节
}YC_DATA;
/************************************************************!
*Function: YC_INFO结构体
*Description:
*************************************************************/
typedef union
{
	uint8_t YCINFO[22];
	struct
	{
		uint16_t  CNT;
		uint8_t   SECOND;
		uint8_t   MINUTE;
		uint8_t   HOUR;
		uint8_t   DAY;
		uint8_t   MONTH;
		uint8_t   YEAR;

		uint8_t PHASE[6];

		YC_DATA YC;
	}U;
}YC_INFO;
/************************************************************!
*Function: YC_INFO结构体
*Description:
*************************************************************/
typedef union
{
	uint8_t FORMAT;
	struct
	{
		uint8_t SHUNSHI:1;    //瞬时性故障位
		uint8_t YONGJIU:1;    //永久性故障位
		uint8_t RES:5;        //保留
		uint8_t BOXING:1;     //波形位
	}F;                       //D:位
}SOE_FORMAT_t;
/************************************************************!
*Function: YC_INFO结构体
*Description:
*************************************************************/
typedef union
{
	uint8_t SOE[6];
	struct
	{
		uint8_t   TIME[4];
		uint8_t   TYPE;
		SOE_FORMAT_t  FORMAT;
	}S;
}SOE_INFO;

typedef enum
{
	JIEDI=1,         //接地
	DIANLIANGDI=2,   //电池电量低
	DUANLU=3,        //短路
}SOE_TYPE_t;


typedef struct
{
	uint8_t ADDR[6];
}SYS_ADDR_INFO;

typedef union
{
	uint8_t SYSADDR[18];
	struct
	{
		SYS_ADDR_INFO LOCAL;
		SYS_ADDR_INFO WEIHU;
		SYS_ADDR_INFO BRDCT;
	}S;
}SYS_ADDR;


uint8_t getLine_Info_Line(LINE_INFO *LI,uint8_t *addr);
uint8_t getLine_Info_Lineno(LINE_INFO *LI,uint8_t *addr);
uint8_t getLine_Info_Phase(LINE_INFO *LI,uint8_t *addr);
uint8_t setLine_Info(LINE_INFO *LI,uint8_t line,uint8_t *a,uint8_t *b,uint8_t *c);
uint8_t delLine_Info(LINE_INFO *LI,uint8_t line);
uint8_t serAU_Info(LINE_INFO *LI);


#endif
