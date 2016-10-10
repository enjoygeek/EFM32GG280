/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_dma.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_dma.h"

/************************************************************!
*Function: Dma_Init
*Description: DMA��ʼ��������ͨ��0
*Input: null
*Output: null
*Return: null
*************************************************************/
void Dma_Init(void)
{
	CMU_ClockEnable(cmuClock_DMA,true);

	DMA_Init_TypeDef dmainit;
	DMA_CfgDescr_TypeDef descrcfg;
	DMA_CfgChannel_TypeDef chnlcfg;

	dmainit.hprot=0;
	dmainit.controlBlock=dmaControlBlock;

	chnlcfg.highPri=false;
	chnlcfg.enableInt=false;
	chnlcfg.select=DMAREQ_ADC0_SCAN;
	chnlcfg.cb=NULL;

	descrcfg.dstInc=dmaDataInc4;
	descrcfg.srcInc=dmaDataIncNone;
	descrcfg.size=dmaDataSize4;
	descrcfg.arbRate=dmaArbitrate1;
	descrcfg.hprot=0;

	DMA_Init(&dmainit); //DMA������
	DMA_CfgChannel(0,&chnlcfg); //DMAͨ������,ͨ��0
	DMA_CfgDescr(0,true,&descrcfg); //DMAͨ������������,ͨ��0
}

