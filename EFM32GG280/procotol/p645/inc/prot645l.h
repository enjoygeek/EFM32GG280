/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: prot645l.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description: ��ԼV1.0.3����
*******************************************************************/
#ifndef PROT645L_H_
#define PROT645L_H_


#include <stdint.h>

/**********645Э����غ궨��(��ԼV1.0.3)**********/
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
#define	PDULEN             242   //֡������PDU��󳤶� 255-13
#define LPDUSIZE       	   261   //��·�㱨�ĵ���󳤶�255+6
/************************************************************!
*Function: CTRL������
*Description: ��ָͨ�Ź�Լ��CONTROLλ�����嶨��(��ԼV1.0.3)
*************************************************************/
typedef union
{
	uint8_t CTRL;
	struct
	{
		uint8_t CC:4;       //������
		uint8_t SFV:1;      //����֡��־
		uint8_t FCB:1;      //����λ
		uint8_t PRM:1;      //��վӦ���־
		uint8_t DIR:1;      //���ͷ���
	}C;                     //b:λ
}CTRL_t;
/************************************************************!
*Function: PDUFRAME������
*Description: ��ָͨ�Ź�Լ��PDUFRAME����(��ԼV1.0.3)
*************************************************************/
typedef union
{
	uint8_t PDUFRAME[LPDUSIZE+2];
	struct
	{
		uint8_t  SF1;                    //��ʼ�ֽ�
		uint8_t  LEN1;    		         //����1
		uint8_t  LEN2;    		         //����2
		uint8_t  SF2;                    //��ʼ�ֽ�
		CTRL_t   CTRL;                   //������
		uint8_t  SRCADDR[ADDR_LEN];      //Դ��ַ
		uint8_t  DSTADDR[ADDR_LEN];	     //Ŀ�ĵ�ַ
		uint8_t  TYPE;                   //��������
		uint8_t  FRAMENUM;               //֡����
		uint8_t  FRAMESEQ;               //֡���
		uint8_t  DATA[MAX_DATA_LEN];     //��������
		uint8_t  CS;                    //У����
		uint8_t  EF;                     //�����ַ�

		uint8_t  ISLOCK;                 //�ṹ��ռ�ñ�־
		uint8_t  PARSE;                  //���ݽ�����־
	}PF;                                  //B���ֽ�
}PDUFRAME;
/**********PRMö��**********/
typedef enum
{
	PRMSEC=0, //������
	PRMPRI=1, //�Ӷ���
}PRM_t;
/**********�Ӷ��߹����붨��PRM=0**********/
typedef enum
{
	FCCON=0,     //ȷ��
	FCNOCON=1,   //ȷ��
	FCRESDATA=2, //��Ӧ֡
	FCNODATA=3,  //��Ӧ֡
}CC0_t;
/**********�����߹����붨��PRM=1**********/
typedef enum
{
	FCRESETUSE=0,    //����/��Ӧ��
	FCRESETFAULT=1,  //����/ȷ��
	FCSENDDATANA=2,  //����/��Ӧ��
	FCSENDDATA=3,    //����/ȷ��
	FCREQUESTD=4,    //����/��Ӧ
}CC1_t;
/**********���ͱ�ʶ����**********/
typedef enum
{
	TIMESTAMP=1,    //ʱ��
	DBYC=2,         //ң��
	DBYXTIM=3,      //SOE
	DBWAVE=4,       //����¼������
	DBPARA=5,       //����
	DBPROG=6,       //����
	FAULTWRG=7,     //����¼��
	LED=8,          //��Ʋ���
	FANPAI=9,       //���Է���
	WAKEUP=10,      //����(�Բɼ���Ԫ��Ч)
	PHYID=11,       //�����ַ
}TYPE_t;


#endif

