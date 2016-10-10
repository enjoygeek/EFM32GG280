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


/**********�㼯��Ԫ���ص��������(�ɼ���Ԫ����)**********/
#define MAX_LINE_GROUP         6
/**********�ɼ���Ԫ��ַ����**********/
#define AU_ADDR_LEN            6
/**********¼������֡�����֡��**********/
#define MAX_WAVE_FRAME_NUM     100
/**********�ɼ���Ԫ¼�����ݵ���󳤶�(�ֽ�)**********/
#define MAX_WAVE_LEN           3200

/************************************************************!
*Function: AU_INFO�ṹ��
*Description:�ɼ���Ԫ��Ϣ������Ϣ����չ
*************************************************************/
typedef struct
{
	uint8_t  ADDR[AU_ADDR_LEN]; //�ɼ���Ԫ��ַ
	                            //����չ�ɼ���Ԫ��������Ϣ
}AU_INFO;
/************************************************************!
*Function: AU_GROUP�ṹ��
*Description:�㼯��Ԫ��Ҫά���Ĳɼ���Ԫ����Ϣ
*************************************************************/
typedef struct
{
	uint8_t  LINENO; //�ɼ���Ԫ�������ߺţ��ߺ�Ϊ0�������Ϊ���ߣ��޲ɼ���Ԫ��
	AU_INFO  AU[3]; //�ɼ���Ԫ��Ϣ
}LINE_GROUP;
/************************************************************!
*Function: LINE_INFO�ṹ��
*Description:����Ϣ
*************************************************************/
typedef struct
{
	LINE_GROUP LINE[MAX_LINE_GROUP];
}LINE_INFO;

/************************************************************!
*Function: Sys_Time�ṹ��
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
*Function: GPS_Info�ṹ��
*Description:
*************************************************************/
typedef struct
{
    int8_t   NS_LOC;        //��/��γ
	int8_t   EW_LOC;        //��/����
    uint16_t LATITUDE;      //γ�ȣ�����
    uint16_t LATITUDE_S;    //γ�ȣ��룩
    uint16_t LONGITUDE;     //���ȣ�����
    uint16_t LONGITUDE_S;   //���ȣ��룩
    uint8_t  ABSTIME_HH;
    uint8_t  ABSTIME_MM;
    uint8_t  ABSTIME_SS;
    uint16_t ABSTIME_MS;
}GPS_INFO;
/************************************************************!
*Function: T645�ṹ����CTRLλBIT����
*Description: �ο�DLT_645_2007ͨ��Э�鶨��Ŀ������е�λ����
*************************************************************/
typedef struct
{
	float    SUN_BAT; //̫���ܵ�ص�ѹ
	float    CAP_BAT; //�������ݵ�ص�ѹ
	float    XDC_BAT; //���ص�ص�ѹ
	uint32_t TICKS;      //��������ά��
	uint32_t TICKS_10MS; //10ms����
	GPS_INFO GPS;
	uint8_t  RF_Timing;
	uint8_t  GPS_Timing;
	uint8_t  rs232_report_type;
	uint8_t  ertu_devid[6];
}SYS_INFO;
/************************************************************!
*Function: FAULT_WAVE�ṹ��
*Description:�ɼ���Ԫ����ϲ���
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
*Function: FAULT_INFO�ṹ��
*Description:�ɼ���Ԫ������Ϣ
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
*Function: YC_INFO�ṹ��
*Description:
*************************************************************/
typedef union
{
	uint8_t YCDATA[8];
	struct
	{
		uint16_t	I;  //����
		uint16_t	U;  //�糡
		uint16_t	T;  //����
		uint16_t	B;  //��ص�ѹ
	}Y;                 //B:�ֽ�
}YC_DATA;
/************************************************************!
*Function: YC_INFO�ṹ��
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
*Function: YC_INFO�ṹ��
*Description:
*************************************************************/
typedef union
{
	uint8_t FORMAT;
	struct
	{
		uint8_t SHUNSHI:1;    //˲ʱ�Թ���λ
		uint8_t YONGJIU:1;    //�����Թ���λ
		uint8_t RES:5;        //����
		uint8_t BOXING:1;     //����λ
	}F;                       //D:λ
}SOE_FORMAT_t;
/************************************************************!
*Function: YC_INFO�ṹ��
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
	JIEDI=1,         //�ӵ�
	DIANLIANGDI=2,   //��ص�����
	DUANLU=3,        //��·
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
