/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/11/6
  * @brief   这是利用RTC外设实现日历更新的代码案例
  ******************************************************************************
  * @attention
  *					1.RTC采用的时钟源是LSE，频率是32.768KHZ
	*    
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  //用户必须包含
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "delay.h"
#include "uart.h"



RTC_TimeTypeDef  	RTC_TimeStructure;
__IO uint32_t uwTimeDisplay;


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

//待连接热点的参数，需要修改
#define WIFI_SSID		"cb"
#define WIFI_PASSWD "12345678"

//待连接服务器的参数，无需修改
#define SERVER_ADDR "bemfa.com"
#define SERVER_PORT 8344

//巴法云服务器的参数，需要修改
#define BEMFA_UID		"a64a8c06bd5f4df8ba03c205a36cca7e"

extern __IO uint8_t  recvflag;						 			//接收完成的标志
extern __IO uint8_t  initflag;						 	//获取时间完成的标志


extern __IO uint8_t  u3_recvbuf[BUFSIZE]; //UART3的接收缓冲区
extern __IO uint16_t u3_recvcnt;          //UART3的接收计数器


uint8_t hour = 0;
uint8_t min = 0;
uint8_t sec = 0;

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
	if( WIFI_SendAT(at_buf,10000) )
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
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
void RTC_Config(void)
{
	RTC_InitTypeDef  	RTC_InitStructure;
  RTC_DateTypeDef 	RTC_DateStructure;
	
  NVIC_InitTypeDef  NVIC_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;
	
  //1.打开PWR外设的时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  //2.使能RTC备份域访问
  PWR_BackupAccessCmd(ENABLE);
    
	//3.开启LSE时钟
  RCC_LSEConfig(RCC_LSE_ON);

  //4.等待LSE就绪 
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

  //5.选择LSE作为RTC的时钟
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
  //6.使能RTC时钟
  RCC_RTCCLKCmd(ENABLE);

  //7.解除所有RTC寄存器的写保护
  RTC_WaitForSynchro();
  
  //8.配置RTC的预分频值和小时格式
  RTC_InitStructure.RTC_AsynchPrediv 	= 128-1;							//异步分频
  RTC_InitStructure.RTC_SynchPrediv 	= 256-1;							//同步分频
  RTC_InitStructure.RTC_HourFormat 		= RTC_HourFormat_24;  //24小时制
  RTC_Init(&RTC_InitStructure);
  
  //9.设置RTC日期
  RTC_DateStructure.RTC_Year 		= 0x25;								//BCD格式，年份
  RTC_DateStructure.RTC_Month 	= 0x11;								//BCD格式，月份
  RTC_DateStructure.RTC_Date 		= 0x06;								//BCD格式，日期
  RTC_DateStructure.RTC_WeekDay = 0x04;								//BCD格式，星期
  RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
  
  //10.设置RTC时间
	//  RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
	//  RTC_TimeStructure.RTC_Hours   = ihour/10*16+ihour%10;
	//  RTC_TimeStructure.RTC_Minutes = imin/10*16+imin%10;
	//  RTC_TimeStructure.RTC_Seconds = isec/10*16+isec%10; 
	//  RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
  
	//11.配置RTC唤醒中断
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	//12.配置RTC唤醒中断的EXTI中断线
  EXTI_ClearITPendingBit(EXTI_Line22);
	
  EXTI_InitStructure.EXTI_Line = EXTI_Line22;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	//13.关闭RTC唤醒定时器
	RTC_WakeUpCmd(DISABLE);
	
	//14.选择RTC唤醒时钟源 1HZ
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits); 
	
	//15.设置RTC唤醒重载值 1s
	RTC_SetWakeUpCounter(0);
	
	//16.选择RTC的中断源 唤醒中断
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	
	//17.清除RTC的唤醒中断标志
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//18.打开RTC唤醒定时器
  RTC_WakeUpCmd(ENABLE); 
}

/**
* @brief  巴法云获取日期时间
* @param  NULL   
* @retval NULL
* @note   
*/
void Bemfa_GetTime(void)
{
	char topic_buf[256] = {0};
	
	//清空缓冲区
	memset((char *)u3_recvbuf,0,BUFSIZE);
	
	//清空计数器
	u3_recvcnt = 0;
	
	//向服务器发送指令,获取实时时间
	sprintf(topic_buf,"cmd=7&uid=%s&type=2\r\n",BEMFA_UID);
	USART3_SendStr(topic_buf);
}

/**
* @brief  处理时间日期时间
* @param  NULL  
* @retval true or false
* @note   
*/
void Time_Handle(void)
{
	char * p;
	p = strtok((char *)u3_recvbuf,":");
	if(p != NULL)
	{
		hour = atoi(p);
		printf("hour = %d\r\n",hour);
		RTC_TimeStructure.RTC_Hours   = hour/10*16+hour%10;
		p = strtok(NULL,":");
		if(p != NULL)
		{
			min = atoi(p);
			printf("min = %d\r\n",min);
			RTC_TimeStructure.RTC_Minutes   = min/10*16+min%10;
			p = strtok(NULL,":");
			if(p != NULL)
			{
				sec = atoi(p);
				printf("sec = %d\r\n",sec);
				RTC_TimeStructure.RTC_Seconds   = sec/10*16+sec%10;
			}
		}
	}
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure); 
}

/**
  * @brief  Display the current time.
  * @param  None
  * @retval None
  */
void RTC_TimeShow(void)
{
  /* Get the current Time */
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  /* Display time Format : hh:mm:ss */
  printf("%0.2d:%0.2d:%0.2d\r\n",RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
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
  USART1_Config(115200); 	//PC
	WIFI_Config(115200);		
	RTC_Config();           //RTC初始化
	//清空缓冲区
	memset((char *)u3_recvbuf,0,BUFSIZE);
	//清空计数器
	u3_recvcnt = 0;
	//2.获取实时时间
	Bemfa_GetTime();
	initflag = 1;
  /* Infinite loop */
  for(;;)
  {
		if(recvflag == 1)
		{
			recvflag = 0;
			Time_Handle();
		}
		if(uwTimeDisplay)
		{
			uwTimeDisplay = 0;
			RTC_TimeShow();
		}	
  }
}



/**
  * @brief  This function handles RTC Wakeup Timer Handler.
  * @param  None
  * @retval None
  */
void RTC_WKUP_IRQHandler(void)
{
  if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
  {
    uwTimeDisplay = 1;
    RTC_ClearITPendingBit(RTC_IT_WUT);
    EXTI_ClearITPendingBit(EXTI_Line22);
  }
}



/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
