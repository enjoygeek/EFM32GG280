/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: flashstorage.h
*Version: 0.1.0
*Date: 2016-09-20
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef FLASHSTORAGE_H_
#define FLASHSTORAGE_H_


#include "hal_mx25.h"

#define SECTOR_LEN            4096
#define PARA_ADDR             0x0000000
#define PARA_SECTOR_NUM       1
#define PARA_FLASH_LEN        4096
#define PROGRAM_ADDR          0x0001000
#define PROGRAM_SECTOR_NUM    128
#define PROGRAM_FLASH_LEN     4096*128
#define YC_INFO_ADDR          0x0081000
#define YC_SECTOR_NUM         128
#define YC_FLASH_LEN          4096*128
#define SOE_INFO_ADDR         0x0101000
#define SOE_SECTOR_NUM        128
#define SOE_FLASH_LEN         4096*128
#define VOL_INFO_ADDR         0x0181000
#define VOL_SECTOR_NUM        128
#define VOL_FLASH_LEN         4096*128
#define CUR_INFO_ADDR         0x0201000
#define CUR_SECTOR_NUM        128
#define CUR_FLASH_LEN         4096*128

#define YC_INFO_LEN           10
#define SOE_INFO_LEN          10
#define VOL_INFO_LEN          10
#define CUR_INFO_LEN          10

typedef enum
{
	PARA = 0,
	PROGRAM = 1,
	YC = 2,
	SOE = 3,
	VOL = 4,
	CUR = 5
}STORAGE_TYPE;


typedef struct
{
	uint32_t  pPARA;
	uint32_t  pPROGRAM;
	uint32_t  pYC;
	uint32_t  pSOE;
	uint32_t  pVOL;
	uint32_t  pCUR;
}FLASH_POINTER;


void Flash_Pointer_Init(void);
uint32_t Find_Flash_Pointer(uint32_t startaddr,uint32_t sectornum);
void Flash_Storage(STORAGE_TYPE st,uint8_t *data,uint8_t lens);
void Set_PARA_Storage(uint8_t *data,uint32_t lens);
uint8_t Get_PARA_Storage(uint8_t *data,uint32_t lens);
void Set_PROGRAM_Storage(uint8_t *data,uint32_t lens);
void Set_INFO_Storage(uint32_t newflashaddr,uint32_t newpointer,uint32_t newlens,uint8_t *data);
void Get_INFO_Storage(STORAGE_TYPE st,uint8_t *targetdata,uint8_t *data);
uint8_t Para_Init(void);
void Refresh_Para(uint8_t *data,uint8_t lens);


#endif



