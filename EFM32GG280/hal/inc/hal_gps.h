/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_gpio.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_GPS_H_
#define HAL_GPS_H_


#include <stdbool.h>
#include <string.h>
#include "sysdef.h"


uint8_t Get_Abstime_Now(void);
uint8_t Gps_Abstime_Process(void);
uint8_t Get_Gps_Abstime(uint8_t *data,uint8_t lens);
uint8_t Check_Gps_Crc(uint8_t *data,uint8_t lens);


#endif

