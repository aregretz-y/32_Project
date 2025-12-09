#ifndef __UART_H__
#define __UART_H__


#include "stm32f4xx.h"  //用户必须包含
#include <stdio.h>

#define BUFSIZE 512




/**
  * @brief  Configures the USART1 Peripheral.
  * @param  None
  * @retval None
  */
void USART1_Config(u32 baud);

/**
  * @brief  Configures the USART3 Peripheral.
  * @param  None
  * @retval None
  */
void USART3_Config(u32 baud);

/**
* @brief  通过UART1发送字符串
* @param  str：待发送的字符串
* @retval None
*/
void USART1_SendStr(char *str);

/**
* @brief  通过UART3发送字符串
* @param  str：待发送的字符串
* @retval None
*/
void USART3_SendStr(char *str);

/**
* @brief  通过UART3发送字节
* @param  str：待发送的字节地址
* @param  str：待发送的字节数量
* @retval None
*/
void USART3_SendBytes(char *str,uint16_t len);
	

#endif


