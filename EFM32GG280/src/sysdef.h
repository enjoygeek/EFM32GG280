/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: sysdef.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef SYSDEF_H_
#define SYSDEF_H_

#include <stdint.h>
#include "include.h"





extern uint8_t GPS_Interval;
extern uint8_t Radio_Flag;
extern FLASH_POINTER gFLASH_POINTER;
extern SOE_INFO gSOE_INFO;
extern PDUFRAME USART0_RX_PF;
extern PDUFRAME USART0_TX_PF;
extern PDUFRAME RADIO_TX_PF;
extern PDUFRAME RADIO_RX_PF;

extern CircleQueue RADIO_RXCQ;
extern CircleQueue RADIO_TXCQ;
extern CircleQueue GPS_RXCQ;
extern CircleQueue WEIHU_RXCQ;
extern CircleQueue WEIHU_TXCQ;

extern SYS_TIME gSYS_TIME;
extern LINE_INFO gLINE_INFO;
extern FAULT_INFO gFAULT_INFO;
extern YC_INFO gYC_INFO;
extern uint8_t  Rf_Channel;
extern SYS_INFO gSYS_INFO;
extern uint8_t LOCAL_ADDR[6];
extern uint8_t BROADCAST_ADDR[6];
extern uint8_t WEIHU_ADDR[6];



#endif

