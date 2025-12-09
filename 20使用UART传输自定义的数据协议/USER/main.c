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
	*						5.LED0使用PF9引脚
	*			
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  //用户必须包含
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
// 简化版数据包结构体定义
typedef struct 
{
    uint8_t header;     // 包头 0xAA
    uint8_t length;     // 包长 0x08
    uint8_t source;     // 数据包源
    uint8_t command;    // 指令码
    uint8_t data[3];    // 数据字节
    uint8_t checksum;   // 校验和
} Packet_t;

/* Private define ------------------------------------------------------------*/
#define PACKET_HEADER       0xAA
#define PACKET_LENGTH       0x08
#define RX_BUFFER_SIZE      16

// 数据包源定义
#define SOURCE_PC           0x01  // PC端
#define SOURCE_MCU          0x55  // MCU响应

// 指令码定义
#define CMD_LED0_CONTROL    0x01  // LED0控制
#define CMD_ECHO_TEST       0x02  // 回显测试

// LED0引脚定义
#define LED0_PIN            GPIO_Pin_9
#define LED0_GPIO_PORT      GPIOF

/* Private macro -------------------------------------------------------------*/
#define LED0_ON()           GPIO_ResetBits(LED0_GPIO_PORT, LED0_PIN)
#define LED0_OFF()          GPIO_SetBits(LED0_GPIO_PORT, LED0_PIN)
#define LED0_TOGGLE()       GPIO_ToggleBits(LED0_GPIO_PORT, LED0_PIN)

/* Private variables ---------------------------------------------------------*/
uint8_t rx_buffer[RX_BUFFER_SIZE];     // 接收缓冲区
uint8_t rx_index = 0;                  // 接收索引
uint8_t packet_received = 0;           // 数据包接收完成标志

/* Private function prototypes -----------------------------------------------*/
void USART_Config(void);
void LED_Config(void);
uint8_t Verify_Checksum(Packet_t *packet);
void Process_Packet(Packet_t *packet);
void Send_Response(uint8_t command, uint8_t* data);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  LED引脚配置
  * @param  None
  * @retval None
  */
void LED_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 开启GPIOF时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    
    // 配置PF9为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(LED0_GPIO_PORT, &GPIO_InitStructure);
    
    //初始状态：LED熄灭
    LED0_OFF();
}

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
    USART_InitStructure.USART_BaudRate = 115200;                     //波特率：115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;      //字长：8位数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;           //停止位：1位
    USART_InitStructure.USART_Parity = USART_Parity_No;              //无校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无数据流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //双模式，接收与发送
    USART_Init(USART1, &USART_InitStructure);
    
    //5.配置NVIC参数
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    //6.设置中断配置
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    //7.启动UART
    USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  校验和验证
  * @param  packet: 数据包指针
  * @retval 校验结果: 1-成功, 0-失败
  */
uint8_t Verify_Checksum(Packet_t *packet)
{
    uint8_t calculated_checksum = 0;
    uint8_t *data = (uint8_t*)packet;
    
    // 计算前7个字节的和
    for(int i = 0; i < 7; i++) 
		{
        calculated_checksum += data[i];
    }
    
    // 取低字节
    calculated_checksum &= 0xFF;
    return (calculated_checksum == packet->checksum);
}

/**
  * @brief  处理接收到的数据包
  * @param  packet: 数据包指针
  * @retval None
  */
void Process_Packet(Packet_t *packet)
{
    uint8_t response_data[3] = {0};
    
    // 验证数据包
    if(packet->header != PACKET_HEADER) 
		{
        // 包头错误
        response_data[0] = 0xFF; // 错误码
        Send_Response(packet->command, response_data);
        return;
    }
    
    if(packet->length != PACKET_LENGTH) 
		{
        // 包长错误
        response_data[0] = 0xFE; // 错误码
        Send_Response(packet->command, response_data);
        return;
    }
    
    if(!Verify_Checksum(packet)) 
		{
        // 校验和错误
        response_data[0] = 0xFD; // 错误码
        Send_Response(packet->command, response_data);
        return;
    }
    
    // 根据指令码处理数据
    switch(packet->command) 
		{
        case CMD_LED0_CONTROL:
            // 处理LED0控制命令
            // packet->data[0]: 状态 (0-关, 1-开, 2-翻转)
            if(packet->data[0] == 0x00) 
						{
                LED0_OFF();
                response_data[0] = 0x00; //成功，LED已关闭
            }
            else if(packet->data[0] == 0x01) 
						{
                LED0_ON();
                response_data[0] = 0x01; //成功，LED已打开
            }
            else if(packet->data[0] == 0x02) {
                LED0_TOGGLE();
                response_data[0] = 0x02; //成功，LED已翻转
            }
            else {
                response_data[0] = 0xFC; // 无效参数
            }
            break;
            
        case CMD_ECHO_TEST:
            // 回显测试，直接返回接收到的数据
            memcpy(response_data, packet->data, 3);
            response_data[0] = 0x00; // 测试成功
            break;
            
        default:
            // 未知指令码
            response_data[0] = 0xFB; // 未知指令
            break;
    }
    
    // 发送响应
    Send_Response(packet->command, response_data);
}

/**
  * @brief  发送响应数据包
  * @param  command: 指令码
  * @param  data: 数据指针
  * @retval None
  */
void Send_Response(uint8_t command, uint8_t* data)
{
    Packet_t response_packet;
    uint8_t *packet_bytes = (uint8_t*)&response_packet;
    
    // 填充数据包
    response_packet.header = PACKET_HEADER;
    response_packet.length = PACKET_LENGTH;
    response_packet.source = SOURCE_MCU;  // 来自MCU的响应
    response_packet.command = command;
    
    if(data != NULL) {
        memcpy(response_packet.data, data, 3);
    } else {
        memset(response_packet.data, 0, 3);
    }
    
    // 计算校验和
    response_packet.checksum = 0;
    for(int i = 0; i < 7; i++) 
		{
        response_packet.checksum += packet_bytes[i];
    }
    response_packet.checksum &= 0xFF;
    
    // 发送数据包
    for(int i = 0; i < sizeof(Packet_t); i++) {
        USART_SendData(USART1, packet_bytes[i]);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
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
        uint8_t data = USART_ReceiveData(USART1);
        
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
    }
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void) 
{     
    // 系统初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 配置NVIC优先级组
    LED_Config();     // 初始化LED
    USART_Config();   // 初始化串口
    
    // 初始化接收缓冲区
    memset(rx_buffer, 0, RX_BUFFER_SIZE);
    rx_index = 0;
    packet_received = 0;
    
    // 发送启动消息
    uint8_t startup_data[3] = {0x00, 0x55, 0xAA}; // 启动标识
    Send_Response(CMD_ECHO_TEST, startup_data);
    
    for(;;)
    {
        // 检查是否收到完整的数据包
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







