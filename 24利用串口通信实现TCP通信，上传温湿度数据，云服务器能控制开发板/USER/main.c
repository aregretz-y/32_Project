/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/10/29
  * @brief   这是利用UART串口实现对ESP8266模组的参数配置
  ******************************************************************************
  * @attention
  *				
	*       		1.WIFI模块默认的波特率是115200bps,字符格式：8N1，采用UART3(PB10、PB11)
	*						2.WIFI模组是支持AT指令的，所有的AT指令必须采用大写，必须以"\r\n"结尾
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  //用户必须包含
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "dht11.h"
#include "uart.h"
#include "delay.h"
#include "time.h"

/* Private typedef -----------------------------------------------------------*/



/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

#define LED_ON()		GPIO_ResetBits(GPIOF,GPIO_Pin_9)
#define LED_OFF()	  GPIO_SetBits(GPIOF,GPIO_Pin_9)

#define SG90_TURN180()	  TIM_SetCompare1(TIM3,500);
#define SG90_TURN0()	  	TIM_SetCompare1(TIM3,2500);

//待连接热点的参数，需要修改
#define WIFI_SSID		"cb"
#define WIFI_PASSWD "12345678"

//待连接服务器的参数，无需修改
#define SERVER_ADDR "bemfa.com"
#define SERVER_PORT 8344

//巴法云服务器的参数，需要修改
#define BEMFA_UID		"a64a8c06bd5f4df8ba03c205a36cca7e"


//巴法云主题的参数，需要修改
#define LED_TOPIC   "led001"
#define SG90_TOPIC  "sg90009"
#define DHT11_TOPIC  "DHT11"

/* Private variables ---------------------------------------------------------*/
extern __IO uint8_t  recvflag;						 			//接收完成的标志
extern __IO uint8_t  ledinitflag;						 		//订阅LED完成的标志
extern __IO uint8_t  sg90initflag;						 //订阅SG90完成的标志    
extern __IO uint8_t dht11initflag;						 	//订阅DHT11完成的标志

extern __IO uint8_t  u3_recvbuf[BUFSIZE]; //UART3的接收缓冲区
extern __IO uint16_t u3_recvcnt;          //UART3的接收计数器
 
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  BEEP的初始化
  * @param  None
  * @retval None
  */
void LED_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	LED_OFF();
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
		USART1_SendStr("WIFI在线\r\n");
	}
	else
	{
		USART1_SendStr("WIFI离线\r\n");
	}
	
	//3.发送设置WiFi模式指令
	if( WIFI_SendAT("AT+CWMODE_DEF=3\r\n",3000) )
	{
		USART1_SendStr("设置WIFI模式成功\r\n");
	}
	else
	{
		USART1_SendStr("设置WIFI模式失败\r\n");
	}
	
	//4.发送连接AP的指令
	memset(at_buf,0,128);
	sprintf(at_buf,"AT+CWJAP_DEF=\"%s\",\"%s\"\r\n",WIFI_SSID,WIFI_PASSWD);
	if( WIFI_SendAT(at_buf,10000) )
	{
		USART1_SendStr("WIFI连接AP成功\r\n");
	}
	else
	{
		USART1_SendStr("WIFI连接AP失败\r\n");
	}
	//5.发送开启透传模式指令
	if( WIFI_SendAT("AT+CIPMODE=1\r\n",3000) )
	{
		USART1_SendStr("开启WIFI透传模式成功\r\n");
	}
	else
	{
		USART1_SendStr("开启WIFI透传模式失败\r\n");
	}
	//6.发送连接服务器的指令
	memset(at_buf,0,128);
	sprintf(at_buf,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",SERVER_ADDR,SERVER_PORT);
	if( WIFI_SendAT(at_buf,8000) )
	{
		USART1_SendStr("WIFI连接服务器成功\r\n");
	}
	else
	{
		USART1_SendStr("WIFI连接服务器失败\r\n");
	}
	//5.发送开启透传模式指令
	if( WIFI_SendAT("AT+CIPSEND\r\n",3000) )
	{
		USART1_SendStr("WIFI进入透传成功\r\n");
	}
	else
	{
		USART1_SendStr("WIFI进入透传失败\r\n");
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
  * @brief  配置TIM3--PC6控制舵机,SG90初始化
  * @param  None
  * @retval None
  */
void SG90_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  //1.打开时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
  //2.配置引脚与定时器: TIM3 CH1 (PC6)*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	//3.选择GPIO引脚要复用的功能
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3); 
	
	//4.配置时基---频率
	TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;     // 分频：84MHz/84 = 1MHz
	TIM_TimeBaseStructure.TIM_Period = 20000 - 1;     // 周期：20ms (50Hz舵机标准)
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//5.配置定时器变量
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;							//选择PWM模式1，递增计数，当CNT < CCR时，通道有效
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //启动输出比较
  TIM_OCInitStructure.TIM_Pulse = 500;                          //CCR寄存器初值
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;			//输出的极性为高电平有效
	
	//6.初始化通道，注意定时器通道最多有4个，需要根据我们使用的定时器通道来选择
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	//7.使能相应预装载寄存器
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	//8.使能自动重载预装载寄存器
  TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	//9.启动定时器
	TIM_Cmd(TIM3, ENABLE);
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
	WIFI_Config(115200);   //WIFI
	LED_Config();
	TIM_Config();
	SG90_Config();
	TIM3_Config();
	
	//2.订阅主题
	if( Bemfa_Subscribe(LED_TOPIC,3000) )
	{
		ledinitflag = 1;
		//清空缓冲区
		memset((char *)u3_recvbuf,0,BUFSIZE);
		//清空计数器
		u3_recvcnt = 0;
		USART1_SendStr("订阅LED主题成功\r\n");
	}
	else
	{
		USART1_SendStr("订阅LED主题失败\r\n");
	}
	
	if( Bemfa_Subscribe(SG90_TOPIC,3000) )
	{
		sg90initflag = 1;
		//清空缓冲区
		memset((char *)u3_recvbuf,0,BUFSIZE);
		//清空计数器
		u3_recvcnt = 0;
		USART1_SendStr("订阅SG90主题成功\r\n");
	}
	else
	{
		USART1_SendStr("订阅SG90主题失败\r\n");
	}
	
	if( Bemfa_Subscribe(DHT11_TOPIC,3000) )
	{
		dht11initflag = 1;
		//清空缓冲区
		memset((char *)u3_recvbuf,0,BUFSIZE);
		//清空计数器
		u3_recvcnt = 0;
		USART1_SendStr("订阅DHT11主题成功\r\n");
	}
	else
	{
		USART1_SendStr("订阅DHT11主题失败\r\n");
	}
	
  /* Infinite loop */
  for(;;)
  {
		//说明收到来自服务器的消息
		if(recvflag == 1)
		{
			recvflag = 0; //重置接收标志
			
			//测试
			if (!strstr((char *)u3_recvbuf, "cmd=0&res=1")) 
			{
				USART1_SendStr((char *)u3_recvbuf);  // 不是心跳响应才打印
			}	
			
			//处理响应数据
			if(strstr((char *)u3_recvbuf,"cmd=2"))
			{
				if(strstr((char *)u3_recvbuf,"topic=led001"))
				{
					if(strstr((char *)u3_recvbuf,"msg=on"))
					{
						LED_ON();
					}
					else if(strstr((char *)u3_recvbuf,"msg=off"))
					{
						LED_OFF();
					}
				}
				else if(strstr((char *)u3_recvbuf,"topic=sg90009"))
				{
					if(strstr((char *)u3_recvbuf,"msg=on"))
					{
						SG90_TURN180();
						delay_ms(500);
					}
					else if(strstr((char *)u3_recvbuf,"msg=off"))
					{
						SG90_TURN0();
						delay_ms(500);
					}
				}
				//清空缓冲区
				memset((char *)u3_recvbuf,0,BUFSIZE);
				//清空计数器
				u3_recvcnt = 0;	
			}
			else
			{
				//清空缓冲区
				memset((char *)u3_recvbuf,0,BUFSIZE);
				//清空计数器
				u3_recvcnt = 0;	
			}
		}
  }
}


/************************ (C) COPYRIGHT Your Name *************END OF FILE****/



