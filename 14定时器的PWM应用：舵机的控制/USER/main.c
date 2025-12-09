/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/10/24
  * @brief   这是使用定时器实现对舵机的控制
  ******************************************************************************
  * @attention
	*				1.使用TIM3定时器控制舵机
	*				2.通道使用CH1，对应的引脚为PC6,舵机信号线连接到PC6
	*				3.使用TIM6定时器实现定时切换舵机角度
	*				4.舵机型号为SG90,支持180度转动，3-7.2V驱动，采用5V
	*				5.舵机需要时基信号20ms---50HZ
	*				6.高电平0.5ms-2.5ms才能去控制角度
	*				
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  //用户必须包含

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
volatile uint32_t a = 500;
volatile int b = 1;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  延时微秒函数
  * @param  nus:延时时间
  * @retval None
  */
void delay_us(u32 nus)
{
	while(nus--)
	{
		SysTick->CTRL = 0; // Disable SysTick
		SysTick->LOAD = 168 - 1; //
		SysTick->VAL 	= 0; // Clear current value as well as count flag
		SysTick->CTRL = 5; // Enable SysTick timer with processor clock
		while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set
		SysTick->CTRL = 0; // Disable SysTick
	}	
}

/**
  * @brief  延时毫秒函数
  * @param  nms:延时时间
  * @retval None
  */
void delay_ms(u32 nms)
{
	while(nms--)
	{
		delay_us(1000);
		
	}	
}

/**
  * @brief  配置TIM6定时2秒
  * @param  None
  * @retval None
  */
void TIM6_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  //1.打开时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	//2.初始化结构体，赋值并初始化
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;//设置分频值
  TIM_TimeBaseStructure.TIM_Period    = 10000-1;//设置计数次数
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //不二次分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//设置递增模式
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

  //3.配置NVIC，赋值并初始化
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	//4.选择中断源 更新事件：计数器上溢
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	
	//5.启动定时器
	TIM_Cmd(TIM6, ENABLE);
}

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

/**
  * @brief  配置TIM6的ISR
  * @param  None
  * @retval None
  */
void TIM6_DAC_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
  {
		if(b)
		{
			a += 2000;
			b -= 1;
		}
		else
		{
			a -= 2000;
			b += 1;
		}
		TIM_SetCompare1(TIM3,a);
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	}
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{     

	//硬件初始化
	SG90_Config();
	TIM6_Config();
  /* Infinite loop */
  for(;;)
  {
		
  }
}

/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
