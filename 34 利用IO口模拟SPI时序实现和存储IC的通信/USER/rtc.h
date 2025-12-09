#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f4xx.h"  //用户必须包含
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "uart.h"

/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
void RTC_Config(void);

/**
* @brief  处理时间日期时间
* @param  NULL  
* @retval true or false
* @note   
*/
void Time_Handle(void);

/**
  * @brief  Display the current time.
  * @param  None
  * @retval None
  */
void RTC_TimeShow(void);



#endif

