#include "uart.h"


__IO uint8_t  u3_recvbuf[BUFSIZE]; //UART3的接收缓冲区
__IO uint16_t u3_recvcnt = 0;      //UART3的接收计数器

__IO uint8_t  recvflag = 0;				 //接收完成的标志
__IO uint8_t  initflag = 0;				 //初始化完成的标志

__IO uint8_t  u3_msg_topic[64];      // 主题名
__IO uint8_t  u3_msg_pld[64];        // payload
__IO uint8_t  u3_msg_topic_len = 0;      // 主题长度
__IO uint8_t  u3_msg_pld_len = 0;        // payload 长度


//需要对fputc函数重定向到UART
int fputc(int ch, FILE *f) 
{
	//为了提高程序可靠性，需要确保上一个字节发送完成
	while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET );
	
	//通过UART1发送字节
	USART_SendData(USART1,ch);
	return ch;
}



/**
  * @brief  Configures the USART1 Peripheral.
  * @param  None
  * @retval None
  */
void USART1_Config(u32 baud)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  //1.打开GPIO时钟 PA9 PA10
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  //2.打开UART时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  //3.选择引脚的复用功能
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9 ,GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
  
  //4.配置GPIO引脚
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9|GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //5.配置UART参数
  USART_InitStructure.USART_BaudRate = baud;										  //波特率
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;			//数据位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;					//停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;							//校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //全双工
  USART_Init(USART1, &USART_InitStructure);
  
  //6.配置NVIC
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
	//7.选择中断源  接收数据时触发
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
  //8.打开UART
  USART_Cmd(USART1, ENABLE);
}



/**
  * @brief  Configures the USART3 Peripheral.
  * @param  None
  * @retval None
  */
void USART3_Config(u32 baud)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  //1.打开GPIO时钟 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
  //2.打开UART时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  
  //3.选择引脚的复用功能
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource10 ,GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource11 ,GPIO_AF_USART3);
  
  //4.配置GPIO引脚
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_10|GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //5.配置UART参数
  USART_InitStructure.USART_BaudRate = baud;										  //波特率
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;			//数据位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;					//停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;							//校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //全双工
  USART_Init(USART3, &USART_InitStructure);
  
  //6.配置NVIC
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
	//7.选择中断源  接收数据时触发
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
  //8.打开UART
  USART_Cmd(USART3, ENABLE);
}



/**
* @brief  通过UART1发送字符串
* @param  str：待发送的字符串
* @retval None
*/
void USART1_SendStr(char *str)
{
	while(*str != '\0')
	{
		//为了提高程序可靠性，需要确保上一个字节发送完成
		while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET );
		
		//通过UART1发送字节
		USART_SendData(USART1,*str++);
	}

}

/**
* @brief  通过UART3发送字符串
* @param  str：待发送的字符串
* @retval None
*/
void USART3_SendStr(char *str)
{
	while(*str != '\0')
	{
		//为了提高程序可靠性，需要确保上一个字节发送完成
		while( USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET );
		
		//通过UART3发送字节
		USART_SendData(USART3,*str++);		
	}
}


/**
* @brief  通过UART3发送字节
* @param  str：待发送的字节地址
* @param  str：待发送的字节数量
* @retval None
*/
void USART3_SendBytes(char *str,uint16_t len)
{
	while(len--)
	{
		//为了提高程序可靠性，需要确保上一个字节发送完成
		while( USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET );
		
		//通过UART3发送字节
		USART_SendData(USART3,*str++);		
	}
}


/**
* @brief  This function handles USRAT1 interrupt request.
* @param  None
* @retval None
*/
void USART1_IRQHandler(void)
{
	uint8_t data = 0;
	
  //判断是否收到数据
  if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
  {
    
    //接收字节
    data = USART_ReceiveData(USART1);
		
		//发送字节给PC
		USART_SendData(USART1,data);
    
  }

}

/**
* @brief  This function handles USRAT3 interrupt request.
* @param  None
* @retval None
*/
void USART3_IRQHandler(void)
{
    uint8_t data;
    static uint16_t payload_len;   // 还需要收多少 payload
    static uint8_t  step = 0;      // 0=等固定头 1=等剩余长度 2=收主题长度高 3=收主题长度低...
    static uint8_t  remlen;        // 剩余长度字节值
    static uint8_t  topic_len;     // 主题长度
    static uint8_t  topic_cnt;     // 已收主题字节
    static uint8_t  pld_cnt;       // 已收 payload 字节

    if (USART_GetITStatus(USART3, USART_IT_RXNE) != SET)
        return;

    data = USART_ReceiveData(USART3);

    /* 防溢出 */
    if (u3_recvcnt >= BUFSIZE)
        return;

    u3_recvbuf[u3_recvcnt++] = data;

		if(initflag == 1)
		{
			/* ---------- 状态机解析 ---------- */
			switch (step)
			{
			case 0:                     /* 等固定头 0x30 */
					if (data == 0x30)
					{
							step = 1;
							remlen = 0;
					}
					else                    /* 不是 0x30 直接丢掉，防止错步 */
					{
							u3_recvcnt = 0;
					}
					break;

			case 1:                     /* 收剩余长度（假设 <128，单字节） */
					remlen = data;
					payload_len = remlen;   // 后面还要收这么多字节
					step = 2;
					topic_len = 0;
					topic_cnt = 0;
					pld_cnt  = 0;
					break;

			case 2:                     /* 主题长度高字节 */
					topic_len = data << 8;
					step = 3;
					break;

			case 3:                     /* 主题长度低字节 */
					topic_len |= data;
					u3_msg_topic_len = topic_len;
					payload_len -= 2;       // 减去主题长度字段本身 2 字节
					step = 4;
					break;

			case 4:                     /* 收主题名 */
					u3_msg_topic[topic_cnt++] = data;
					payload_len--;
					if (topic_cnt == topic_len)
					{
							u3_msg_topic[topic_cnt] = 0;   // 字符串结束
							step = 5;
					}
					break;

			case 5:                     /* 收 payload */
					u3_msg_pld[pld_cnt++] = data;
					if (--payload_len == 0)
					{
							u3_msg_pld[pld_cnt] = 0;       // 字符串结束
							u3_msg_pld_len = pld_cnt;

							/* 一帧完整，通知主循环 */
							recvflag = 1;

							/* 状态机复位，准备下一帧 */
							step = 0;
							u3_recvcnt = 0;
					}
					break;
			}
		}
}
