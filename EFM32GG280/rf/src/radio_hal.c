/*************************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: radio_hal.c
*Version: 0.1.0
*Date: 2016-08-26
*Author: WANGJIANHUI
*Description:
*************************************************************************/
#include "radio_hal.h"

/**********RADIO�����շ�����ṹ��**********/
Radio_Packet RadioPkt;

uint8_t ctsWentHigh = 0;
uint8_t Radio_Configuration_Data_Array[] = RADIO_CONFIGURATION_DATA_ARRAY;
tRadioConfiguration RadioConfiguration = RADIO_CONFIGURATION_DATA;
tRadioConfiguration *pRadioConfiguration = &RadioConfiguration;
/************************************************************!
*Function: Radio_PowerUp
*Description: RADIOӲ���ϵ縴λ
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_PowerUp(void)
{
	si446x_reset();

	for(uint16_t i=0;i<0xffff;i++);
}
/************************************************************!
*Function: Radio_Init
*Description: RADIOӲ���ϵ縴λ������radio_config.h���ɵļĴ�����
*             ʼ�����鷢����RADIO������ʼ��ʧ�ܣ������ϵ縴λ��ʼ����
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_Init(void)
{
	Radio_PowerUp();

	while(SI446X_SUCCESS!=si446x_configuration_init(pRadioConfiguration->Radio_ConfigurationArray))
	{
		for(uint16_t i=0;i<10000;i++);
		Radio_PowerUp();
	}

	si446x_get_int_status(0u, 0u, 0u);
}
/************************************************************!
*Function: Radio_StartRx
*Description: RADIO��ʼ�������ݺ���
*Input: 1.channel���������ݵ��ŵ�
*       2.lens�������յ����ݳ���
*Output: null
*Return: null
*************************************************************/
void Radio_StartRx(uint8_t channel,uint8_t lens)
{
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT); //��λFIFO

	si446x_get_int_status(0u, 0u, 0u);

	si446x_start_rx(channel,0u,lens,
                  SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_RX,
                  SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX);
}
/************************************************************!
*Function: Radio_LDC_StartRx
*Description: RADIO��LDCģʽ�¿�ʼ�������ݺ���
*Input: 1.channel���������ݵ��ŵ�
*       2.lens�������յ����ݳ���
*Output: null
*Return: null
*************************************************************/
void Radio_LDC_StartRx(uint8_t channel,uint8_t lens)
{
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT); //��λFIFO

	si446x_get_int_status(0u, 0u, 0u);

	si446x_start_rx(channel,0u,lens,
			      SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_SLEEP,
				  SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_SLEEP,
				  SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX);
}
/************************************************************!
*Function: Radio_StartTx
*Description: RADIO���ݷ��ͺ���
*Input: 1.channel���������ݵ��ŵ�
*       2.data�� ���������ݵ��׵�ַ
*       3.lens�����������ݵĳ���
*Output: null
*Return: null
*************************************************************/
void Radio_StartTx(uint8_t channel,uint8_t *data,uint8_t lens)
{
	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_READY);

	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT); //��λFIFO

	si446x_get_int_status(0u,0u,0u);

	si446x_write_tx_fifo(lens,data);

	si446x_start_tx(channel,0X30,lens);
}
/************************************************************!
*Function: Radio_LongTx
*Description: RADIO�������ͺ������˺����Ὣuint32_t����֮�ڵĳ�����
*             ����ÿ��fixlens���ֽڷ��ͣ�
*Input: 1.channel���������ݵ��ŵ�
*       2.data�� ���������ݵ��׵�ַ
*       3.fixlens��ÿ�����͵ĳ���
*       4.totallens�������͵������ܳ���
*Output: null
*Return: ��������״̬
*************************************************************/
uint8_t Radio_LongTx(uint8_t channel,uint8_t *data,uint8_t fixlens,uint32_t totallens)
{
	uint8_t cnt=0;

	if(fixlens==0||fixlens>0x40)
		return false;

#ifdef LDC_MODE
	Exit_LDCRx_Mode();
#endif

	while(totallens>=fixlens)
	{
		Radio_StartTx(channel,(data+(cnt++)*fixlens),fixlens);
		Radio_Wait_Transmitted();
		totallens-=fixlens;
	}

	while(totallens<fixlens&&totallens>0)
	{
		Radio_StartTx(channel,(data+cnt*fixlens),fixlens);
		Radio_Wait_Transmitted();
		totallens=0;
	}

#ifdef LDC_MODE
	Enter_LDCRx_Mode();
#endif

	return true;
}
/************************************************************!
*Function: Radio_Tx_Repeat
*Description: RADIO�ظ���������
*Input: 1.channel���������ݵ��ŵ�
*       2.data�� ���������ݵ��׵�ַ
*       3.lens�����������ݵĳ���
*       4.cnt���ظ���������
*Output: null
*Return: null
*************************************************************/
uint8_t Radio_Tx_Repeat(uint8_t channel,uint8_t *data,uint8_t lens,uint32_t cnt)
{
	uint32_t flags=0;

	if(lens>0x40)
		return false;

#ifdef LDC_MODE
	Exit_LDCRx_Mode();
#endif

	while(flags++<cnt)
	{
		Radio_StartTx(channel,data,lens);
		Radio_Wait_Transmitted();
	}

#ifdef LDC_MODE
	Enter_LDCRx_Mode();
#endif

	return true;
}
/************************************************************!
*Function: Radio_Wait_Transmitted
*Description: �ȴ�RADIO����������ɡ�
*Input: null
*Output: null
*Return: ��������״̬
*************************************************************/
uint8_t Radio_Wait_Transmitted(void)
{
	while(true)
	{
		if(Radio_Check_Transmitted()==true)
		{
			return true;
		}
	}

	return false;
}
/************************************************************!
*Function: Radio_Check_Transmitted
*Description: ���RADIO�����Ƿ����,�˰�API����ȡ���жϺ���NIRQ��
*             ƽ������ֱ�Ӷ�ȡ�жϼĴ������ж��ж����͡�
*Input: null
*Output: null
*Return: ��������״̬
*************************************************************/
uint8_t Radio_Check_Transmitted(void)
{
	si446x_get_int_status(0u,0u,0u);

	if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT)
	{
		return true;
	}

	return false;
}
/************************************************************!
*Function: Radio_Check_Received
*Description: ���RADIO�Ƿ���յ����ݣ������յ����ݣ��������ݳ��ȡ�
*             �˰�API����ȡ���жϺ���NIRQ��ƽ������ֱ�Ӷ�ȡ�жϼ�
*             �������ж��ж����͡�
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t Radio_Check_Received(void)
{
#ifdef PAKETFILTER
	uint8_t RadioBuffer[RADIO_MAX_PACKET_LENGTH];
#endif

	si446x_get_int_status(0u, 0u, 0u);

	if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT)
	{
		si446x_fifo_info(0u);    //��ȡRX_FIFO���ݳ���

#ifdef PAKETFILTER
		si446x_read_rx_fifo(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT,&RadioBuffer[0]);

		if(Radio_Check_Packet(&RadioBuffer[0],Si446xCmd.FIFO_INFO.RX_FIFO_COUNT)==true)
		{
			return RadioPkt.lens;
		}
#endif

#ifndef PAKETFILTER

		RadioPkt.lens=Si446xCmd.FIFO_INFO.RX_FIFO_COUNT;

		si446x_read_rx_fifo(RadioPkt.lens,&RadioPkt.data[0]);

		return RadioPkt.lens;
#endif
	}

	return false;
}
/************************************************************!
*Function: Radio_Check_Packet
*Description: �յ�һ�������жϴ˰��������ϴν��յİ������Ƿ�һ�£���
*             ����һ�£��ж������Ƿ�һ�£�����һ����Ϊ�ظ�������������
*             ��false�����ݲ�һ�£�����Ϊ���°�������true���������Ȳ�
*             һ�£�����Ϊ���°�������true��
*Input: 1.data�� �������ݵ��׵�ַ
*       2.lens���������ݵĳ���
*Output: null
*Return: ��������״̬
*************************************************************/
uint8_t Radio_Check_Packet(uint8_t *data,uint8_t lens)
{
	if(RadioPkt.lens==lens)
	{
		for(uint8_t i=0;i<lens;i++)
		{
			if(RadioPkt.data[i]!=data[i])
			{
				memcpy(RadioPkt.data,data,lens);
				return true;
			}
		}
	}
	else
	{
		RadioPkt.lens=lens;
		memcpy(RadioPkt.data,data,lens);
		return true;
	}

	return false;
}
/************************************************************!
*Function: Enter_LDCRx_Mode
*Description: RADIO����LDC��������ģʽ,��������LDC���Ĵ���ֵ��
*Input: null
*Output: null
*Return: null
*************************************************************/
void Enter_LDCRx_Mode(void)
{
	uint8_t cmd[5]={0x11, 0x00, 0x01, 0x04, 0x42};  //����WUT_LDC

	Radio_Cmd(cmd,0x05);
}
/************************************************************!
*Function: Exit_LDCRx_Mode
*Description: �˳�LDC��������ģʽ��
*Input: null
*Output: null
*Return: null
*************************************************************/
void Exit_LDCRx_Mode(void)
{
	uint8_t cmd[5]={0x11, 0x00, 0x01, 0x04, 0x02}; //�ر�LDCʹ�ܣ�����Ҫ�ر�WUT

	Radio_Cmd(cmd,0x05);
}
/************************************************************!
*Function: Radio_Set_LDC_Time
*Description: RADIO����LDC��RX��SLEEP���ڡ�������������ο������ֲᡣ
*Input:
*Output: null
*Return: null
*************************************************************/
void Radio_Set_LDC_Time(uint8_t wut_m_15_8,uint8_t wut_m_7_0,
		                uint8_t wut_r,uint8_t wut_ldc)
{
	uint8_t cmd[5]={0x11, 0x00, 0x01, 0x04, 0x00};
	uint8_t cmd1[8]={0x11, 0x00, 0x04, 0x05, 0x00,0x00,0x00,0x00};

	Radio_Cmd(cmd,0x05); //����LDC�Ĵ���ǰ�ȹر�LDCģʽ����

	cmd1[4]=wut_m_15_8;
	cmd1[5]=wut_m_7_0;
	cmd1[6]=wut_r;
	cmd1[7]=wut_ldc;

	Radio_Cmd(cmd1,0x08);
}
/************************************************************!
*Function: Radio_Wait_Channel_Idle
*Description: RADIO�ȴ��ŵ��Ƿ���С�GPIO�ܽ�Ϊ�ߵ�ƽʱ�ж��ŵ���ռ
*             �ã�Ϊ�͵�ƽʱ����Ϊ�ŵ����У����Է������ݣ��˺�����Ҫ
*             ���Set_RSSI_Thresh����ʹ�ã���Ҫ������RSSI����ֵ��
*             �˺�����ȶ�RSSI��ֵ�ж��ŵ��Ƿ���С�
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_Wait_Channel_Idle(void)
{
	while(radio_hal_Gpio1Level()==true);
}
/************************************************************!
*Function: Radio_Set_RSSI_Thresh
*Description: RADIO����RSSI��ֵ,�����ŵ�������ֵ�жϡ�
*Input: thresh��RSSI��ֵ
*Output: null
*Return: null
*************************************************************/
void Radio_Set_RSSI_Thresh(uint8_t thresh)
{
	uint8_t cmd[5]={0x11, 0x20, 0x01, 0x4a, 0x03};  //����RSSI��ֵ����

	cmd[4]=thresh;

	Radio_Cmd(cmd,0x05);
}
/************************************************************!
*Function: Radio_Set_TX_FIFOAE_Thresh
*Description: RADIO����TX_FIFO_ALMOST_EMPTY��ֵ��
*Input: thresh����ֵ
*Output: null
*Return: null
*************************************************************/
void Radio_Set_TX_FIFOAE_Thresh(uint8_t thresh)
{
	uint8_t cmd[5]={0x11, 0x20, 0x01, 0x4a, 0x30}; //����RSSI��ֵ

	cmd[4]=thresh;

	Radio_Cmd(cmd,0x05);
}
/************************************************************!
*Function: Radio_Set_Preamble
*Description: RADIO����ǰ�����׼��ʽ�����ȡ�
*Input: 1.format: 0:010101...
*                 1:101010...
*       2.lens��ǰ���볤��,��λbyte
*Output: null
*Return: null
*************************************************************/
uint8_t Radio_Set_Preamble(uint8_t format,uint8_t lens)
{
	if((format!=0&&format!=1)||lens==0||lens>0xff)
		return false;

	uint8_t cmd[9]={0x11,0x10,0x05,0x00,0x08,0x14,0x00,0x0f,0x31};

	cmd[4]=lens;

	if(format==0)
		cmd[8]=0x12;

	if(format==1)
		cmd[8]=0x31;

	Radio_Cmd(cmd,0x09);

	return true;
}
/************************************************************!
*Function: Radio_Cmd
*Description: RADIO��������
*Input: 1.data�������׵�ַ
*       2.lens�����ݳ���
*Output: null
*Return: ��������״̬
*************************************************************/
uint8_t Radio_Cmd(uint8_t *data,uint8_t lens)
{
	if (radio_comm_SendCmdGetResp(lens,data,0,0)!=0xFF)
		return false;

	return true;
}
/************************************************************!
*Function: Radio_StartSleep
*Description: RADIO˯��״̬����
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_StartSleep(void)
{
  si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);
}
/************************************************************!
*Function: Radio_ReadyTx
*Description: RADIO���ݷ���״̬����
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_ReadyTx(void)
{
  si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_READY);
}
/************************************************************!
*Function: radio_comm_GetResp
*Description: RADIO�õ�Ӧ�����ݺ���
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_comm_GetResp(uint8_t byteCount, uint8_t* pData)
{
	uint8_t ctsVal = 0;
	uint16_t errCnt = RADIO_CTS_TIMEOUT;

	while (errCnt != 0)      //wait until radio IC is ready with the data
	{
		radio_hal_ClearNsel();
		radio_hal_SpiWriteByte(0x44);    //read CMD buffer
		ctsVal = radio_hal_SpiReadByte();
		if (ctsVal == 0xFF)
		{
			if (byteCount)
			{
				radio_hal_SpiReadData(byteCount, pData);
			}
			radio_hal_SetNsel();
			break;
		}
		radio_hal_SetNsel();
		errCnt--;
	}
	if (errCnt == 0)
	{
		while(1);
	}
	if (ctsVal == 0xFF)
	{
		ctsWentHigh = 1;
	}
	return ctsVal;
}
/************************************************************!
*Function: radio_comm_SendCmd
*Description: RADIO�����������ݺ���
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_comm_SendCmd(uint8_t byteCount, uint8_t* pData)
{
	/* There was a bug in A1 hardware that will not handle 1 byte commands.
	It was supposedly fixed in B0 but the fix didn't make it at the last minute, so here we go again */
    if (byteCount == 1)
    	byteCount++;

	while (!ctsWentHigh)
    {
		radio_comm_PollCTS();
    }
    radio_hal_ClearNsel();
    radio_hal_SpiWriteData(byteCount, pData);
    radio_hal_SetNsel();
    ctsWentHigh = 0;
}
/************************************************************!
*Function: radio_comm_ReadData
*Description: RADIO ��ȡ�������ݺ���
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_comm_ReadData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData)
{
	if(pollCts)
	{
		while(!ctsWentHigh)
        {
			radio_comm_PollCTS();
        }
    }
    radio_hal_ClearNsel();
    radio_hal_SpiWriteByte(cmd);
    radio_hal_SpiReadData(byteCount, pData);
    radio_hal_SetNsel();
    ctsWentHigh = 0;
}
/************************************************************!
*Function: radio_comm_WriteData
*Description: RADIOд�������ݺ���
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_comm_WriteData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData)
{
	if(pollCts)
	{
		while(!ctsWentHigh)
        {
			radio_comm_PollCTS();
        }
    }
	radio_hal_ClearNsel();
    radio_hal_SpiWriteByte(cmd);
    radio_hal_SpiWriteData(byteCount, pData);
    radio_hal_SetNsel();
    ctsWentHigh = 0;
}
/************************************************************!
*Function: radio_comm_PollCTS
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_comm_PollCTS(void)
{
	#ifdef RADIO_USER_CFG_USE_GPIO1_FOR_CTS
		while(!radio_hal_Gpio1Level())
		{
			/* Wait...*/
		}
		ctsWentHigh = 1;
		return 0xFF;
	#else
		return radio_comm_GetResp(0, 0);
	#endif
}
/************************************************************!
*Function: radio_comm_ClearCTS
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_comm_ClearCTS()
{
	ctsWentHigh = 0;
}
/************************************************************!
*Function: radio_comm_SendCmdGetResp
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_comm_SendCmdGetResp(uint8_t cmdByteCount, uint8_t* pCmdData, uint8_t respByteCount, uint8_t* pRespData)
{
	radio_comm_SendCmd(cmdByteCount, pCmdData);
	return radio_comm_GetResp(respByteCount, pRespData);
}
/************************************************************!
*Function: radio_hal_AssertShutdown
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_hal_AssertShutdown(void)
{
	GPIO_PinOutSet(RF_SDN_PORT, RF_SDN_PIN);
}
/************************************************************!
*Function: radio_hal_DeassertShutdown
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_hal_DeassertShutdown(void)
{
	GPIO_PinOutClear(RF_SDN_PORT, RF_SDN_PIN);
}
/************************************************************!
*Function: radio_hal_ClearNsel
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_hal_ClearNsel(void)
{
    GPIO_PinOutClear(SPI_CS_PORT, SPI_CS_PIN);
}
/************************************************************!
*Function: radio_hal_SetNsel
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_hal_SetNsel(void)
{
	GPIO_PinOutSet(SPI_CS_PORT, SPI_CS_PIN);
}
/************************************************************!
*Function: radio_hal_NirqLevel
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_hal_NirqLevel(void)
{
	return GPIO_PinInGet(RF_nIRQ_PORT, RF_nIRQ_PIN);
}
/************************************************************!
*Function: radio_hal_SpiWriteByte
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_hal_SpiWriteByte(uint8_t byteToWrite)
{
	Spi_ReadWrite(byteToWrite);
}
/************************************************************!
*Function: radio_hal_SpiReadByte
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_hal_SpiReadByte(void)
{
	return Spi_ReadWrite(0x00);
}
/************************************************************!
*Function: radio_hal_SpiWriteData
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_hal_SpiWriteData(uint8_t byteCount, uint8_t* pData)
{
	Spi_WriteDataBurst(byteCount, pData);
}
/************************************************************!
*Function: radio_hal_SpiReadData
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_hal_SpiReadData(uint8_t byteCount, uint8_t* pData)
{
	Spi_ReadDataBurst(byteCount, pData);
}
/************************************************************!
*Function: radio_hal_Gpio0Level
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
#ifdef RADIO_DRIVER_EXTENDED_SUPPORT
uint8_t radio_hal_Gpio0Level(void)
{
	return GPIO_PinInGet(RF_GPIO0_PORT, RF_GPIO0_PIN);
}
/************************************************************!
*Function: radio_hal_Gpio1Level
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_hal_Gpio1Level(void)
{
    return GPIO_PinInGet(RF_GPIO1_PORT, RF_GPIO1_PIN);
}
/************************************************************!
*Function: radio_hal_Gpio2Level
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_hal_Gpio2Level(void)
{
	return GPIO_PinInGet(RF_GPIO2_PORT, RF_GPIO2_PIN);
}
/************************************************************!
*Function: radio_hal_Gpio3Level
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_hal_Gpio3Level(void)
{
	return GPIO_PinInGet(RF_GPIO3_PORT, RF_GPIO3_PIN);
}
#endif

