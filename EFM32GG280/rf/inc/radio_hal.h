/*************************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: radio_hal.h
*Version: 0.1.0
*Date: 2016-08-26
*Author: WANGJIANHUI
*Description:
*************************************************************************/
#ifndef RADIO_HAL_H
#define RADIO_HAL_H


/**********LDCģʽ����ӦAPIʹ�ܺ궨��**********/
//#define LDC_MODE
/**********�ظ������չ��˺궨��**********/
//#define PAKETFILTER
/**********RADIO�����շ���󳤶Ⱥ�**********/
#define RADIO_MAX_PACKET_LENGTH     64
/**********CTS������**********/
#define RADIO_CTS_TIMEOUT           10000


#define SILABS_RADIO_SI446X
#undef  SILABS_RADIO_SI4455

#define RADIO_DRIVER_EXTENDED_SUPPORT
#define RADIO_DRIVER_FULL_SUPPORT


#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "em_gpio.h"
#include "rf_spi.h"
#include "radio_config.h"
#include "si446x_defs.h"
#include "si446x_api_lib.h"
#include "si446x_patch.h"
/************************************************************!
*Function: RADIO���ݽ��սṹ��
*Description:�˽ṹ����RADIO���������Ψһ�����ݽ��սӿ�
*************************************************************/
typedef struct
{
	uint8_t lens; //���յ������ݳ���
	uint8_t data[RADIO_MAX_PACKET_LENGTH];
}Radio_Packet;
/************************************************************!
*Function: tRadioConfiguration
*Description:�˽ṹ������RADIO��ʼ���Ĵ�������
*************************************************************/
typedef struct
{
    uint8_t   *Radio_ConfigurationArray;
    uint8_t   Radio_ChannelNumber;
    uint8_t   Radio_PacketLength;
    uint8_t   Radio_State_After_Power_Up;
    uint16_t  Radio_Delay_Cnt_After_Reset;
} tRadioConfiguration;

/************************************************************!
*Function: RADIO�������ݽӿ�
*Description:
*************************************************************/
extern Radio_Packet RadioPkt;
extern uint8_t Radio_Configuration_Data_Array[];
extern uint8_t radioCmd[16u];


void Radio_PowerUp(void);
void Radio_Init(void);

void Radio_StartRx(uint8_t channel,uint8_t lens);
void Radio_LDC_StartRx(uint8_t channel,uint8_t lens);

void Radio_StartTx(uint8_t channel,uint8_t *data,uint8_t lens);
uint8_t Radio_LongTx(uint8_t channel,uint8_t *data,uint8_t fixlens,uint32_t totallens);
uint8_t Radio_Tx_Repeat(uint8_t channel,uint8_t *data,uint8_t lens,uint32_t cnt);

uint8_t Radio_Check_Received(void);
uint8_t Radio_Check_Transmitted(void);
uint8_t Radio_Wait_Transmitted(void);
uint8_t Radio_Check_Packet(uint8_t *data,uint8_t lens);
void Enter_LDCRx_Mode(void);
void Exit_LDCRx_Mode(void);

void Radio_Set_LDC_Time(uint8_t wut_m_15_8,uint8_t wut_m_7_0,uint8_t wut_r,uint8_t wut_ldc);
void Radio_Wait_Channel_Idle(void);
void Radio_Set_RSSI_Thresh(uint8_t thresh);
void Radio_Set_TX_FIFOAE_Thresh(uint8_t thresh);
uint8_t Radio_Set_Preamble(uint8_t format,uint8_t lens);
uint8_t Radio_Cmd(uint8_t *data,uint8_t lens);
void Radio_StartSleep(void);
void Radio_ReadyTx(void);

void radio_hal_AssertShutdown(void);
void radio_hal_DeassertShutdown(void);
void radio_hal_ClearNsel(void);
void radio_hal_SetNsel(void);
uint8_t radio_hal_NirqLevel(void);

void radio_hal_SpiWriteByte(uint8_t byteToWrite);
uint8_t radio_hal_SpiReadByte(void);
void radio_hal_SpiWriteData(uint8_t byteCount, uint8_t* pData);
void radio_hal_SpiReadData(uint8_t byteCount, uint8_t* pData);

void radio_comm_SendCmd(uint8_t byteCount, uint8_t* pData);
void radio_comm_ReadData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData);
void radio_comm_WriteData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData);
uint8_t radio_comm_GetResp(uint8_t byteCount, uint8_t* pData);
uint8_t radio_comm_SendCmdGetResp(uint8_t cmdByteCount, uint8_t* pCmdData, uint8_t respByteCount, uint8_t* pRespData);
void radio_comm_ClearCTS(void);
uint8_t radio_comm_PollCTS(void);

uint8_t radio_hal_Gpio0Level(void);
uint8_t radio_hal_Gpio1Level(void);
uint8_t radio_hal_Gpio2Level(void);
uint8_t radio_hal_Gpio3Level(void);


#endif

