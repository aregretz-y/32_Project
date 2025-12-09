#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f4xx.h"  //用户必须包含
#include <stdio.h>

/**
  * @brief  延时微秒函数
  * @param  nus:延时时间
  * @retval None
  */
void delay_us(u32 nus);

/**
  * @brief  延时毫秒函数
  * @param  nms:延时时间
  * @retval None
  */
void delay_ms(u32 nms);


#endif
