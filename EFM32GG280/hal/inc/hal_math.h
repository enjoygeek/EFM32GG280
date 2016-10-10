/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_math.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: root
*Modified: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_MATH_H_
#define HAL_MATH_H_


#include <stdbool.h>
#include <stdint.h>


uint8_t Bcd2Byte(uint8_t bcd);
uint8_t Byte2Bcd(uint8_t byte);
uint8_t Is_Valid_BCD(uint8_t *bcd,uint8_t lens);
uint8_t Char2Uint8_t(uint8_t *data);
uint8_t Get_Hex(uint8_t data);
uint16_t Str2Uint(uint8_t *data);
uint16_t Str2Uint_Gps(uint8_t *data,uint8_t lens);
uint8_t Float2Char(float value,uint8_t *data);


#endif

