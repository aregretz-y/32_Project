/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/10/24
  * @brief   这是一个使用KEY控制舵机运行，实现懒人开关的作用，按下KEY0，实现舵机180度，LED0亮，然后松开KEY0，实现舵机0度，LED0灭
  ******************************************************************************
  * @attention
	*				1.使用TIM3定时器控制舵机
	*				2.通道使用CH1，对应的引脚为PC6,舵机信号线连接到PC6
	*				3.使用TIM6定时器实现定时切换舵机角度
	*				4.舵机型号为SG90,支持180度转动，3-7.2V驱动，采用5V
	*				5.舵机需要时基信号20ms---50HZ
	*				6.高电平0.5ms-2.5ms才能去控制角度
	*				7.KEY0---PA0;
	*				8.LED0---PF9
	*				
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  //用户必须包含

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint32_t count = 0;
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
  * @brief  Configures EXTI Line0 (connected to PA0 pin) in interrupt mode
  * @param  None
  * @retval None
  */
void EXTILine0_Config(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  //1.打开GPIOA端口时钟+SYSCFG外设时钟+GPIO端口F时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  //2.配置PA0为输入模式并且初始化
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;			//输入模式
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_0;				//引脚编号
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //3.把PA0映射到EXTI0
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

  //4.配置EXTI0中断线参数并初始化
  EXTI_InitStructure.EXTI_Line 		= EXTI_Line0;						
  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //下降沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  //5.配置NVIC参数并初始化
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;	//抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
	//获取中断线的中断状态
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
		//消抖处理，提高可靠性
		delay_ms(5);
		
		//读取PA0管脚电平
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0)
		{
			/* Toggle LED */
			count = 1 - count;
		}

    //清除中断标志，提高可靠性
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}


/**
  * @brief  配置GPIO的参数
  * @param  None
  * @retval None
  */
void LED_Config(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

	//1.打开时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//2.配置参数,LED的，低电平触发,并初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOF, &GPIO_InitStructure); 
}

void KEY_Config(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

	//1.打开时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//2.配置参数，KEY0的，输入模式，并初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
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
	LED_Config();
	KEY_Config();
	EXTILine0_Config(); 
  /* Infinite loop */
  for(;;)
  {
		if(count == 1)
		{
			GPIO_ResetBits(GPIOF, GPIO_Pin_9);
			TIM_SetCompare1(TIM3,2500);
		}
		else
		{
			GPIO_SetBits(GPIOF, GPIO_Pin_9);
			TIM_SetCompare1(TIM3,500);
		}
  }
}

/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
