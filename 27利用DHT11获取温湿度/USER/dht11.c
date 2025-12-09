#include "dht11.h"

/**
	* @brief  初始化GPIOG--PG9为输出模式
	* @param  None
	* @retval None
	*/
void DHT11_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  //1.打开GPIO时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  
  //2.配置GPIO引脚
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/**
	* @brief  设置引脚工作模式
	* @param  GPIO_Mode：指的是要选择的工作模式
	* @retval None
	*/
void DHT11_PinMode(GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  //1.打开GPIO时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  
  //2.配置GPIO引脚
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
}


/**
	* @brief  发出起始信号
	* @param  None
	* @retval None
	*/
void DHT11_Start(void)
{
	//1.设置输出模式
	DHT11_PinMode(GPIO_Mode_OUT);

	//2.拉低电平
	DHT11_SET(0);
	
	//3.保持20ms
	delay_ms(20);
	
	//4.设置端口为输入模式
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
	
	if(cnt >= 100)
		return false;
	
	cnt = 0;
	//2.检测PG9引脚低电平的有效时间,应该人为的增加超时机制
	while( DHT11_READ() == RESET && cnt < 100)
	{
		delay_us(1);
		cnt++;
	}
	
	if(cnt >= 100)
		return false;
	else
		return true;

}

/**
  * @brief  获取一个bit数据
  * @param  None
  * @retval uint8_t
  */
uint8_t DHT11_ReadBit(void)
{
	uint8_t cnt = 0;
	
	//1.等待检测低电平
	while( DHT11_READ() == SET && cnt < 100)
	{
		delay_us(1);
		cnt++;
	}
	
	//2.等待检测高电平
	while( DHT11_READ() == RESET && cnt < 100)
	{
		delay_us(1);
		cnt++;
	}
	
	//3.延时等待查看此时电平状态，要是电平为高电平，则返回0x01,反之，返回0x00
	delay_us(40);
	
	if( DHT11_READ() )
	{
			return 0x01;
	}
	else
		return 0x00;

}

/**
  * @brief  获取一个字节数据
  * @param  None
  * @retval uint8_t
  */
uint8_t DHT11_ReadByte(void)
{
	uint8_t i;
	uint8_t data;
	for(i = 0; i<8;i++)
	{
		data <<= 1;
		data |= DHT11_ReadBit();
	}
	return data;
}

/**
	* @brief  获取DHT11发送过来的数据
	* @param  dhtbuf:指的是存储数据的数组
  * @retval void
	*/
bool DHT11_GetVal(uint8_t *dhtbuf)
{
	//1.发送起始信号
	DHT11_Start();

	//2.接收应答信号
	if( DHT11_IsACK() )
	{
		for(uint8_t i = 0; i<5; i++)
		{
			dhtbuf[i] = DHT11_ReadByte();
		}
		
		//3.进行和校验
		if( dhtbuf[0] + dhtbuf[1] + dhtbuf[2] + dhtbuf[3] == dhtbuf[4])
			return true;
		else
			return false;//读取数据失败，因为数据校验失败
	}
	else
		return false;//读取数据失败，因为DHT11未响应
}











