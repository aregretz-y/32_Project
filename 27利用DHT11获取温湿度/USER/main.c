
/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/10/30
  * @brief   这是利用MQTT协议实现与巴法云的连接
  ******************************************************************************
  * @attention
  *				
	*       1.WIFI模块默认的波特率是115200bps,字符格式：8N1，采用UART3(PB10、PB11)
	*				2.WIFI模组是支持AT指令的，所有的AT指令必须采用大写，必须以"\r\n"结尾
	*       3.巴法云是有MQTT设备云，MQTT设备的端口是9501
	*       4.需要重新再MQTT设备云上创建新的主题，和TCP设备云是独立的
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  //用户必须包含
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#include "delay.h"
#include "uart.h"
#include "dht11.h"


/* Private typedef -----------------------------------------------------------*/

extern __IO uint8_t  u3_recvbuf[BUFSIZE]; //UART3的接收缓冲区
extern __IO uint16_t u3_recvcnt;          //UART3的接收计数器

extern __IO uint8_t  recvflag;				 //接收完成的标志
extern __IO uint8_t  initflag;				 //初始化完成的标志

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

#define  BYTE0(addr) (*((unsigned char *)addr+0))
#define  BYTE1(addr) (*((unsigned char *)addr+1))
#define  BYTE2(addr) (*((unsigned char *)addr+2))
#define  BYTE3(addr) (*((unsigned char *)addr+3))


#define BEEP_ON()		GPIO_SetBits(GPIOF,GPIO_Pin_8)
#define BEEP_OFF()	GPIO_ResetBits(GPIOF,GPIO_Pin_8)



//待连接热点的参数，需要修改
#define WIFI_SSID		"cb"
#define WIFI_PASSWD "12345678"

//待连接服务器的参数，无需修改
#define SERVER_ADDR "bemfa.com"
#define SERVER_PORT 9501          //MQTT设备云的端口

//巴法云服务器的参数，需要修改
#define BEMFA_UID		"a64a8c06bd5f4df8ba03c205a36cca7e"


//巴法云MQTT设备的主题的参数，需要修改
#define LED_MQTT_TOPIC  "led001"


/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/




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
  * @brief  巴法云的CONNECT报文
  * @param  client_id:指的是客户端ID
  * @param  timeout:指的是超时时间,以ms为单位
  * @retval true or false
  * @note   客户端ID指的是巴法云的私钥
  */
bool Bemfa_Connect(char *client_id,uint32_t timeout)
{
	char sendbuf[512] = {0}; 		//用于存储报文内容
	uint16_t cnt = 0;        		//用于记录报文大小
	uint16_t clientid_len = 0; 	//用于记录客户端ID的长度
	uint32_t remain_len = 0; 		//用于记录剩余长度
	
	//1.构造固定报头
	sendbuf[cnt++] = 0x10;
	
	//剩余长度 = 可变报头（10） + 有效载荷（客户端ID + 2）
	remain_len = 10 + (2+strlen(client_id));
	
	//进行编码
	uint8_t encodedByte = 0;
	do
	{
		encodedByte = remain_len % 128;
		remain_len = remain_len / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( remain_len > 0 )
		{
			encodedByte = encodedByte | 128;
		}
		sendbuf[cnt++] = encodedByte;
		
	}while ( remain_len > 0 );
	

	//2.构造可变报头
	sendbuf[cnt++] = 0;    //协议名称
	sendbuf[cnt++] = 4;
	sendbuf[cnt++] = 'M';
	sendbuf[cnt++] = 'Q';
	sendbuf[cnt++] = 'T';
	sendbuf[cnt++] = 'T';
	sendbuf[cnt++] = 0x04; //协议版本
	sendbuf[cnt++] = 0x02; //连接标志
	sendbuf[cnt++] = 0;    //心跳时间 MSB
	sendbuf[cnt++] = 120;  //心跳时间 LSB
	
	//3.构造有效载荷
	clientid_len = strlen(client_id);
	
	
	sendbuf[cnt++] = BYTE1(&clientid_len);    //字符串长度 MSB 
	
	sendbuf[cnt++] = BYTE0(&clientid_len);    //字符串长度 LSB 
	strcpy(&sendbuf[cnt],client_id);
	cnt+=clientid_len;
	
	//4.发送报文
	
	memset((char *)u3_recvbuf,0,BUFSIZE);//清空缓冲区
	u3_recvcnt = 0;//清空计数器
	
	USART3_SendBytes(sendbuf,cnt);

	
	//超时处理
	while(--timeout)
	{
		delay_ms(1);
		
		if( u3_recvbuf[0] == 0x20 && u3_recvbuf[1] == 0x02)
		{
			
			switch( u3_recvbuf[3] )
			{
				case 0x00:printf("连接已被服务端接受\r\n");break;
				case 0x01:printf("服务端不支持客户端请求的 MQTT 协议级别\r\n");break;
				case 0x02:printf("客户端标识符是正确的 UTF-8 编码，但服务端不允许使用\r\n");break;
				case 0x03:printf("网络连接已建立，但 MQTT 服务不可用\r\n");break;
				case 0x04:printf("用户名或密码的数据格式无效\r\n");break;
				case 0x05:printf("客户端未被授权连接到此服务器\r\n");break;
			}
			
			//提前终止循环
			break;
		}  
	}
	
	if(timeout>0)
		return true;
	else
		return false;
	
}


/**
  * @brief  巴法云的SUBCRIBE报文
  * @param  topic_name:指的是要订阅的主题名
  * @param  timeout:指的是超时时间,以ms为单位
  * @retval true or false
  * @note   客户端ID指的是巴法云的私钥
  */
bool Bemfa_Subcribe(char *topic_name,uint32_t timeout)
{
	char sendbuf[512] = {0}; 		//用于存储报文内容
	uint16_t cnt = 0;        		//用于记录报文大小
	uint16_t topic_name_len = 0; 	//用于记录主题名长度
	uint32_t remain_len = 0; 		//用于记录剩余长度
	
	//1.构造固定报头
	sendbuf[cnt++] = 0x82;
	
	//剩余长度 = 可变报头（2） + 有效载荷（主题名 + 2 + 1）
	remain_len = 2 + (2+strlen(topic_name)+1);
	
	//进行编码
	uint8_t encodedByte = 0;
	do
	{
		encodedByte = remain_len % 128;
		remain_len = remain_len / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( remain_len > 0 )
		{
			encodedByte = encodedByte | 128;
		}
		sendbuf[cnt++] = encodedByte;
		
	}while (remain_len > 0 );
	
	//2.构造可变报头
	//报文标识符
	sendbuf[cnt++] = 0;
	sendbuf[cnt++] = 10;

	//3.构造有效载荷
	sendbuf[cnt++] = 0;    
	topic_name_len = strlen(topic_name);
	sendbuf[cnt++] = topic_name_len;
	
	strcpy(&sendbuf[cnt],topic_name);
	cnt+=topic_name_len;
	sendbuf[cnt++] = 0x01; //质量要求等级
	
	//4.发送报文
	memset((char *)u3_recvbuf,0,BUFSIZE);//清空缓冲区
	u3_recvcnt = 0;//清空计数器
	
	USART3_SendBytes(sendbuf,cnt);
	delay_ms(200);                         // 给服务器200ms回包
	
	//超时处理
	while(--timeout)
	{
		delay_ms(1);
		
		if( u3_recvbuf[0] == 0x90 && u3_recvbuf[1] == 0x03 && u3_recvbuf[2] == 0x00 && u3_recvbuf[3] == 0x0A)
		{
			
			switch( u3_recvbuf[4] )
			{
				case 0x00:printf("最大 QoS 0\r\n");break;
				case 0x01:printf("最大 QoS 1\r\n");break;
				case 0x02:printf("最大 QoS 2\r\n");break;
				case 0x80:printf("失败\r\n");break;
			}
			
			//提前终止循环
			break;
		}  
	}
	
	if(timeout>0)
		return true;
	else
		return false;
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
//	WIFI_Config(115200);   //WIFI
	DHT11_Config();					//DHT11
	
//	//2.发送连接请求
//	if( Bemfa_Connect(BEMFA_UID,6000))
//	{
//		printf("巴法云MQTT连接成功\r\n");
//	}
//	else
//		printf("巴法云MQTT连接失败\r\n");
//	
//	//2.发送订阅请求
//	if( Bemfa_Subcribe(LED_MQTT_TOPIC,10000))
//	{
//		initflag = 1;
//		printf("巴法云MQTT主题订阅成功\r\n");
//	}
//	else
//		printf("巴法云MQTT主题订阅连接失败\r\n");
  /* Infinite loop */
	uint8_t dhtbuf[5] = {0};
  for(;;)
  {
		DHT11_GetVal(dhtbuf);
		printf("tmep = %d ℃ humi = %d %%RH \r\n",dhtbuf[2],dhtbuf[0]);
		delay_ms(1000);
  }
}







/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
