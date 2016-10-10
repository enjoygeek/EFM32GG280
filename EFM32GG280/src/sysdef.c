/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: sysdef.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "sysdef.h"


PDUFRAME USART0_RX_PF;
PDUFRAME USART0_TX_PF;
PDUFRAME RADIO_TX_PF;
PDUFRAME RADIO_RX_PF;

CircleQueue RADIO_RXCQ;
CircleQueue RADIO_TXCQ;
CircleQueue GPS_RXCQ;
CircleQueue WEIHU_RXCQ;
CircleQueue WEIHU_TXCQ;

uint8_t GPS_Interval=15; //单位分钟
uint8_t Radio_Flag=1;
FLASH_POINTER gFLASH_POINTER;
SOE_INFO gSOE_INFO;
SYS_INFO gSYS_INFO;
/**********线信息结构体**********/
LINE_INFO gLINE_INFO;
/**********信息结构体**********/
FAULT_INFO gFAULT_INFO;
/**********遥信信息结构体**********/
YC_INFO gYC_INFO;
/**********系统信息结构体**********/
SYS_TIME gSYS_TIME;
/**********射频通信通道变量**********/
uint8_t  Rf_Channel=8;
/**********本机地址**********/
uint8_t LOCAL_ADDR[6]={0x1,0x1,0x1,0x1,0x1,0x1};
/**********广播地址**********/
uint8_t BROADCAST_ADDR[6]={0xff,0xff,0xff,0xff,0xff,0xff};
/**********维护地址**********/
uint8_t WEIHU_ADDR[6]={0x0,0x0,0x0,0x0,0x0,0x0};


