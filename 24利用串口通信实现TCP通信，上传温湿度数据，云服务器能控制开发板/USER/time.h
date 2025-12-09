#ifndef __TIME_H__
#define __TIME_H__

#include "stm32f4xx.h"  //用户必须包含
#include <stdio.h>

#include "uart.h"
#include "dht11.h"


/**
  * @brief  配置定时器TIM6定时1s
  * @param  None
  * @retval None
  */
void TIM_Config(void);


/**
  * @brief  配置定时器TIM3定时1s
  * @param  None
  * @retval None
  */
void TIM3_Config(void);
	
#endif
