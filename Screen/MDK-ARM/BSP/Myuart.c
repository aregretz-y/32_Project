#include "include.h"
char Text[30];
char rxdata[30];
unsigned char rxdat;
unsigned char rx_pointer;

extern QueueHandle_t g_xQueueUartRx;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	rxdata[rx_pointer++]=rxdat;
	/*队列写入*/
	xQueueSendToBackFromISR(g_xQueueUartRx,&rxdata[0],NULL);
	
	HAL_UART_Receive_IT(&huart2,&rxdat,1);
}

	
