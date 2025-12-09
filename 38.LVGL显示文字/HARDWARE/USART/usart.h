#ifndef __USART_H
#define __USART_H

#include "stm32f4xx.h"
#include "delay.h"
#include "sys.h"
#include <stdio.h>

void USART1_Init(u32 BaudRate);
void USART2_Init(u32 BaudRate);

void USART_SendString(USART_TypeDef *USARTx,const char  *String);
#endif
