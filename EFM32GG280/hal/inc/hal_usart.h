/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_usart.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_USART_H_
#define HAL_USART_H_


#include <stddef.h>
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_usart.h"
#include "em_leuart.h"

/**********函数指针回调声明**********/
typedef void(*USART0_RX_CB)(uint8_t *data,uint8_t lens);
/**********函数指针回调声明**********/
typedef void(*LEUART0_RX_CB)(void);
/**********函数指针回调声明**********/
typedef void(*LEUART0_TX_CB)(void);
/**********函数指针回调声明**********/
typedef void(*UART0_RX_CB)(void);
/**********函数指针回调声明**********/
typedef void(*UART0_TX_CB)(void);


/**********GPRS端口定义**********/
#define TX_WEIHU_PORT         gpioPortE
#define TX_WEIHU_PIN          10
#define RX_WEIHU_PORT         gpioPortE
#define RX_WEIHU_PIN          11

/**********GPRS端口定义**********/
#define TX_GPRS_PORT         gpioPortF
#define TX_GPRS_PIN          6
#define RX_GPRS_PORT         gpioPortF
#define RX_GPRS_PIN          7

/**********GPS端口定义**********/
#define TX_GPS_PORT          gpioPortE
#define TX_GPS_PIN           14
#define RX_GPS_PORT          gpioPortE
#define RX_GPS_PIN           15


void Usart0_Init(uint32_t baudrate);
void Usart0_TxU8(uint8_t data);
void Usart0_TxU32(uint32_t data);
void Uart0_Init(uint32_t baudrate);
void Uart0_TxU8(uint8_t data);
void Uart0_TxU32(uint32_t data);
void Uart0_Rx_Register(UART0_RX_CB func);
void Uart0_Tx_Register(UART0_TX_CB func);
void Leuart0_Init(uint32_t baudrate);
void Leuart0_TxU8(uint8_t data);
void Usart0_Rx_Register(USART0_RX_CB func);
void Leuart0_Rx_Register(LEUART0_RX_CB func);
void Leuart0_Tx_Register(LEUART0_TX_CB func);
void LEUART0_IRQHandler(void);
void UART0_RX_IRQHandler(void);
void UART0_TX_IRQHandler(void);


#endif

