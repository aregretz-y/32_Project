#include "iic.h"

#define SCK    PBout(8)
#define SDA    PBout(9)
#define SDA_R  PBin(9)

//这种方法可以在你移植代码时，简单一些
#define delay_us(n)  delay_us(n)
#define delay_ms(n)  delay_ms(n)
#define delay_s(n)   delay_s(n)

//这个函数建议放在主函数开头 初始化一次
void IIC_GPIOInit(void)
{
	GPIO_InitTypeDef        GPIOInitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIOInitStruct.GPIO_Pin   = GPIO_Pin_8;
	GPIOInitStruct.GPIO_Mode  = GPIO_Mode_OUT;// IN OUT AF AN
	GPIOInitStruct.GPIO_OType = GPIO_OType_OD;//开漏输出 --- IIC支持1对多 开漏能够有更强功率供给
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_100MHz;//最高速
	GPIOInitStruct.GPIO_PuPd  = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB,&GPIOInitStruct);
}

static void IIC_SDAModeSel(GPIOMode_TypeDef Mode)
{
	GPIO_InitTypeDef        GPIOInitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIOInitStruct.GPIO_Pin   = GPIO_Pin_9;
	GPIOInitStruct.GPIO_Mode  = Mode;// IN OUT AF AN
	GPIOInitStruct.GPIO_OType = GPIO_OType_OD;//开漏输出 --- IIC支持1对多 开漏能够有更强功率供给
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_100MHz;//最高速
	GPIOInitStruct.GPIO_PuPd  = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB,&GPIOInitStruct);

}


////////////////////////////////下面代码不用动/////////////////////////////////////////


//IIC启动信号
void IIC_Start(void)
{
	//SDA相对STM32设置为输出模式
	IIC_SDAModeSel(GPIO_Mode_OUT);
	
	//保持一段时间空闲
	SCK = 1;
	SDA = 1;
	delay_us(10);

	SDA  = 0;//在SCK高电平期间  拉低SDA  产生下降沿
	delay_us(5);

	SCK = 0;//拉低SCK  产生通信状态下的第一个脉冲
}

//IIC停止信号
void IIC_Stop(void)
{
	//先将SDA设置为输出
	IIC_SDAModeSel(GPIO_Mode_OUT);
	
	SDA = 0;
	SCK = 1;
	delay_us(5);
	
	SDA = 1;//SDA上升沿--停止信号
	
	//SDA==1  SCK==1  IIC总线回到空闲状态
}


//主机写给从机8bit数据---主机是发送者
void IIC_WriteByte(u8 WData)
{
	int i;
	//因为主发从 SDA应配置为输出模式
	IIC_SDAModeSel(GPIO_Mode_OUT);
	
	for(i=0;i<8;i++)
	{
		//SCK低电平期间通过判断WData每一个位来决定SDA是发送1还是0
		SCK = 0;
		delay_us(2);
		if(WData & (0x80>>i)) //先判断当前要发送的是WData第几位 并且是0还是1
			SDA = 1;
		else
			SDA = 0;
		delay_us(3);
		
		
		//SCK的高电平期间 是从机接收时间 发送者要维持SDA的稳定 不能修改
		SCK = 1;
		delay_us(5);
	}
	
	SCK = 0;

}


//主机读取从机的8bit数据---主机作为接收者  函数返回读到的8bit结果
//0x55  0101  0101
u8 IIC_ReadByte(void)
{
	int i;
	u8 RData = 0;//准备存放接收数据的容器
	//SDA变为输入模式
	IIC_SDAModeSel(GPIO_Mode_IN);
	
	for(i=0;i<8;i++)
	{
		//低电平期间给从机发送数据位的
		SCK = 0;
		delay_us(5);
		
		//高电平期间主机读取SDA获取数据位
		SCK = 1;
		delay_us(2);
		if(SDA_R)
			RData |= (0x80>>i);//数字1
		//数字0可以跳过不存入RData 因为RData默认就是0
		delay_us(3);
	}
	
	SCK = 0;//循环结束 拉低SCK保证从下一个脉冲的低电平开始
	return RData;//返回读到的结果
}

//主机作为发送者发完8bit数据  要在第9个节拍读取从机反馈回来的应答信号---这个函数必须在你调用了写8bit
//函数后调用
u8 IIC_ReadAck(void)
{
	u8 Ack;
	//将SDA变为输入模式---SDA引脚有可能会被修改
	IIC_SDAModeSel(GPIO_Mode_IN);
	
	//从机发应答信号时间
	SCK = 0;
	delay_us(5);
	
	//主机在SCK高电平期间读取SDA
	SCK = 1;
	delay_us(2);
	Ack = SDA_R;
	delay_us(3);
	
	SCK = 0;
	return Ack;//返回读到的应答信号结果
}

//主机给从机一个应答信号----主机作为接受者 从机作为发送者
//主机发给从机的应答可以你自己决定  传参ACk
void IIC_WriteAck(u8 Ack)
{
	//SDA设置为输出
	IIC_SDAModeSel(GPIO_Mode_OUT);
	
	//主机在SCK的低电平期间给从机发一个应答
	SCK = 0;
	delay_us(2);
	SDA = Ack;
	delay_us(3);
	
	//SCK高电平期间 给从机接收这个应答信号时间
	SCK = 1;
	delay_us(5);
	
	SCK = 0;
}

//IIC_WriteByte(0x12);
//if(IIC_ReadAck() == 1)
//{
//	printf("从机无应答");
//}

//
//int i;
//	u8 data = 0;
//	//SDA应设置为输入模式
//	
//	for(i=0;i<8;i++)
//	{
//		//SCK高电平期间通过判断SDA_R是读到到1还是0
//		SCK = 0;
//		delay_us(5);
//		
//		
//		
//		SCK = 1;
//		delay_us(2);
//		if(SDA_R == 1)
//			data |= 0x01; //0001 0101
//			//data |= (0x80>>i);
//		
//		data <<=1;//
//		delay_us(3);
//	}
//	
//	return data;
