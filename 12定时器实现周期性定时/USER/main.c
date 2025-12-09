/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/10/21
  * @brief   这是使用定时器实现周期性中断，前后台结构（裸机开发）
  ******************************************************************************
  * @attention
  *				1.NVIC外设相关的驱动文件是misc.c和misc.h
	*				2.例如打算使用KEY0，对应的引脚是PA0，需要映射到EXTI0中断线
	*				3.KEY1----PE2----EXTI2,KEY2----PE3----EXTI3,KEY3---PE4----EXTI4
	*				4.LED0---PF9,LED1---PF10,FSMC_D10---PE13,FSMC_D11---PE14
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
  * @brief  配置GPIO--PF9
  * @param  None
  * @retval None
  */
void LED_Config(void)
{
  GPIO_InitTypeDef   GPIO_InitStructure;

  //1.打开GPIOE、F端口时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//6.配置负载4个LED的参数
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;										//输出模式
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;   							//无上下拉
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;				  							//引脚编号
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;    							//输出速度为100MHZ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      							//推挽输出
	
  GPIO_Init(GPIOF, &GPIO_InitStructure);
}

/**
  * @brief  配置定时器TIM6
  * @param  None
  * @retval None
  */
void TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  //1.打开时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	//2.初始化结构体，赋值并初始化
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;//设置分频值
  TIM_TimeBaseStructure.TIM_Period    = 5000-1;//设置计数次数
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
  * @brief  定义定时器TIM6的ISR
  * @param  None
  * @retval None
  */
void TIM6_DAC_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
  {

		GPIO_ToggleBits(GPIOF,GPIO_Pin_9);
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//中断优先级分组，采用分组4则表示抢占优先级的范围是0~15，子优先级的值固定为0
	TIM_Config();
	LED_Config();
  /* Infinite loop */
  for(;;)
  {
		
  }
}

/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
