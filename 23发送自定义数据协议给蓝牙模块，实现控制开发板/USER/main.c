/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/10/28
  * @brief   这是一个使用UART串口对蓝牙4.0模块连接，然后手机小程序对蓝牙发送命令，达到控制开发板的目的
  ******************************************************************************
  * @attention
	*						1.使用硬件配置，需要UART1进行1、3短接，2、4短接
	*						2.MCU的发送端USART1_TX，对应引脚PA9
	*						3.MCU的发送端USART1_RX，对应引脚PA10
	*						4.使用中断去响应处理接收到的信息
	*						5.使用UART2对蓝牙4.0模块进行参数修改，PA2、PA3
	*						6.控制开发板LED0---PF9
	*						7.自定义数据协议，
	*										包头：0xAA；
	*										包长：0x06；
	*										指令码：0x00(关灯)、0x01(开灯)；
	*										数据码：XX；
	*										校验：和校验；
	*										包尾：0xBB；
	*						8.命令说明：
	*										开灯：AA 06 00 01 B1 BB
	*										关灯：AA 06 00 00 B0 BB
	*										舵机转180度：
	*													AA 06 01 00 B1 BB
	*										舵机转0度：
	*													AA 06 01 01 B2 BB
	*						9.和校验：包头+包长+指令码+数据
	*				
	*			
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  //用户必须包含
#include <stdio.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/

typedef enum
{
		led_free = 0,
		led_off,
		led_on
}led_status;

#pragma pack(1)

typedef struct 
{
    uint8_t header;     // 包头 0xAA
    uint8_t length;     // 包长 0x06
    uint8_t command;    // 指令码
    uint8_t data;    		// 数据字节
    uint8_t checksum;   // 校验和
		uint8_t tail;     	// 包尾 0xBB
} Packet_t;


/* Private define ------------------------------------------------------------*/
#define BLE_NAME "My_BLE"
#define RX_BUFFER_SIZE      16

#define LED_CTRL 0x00
#define SG90_CTRL 0x01


/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
volatile int status = led_free;
uint8_t rx_buffer[RX_BUFFER_SIZE];     // 接收缓冲区
uint8_t rx_index = 0;                  // 接收索引
uint8_t packet_received = 0;           // 数据包接收完成标志

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
  * @brief  计算校验和
  * @param  packet: 数据包指针
  * @retval 计算出的校验和
  */
uint8_t Calculate_Checksum(Packet_t *packet)
{
    // 只计算 header + length + command + data
    uint8_t checksum = packet->header + packet->length + 
                      packet->command + packet->data;
    return checksum & 0xFF;
}

/**
  * @brief  校验和验证
  * @param  packet: 数据包指针
  * @retval 校验结果: 1-成功, 0-失败
  */
uint8_t Verify_Checksum(Packet_t *packet)
{
    uint8_t calculated_checksum = Calculate_Checksum(packet);
    return (calculated_checksum == packet->checksum);
}

/**
  * @brief  配置LED0参数
  * @param  None
  * @retval None
  */

void LED_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	//3.配置GPIO参数
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
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
	
  //7.启动UART2
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
  * @brief  发送响应数据包
  * @param  command: 指令码
  * @param  data: 数据指针
  * @retval None
  */
void Send_Response(uint8_t command, uint8_t data)
{
    Packet_t response_packet;
    uint8_t * packet_bytes = (uint8_t*)&response_packet;
    
    // 填充数据包
    response_packet.header = 0xAA;
    response_packet.length = 0x06;
    response_packet.command = command;
		response_packet.tail = 0xBB;
    
    if(data != NULL) 
		{
			response_packet.data = (uint8_t)data;
    } 
		else 
		{
      response_packet.data = 0;
    }
    
    // 计算校验和
		response_packet.checksum = Calculate_Checksum(&response_packet);

    
    // 发送数据包
    for(int i = 0; i < sizeof(Packet_t); i++) 
		{
        USART_SendData(USART1, packet_bytes[i]);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
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
				// 读取接收到的数据
        uint8_t data = USART_ReceiveData(USART2);  
        
        // 将数据存入缓冲区
        if(rx_index < RX_BUFFER_SIZE) 
				{
            rx_buffer[rx_index++] = data;
            
            // 检查是否收到完整的数据包
            if(rx_index >= sizeof(Packet_t)) 
						{
                packet_received = 1;
            }
        } 
				else 
				{
            // 缓冲区溢出，重置
            rx_index = 0;
            memset(rx_buffer, 0, RX_BUFFER_SIZE);
        }
			
        //将蓝牙模块的响应通过USART1发送给PC查看
        //USART_SendData(USART1, data);

        //清除中断标志
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

/**
  * @brief  处理接收到的数据包
  * @param  packet: 数据包指针
  * @retval None
  */
void Process_Packet(Packet_t *packet)
{
    uint8_t response_data = 0;
    
    //计算期望的校验和
    uint8_t expected_checksum = packet->header + packet->length + 
                               packet->command + packet->data;
    expected_checksum &= 0xFF;
    
    // 验证数据包
    if(packet->header != 0xAA) 
    {
        response_data = 0xFF; // 包头错误
        Send_Response(packet->command, response_data);
        return;
    }
    
    if(packet->length != 0x06) 
    {
        response_data = 0xFE; // 包长错误
        Send_Response(packet->command, response_data);
        return;
    }
    
    if(!Verify_Checksum(packet)) 
    {
        response_data = 0xFD; // 校验和错误
        
        //发送实际接收到的校验和和期望的校验和
        Send_Response(0xFD, packet->checksum); // 发送实际校验和
        // 或者发送期望的校验和：
        // Send_Response(0xFD, expected_checksum);
        return;
    }
    
    if(packet->tail != 0xBB) 
    {
        response_data = 0xFC; // 包尾错误
        Send_Response(packet->command, response_data);
        return;
    }
    
    // 根据指令码处理数据
    switch(packet->command) 
    {
        case LED_CTRL:
            if(packet->data == 0x00) 
            {
                GPIO_SetBits(GPIOF, GPIO_Pin_9);
                response_data = 0x00; //成功，LED已关闭
            }
            else if(packet->data == 0x01) 
            {
                GPIO_ResetBits(GPIOF, GPIO_Pin_9);
                response_data = 0x01; //成功，LED已打开
            }
            else 
            {
                response_data = 0xFB; // 无效参数
            }
            break;
				case SG90_CTRL:
            if(packet->data == 0x00) 
            {
                TIM_SetCompare1(TIM3,2500);
                response_data = 0x00; //成功，舵机180度
            }
            else if(packet->data == 0x01) 
            {
                TIM_SetCompare1(TIM3,500);
                response_data = 0x01; //成功，舵机0度
            }
            else 
            {
                response_data = 0xFB; // 无效参数
            }
            break;
            
        default:
            response_data = 0xFA; // 未知指令
            break;
    }
    
    // 发送响应
    Send_Response(packet->command, response_data);
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
  TIM_OCInitStructure.TIM_Pulse = 2500;                          //CCR寄存器初值
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//配置NVIC优先级组
	USART1_Config();
	USART2_Config();
	LED_Config();
	SG90_Config();
	
	char ble_name[256] = {0};
	
	//发送测试指令"AT\r\n"
	USART2_SendStr("AT\r\n");
	delay_ms(500);
	
	//发送测试指令"AT+NAME蓝牙4.0\r\n"
	sprintf(ble_name,"AT+NAME%s\r\n",BLE_NAME);
	USART2_SendStr(ble_name);
	delay_ms(500);
	
	//发送测试指令"AT+RESET\r\n"
	USART2_SendStr("AT+RESET\r\n");
	delay_ms(500);
	
	// 初始化接收缓冲区
  memset(rx_buffer, 0, RX_BUFFER_SIZE);
  rx_index = 0;
  packet_received = 0;
	
	for(;;)
	{
			if(packet_received)
			{
         // 处理数据包
         Process_Packet((Packet_t*)rx_buffer);
            
         // 重置接收状态
         rx_index = 0;
         packet_received = 0;
         memset(rx_buffer, 0, RX_BUFFER_SIZE);
      }
	}
}

/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
