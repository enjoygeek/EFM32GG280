/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_mx25.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_mx25.h"

/************************************************************!
*Function: spi_open_flash
*Description: MX25 ��SPI����
*Input: null
*Output: null
*Return: null
*************************************************************/
void spi_open_flash(void)
{
	SPI_D2_INPUT; //PD6-INPUT-WP��������Ϊ����
	SPI_D3_INPUT; //PD7-INPUT-SO3��������Ϊ����

	SPISI_INPUT; //MCU-SI��������Ϊ����
	SPISO_OUTPUT; //MCU-SO��������Ϊ���
	SPISCK_OUTPUT; //PG0-OUTPUT-SCK��������Ϊ���
	SPICS_FLASH_OUTPUT; //CS��������Ϊ���
	SPICS_FLASH_LAT_HIGH; //CS���ߣ��ر�Flash

	SPISCK_LAT_HIGH; //SCK����

	SPICS_FLASH_LAT_LOW; //CS��������ѡ��
}
/************************************************************!
*Function: spi_close_flash
*Description: MX25 �ر�SPI����
*Input: null
*Output: null
*Return: null
*************************************************************/
void spi_close_flash(void)
{
	SPICS_FLASH_OUTPUT; //CS�������
	SPICS_FLASH_LAT_HIGH; //CS�������߹ر�

	SPI_D0_INPUT; //�˳�ʱĬ�����߷���������ģʽ
	SPI_D1_INPUT;
	SPI_D2_INPUT;
	SPI_D3_INPUT;
	//SPI_D3_OUTPUT;
	//SPI_D3_HIGH;
}
/************************************************************!
*Function: enter_quad
*Description: SPI�������ַ����������л�
*Input: null
*Output: null
*Return: null
*************************************************************/
void enter_quad(void)
{
	SPI_D0_INPUT;
	SPI_D1_INPUT;
	SPI_D2_INPUT;
	SPI_D3_INPUT;
}
/************************************************************!
*Function: exit_quad
*Description: SPI�������ַ����������л�
*Input: null
*Output: null
*Return: null
*************************************************************/
void exit_quad(void)
{
	SPICS_FLASH_OUTPUT;//CS�������
	SPICS_FLASH_LAT_HIGH;//CS�������߹ر�
}
/************************************************************!
*Function: spi_writebyte
*Description: ģ��SPIд�ֽں���
*Input: data���ֽ�
*Output: null
*Return: null
*************************************************************/
void spi_writebyte(uint8_t data)
{
	//BIT7
	SPISCK_LAT_LOW;
	if (data&0x80)
	{
		SPISO_LAT_HIGH;
	}
	else
	{
		SPISO_LAT_LOW;
		SPISO_LAT_LOW;
	}
	Nop();
	SPISCK_LAT_HIGH;
	Nop();

	//BIT6
	SPISCK_LAT_LOW;
	if (data&0x40)
	{
		SPISO_LAT_HIGH;
	}
	else
	{
		SPISO_LAT_LOW;
		SPISO_LAT_LOW;
	}
	Nop();
	SPISCK_LAT_HIGH;
	Nop();

	//BIT5
	SPISCK_LAT_LOW;
	if (data&0x20)
	{
		SPISO_LAT_HIGH;
	}
	else
	{
		SPISO_LAT_LOW;
		SPISO_LAT_LOW;
	}
	Nop();
	SPISCK_LAT_HIGH;
	Nop();

	//BIT4
	SPISCK_LAT_LOW;
	if (data&0x10)
	{
		SPISO_LAT_HIGH;
	}
	else
	{
		SPISO_LAT_LOW;
		SPISO_LAT_LOW;
	}
	Nop();
	SPISCK_LAT_HIGH;
	Nop();

	//BIT3
	SPISCK_LAT_LOW;
	if (data & 0x08)
	{
		SPISO_LAT_HIGH;
	}
	else
	{
		SPISO_LAT_LOW;
		SPISO_LAT_LOW;
	}
	Nop();
	SPISCK_LAT_HIGH;
	Nop();

	//BIT2
	SPISCK_LAT_LOW;
	if (data & 0x04)
	{
		SPISO_LAT_HIGH;
	}
	else
	{
		SPISO_LAT_LOW;
		SPISO_LAT_LOW;
	}
	Nop();
	SPISCK_LAT_HIGH;
	Nop();

	//BIT1
	SPISCK_LAT_LOW;
	if (data & 0x02)
	{
		SPISO_LAT_HIGH;
	}
	else
	{
		SPISO_LAT_LOW;
		SPISO_LAT_LOW;
	}
	Nop();
	SPISCK_LAT_HIGH;
	Nop();

	//BIT0
	SPISCK_LAT_LOW;
	if (data & 0x01)
	{
		SPISO_LAT_HIGH;
	}
	else
	{
		SPISO_LAT_LOW;
		SPISO_LAT_LOW;
	}
	Nop();
	SPISCK_LAT_HIGH;
	Nop();
}
/************************************************************!
*Function: spi_readbyte
*Description: ģ��SPI���ֽں���������������½��أ����ú󣬱���SCKΪ
*             �ߵ�ƽ���������У���߷����ٶ�
*Input: null
*Output: null
*Return: ��ȡ�����ֽ�
*************************************************************/
uint8_t spi_readbyte(void)
{
	uint8_t ch=0;

	//-----BIT7
	SPISCK_LAT_LOW;
	Nop();
	Nop();
	SPISCK_LAT_HIGH;
	if (SPISI)
	{
		ch |= 0x80;
	}

	//-----BIT6
	SPISCK_LAT_LOW;
	Nop();
	Nop();
	SPISCK_LAT_HIGH;
	if (SPISI)
	{
		ch |= 0x40;
	}

	//-----BIT5
	SPISCK_LAT_LOW;
	Nop();
	Nop();
	SPISCK_LAT_HIGH;
	if (SPISI)
	{
		ch |= 0x20;
	}

	//-----BIT4
	SPISCK_LAT_LOW;
	Nop();
	Nop();
	SPISCK_LAT_HIGH;

	if (SPISI)
	{
		ch |= 0x10;
	}

	//-----BIT3
	SPISCK_LAT_LOW;
	Nop();
	Nop();
	SPISCK_LAT_HIGH;
	if (SPISI)
	{
		ch |= 0x08;
	}

	//-----BIT2
	SPISCK_LAT_LOW;
	Nop();
	Nop();
	SPISCK_LAT_HIGH;
	if (SPISI)
	{
		ch |= 0x04;
	}

	//-----BIT1
	SPISCK_LAT_LOW;
	Nop();
	Nop();
	SPISCK_LAT_HIGH;
	if (SPISI)
	{
		ch |= 0x02;
	}

	//-----BIT0
	SPISCK_LAT_LOW;
	Nop();
	Nop();
	SPISCK_LAT_HIGH;
	if (SPISI)
	{
		ch |= 0x01;
	}

	return ch;
}
/************************************************************!
*Function: spi_read_dummy
*Description: ��ָ���
*Input: count����ָ����
*Output: null
*Return: null
*************************************************************/
void spi_read_dummy(uint8_t count)
{
	while(count > 0)
	{
		SPISCK_LAT_LOW;
		count--;
		SPISCK_LAT_HIGH;
	}
}
/************************************************************!
*Function: quad_readbyte
*Description: 4IO���ֽں���
*Input: null
*Output: null
*Return: ��ȡ���ֽ�
*************************************************************/
uint8_t quad_readbyte(void)
{
	uint8_t ch=0;

	//-----D4,D5,D6,D7
	SPISCK_LAT_LOW;
	Nop();
	Nop();
	SPISCK_LAT_HIGH;

	if (SPI_D0)
		ch |= 0x10;
	if (SPI_D1)
		ch |= 0x20;
	if (SPI_D2)
		ch |= 0x40;
	if (SPI_D3)
		ch |= 0x80;

	//-----D0,D1,D2,D3
	SPISCK_LAT_LOW;
	Nop();
	Nop();
	SPISCK_LAT_HIGH;

	if (SPI_D0)
		ch |= 0x01;
	if (SPI_D1)
		ch |= 0x02;
	if (SPI_D2)
		ch |= 0x04;
	if (SPI_D3)
		ch |= 0x08;

	return ch;
}
/************************************************************!
*Function: quad_writebyte
*Description: 4IOд�ֽں���
*Input: data���ֽ�
*Output: null
*Return: null
*************************************************************/
void quad_writebyte(uint8_t data)
{
	//D4,D5,D6,D7
	SPISCK_LAT_LOW;
	(data & 0x10) ? SPI_D0_HIGH : SPI_D0_LOW;
	(data & 0x20) ? SPI_D1_HIGH : SPI_D1_LOW;
	(data & 0x40) ? SPI_D2_HIGH : SPI_D2_LOW;
	(data & 0x80) ? SPI_D3_HIGH : SPI_D3_LOW;
	SPISCK_LAT_HIGH;
	Nop();

	//D0,D1,D2,D3
	SPISCK_LAT_LOW;
	(data & 0x01) ? SPI_D0_HIGH : SPI_D0_LOW;
	(data & 0x02) ? SPI_D1_HIGH : SPI_D1_LOW;
	(data & 0x04) ? SPI_D2_HIGH : SPI_D2_LOW;
	(data & 0x08) ? SPI_D3_HIGH : SPI_D3_LOW;
	SPISCK_LAT_HIGH;
}
/************************************************************!
*Function: MX25L256_FLASH_INIT
*Description: MX25��ʼ������
*Input: null
*Output: null
*Return: null
*************************************************************/
void MX25L256_FLASH_INIT(void)
{
	uint8_t idcode[3] ={0};

	//SPI��������ʼ��
	spi_close_flash();

	//оƬ��λ
	MX25_MEM_RST();

	//����״̬�Ĵ���
	MX25_WRSR(0);

	//����4��ַ����ģʽ
	SET_4B_MODE(1);

	//��ȡID��
	MX25_RDID(idcode);
}
/************************************************************!
*Function: IS_FLASH_READY
*Description: �ж�FLASH�Ƿ���� 1-��������д���� 0-дæ
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t IS_FLASH_READY(void)
{
	return (((MX25_RDSR()&WIP)==0)?1:0);
}
/************************************************************!
*Function: MX25_WR_ENABLE
*Description: дʹ���ж� 1-дʹ�� 0-д��ֹ
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t MX25_WR_ENABLE(void)
{
	uint32_t timeout = 0;

	do
	{
		spi_open_flash();
		spi_writebyte(MX25_CMD_WREN);
		spi_close_flash();

		if (++timeout > TIME_OUT_LOOP)
			return 0;

	} while ((MX25_RDSR() & WEL)==0);

    return 1;
}
/************************************************************!
*Function: MX25_WR_DISABLE
*Description: ����д��ֹ����
*Input: null
*Output: null
*Return: null
*************************************************************/
void MX25_WR_DISABLE(void)
{
	//����дʹ������
	spi_open_flash();
	spi_writebyte(MX25_CMD_WRDI);
	spi_close_flash();
}
/************************************************************!
*Function: MX25_WR_COMPLETE
*Description: �ȴ�д��ɲ��� 1-�ɹ� 0-����
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t MX25_WR_COMPLETE(void)
{
	uint32_t timeout = 0;

	while ((MX25_RDSR() & WIP))
	{
		if (++timeout > TIME_OUT_LOOP)
			return 0;
	}
	return 1;
}
/************************************************************!
*Function: MX25_MEM_RST
*Description: оƬ�����λ
*Input: null
*Output: null
*Return: null
*************************************************************/
void MX25_MEM_RST(void)
{
	spi_open_flash();
	spi_writebyte(MX25_CMD_RSTEN);
	spi_close_flash();

	spi_open_flash();
	spi_writebyte(MX25_CMD_RST);
	spi_close_flash();

	if (IS_FLASH_READY())
		MX25_WR_COMPLETE();
}
/************************************************************!
*Function: MX25_RDID
*Description: ��ȡID��
*Input: null
*Output: null
*Return: null
*************************************************************/
void MX25_RDID(uint8_t *pdata)
{
	uint8_t i;
	spi_open_flash();
	spi_writebyte(MX25_CMD_RDID);
	for (i = 0; i < 3; i++)
	{
		pdata[i] = spi_readbyte();
	}
	spi_close_flash();
}
/************************************************************!
*Function: MX25_RDSR
*Description: ��ȡ״̬�Ĵ���
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t MX25_RDSR(void)
{
	uint8_t status = 0;

	spi_open_flash();
	spi_writebyte(MX25_CMD_RDSR);
	status = spi_readbyte();
	spi_close_flash();
	return status;
}
/************************************************************!
*Function: MX25_WRSR
*Description: д״̬�Ĵ���
*Input: statval��ֵ
*Output: null
*Return: null
*************************************************************/
void MX25_WRSR(uint8_t statval)
{
	if (IS_FLASH_READY())
	{
		MX25_WR_ENABLE();
		spi_open_flash();
		spi_writebyte(MX25_CMD_WRSR);
		spi_writebyte(statval);
		spi_close_flash();
		MX25_WR_COMPLETE();	//�ȴ�д���
	}
}
/************************************************************!
*Function: SET_4B_MODE
*Description: ʹ��/����4�ֽڵ�ַģʽ
*Input: enable��
*Output: null
*Return: null
*************************************************************/
void SET_4B_MODE(uint8_t enable)
{
	spi_open_flash();
	spi_writebyte((enable ? MX25_CMD_EN4B : MX25_CMD_EX4B));
	spi_close_flash();
}
/************************************************************!
*Function: MX25_SECTOR_ERASE_4B
*Description: ������������(4�ֽڵ�ַģʽ)
*Input: sector_addr��0-8191,ÿsector4k�ֽ�
*Output: null
*Return: null
*************************************************************/
void MX25_SECTOR_ERASE_4B(uint16_t sector_addr)
{
	uint32_t addr = (sector_addr << 12);

	if (IS_FLASH_READY())
	{
		MX25_WR_ENABLE();
		spi_open_flash();
		spi_writebyte(MX25_CMD_SE);
		spi_writebyte((addr >> 24) & 0xff);
		spi_writebyte((addr >> 16) & 0xff);
		spi_writebyte((addr >> 8) & 0xff);
		spi_writebyte((addr >> 0) & 0xff);
		spi_close_flash();
		MX25_WR_COMPLETE();
	}
}
/************************************************************!
*Function: MX25_B32K_ERASE_4B
*Description: 32KB���������(4�ֽڵ�ַģʽ)
*Input: b32k_addr��0-1023
*Output: null
*Return: null
*************************************************************/
void MX25_B32K_ERASE_4B(uint16_t b32k_addr)
{
	uint32_t addr = (b32k_addr << 15);

	if (IS_FLASH_READY())
	{
		MX25_WR_ENABLE();
		spi_open_flash();
		spi_writebyte(MX25_CMD_BE32K);	//
		spi_writebyte((addr >> 24) & 0xff);
		spi_writebyte((addr >> 16) & 0xff);
		spi_writebyte((addr >> 8) & 0xff);
		spi_writebyte((addr >> 0) & 0xff);
		spi_close_flash();
		MX25_WR_COMPLETE();
	}
}
/************************************************************!
*Function: MX25_B64K_ERASE_4B
*Description: 642KB���������(4�ֽڵ�ַģʽ)
*Input: b64k_addr��0-511
*Output: null
*Return: null
*************************************************************/
void MX25_B64K_ERASE_4B(uint16_t b64k_addr)
{
	uint32_t addr = (b64k_addr << 16);

	if (IS_FLASH_READY())
	{
		MX25_WR_ENABLE();
		spi_open_flash();
		spi_writebyte(MX25_CMD_BE);
		spi_writebyte((addr >> 24) & 0xff);
		spi_writebyte((addr >> 16) & 0xff);
		spi_writebyte((addr >> 8) & 0xff);
		spi_writebyte((addr >> 0) & 0xff);
		spi_close_flash();
		MX25_WR_COMPLETE();
	}
}
/************************************************************!
*Function: MX25_CHIP_ERASE
*Description: ��Ƭ��������
*Input: null
*Output: null
*Return: null
*************************************************************/
void MX25_CHIP_ERASE(void)
{
	if (IS_FLASH_READY())
	{
		MX25_WR_ENABLE();
		spi_open_flash();
		spi_writebyte(MX25_CMD_CE);
		spi_close_flash();
		MX25_WR_COMPLETE();
	}
}
/************************************************************!
*Function: MX25_BYTE_READ4B
*Description: ���ֽ�����(4�ֽڵ�ַģʽ)
*Input: null
*Output: null
*Return: null
*************************************************************/
void MX25_BYTE_READ4B(uint32_t addr, uint8_t *pbuf, uint32_t length)
{
	spi_open_flash();
	spi_writebyte(MX25_CMD_READ);	//MX25_CMD_PP4B
	spi_writebyte(addr >> 24);
	spi_writebyte(addr >> 16);
	spi_writebyte(addr >> 8);
	spi_writebyte(addr >> 0);

	for (uint32_t i=0;i<length;i++)
	{
		pbuf[i] = spi_readbyte();
	}
	spi_close_flash();
}
/************************************************************!
*Function: MX25_BYTE_PP4B
*Description: д�ֽ�����(4�ֽڵ�ַģʽ)
*Input: null
*Output: null
*Return: null
*************************************************************/
void MX25_BYTE_PP4B(uint32_t addr, uint8_t *pbuf, uint32_t length)
{
	if (IS_FLASH_READY())
	{
		MX25_WR_ENABLE();
		spi_open_flash();
		spi_writebyte(MX25_CMD_PP);	//MX25_CMD_PP4B
		spi_writebyte(addr >> 24);
		spi_writebyte(addr >> 16);
		spi_writebyte(addr >> 8);
		spi_writebyte(addr >> 0);

		for (uint32_t i=0;i<length;i++)
		{
			spi_writebyte(pbuf[i]);
		}
		spi_close_flash();
		MX25_WR_COMPLETE();
	}
}
/************************************************************!
*Function: flash_waitidle
*Description: �ȴ��豸����
*Input: null
*Output: null
*Return: null
*************************************************************/
void flash_waitidle(void)
{
	uint8_t status;

	do
	{
		status = MX25_RDSR();
	} while (status & 0x01);

	while (!(status & 0x40)) //QE Check
	{
		spi_open_flash();
		spi_writebyte(0x06);   //WREN
		spi_close_flash();

		spi_open_flash();
		spi_writebyte(0x01);   //WRSR
		spi_writebyte(0x40);   //QE 0x40
		spi_close_flash();

		status = MX25_RDSR();
	}
}
/************************************************************!
*Function: MX25_QUAD_BYTE_READ4B
*Description: 4IO�¶��ֽ�����(4�ֽڵ�ַģʽ)
*Input: null
*Output: null
*Return: null
*************************************************************/
void MX25_QUAD_BYTE_READ4B(uint32_t addr, uint8_t *pbuf, uint32_t length)
{
	flash_waitidle();

	spi_open_flash();
	spi_writebyte(MX25_CMD_QREAD);   //QREAD (1In/4 Out fast read)
	spi_writebyte(addr >> 24);
	spi_writebyte(addr >> 16);
	spi_writebyte(addr >> 8);
	spi_writebyte(addr >> 0);

	SPI_D0_INPUT;
	SPI_D1_INPUT;

	spi_read_dummy(8);
	for (uint32_t i = 0; i < length; i++)
	{
		pbuf[i] = quad_readbyte();
	}
	spi_close_flash();
}
/************************************************************!
*Function: MX25_QUAD_BYTE_PP4B
*Description: 4IO��д�ֽ�����(4�ֽڵ�ַģʽ)
*Input: null
*Output: null
*Return: null
*************************************************************/
void MX25_QUAD_BYTE_PP4B(uint32_t addr, uint8_t *pbuf, uint32_t length)
{
	flash_waitidle();

	if (IS_FLASH_READY())
	{
		MX25_WR_ENABLE();
		spi_open_flash();
		spi_writebyte(MX25_CMD_4PP);   //MX25_CMD_PP4B
		SPI_D1_OUTPUT;
		SPI_D2_OUTPUT;
		SPI_D3_OUTPUT;
		SPI_D0_OUTPUT;
		quad_writebyte(addr >> 24);
		quad_writebyte(addr >> 16);
		quad_writebyte(addr >> 8);
		quad_writebyte(addr >> 0);

		for (uint32_t i = 0; i < length; i++)
		{
			quad_writebyte(pbuf[i]);
		}
		spi_close_flash();
		flash_waitidle();
		MX25_WR_COMPLETE();
	}
}

