/*************************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: radio_hal.c
*Version: 0.1.0
*Date: 2016-08-26
*Author: WANGJIANHUI
*Description:
*************************************************************************/
#include "radio_hal.h"

/**********RADIO数据收发数组结构体**********/
Radio_Packet RadioPkt;

uint8_t ctsWentHigh = 0;
uint8_t Radio_Configuration_Data_Array[] = RADIO_CONFIGURATION_DATA_ARRAY;
tRadioConfiguration RadioConfiguration = RADIO_CONFIGURATION_DATA;
tRadioConfiguration *pRadioConfiguration = &RadioConfiguration;
/************************************************************!
*Function: Radio_PowerUp
*Description: RADIO硬件上电复位
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
*Description: RADIO硬件上电复位，并将radio_config.h生成的寄存器初
*             始化数组发送至RADIO，若初始化失败，重新上电复位初始化。
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
*Description: RADIO开始接收数据函数
*Input: 1.channel：接收数据的信道
*       2.lens：待接收的数据长度
*Output: null
*Return: null
*************************************************************/
void Radio_StartRx(uint8_t channel,uint8_t lens)
{
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT); //复位FIFO

	si446x_get_int_status(0u, 0u, 0u);

	si446x_start_rx(channel,0u,lens,
                  SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_RX,
                  SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX);
}
/************************************************************!
*Function: Radio_LDC_StartRx
*Description: RADIO在LDC模式下开始接收数据函数
*Input: 1.channel：接收数据的信道
*       2.lens：待接收的数据长度
*Output: null
*Return: null
*************************************************************/
void Radio_LDC_StartRx(uint8_t channel,uint8_t lens)
{
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT); //复位FIFO

	si446x_get_int_status(0u, 0u, 0u);

	si446x_start_rx(channel,0u,lens,
			      SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_SLEEP,
				  SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_SLEEP,
				  SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX);
}
/************************************************************!
*Function: Radio_StartTx
*Description: RADIO数据发送函数
*Input: 1.channel：发送数据的信道
*       2.data： 待发送数据的首地址
*       3.lens：待发送数据的长度
*Output: null
*Return: null
*************************************************************/
void Radio_StartTx(uint8_t channel,uint8_t *data,uint8_t lens)
{
	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_READY);

	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT); //复位FIFO

	si446x_get_int_status(0u,0u,0u);

	si446x_write_tx_fifo(lens,data);

	si446x_start_tx(channel,0X30,lens);
}
/************************************************************!
*Function: Radio_LongTx
*Description: RADIO长包发送函数。此函数会将uint32_t长度之内的长包拆
*             包成每包fixlens个字节发送，
*Input: 1.channel：发送数据的信道
*       2.data： 待发送数据的首地址
*       3.fixlens：每包发送的长度
*       4.totallens：待发送的数据总长度
*Output: null
*Return: 函数运行状态
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
*Description: RADIO重复发包函数
*Input: 1.channel：发送数据的信道
*       2.data： 待发送数据的首地址
*       3.lens：待发送数据的长度
*       4.cnt：重复发包次数
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
*Description: 等待RADIO发送数据完成。
*Input: null
*Output: null
*Return: 函数运行状态
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
*Description: 检测RADIO发送是否完成,此版API函数取消中断后检测NIRQ电
*             平操作，直接读取中断寄存器，判断中断类型。
*Input: null
*Output: null
*Return: 函数运行状态
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
*Description: 检测RADIO是否接收到数据，若接收到数据，返回数据长度。
*             此版API函数取消中断后检测NIRQ电平操作，直接读取中断寄
*             存器，判断中断类型。
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
		si446x_fifo_info(0u);    //读取RX_FIFO数据长度

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
*Description: 收到一个包，判断此包长度与上次接收的包长度是否一致，若
*             长度一致，判断内容是否一致，内容一致则为重复包，丢弃，返
*             回false。内容不一致，则认为是新包，返回true，若包长度不
*             一致，则认为是新包，返回true。
*Input: 1.data： 接收数据的首地址
*       2.lens：接收数据的长度
*Output: null
*Return: 函数运行状态
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
*Description: RADIO进入LDC接收数据模式,重新配置LDC各寄存器值。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Enter_LDCRx_Mode(void)
{
	uint8_t cmd[5]={0x11, 0x00, 0x01, 0x04, 0x42};  //开启WUT_LDC

	Radio_Cmd(cmd,0x05);
}
/************************************************************!
*Function: Exit_LDCRx_Mode
*Description: 退出LDC接收数据模式。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Exit_LDCRx_Mode(void)
{
	uint8_t cmd[5]={0x11, 0x00, 0x01, 0x04, 0x02}; //关闭LDC使能，不需要关闭WUT

	Radio_Cmd(cmd,0x05);
}
/************************************************************!
*Function: Radio_Set_LDC_Time
*Description: RADIO设置LDC的RX和SLEEP周期。各参数配置请参考数据手册。
*Input:
*Output: null
*Return: null
*************************************************************/
void Radio_Set_LDC_Time(uint8_t wut_m_15_8,uint8_t wut_m_7_0,
		                uint8_t wut_r,uint8_t wut_ldc)
{
	uint8_t cmd[5]={0x11, 0x00, 0x01, 0x04, 0x00};
	uint8_t cmd1[8]={0x11, 0x00, 0x04, 0x05, 0x00,0x00,0x00,0x00};

	Radio_Cmd(cmd,0x05); //配置LDC寄存器前先关闭LDC模式！！

	cmd1[4]=wut_m_15_8;
	cmd1[5]=wut_m_7_0;
	cmd1[6]=wut_r;
	cmd1[7]=wut_ldc;

	Radio_Cmd(cmd1,0x08);
}
/************************************************************!
*Function: Radio_Wait_Channel_Idle
*Description: RADIO等待信道是否空闲。GPIO管脚为高电平时判定信道被占
*             用，为低电平时则认为信道空闲，可以发送数据，此函数需要
*             配合Set_RSSI_Thresh函数使用，需要先设置RSSI的阈值。
*             此函数会比对RSSI阈值判断信道是否空闲。
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
*Description: RADIO设置RSSI阈值,用于信道检测的阈值判断。
*Input: thresh：RSSI阈值
*Output: null
*Return: null
*************************************************************/
void Radio_Set_RSSI_Thresh(uint8_t thresh)
{
	uint8_t cmd[5]={0x11, 0x20, 0x01, 0x4a, 0x03};  //设置RSSI阈值命令

	cmd[4]=thresh;

	Radio_Cmd(cmd,0x05);
}
/************************************************************!
*Function: Radio_Set_TX_FIFOAE_Thresh
*Description: RADIO设置TX_FIFO_ALMOST_EMPTY阈值。
*Input: thresh：阈值
*Output: null
*Return: null
*************************************************************/
void Radio_Set_TX_FIFOAE_Thresh(uint8_t thresh)
{
	uint8_t cmd[5]={0x11, 0x20, 0x01, 0x4a, 0x30}; //设置RSSI阈值

	cmd[4]=thresh;

	Radio_Cmd(cmd,0x05);
}
/************************************************************!
*Function: Radio_Set_Preamble
*Description: RADIO设置前导码标准格式及长度。
*Input: 1.format: 0:010101...
*                 1:101010...
*       2.lens：前导码长度,单位byte
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
*Description: RADIO命令设置
*Input: 1.data：数据首地址
*       2.lens：数据长度
*Output: null
*Return: 函数运行状态
*************************************************************/
uint8_t Radio_Cmd(uint8_t *data,uint8_t lens)
{
	if (radio_comm_SendCmdGetResp(lens,data,0,0)!=0xFF)
		return false;

	return true;
}
/************************************************************!
*Function: Radio_StartSleep
*Description: RADIO睡眠状态设置
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
*Description: RADIO数据发送状态设置
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
*Description: RADIO得到应答数据函数
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
*Description: RADIO发送命令数据函数
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
*Description: RADIO 读取命令数据函数
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
*Description: RADIO写命令数据函数
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

