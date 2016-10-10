/*************************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: rf_spi.c
*Version: 0.1.0
*Date: 2016-08-26
*Author: WANGJIANHUI
*Description:
*************************************************************************/
#include "rf_spi.h"

/************************************************************!
*Function: Spi_Init
*Description: SPI��ʼ������
*Input: null
*Output: null
*Return: null
*************************************************************/
void Spi_Init(void)
{
	CMU_ClockEnable(cmuClock_USART2, true);

	USART_InitSync_TypeDef usart2init = USART_INITSYNC_DEFAULT;

	usart2init.baudrate = 1000000;
	usart2init.msbf = 1;

	USART_InitSync(USART2, &usart2init);

	GPIO_PinModeSet(SPI_MOSI_PORT, SPI_MOSI_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(SPI_MISO_PORT, SPI_MISO_PIN, gpioModeInput, 0);
	GPIO_PinModeSet(SPI_SCLK_PORT, SPI_SCLK_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(SPI_CS_PORT, SPI_CS_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(RF_SDN_PORT, RF_SDN_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(RF_nIRQ_PORT, RF_nIRQ_PIN, gpioModeInputPull, 0);

	USART2->ROUTE =USART_ROUTE_CLKPEN|USART_ROUTE_RXPEN|USART_ROUTE_TXPEN|USART_ROUTE_LOCATION_LOC1;

	GPIO_PinModeSet(gpioPortC, RF_nIRQ_PIN, gpioModeInput, 0); //PD15:��Ƶ�жϹܽ�
	GPIO_IntConfig(RF_nIRQ_PORT,RF_nIRQ_PIN, false,true, true); //�½��ش����ж�
	NVIC_EnableIRQ(GPIO_ODD_IRQn); //PC5:ODD
}
/************************************************************!
*Function: Spi_ReadWrite
*Description: SPI��д����
*Input: data����Ҫд�������
*Output: null
*Return: ��Ҫ����������
*************************************************************/
uint8_t Spi_ReadWrite(uint8_t data)
{
  USART2->TXDATA = data;
  while (!(USART2->STATUS & USART_STATUS_TXC));
  return (uint8_t)(USART2->RXDATA);
}
/************************************************************!
*Function: Spi_WriteDataBurst
*Description: SPIд�ֽ����麯��
*Input: 1.data��������׵�ַ
*       2.lens�����鳤��
*Output: null
*Return: null
*************************************************************/
void Spi_WriteDataBurst(uint8_t lens,uint8_t *data)
{
	while (lens--)
	{
		Spi_ReadWrite(*data++);
	}
}
/************************************************************!
*Function: Spi_ReadDataBurst
*Description: SPI���ֽ����麯��
*Input: 1.data��������׵�ַ
*       2.lens�����鳤��
*Output: null
*Return: null
*************************************************************/
void Spi_ReadDataBurst(uint8_t lens,uint8_t* data)
{
	while (lens--)
	{
		*data++ = Spi_ReadWrite(0xFF);
	}
}
/************************************************************!
*Function: Spi_ClearNsel
*Description: �ر�SPIʹ��
*Input: null
*Output: null
*Return: null
*************************************************************/
void Spi_ClearNsel(eSpi_Nsel qiSelect)
{
	switch (qiSelect)
	{
		case eSpi_Nsel_RF:
			GPIO_PinOutClear(SPI_CS_PORT, SPI_CS_PIN);
			break;
		default:
			break;
	}
}
/************************************************************!
*Function: Spi_SetNsel
*Description: SPIʹ��
*Input: null
*Output: null
*Return: null
*************************************************************/
void Spi_SetNsel(eSpi_Nsel qiSelect)
{
	switch (qiSelect)
	{
		case eSpi_Nsel_RF:
			GPIO_PinOutSet(SPI_CS_PORT, SPI_CS_PIN);
			break;
		default:
			break;
	}
}

