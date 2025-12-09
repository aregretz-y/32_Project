/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/10/29
  * @brief   这是利用UART串口实现printf函数的重定向，需要对futc()进行重定向
  ******************************************************************************
  * @attention
  *				
	*       1.WIFI模块默认的波特率是115200bps,字符格式：8N1，采用UART3(PB10、PB11)
	*				2.WIFI模组是支持AT指令的，所有的AT指令必须采用大写，必须以"\r\n"结尾
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  //用户必须包含
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
/* Private typedef -----------------------------------------------------------*/


//需要对fputc函数重定向到UART
int fputc(int ch, FILE *f) 
{
	//为了提高程序可靠性，需要确保上一个字节发送完成
	while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET );
	
	//通过UART1发送字节
	USART_SendData(USART1,ch);
	return ch;
}

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

#define BEEP_ON()		GPIO_SetBits(GPIOF,GPIO_Pin_8)
#define BEEP_OFF()	GPIO_ResetBits(GPIOF,GPIO_Pin_8)

#define BUFSIZE 512

//待连接热点的参数，需要修改
#define WIFI_SSID		"lmx"
#define WIFI_PASSWD "12345678"

//待连接服务器的参数，无需修改
#define SERVER_ADDR "bemfa.com"
#define SERVER_PORT 8344

//巴法云服务器的参数，需要修改
#define BEMFA_UID		"95783ff664654238b71c6a12b0db1bef"


//巴法云主题的参数，需要修改
#define LED_TOPIC  "led002"


/* Private variables ---------------------------------------------------------*/

__IO uint8_t  u3_recvbuf[BUFSIZE]; //UART3的接收缓冲区
__IO uint16_t u3_recvcnt = 0;      //UART3的接收计数器

__IO uint8_t  recvflag = 0;						 //接收完成的标志
__IO uint8_t  initflag = 0;						 //初始化完成的标志
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  延时微秒函数
  * @param  nus:延时时间
  * @retval None
  */
void delay_us(u32 nus)
{
	
	SysTick->CTRL = 0; // Disable SysTick
	SysTick->LOAD = 168*nus - 1;
	SysTick->VAL 	= 0; // Clear current value as well as count flag
	SysTick->CTRL = 5; // Enable SysTick timer with processor clock
	while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set
	SysTick->CTRL = 0; // Disable SysTick
	
}


/**
  * @brief  延时毫秒函数
  * @param  nms:延时时间
  * @retval None
  */
void delay_ms(u32 nms)
{
	while(nms--)
	{
		delay_us(1000);
	}
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
  * @brief  BEEP的初始化
  * @param  None
  * @retval None
  */
void BEEP_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
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
* @brief  WIFI发送AT指令
* @param  at_str:待发送的AT指令
* @param  cnt:超时时间,以ms为单位   
* @retval None
* @note   
*/
bool WIFI_SendAT(char *at_str,uint16_t cnt)
{
	//清空缓冲区
	memset((char *)u3_recvbuf,0,BUFSIZE);
	
	//清空计数器
	u3_recvcnt = 0;
	
	//发送指令 
	USART3_SendStr(at_str);
	
	//超时处理
	while(--cnt)
	{
		delay_ms(1);
		
		if( strstr((char *)u3_recvbuf,"OK") || strstr((char *)u3_recvbuf,">") )
		{
			//提前终止循环
			break;
		}  
	}
	
	if(cnt>0)
		return true;   //已响应
	else
		return false;  //未响应
}


/**
* @brief  WIFI的初始化
* @param  baud:WIFI的波特率
* @retval None
* @note   默认的波特率是115200
*/
void WIFI_Config(u32 baud)
{
	char at_buf[128]={0};
	
	//1.初始化UART3，并退出透传模式
	USART3_Config(baud);
	
	//在透传模式下，单独发送+++则会退出透传
	USART3_SendStr("+++");
	delay_ms(1000);
	
	//2.发送测试指令
	if( WIFI_SendAT("AT\r\n",3000) )
	{
		printf("WIFI在线\r\n");
	}
	else
	{
		printf("WIFI离线\r\n");
	}
	
	//3.发送设置WiFi模式指令
	if( WIFI_SendAT("AT+CWMODE_DEF=3\r\n",3000) )
	{
		printf("设置WIFI模式成功\r\n");
	}
	else
	{
		printf("设置WIFI模式失败\r\n");
	}
	
	//4.发送连接AP的指令
	memset(at_buf,0,128);
	sprintf(at_buf,"AT+CWJAP_DEF=\"%s\",\"%s\"\r\n",WIFI_SSID,WIFI_PASSWD);
	if( WIFI_SendAT(at_buf,8000) )
	{
		printf("WIFI连接AP成功\r\n");
	}
	else
	{
		printf("WIFI连接AP失败\r\n");
	}
	//5.发送开启透传模式指令
	if( WIFI_SendAT("AT+CIPMODE=1\r\n",3000) )
	{
		printf("开启WIFI透传模式成功\r\n");
	}
	else
	{
		printf("开启WIFI透传模式失败\r\n");
	}
	//6.发送连接服务器的指令
	memset(at_buf,0,128);
	sprintf(at_buf,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",SERVER_ADDR,SERVER_PORT);
	if( WIFI_SendAT(at_buf,8000) )
	{
		printf("WIFI连接服务器成功\r\n");
	}
	else
	{
		printf("WIFI连接服务器失败\r\n");
	}
	//5.发送开启透传模式指令
	if( WIFI_SendAT("AT+CIPSEND\r\n",3000) )
	{
		printf("WIFI进入透传成功\r\n");
	}
	else
	{
		printf("WIFI进入透传失败\r\n");
	}
	
	
}



/**
* @brief  巴法云订阅主题
* @param  topic:待订阅的主题
* @param  cnt:超时时间,以ms为单位   
* @retval true or false
* @note   
*/
bool Bemfa_Subscribe(char *topic,uint16_t cnt)
{
	char topic_buf[256] = {0};
	
	//清空缓冲区
	memset((char *)u3_recvbuf,0,BUFSIZE);
	
	//清空计数器
	u3_recvcnt = 0;
	
	//向服务器发送指令
	sprintf(topic_buf,"cmd=1&uid=%s&topic=%s\r\n",BEMFA_UID,topic);
	USART3_SendStr(topic_buf);
	
	//超时处理
	while(--cnt)
	{
		delay_ms(1);
		
		if( strstr((char *)u3_recvbuf,"cmd=1&res=1") )
		{
			//提前终止循环
			break;
		}  
	}
	
	if(cnt>0)
		return true;   //已响应
	else
		return false;  //未响应

}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{   

	//1.硬件初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//中断优先级分组，采用分组4则表示抢占优先级的范围是0~15，子优先级的值固定为0
  USART1_Config(115200); //PC
	
	
  /* Infinite loop */
  for(;;)
  {
		printf("hello world\r\n");
		delay_ms(1000);
  }
}



/**
* @brief  This function handles USRAT3 interrupt request.
* @param  None
* @retval None
*/
void USART3_IRQHandler(void)
{
  //判断是否收到数据
  if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
  {
		if(u3_recvcnt < BUFSIZE)
		{
			//接收字节
			u3_recvbuf[u3_recvcnt++] = USART_ReceiveData(USART3);
			
		}
		
		//对数据完整性进行检验
		if( u3_recvbuf[u3_recvcnt-2] == '\r' && u3_recvbuf[u3_recvcnt-1] == '\n' && initflag == 1)
		{
			recvflag = 1;
		}
		
  }
}




/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
