#include "dht11.h"

#define DATA   PGout(9)
#define DATA_R PGin(9)

//更改DHT11 单总线引脚模式函数
void DHT11_DataMode(GPIOMode_TypeDef Mode)
{
	GPIO_InitTypeDef GPIOInitStruct;
	
	//①使能GPIOG9时钟  库函数操作和直接寄存器操作
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);

	//配置PF9 PF10 PE13  PE14  初始化
	GPIOInitStruct.GPIO_Pin   = GPIO_Pin_9;
	GPIOInitStruct.GPIO_Mode  = Mode;//输出模式
	GPIOInitStruct.GPIO_OType = GPIO_OType_PP;//推挽
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;//快速
	GPIOInitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;//浮空
	GPIO_Init(GPIOG,&GPIOInitStruct);
}

//主机发送起始信号 让DHT11采集温湿度 并让他回应
void DHT11_Start(void)
{
	//先设置引脚为输出模式
	DHT11_DataMode(GPIO_Mode_OUT);
	
	DATA = 1;//拉高一小段时间 表示总线空闲
	delay_ms(100);
	
	//真正起始信号
	DATA = 0;
	delay_ms(20);//>18ms <30ms  
	DATA = 1;
}

//从机应答过程
int DHT11_SlaveACK(void)
{
	int i=0;
	//先设置引脚为输入模式
	DHT11_DataMode(GPIO_Mode_IN);
	
	//等待从机将数据线拉低 应答
	while(DATA_R == 1)
	{
		//超时处理
		delay_us(1);
		i++;
		if(i>10000)
			return -1;//表示超时退出
	}

	
	//等待83us低电平过去
	i=0;
	while(DATA_R == 0)
	{
		//超时处理
		delay_us(1);
		i++;
		if(i>500)
			return -2;//表示超时退出
	}
	
	//等待87us高电平过去
	i=0;
	while(DATA_R == 1)
	{
		//超时处理
		delay_us(1);
		i++;
		if(i>500)
			return -3;//表示超时退出
	}
	//DATA已经变为低电平
	return 0;//正常应答
}


//总的数据位 40Bit 8+8+  8+8  +8
unsigned char DHT11_ReadByte(void)
{
	unsigned char TempData=0;
	int i=0,j;
	//先设置引脚为输入模式
	DHT11_DataMode(GPIO_Mode_IN);
	
	for(j=0;j<8;j++)
	{
		//////////////////第一位--最高位///////////////////////////
		//等待每一个数据位前面54us的低电平过去
		while(DATA_R == 0)
		{
			//超时处理
			delay_us(1);
			i++;
			if(i>5000)
				return 0;//表示超时退出
		}
//		i = 0;
		//DATA引脚刚刚变为高电平
//		while(DATA_R == 1)
//		{
//			delay_us(1);
//			i++;
//		}
		
		delay_us(35);//直接延时35us  再判断DATA电平  数据0和数字1就区分开来了
		if(DATA_R == 1)
		{
			//数字1
			TempData |= 0x01<<(7-j);
			while(DATA_R == 1);//等待剩下的高电平过去
		}
		
		
		//printf("i:%d\r\n",i);
		//高电平期间结束 判断i的值来知道刚刚高电平持续了多少us
//		if(i>68)
//		{
//			//数字1
//			TempData |= 0x01<<(7-j);
//			
//		}
//		else
//		{
//			//数字0  跳过
//			TempData &= ~(0x01<<(7-j));
//		}
	}
	
	return TempData;
}


//停止信号
void DHT11_Stop(void)
{
	delay_us(54);
	
	//再把引脚设置为输出模式  
	DHT11_DataMode(GPIO_Mode_OUT);

	DATA = 1;//将总线拉高 结束本次通信
}


unsigned char TH_RH[5];

//把以上4部分函数 组合成一个完整的DHT11时序
int DHT11_Read(void)
{
	
	//先关闭全部中断
	
	DHT11_Start();//主机发起始信号
	
	if(DHT11_SlaveACK() != 0)//从机响应
	{
		//printf("超时！！！\r\n");
		return -2;//表示超时
	}
	
	TH_RH[0] = DHT11_ReadByte();
	TH_RH[1] = DHT11_ReadByte();
	TH_RH[2] = DHT11_ReadByte();
	TH_RH[3] = DHT11_ReadByte();
	TH_RH[4] = DHT11_ReadByte();
	
	DHT11_Stop();
	
	//再开启全部中断
	
	//校验
	if((unsigned char)(TH_RH[0] + TH_RH[1] + TH_RH[2] + TH_RH[3]) == TH_RH[4])
	{
		//数据正确
		return 0;
	}
	else
	{
		//数据错误
		return -1;
	}
	
}



