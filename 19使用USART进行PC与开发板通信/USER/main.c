/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/10/24
  * @brief   这是一个使用UART进行双端通信的案例
  ******************************************************************************
  * @attention
	*						1.使用硬件配置，需要UART1进行1、3短接，2、4短接
	*						2.MCU的发送端USART1_TX，对应引脚PA9
	*						3.MCU的发送端USART1_RX，对应引脚PA10
	*						4.使用中断去响应处理接收到的信息
	*				
	*			
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  //用户必须包含

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//按键对应的IO管脚 KEY1  PA.15
/* Private macro -------------------------------------------------------------*/
//Key: 1:高电平，按键未按下， 0：低电平，按键按下
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  配置USRT1参数
  * @param  None
  * @retval None
  */
void USART_Config(void)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  //1.打开时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
  
  //2.打开GPIO复用功能
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
  
  //3.配置GPIO参数+
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	//4.配置UART参数并初始化
  USART_InitStructure.USART_BaudRate = 115200;                     	//波特率：115200
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;				//子长：8位数据位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;						//停止位：1位
  USART_InitStructure.USART_Parity = USART_Parity_No;								//无校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无数据流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//双模式，接收与发送
  USART_Init(USART1, &USART_InitStructure);
  
  //5.配置NVIC参数
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
	//6.设置中断配置
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
  //7.启动UART
  USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  ISR函数
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
    // 检查是否是RXNE中断
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        // 读取接收到的数据
        char data = USART_ReceiveData(USART1);
        
        // 处理接收到的数据
        USART_SendData(USART1, data);
 
    }
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void) 
{     
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//配置NVIC优先级组
	USART_Config();
	for(;;)
	{
		
	}
}

/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
