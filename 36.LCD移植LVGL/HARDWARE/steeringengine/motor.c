#include "motor.h"

void Motor_GPIOInit(void)
{
	
	GPIO_InitTypeDef        GPIOInitStruct;
	TIM_TimeBaseInitTypeDef TIMBaseInitStruct;
	TIM_OCInitTypeDef       OCInitStruct;
	
	
	//使能时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	//配置GPIOF9为复用功能
	GPIOInitStruct.GPIO_Pin   = GPIO_Pin_10;
	GPIOInitStruct.GPIO_Mode  = GPIO_Mode_AF;//复用功能 IN OUT AF AN
	GPIOInitStruct.GPIO_OType = GPIO_OType_PP;//复用推挽输出 
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_100MHz;//最高速
	GPIOInitStruct.GPIO_PuPd  = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB,&GPIOInitStruct);
	
	//指定PF9为指定的功能---TIM14
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_TIM2);
	
	//TIM14 时基单元配置
	TIMBaseInitStruct.TIM_Prescaler     = 84-1;//84Mhz/84 = 1Mhz  1us/脉冲
	TIMBaseInitStruct.TIM_Period        = 20000-1;//20ms---周期时间较小
	TIMBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//不分频
	TIMBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;//递增计数模式
	TIM_TimeBaseInit(TIM2,&TIMBaseInitStruct);
	
	//TIM14 CH1通道1配置
	OCInitStruct.TIM_OCMode      = TIM_OCMode_PWM1;//CNT<CCR 输出有效 
	OCInitStruct.TIM_OCPolarity  = TIM_OCPolarity_High;//高电平有效
	OCInitStruct.TIM_Pulse       = 500;//初值约为50%占空比
	OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//PWM输出通道使能---使能关闭
	TIM_OC3Init(TIM2,&OCInitStruct);
	
	//CCR预装载使能--也可以不使能
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	//开启定时器
	TIM_Cmd(TIM2,ENABLE);
	
	
}


