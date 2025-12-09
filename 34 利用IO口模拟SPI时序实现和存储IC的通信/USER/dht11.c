#include "dht11.h"


/**
  * @brief  DHT11的初始化
  * @param  None
  * @retval None
  */
void DHT11_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
}


/**
  * @brief  DHT11引脚模式设置
  * @param  GPIO_Mode：引脚模式
  * @retval None
  */
void DHT11_PinMode(GPIOMode_TypeDef GPIO_Mode)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

}


/**
  * @brief  DHT11的开始信号
  * @param  None
  * @retval None
  */
void DHT11_Start(void)
{
	//1.设置引脚为输出模式
	DHT11_PinMode(GPIO_Mode_OUT);
	
	//2.把总线的电平拉低
	DHT11_SET(0);
	
	//3.延时一段时间 18ms ~ 30ms
	delay_ms(20);
	
	//4.设置引脚为输入模式
	DHT11_PinMode(GPIO_Mode_IN);

}

/**
  * @brief  DHT11的响应信号
  * @param  None
  * @retval true 表示应答了  false 表示未应答
  */
bool DHT11_IsACK(void)
{
	uint8_t cnt = 0;
	
	//1.等待PG9引脚检测到低电平,应该人为的增加超时机制
	while( DHT11_READ() == SET && cnt < 100)
	{
		delay_us(1);
		cnt++;
	}
	
	if(cnt>=100)
		return false; 
	
	//2.判断PG9引脚的低电平的持续时间,应该人为的增加超时机制
	cnt = 0;
	while( DHT11_READ() == RESET && cnt < 100)
	{
		delay_us(1);
		cnt++;
	}
	
	if(cnt>=100)
		return false;
	else
		return true;
}


/**
  * @brief  DHT11的读取bit
  * @param  None
  * @retval 读取的bit的值 0000000 1 -- 0x01  0000000 0 -- 0x00
  */
uint8_t DHT11_ReadBit(void)
{
	uint8_t cnt = 0;
	
	//1.等待PG9引脚检测到低电平
	while( DHT11_READ() == SET && cnt < 100)
	{
		delay_us(1);
		cnt++;
	}
	
	cnt = 0;
	
	//2.等待PG9引脚检测到高电平
	while( DHT11_READ() == RESET && cnt < 100)
	{
		delay_us(1);
		cnt++;
	}
	
	//3.出现高电平之后直接延时一段时间 >28us && < 68us
	delay_us(40);
	
	if(DHT11_READ()== SET)
		return 0x01;
	else
		return 0x00;

}


/**
  * @brief  DHT11的读取字节
  * @param  None
  * @retval 读取的字节
  */
uint8_t DHT11_ReadByte(void)
{
	uint8_t data = 0; 
	uint8_t cnt = 0;
	
	//1.循环读取8bit,MSB先出
	for(cnt=0;cnt<8;cnt++)
	{
		//读取1bit
		data <<= 1;
		data |= DHT11_ReadBit(); 
	  
	}
	
	return data;
}

/**
  * @brief  DHT11的读取字节
  * @param  dhtbuf：用于存储数据的数组地址
  * @retval true 表示应答了  false 表示未应答
  */
bool DHT11_GetVal(uint8_t * dhtbuf)
{
	uint8_t i = 0;
	
	//1.发送开始信号
	DHT11_Start();
	
	//2.等待DHT11响应
	if( DHT11_IsACK() )
	{
		//3.循环读取40bit
		for(i=0;i<5;i++)
		{
			dhtbuf[i] = DHT11_ReadByte();
		}
		
		//4.进行数据校验
		if( dhtbuf[0] + dhtbuf[1] + dhtbuf[2] + dhtbuf[3] == dhtbuf[4] )
			return true;
		else
			return false; //读取数据失败，因为数据校验失败
		
	}
	else
		return false;   //读取数据失败，因为DHT11未响应

}

