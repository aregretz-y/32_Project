#include "sg90.h"

/**
  * @brief  配置TIM3--PC6控制舵机,SG90初始化
  * @param  None
  * @retval None
  */
void SG90_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  //1.打开时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
  //2.配置引脚与定时器: TIM3 CH1 (PC6)*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	//3.选择GPIO引脚要复用的功能
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3); 
	
	//4.配置时基---频率
	TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;     // 分频：84MHz/84 = 1MHz
	TIM_TimeBaseStructure.TIM_Period = 20000 - 1;     // 周期：20ms (50Hz舵机标准)
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//5.配置定时器变量
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;							//选择PWM模式1，递增计数，当CNT < CCR时，通道有效
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //启动输出比较
  TIM_OCInitStructure.TIM_Pulse = 500;                          //CCR寄存器初值
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;			//输出的极性为高电平有效
	
	//6.初始化通道，注意定时器通道最多有4个，需要根据我们使用的定时器通道来选择
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	//7.使能相应预装载寄存器
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	//8.使能自动重载预装载寄存器
  TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	//9.启动定时器
	TIM_Cmd(TIM3, ENABLE);
}





