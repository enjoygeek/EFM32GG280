/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_mx25.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_MX25_H_
#define HAL_MX25_H_


#include "em_gpio.h"


#define __EN_QUAD_MODE__

#define Nop()				__asm("nop")

#define    TIME_OUT_LOOP	100000


/*** MX25 命令预定义***/
//ID comands
#define    MX25_CMD_RDID      0x9F    //RDID (Read Identification)
#define    MX25_CMD_RES       0xAB    //RES (Read Electronic ID)
#define    MX25_CMD_REMS      0x90    //REMS (Read Electronic & Device ID)
#define    MX25_CMD_QPIID     0xAF    //QPIID (QPI ID Read)

//Register comands
#define    MX25_CMD_WRSR      0x01    //WRSR (Write Status Register)
#define    MX25_CMD_RDSR      0x05    //RDSR (Read Status Register)
#define    MX25_CMD_WRSCUR    0x2F    //WRSCUR (Write Security Register)
#define    MX25_CMD_RDSCUR    0x2B    //RDSCUR (Read Security Register)
#define    MX25_CMD_RDCR      0x15    //RDCR (Read Configuration Register)
#define    MX25_CMD_RDEAR     0xC8    //RDEAR (Read Extended Address Register)
#define    MX25_CMD_WREAR     0xC5    //WREAR (Write Extended Address Register)
#define    MX25_CMD_RDFBR     0x16    //RDFBR (read fast boot Register)
#define    MX25_CMD_WRFBR     0x17    //WRFBR (write fast boot Register)
#define    MX25_CMD_ESFBR     0x18    //ESFBR (erase fast boot Register)
#define    MX25_CMD_WRLR      0x2C    //WRLR (write lock Register)
#define    MX25_CMD_RDLR      0x2D    //RDLR (read lock Register)
#define    MX25_CMD_RDSPB     0xE2    //RDSPB (read SPB status)
#define    MX25_CMD_WRSPB     0xE3    //WRSPB (write SPB bit)
#define    MX25_CMD_ESSPB     0xE4    //ESSPB (erase all SPB status)
#define    MX25_CMD_SPBLK     0xA6    //SPBLK (SPB lock set)
#define    MX25_CMD_RDSPBLK   0xA7    //SPBLK (read SPB lock register)
#define    MX25_CMD_WRPASS    0x28    //WRPASS (write password Register)
#define    MX25_CMD_RDPASS    0x27    //RDPASS (read password Register)
#define    MX25_CMD_PASSULK   0x29    //RDPASS (password unlock)
#define    MX25_CMD_RDDPB     0xE0    //RDDPB (read DPB register)
#define    MX25_CMD_WRDPB     0xE1    //WRDPB (write DPB register)

//READ comands
#define    MX25_CMD_READ        0x03    //READ (1 x I/O)
#define    MX25_CMD_2READ       0xBB    //2READ (2 x I/O)
#define    MX25_CMD_4READ       0xEB    //4READ (4 x I/O)
#define    MX25_CMD_4READ_TOP   0xEA    //4READ (4 x I/O read from top 128Mb)
#define    MX25_CMD_FASTREAD    0x0B    //FAST READ (Fast read data)
#define    MX25_CMD_DREAD       0x3B    //DREAD (1In/2 Out fast read)
#define    MX25_CMD_QREAD       0x6B    //QREAD (1In/4 Out fast read)
#define    MX25_CMD_RDSFDP      0x5A    //RDSFDP (Read SFDP)


//Program comands
#define    MX25_CMD_WREN     0x06    //WREN (Write Enable)
#define    MX25_CMD_WRDI     0x04    //WRDI (Write Disable)
#define    MX25_CMD_PP       0x02    //PP (page program)
#define    MX25_CMD_4PP      0x38    //4PP (Quad page program)
#define    MX25_CMD_PP4B     0x12    //PP4B (page program with 4 byte address)
#define    MX25_CMD_4PP4B    0x3E    //4PP4B (Quad page program with 4 byte address)

//Erase comands
#define    MX25_CMD_SE       0x20    //SE (Sector Erase)
#define    MX25_CMD_BE32K    0x52    //BE32K (Block Erase 32kb)
#define    MX25_CMD_BE       0xD8    //BE (Block Erase)
#define    MX25_CMD_BE4B     0xDC    //BE4B (Block Erase with 4 byte address)
#define    MX25_CMD_CE       0x60    //CE (Chip Erase) hex code: 60 or C7
#define    MX25_CMD_SE4B     0x21    //SE (Sector Erase with 4 byte addr)
#define    MX25_CMD_BE32K4B  0x5C    //BE32K4B (Block Erase 32kb with 4 byte addr)

//Mode setting comands
#define    MX25_CMD_DP       0xB9    //DP (Deep Power Down)
#define    MX25_CMD_RDP      0xAB    //RDP (Release form Deep Power Down)
#define    MX25_CMD_ENSO     0xB1    //ENSO (Enter Secured OTP)
#define    MX25_CMD_EXSO     0xC1    //EXSO  (Exit Secured OTP)
#define    MX25_CMD_EQIO     0x35    //EQIO (Enable Quad I/O)
#define    MX25_CMD_WPSEL    0x68    //WPSEL (Enable block protect mode)
#ifdef SBL_CMD_0x77
#define    MX25_CMD_SBL      0x77    //SBL (Set Burst Length), new: 0x77
#else
#define    MX25_CMD_SBL      0xC0    //SBL (Set Burst Length), old: 0xC0
#endif
#define    MX25_CMD_EN4B     0xB7    //EN4B( Enter 4-byte Mode )
#define    MX25_CMD_EX4B     0xE9    //EX4B( Exit 4-byte Mode )

//Reset comands
#define    MX25_CMD_RSTEN     0x66    //RSTEN (Reset Enable)
#define    MX25_CMD_RST       0x99    //RST (Reset Memory)
#define    MX25_CMD_RSTQIO    0xF5    //RSTQIO (Reset Quad I/O)

//Security comands
#define    MX25_CMD_GBLK       0x7E    //GBLK (Gang Block Lock)
#define    MX25_CMD_GBULK      0x98    //GBULK (Gang Block Unlock)

//Suspend/Resume comands
#ifdef PGM_ERS_0xB0
#define    MX25_CMD_PGM_ERS_S    0xB0    //PGM/ERS Suspend (Suspends Program/Erase) old: 0xB0
#define    MX25_CMD_PGM_ERS_R    0x30    //PGM/ERS Erase (Resumes Program/Erase) old: 0x30
#else
#define    MX25_CMD_PGM_ERS_S    0x75    //PGM/ERS Suspend (Suspends Program/Erase) old: 0xB0
#define    MX25_CMD_PGM_ERS_R    0x7A    //PGM/ERS Erase (Resumes Program/Erase) old: 0x30
#endif

#define    MX25_CMD_NOP          0x00    //NOP (No Operation)

//状态寄存器
#define WIP		(1<<0)//写处理标记 1-写操作中 0-无写操作
#define WEL		(1<<1)//写使能标记 1-写使能   0-写禁止
#define BP0		(1<<2)//块保护设置
#define BP1		(1<<3)//块保护设置
#define BP2		(1<<4)//块保护设置
#define BP3		(1<<5)//块保护设置
#define QE		(1<<6)//4线操作使能
#define SRWD	(1<<7)//状态寄存器写保护 1-写禁止 0-写使能

//配置寄存器
#define ODS0	(1<<0)//驱动能力设置
#define ODS1	(1<<1)//驱动能力设置
#define ODS2	(1<<2)//驱动能力设置
#define TB		(1<<3)//0-顶部区域保护 1-底部区域保护
#define RESERVE	(1<<4)//空
#define MOD_4B	(1<<5)//地址模式 0->3地址模式, 1->4地址模式
#define DC0		(1<<6)//dummy clock cycles设置
#define DC1		(1<<7)//dummy clock cycles设置

//Reset comands
#define    MX25_CMD_RSTEN     0x66    //RSTEN (Reset Enable)
#define    MX25_CMD_RST       0x99    //RST (Reset Memory)
#define    MX25_CMD_RSTQIO    0xF5    //RSTQIO (Reset Quad I/O)

//FLASH_RST
#define FLASH_RST_LOW         GPIO_PinOutClear(gpioPortA, 11)
#define FLASH_RST_HIGH        GPIO_PinOutSet(gpioPortA, 11)
#define FLASH_RST_INPUT      GPIO_PinModeSet(gpioPortA, 11, gpioModeInput, 0)
#define FLASH_RST_OUTPUT  GPIO_PinModeSet(gpioPortA, 11, gpioModePushPull, 0)

//Flash-CS
#define SPICS_FLASH_LOW         GPIO_PinOutClear(gpioPortD, 3)
#define SPICS_FLASH_HIGH        GPIO_PinOutSet(gpioPortD, 3)
#define SPICS_FLASH_LAT_LOW     GPIO_PinOutClear(gpioPortD, 3)
#define SPICS_FLASH_LAT_HIGH    GPIO_PinOutSet(gpioPortD, 3)
#define SPICS_FLASH_INPUT       GPIO_PinModeSet(gpioPortD, 3, gpioModeInput, 0)
#define SPICS_FLASH_OUTPUT      GPIO_PinModeSet(gpioPortD, 3, gpioModePushPull, 0)

//Flash-SCK
#define SPISCK_LAT_LOW     GPIO_PinOutClear(gpioPortD, 2)
#define SPISCK_LAT_HIGH    GPIO_PinOutSet(gpioPortD, 2)
#define SPISCK_OUTPUT      GPIO_PinModeSet(gpioPortD, 2, gpioModePushPull, 0)
#define SPISCK_INPUT           GPIO_PinModeSet(gpioPortD, 2, gpioModeInput, 0)

//MCU-SO(FLASH侧SI管脚)
#define SPISO             GPIO_PinOutGet(gpioPortD, 0)
#define SPISO_LAT_LOW     GPIO_PinOutClear(gpioPortD, 0)
#define SPISO_LAT_HIGH    GPIO_PinOutSet(gpioPortD, 0)
#define SPISO_INPUT       GPIO_PinModeSet(gpioPortD, 0, gpioModeInput, 0)
#define SPISO_OUTPUT      GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 0)

//MCU-SI(FLASH侧SO管脚)
#define SPISI             GPIO_PinInGet(gpioPortD, 1)//设置为获取输入电平状态
#define SPISI_LAT_LOW     GPIO_PinOutClear(gpioPortD, 1)
#define SPISI_LAT_HIGH    GPIO_PinOutSet(gpioPortD, 1)
#define SPISI_INPUT       GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 0)
#define SPISI_OUTPUT      GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 0)

/************************************************************************
 * D0、D	1、D2、D3为四线模式下使用引脚 SIO0、SIO1、SIO2、SIO3
 ***********************************************************************/

//FLASH_D0(FLASH侧SI管脚)
#define SPI_D0             GPIO_PinInGet(gpioPortD, 0)
#define SPI_D0_LOW         GPIO_PinOutClear(gpioPortD, 0)
#define SPI_D0_HIGH        GPIO_PinOutSet(gpioPortD, 0)
#define SPI_D0_INPUT       GPIO_PinModeSet(gpioPortD, 0, gpioModeInput, 0)
#define SPI_D0_OUTPUT      GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 0)

//FLASH_D1(FLASH侧SO管脚)
#define SPI_D1             GPIO_PinInGet(gpioPortD, 1)
#define SPI_D1_LOW         GPIO_PinOutClear(gpioPortD, 1)
#define SPI_D1_HIGH        GPIO_PinOutSet(gpioPortD, 1)
#define SPI_D1_INPUT       GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 0)
#define SPI_D1_OUTPUT      GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 0)

//FLASH D2(WP)
#define SPI_D2             GPIO_PinInGet(gpioPortA, 9)
#define SPI_D2_LOW         GPIO_PinOutClear(gpioPortA, 9)
#define SPI_D2_HIGH        GPIO_PinOutSet(gpioPortA, 9)
#define SPI_D2_INPUT       GPIO_PinModeSet(gpioPortA, 9, gpioModeInput, 0)
#define SPI_D2_OUTPUT      GPIO_PinModeSet(gpioPortA, 9, gpioModePushPull, 0)

//FLASH D3(SO3)
#define SPI_D3             GPIO_PinInGet(gpioPortA, 10)
#define SPI_D3_LOW         GPIO_PinOutClear(gpioPortA, 10)
#define SPI_D3_HIGH        GPIO_PinOutSet(gpioPortA, 10)
#define SPI_D3_INPUT       GPIO_PinModeSet(gpioPortA, 10, gpioModeInput, 0)
#define SPI_D3_OUTPUT      GPIO_PinModeSet(gpioPortA, 10, gpioModePushPull, 0)

/**********************************************************************************/

/**********************************************************************************
 * Flash 空间划分预定义
 **********************************************************************************/

void MX25L256_FLASH_INIT(void);
void MX25_SECTOR_ERASE_4B(uint16_t sector_addr);
void MX25_B32K_ERASE_4B(uint16_t b32k_addr);
void MX25_B64K_ERASE_4B(uint16_t b64k_addr);
void MX25_CHIP_ERASE(void);
void MX25_QUAD_BYTE_READ4B(uint32_t addr,uint8_t *pbuf, uint32_t length);
void MX25_QUAD_BYTE_PP4B(uint32_t addr,uint8_t *pbuf, uint32_t length);

void  spi_open_flash(void);
void  spi_close_flash(void);
void enter_quad(void);
void exit_quad(void);
void spi_writebyte(uint8_t data);
uint8_t spi_readbyte(void);
void spi_read_dummy(uint8_t count);
uint8_t quad_readbyte(void);
void  quad_writebyte(uint8_t data);
uint8_t IS_FLASH_READY(void);
uint8_t MX25_WR_ENABLE(void);
void MX25_WR_DISABLE(void);
uint8_t MX25_WR_COMPLETE(void);
void MX25_MEM_RST(void);
void MX25_RDID(uint8_t *pdata);
uint8_t MX25_RDSR(void);
void MX25_WRSR(uint8_t statval);
void SET_4B_MODE(uint8_t enable);
void MX25_BYTE_READ4B(uint32_t addr,uint8_t *pbuf, uint32_t length);
void MX25_BYTE_PP4B(uint32_t addr,uint8_t *pbuf, uint32_t length);
void flash_waitidle(void);


#endif

