/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: include.h
*Version: 0.1.0
*Date: 2016-09-20
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef INCLUDE_H_
#define INCLUDE_H_


/**********标准C文件及库函数**********/
#include <stddef.h>
#include <string.h>
#include "em_chip.h"
#include "em_device.h"
/**********操作系统头文件**********/
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"

/**********TASK层头文件**********/
#include "init.h"
#include "mytask.h"
/**********MID层头文件**********/
#include "circlequeue.h"
#include "database.h"
#include "flashstorage.h"
#include "mid.h"
/**********PROCOTOL层头文件**********/
#include "prot645l.h"
#include "p645parse.h"
/**********SYSDEF头文件**********/
#include "sysdef.h"
/**********射频驱动头文件**********/
#include "radio_hal.h"
#include "rf_spi.h"
/**********底层驱动头文件**********/
#include "hal_adc.h"
#include "hal_cmu.h"
#include "hal_dma.h"
#include "hal_gpio.h"
#include "hal_gps.h"
#include "hal_math.h"
#include "hal_mx25.h"
#include "hal_r8025t.h"
#include "hal_timer.h"
#include "hal_usart.h"
#include "hal_wdog.h"


#endif

