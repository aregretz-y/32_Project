/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/10/21
  * @brief   这是使用定时器实现周期性中断，LED翻转前后台结构（裸机开发）
  ******************************************************************************
  * @attention
  * 			1.LED---PF9对应定时器TIM9---60s---APB2---168MHZ
	*				2.LED---PE14对应定时器TIM2---1s---APB1---84MHZ
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  //用户必须包含

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

volatile uint8_t count = 0;

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
  * @brief  Configures EXTI Line0 (connected to PA0 pin) in interrupt mode
  * @param  None
  * @retval None
  */
void LED_Config(void)
{
  GPIO_InitTypeDef   GPIO_InitStructure;

  //1.打开GPIOE、F端口时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//2.配置负载2个LED的参数
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;										//输出模式
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;   							//无上下拉
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;				  							//引脚编号
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;    							//输出速度为100MHZ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      							//推挽输出
  GPIO_Init(GPIOF, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_14;				  						//引脚编号
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}


/**
  * @brief  Configures EXTI Line0 (connected to PA0 pin) in interrupt mode
  * @param  None
  * @retval None
  */
void TIM2_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  //1.打开时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	//2.初始化结构体，赋值并初始化
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;//设置分频值
  TIM_TimeBaseStructure.TIM_Period    = 10000-1;//设置计数次数
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //不二次分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//设置递增模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  //3.配置NVIC，赋值并初始化
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	//4.选择中断源 更新事件：计数器上溢
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	//5.启动定时器
	TIM_Cmd(TIM2, ENABLE);
}

void TIM9_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  //1.打开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
	
	//2.初始化结构体，赋值并初始化
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;//设置分频值
  TIM_TimeBaseStructure.TIM_Period    = 20000-1;//设置计数次数
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //不二次分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//设置递增模式
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

  //3.配置NVIC，赋值并初始化
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	//4.选择中断源 更新事件：计数器上溢
	TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);
	
	//5.启动定时器
	TIM_Cmd(TIM9, ENABLE);
}


/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
		
		GPIO_ToggleBits(GPIOE, GPIO_Pin_14);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}


/**
  * @brief  This function handles TIM9 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM1_BRK_TIM9_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET)
  {
		count++;
    TIM_ClearITPendingBit(TIM9, TIM_IT_Update);
		if(count == 5)
		{
				GPIO_ToggleBits(GPIOF, GPIO_Pin_9);
				count = 0;
		}
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//中断优先级分组，采用分组4则表示抢占优先级的范围是0~15，子优先级的值固定为0
	TIM2_Config();
	TIM9_Config();
	LED_Config();
	
  /* Infinite loop */
  for(;;)
  {
		
  }
}

/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
