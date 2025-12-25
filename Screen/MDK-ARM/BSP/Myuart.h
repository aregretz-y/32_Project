#ifndef _MYUART_H_
#define	_MYUART_H_


#include "main.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

extern char rxdata[30];
extern unsigned char rxdat;
extern unsigned char rx_pointer;
extern char Text[30];

#endif


