#include "spi.h"

#define SPIMODE 0


//对SPI引脚的配置
void SPI_GPIOInit(void)
{
	GPIO_InitTypeDef        GPIOInitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	
	GPIOInitStruct.GPIO_Pin   = GPIO_Pin_4;//MOSI
	GPIOInitStruct.GPIO_Mode  = GPIO_Mode_OUT;//复用功能 IN OUT AF AN
	GPIOInitStruct.GPIO_OType = GPIO_OType_PP;//复用推挽输出 
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_100MHz;//最高速
	GPIOInitStruct.GPIO_PuPd  = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA,&GPIOInitStruct);
	
	GPIOInitStruct.GPIO_Pin   = GPIO_Pin_7;//SCK
	GPIO_Init(GPIOB,&GPIOInitStruct);
	
	GPIOInitStruct.GPIO_Pin   = GPIO_Pin_7 | GPIO_Pin_9; // D/C  CS
	GPIO_Init(GPIOC,&GPIOInitStruct);
	
	GPIOInitStruct.GPIO_Pin   = GPIO_Pin_15;//RES
	GPIO_Init(GPIOG,&GPIOInitStruct);
	
	//根据SPI模式选择SCK默认电平
#if SPIMODE==0 || SPIMODE==1
	SPI_SCK = 0;
#elif SPIMODE==2 || SPIMODE==3
	SPI_SCK = 1;
#endif
}


//SPI 模式0的读写8位函数
u8  SPI0_ReadWriteByte(u8 WData)
{
	int i;
	u8  RData=0;//定义一个用来接收MISO从机发过来的数据
	for(i=0;i<8;i++)
	{
		SPI_SCK = 0; // 下降沿 SPI mode0  主机可以通过MOSI发出数据
		if(WData & (0x80>>i))
			  SPI_MOSI = 1;
		else
			  SPI_MOSI = 0;
		delay_us(1);


		SPI_SCK = 1;//上升沿 SPI mode0  主机可以通过MISO接收数据
		//给从机接收的时间
		if(SPI_MISO)
			RData |= (0x80>>i);
		delay_us(1);
	}
	return RData;//返回读到的8bit结果 
}


//SPI模式1 极性0--第一边沿发送 第二边沿接收
//上升沿发送 下降沿接收
u8  SPI1_ReadWriteByte(u8 WData)
{
	int i;
	u8 RData = 0;
	
	for(i=0;i<8;i++)
	{
		SPI_SCK = 1;
		if(WData & (0x80>>i))
			SPI_MOSI = 1;
		else
			SPI_MOSI = 0;
		delay_us(1);
		
		SPI_SCK = 0;
		if(SPI_MISO)
			RData |= (0x80>>i);
		delay_us(1);
	}
	
	return RData;
}

//CPOL==1  CPHA==0
u8  SPI2_ReadWriteByte(u8 WData)
{
	int i;
	u8 RData = 0;
	
	for(i=0;i<8;i++)
	{
		SPI_SCK = 1;
		if(WData & (0x80>>i))
			SPI_MOSI = 1;
		else
			SPI_MOSI = 0;
		delay_us(1);
		
		SPI_SCK = 0;
		if(SPI_MISO)
			RData |= (0x80>>i);
		delay_us(1);
	}
	
	return RData;
}


//CPOL==1  CPHA==1
u8  SPI3_ReadWriteByte(u8 WData)
{
	int i;
	u8 RData = 0;
	
	for(i=0;i<8;i++)
	{
		SPI_SCK = 0;//第一边沿--下降沿
		if(WData & (0x80>>i))//先判断待发送数据的最高位是0还是1 
			SPI_MOSI = 1;
		else
			SPI_MOSI = 0;
		delay_us(1);//低电平期间
		
		
		SPI_SCK = 1;//第二边沿--上升沿
		if(SPI_MISO)
			RData |= (0x80>>i);//读到的是数字1 存放容器RData对应位
		else
			RData &= ~(0x80>>i);//读到数字0 
		delay_us(1);
	}
	
	return RData;//返回读到的结果
}
