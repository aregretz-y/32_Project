#include "usart.h"

//串口1 USART1配置函数
void USART1_Init(u32 BaudRate)
{
	GPIO_InitTypeDef        GPIOInitStruct;
	USART_InitTypeDef       USARTInitStruct;
	
	//使能时钟 GPIOA USART1
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	//将PA9 PA10复用为USART1功能
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//配置GPIOF9为复用功能
	GPIOInitStruct.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
	GPIOInitStruct.GPIO_Mode  = GPIO_Mode_AF;//复用功能 IN OUT AF AN
	GPIOInitStruct.GPIO_OType = GPIO_OType_PP;//复用推挽输出 
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_100MHz;//最高速
	GPIOInitStruct.GPIO_PuPd  = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA,&GPIOInitStruct);
	
	//串口配置
	USARTInitStruct.USART_BaudRate = BaudRate;
	USARTInitStruct.USART_WordLength = USART_WordLength_8b;
	USARTInitStruct.USART_StopBits   = USART_StopBits_1;
	USARTInitStruct.USART_Parity     = USART_Parity_No;
	USARTInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不要流控
	USARTInitStruct.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;//收发模式
	USART_Init(USART1,&USARTInitStruct);
	
	//开启串口
	USART_Cmd(USART1,ENABLE);
}


void USART_SendString(USART_TypeDef *USARTx,const char  *String)
{
	while(*String != '\0')
	{
		USARTx->DR = *String++; //USARTx->TDR = 数据
		while((USARTx->SR & (0x01<<7)) == 0);//等待TDR 发送缓冲区的数据全部清空 才能将下一个字节丢到TDR
		//这里不需要清零 循环回到开头 写数据到DR能清零TXE标志位
	}
}



//重写fputc函数
int fputc(int ch, FILE * fp)
{
	USART1->DR = (u8)ch; //USARTx->TDR = 数据
	while((USART1->SR & (0x01<<7)) == 0);
	
	return 0;
}

//scanf底层接口重定向  fgetc重写
int fgetc(FILE * fp)
{
	u8 Data;
	while(!(USART1->SR & 0x01<<5));
	
	Data = USART1->DR;
	
	return Data;
}


/*
串口2配置:连接串口蓝牙模块
*/
//串口1 USART1配置函数
void USART2_Init(u32 BaudRate)
{
	GPIO_InitTypeDef        GPIOInitStruct;
	USART_InitTypeDef       USARTInitStruct;
	
	//使能时钟 GPIOA USART1
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//将PA9 PA10复用为USART1功能
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	//配置GPIOF9为复用功能
	GPIOInitStruct.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;
	GPIOInitStruct.GPIO_Mode  = GPIO_Mode_AF;//复用功能 IN OUT AF AN
	GPIOInitStruct.GPIO_OType = GPIO_OType_PP;//复用推挽输出 
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_100MHz;//最高速
	GPIOInitStruct.GPIO_PuPd  = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA,&GPIOInitStruct);
	
	//串口配置
	USARTInitStruct.USART_BaudRate = BaudRate;
	USARTInitStruct.USART_WordLength = USART_WordLength_8b;
	USARTInitStruct.USART_StopBits   = USART_StopBits_1;
	USARTInitStruct.USART_Parity     = USART_Parity_No;
	USARTInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不要流控
	USARTInitStruct.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;//收发模式
	USART_Init(USART2,&USARTInitStruct);
	
	
	//开启串口
	USART_Cmd(USART2,ENABLE);
}




