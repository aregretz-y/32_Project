/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/10/24
  * @brief   这是一个使用UART串口对蓝牙4.0模块进行修改参数的案例
  ******************************************************************************
  * @attention
	*						1.使用硬件配置，需要UART1进行1、3短接，2、4短接
	*						2.MCU的发送端USART1_TX，对应引脚PA9
	*						3.MCU的发送端USART1_RX，对应引脚PA10
	*						4.使用中断去响应处理接收到的信息
	*						5.使用UART2对蓝牙4.0模块进行参数修改，PA2、PA3
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
/* Private variables ---------------------------------------------------------*/
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
		SysTick->LOAD = 168*nus - 1; //
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
  * @brief  配置USRT1参数
  * @param  None
  * @retval None
  */
void USART1_Config(void)
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
  
  //3.配置GPIO参数
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
  * @brief  配置USRT2参数
  * @param  None
  * @retval None
  */
void USART2_Config(void)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  //1.打开时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
  
  //2.打开GPIO复用功能
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
  
  //3.配置GPIO参数
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//4.配置UART参数并初始化
  USART_InitStructure.USART_BaudRate = 9600;                     	//波特率：9600
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;				//子长：8位数据位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;						//停止位：1位
  USART_InitStructure.USART_Parity = USART_Parity_No;								//无校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无数据流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//双模式，接收与发送
  USART_Init(USART2, &USART_InitStructure);
  
  //5.配置NVIC参数
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
	//6.设置中断配置
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
  //7.启动UART
  USART_Cmd(USART2, ENABLE); 
}

/**
	* @brief  发送字符串给蓝牙4.0
  * @param  None
  * @retval None
  */
void USART2_SendStr(char *Str)
{
    while(*Str != '\0')
    {
        USART_SendData(USART2, *Str);  
        Str++;
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
    }
}

/**
  * @brief  USART2的ISR函数
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
    if(USART_GetFlagStatus(USART2, USART_IT_RXNE) != RESET)
    {
        char data = USART_ReceiveData(USART2);  
        
        //将蓝牙模块的响应通过USART1发送给PC查看
        USART_SendData(USART1, data);
        
        // 清除中断标志
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
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
	USART1_Config();
	USART2_Config();
	
	//发送测试指令"AT\r\n"
	USART2_SendStr("AT\r\n");
	delay_ms(500);
	
	//发送测试指令"AT+NAME蓝牙4.0\r\n"
	USART2_SendStr("AT+NAME20251028\r\n");
	delay_ms(500);
	
	//发送测试指令"AT+RESET\r\n"
	USART2_SendStr("AT+RESET\r\n");
	delay_ms(500);
	
	for(;;)
	{

	}
}

/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
