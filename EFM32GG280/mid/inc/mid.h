/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: mid.h
*Version: 0.1.0
*Date: 2016-09-20
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef MID_H_
#define MID_H_


void CircleQueue_Init(void);
void Radio_INT_Process(void);
void Radio_Direct_Tx(uint8_t *data, uint8_t lens);
void Radio_Queue_Tx(void);
void Usart0_Queue_Rx(uint8_t *data, uint8_t lens);
void Usart0_Queue_Tx(void);
void Leuart0_Queue_Rx(void);
void Leuart0_Queue_Tx(void);


#endif

