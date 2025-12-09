#include "key.h"

void KEY1_GPIOInit(void)
{
	//使能时钟GPIOF--AHB1
	RCC->AHB1ENR |= (0x01<<4);
	
	//PA0--输出 配置GPIOF组的4个配置寄存器MODER OTYPER OSPEEDR PUPDR
	GPIOE->MODER &= ~(0x03<<4);

}

void EXTI0_PA0Init(void)
{
	GPIO_InitTypeDef  GPIOInitStruct;
	EXTI_InitTypeDef  EXTIInitStruct;
	NVIC_InitTypeDef  NVICInitStruct;
	//使能时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);

	//配置指定引脚为浮空输入--PA0
	GPIOInitStruct.GPIO_Pin   = GPIO_Pin_0;
	GPIOInitStruct.GPIO_Mode  = GPIO_Mode_IN;//输入功能 IN OUT AF AN
	GPIOInitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;//浮空
	GPIO_Init(GPIOA,&GPIOInitStruct);
	
	//将指定引脚连接到指定的外部中断线--选择指定引脚组和引脚号
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	
	//配置EXTI 指定的中断线 0~22 
	EXTIInitStruct.EXTI_Line    = EXTI_Line0;
	EXTIInitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;//中断模式
	EXTIInitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发---按下触发
	EXTIInitStruct.EXTI_LineCmd = ENABLE;//中断使能
	EXTI_Init(&EXTIInitStruct);
	
	//配置中断
	NVICInitStruct.NVIC_IRQChannel 					 = EXTI0_IRQn;//6号 中断号
	NVICInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVICInitStruct.NVIC_IRQChannelSubPriority        = 0;
	NVICInitStruct.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVICInitStruct);
}


//中断服务函数
void EXTI0_IRQHandler(void)
{

	if(EXTI->PR & 0x01)
	{
		//粗延时
		delay_ms(60);
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0)
		{
			//GPIO_ToggleBits(GPIOF,GPIO_Pin_9);//翻转LED0
			PFout(9) = !PFout(9);
		}
		EXTI->PR = 0x01;//清除标志位
		
		//EXTI->PR = 1 1101 | 0x01;   1 1101
	}
}



//KEY1配置
void EXTI2_PE2Init(void)
{
	GPIO_InitTypeDef  GPIOInitStruct;
	EXTI_InitTypeDef  EXTIInitStruct;
	NVIC_InitTypeDef  NVICInitStruct;
	//使能时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);

	//配置指定引脚为浮空输入--PE2
	GPIOInitStruct.GPIO_Pin   = GPIO_Pin_2;
	GPIOInitStruct.GPIO_Mode  = GPIO_Mode_IN;//输入功能 IN OUT AF AN
	GPIOInitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;//浮空
	GPIO_Init(GPIOE,&GPIOInitStruct);
	
	//将指定引脚连接到指定的外部中断线--选择指定引脚组和引脚号
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
	
	//配置EXTI 指定的中断线 0~22 
	EXTIInitStruct.EXTI_Line    = EXTI_Line2;
	EXTIInitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;//中断模式
	EXTIInitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发---按下触发
	EXTIInitStruct.EXTI_LineCmd = ENABLE;//中断使能
	EXTI_Init(&EXTIInitStruct);
	
	//配置中断
	NVICInitStruct.NVIC_IRQChannel 					 = EXTI2_IRQn;//6号 中断号
	NVICInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVICInitStruct.NVIC_IRQChannelSubPriority        = 0;
	NVICInitStruct.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVICInitStruct);
}


//中断服务函数
void EXTI2_IRQHandler(void)
{
	//粗延时
	// 0100
	if(EXTI->PR & 0x04)
	{
		delay_ms(90);//延时消抖
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) == 0)
		{
			
			//GPIO_ToggleBits(GPIOF,GPIO_Pin_10);//翻转LED0	
			PFout(10) = !PFout(10);
		}
		
		EXTI->PR = 0x04;//清除标志位
	}
}




