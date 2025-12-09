#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f4xx.h"

#define USER_OS  1   //是否启用RTOS，属于用户自定义的宏

void delay_us(u32 nus);
void delay_ms(u32 nms);

#endif



