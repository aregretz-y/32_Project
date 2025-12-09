#include "iic.h"

/**
	* @brief  IIC引脚初始化
	* @param  None
	* @retval None
	*/
void IIC_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  //1.打开GPIO时钟
  RCC_AHB1PeriphClockCmd(IIC_SCL_RCC, ENABLE);
  RCC_AHB1PeriphClockCmd(IIC_SDA_RCC, ENABLE);
	
  //2.配置GPIO引脚
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin 	= IIC_SDA_PIN;
  GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin 	= IIC_SCL_PIN;
  GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);
	
	//IIC默认处于空闲状态
	IIC_SCL(1);
	IIC_SDA(1);
}


/**
	* @brief  设置引脚工作模式
	* @param  GPIO_Mode：指的是要选择的工作模式
	* @retval None
	*/
void IIC_SDAPinMode(GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  //1.打开GPIO时钟
  RCC_AHB1PeriphClockCmd(IIC_SDA_RCC, ENABLE);
  
  //2.配置GPIO引脚
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin 	= IIC_SDA_PIN;
  GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);
}

/**
  * @brief  IIC的开始信号
  * @param  None
  * @retval None
  */
void IIC_Start(void)
{
	//1.确认SDA为输出模式
	IIC_SDAPinMode(GPIO_Mode_OUT);
	
	//2.确保总线为空闲模式
	IIC_SCL(1);
	IIC_SDA(1);
	delay_us(5);
	
	//3.在时钟线为高电平期间，把数据线电平拉低
	IIC_SDA(0);
	delay_us(5);
	
	//4.把时钟线拉低
	IIC_SCL(0);
	delay_us(5);
}

/**
  * @brief  IIC发送数据
  * @param  uint8_t 指的是待发送的字节   
  * @retval None
  */
void IIC_Send(uint8_t byte)
{
	
	uint8_t cnt = 0;
	
	//1.确认SDA为输出模式
	IIC_SDAPinMode(GPIO_Mode_OUT);
	
	//2.循环输出数据，以MSB为格式
	for(cnt=0;cnt<8;cnt++)
	{
		//3.拉低SCL，此时是第1个脉冲的低电平，主机准备
		IIC_SCL(0);
		delay_us(5);
		
		//4.主机开始准备数据
		if( byte & 0x80)
			IIC_SDA(1);
		else
			IIC_SDA(0);
		byte <<= 1;
		delay_us(5);
		
		//5.拉高SCL，此时是第1个脉冲的高电平，从机接收
		IIC_SCL(1);
		delay_us(5);
	}
	
	//6.拉低SCL，此时是第8个脉冲结束
	IIC_SCL(0);
	delay_us(5);
}

/**
  * @brief  IIC判断从机应答
  * @param  None   
  * @retval true 表示应答 false 表示不应答
  */
bool IIC_IsACK(void)
{
	bool status = false;
	
	//1.确认SDA为输入模式
	IIC_SDAPinMode(GPIO_Mode_IN);
	
	//2.拉低SCL，此时是第9个脉冲的高电平，从机准备
	IIC_SCL(0);
	delay_us(5);
	
	//3.拉高SCL，此时是第9个脉冲的高电平，主机接收
	IIC_SCL(1);
	delay_us(5);
	
	//4.主机准备接收,1为拒绝继续通信，flase，0为继续通信，true
	if( IIC_SDA_READ() ) 
		status = false;
	else
		status = true;

	//5.拉低SCL，此时第9个脉冲结束
	IIC_SCL(0);
	delay_us(5);
	
	return status;
}

/**
  * @brief  IIC的停止条件
  * @param  None
  * @retval None
  */
void IIC_Stop(void)
{
	//1.确认SDA为输出模式
	IIC_SDAPinMode(GPIO_Mode_OUT);
	
	//2.确保SDA SCL都为低电平状态
	IIC_SCL(0);
	IIC_SDA(0);
	delay_us(5);
	
	//3.拉高SCL
	IIC_SCL(1);
	delay_us(5);

	//4.在SCL高电平期间，把数据线电平拉高
	IIC_SDA(1);
	delay_us(5);
}


