/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/10/21
  * @brief   这是使用NVIC结合EXTI实现按键的检测，采用的架构是前后台架构（裸机开发）
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
  * @brief  Configures EXTI Line0 (connected to PA0 pin) in interrupt mode
  * @param  None
  * @retval None
  */
void EXTILine_Config(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  //1.打开GPIOA、E、F端口时钟+SYSCFG外设时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  //2.配置PA0、PE2、PE3、PE4为输入模式并且初始化
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;			//输入模式
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_0;				//引脚编号
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;				//引脚编号
	GPIO_Init(GPIOE, &GPIO_InitStructure);

  //3.把PA0映射到EXTI0 把PE2映射到EXTI2 把PE3映射到EXTI3 把PE4映射到EXTI4
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);
	
  //4.配置EXTI0 2 3 4的中断线参数并初始化
  EXTI_InitStructure.EXTI_Line 		= EXTI_Line0;						
  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //下降沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line 		= EXTI_Line2;		
  EXTI_Init(&EXTI_InitStructure);
	
  EXTI_InitStructure.EXTI_Line 		= EXTI_Line3;				
  EXTI_Init(&EXTI_InitStructure);
	
  EXTI_InitStructure.EXTI_Line 		= EXTI_Line4;		
  EXTI_Init(&EXTI_InitStructure);
	
  //5.配置NVIC参数并初始化,打开中断通道0 2 3 4
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;	//抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	//6.配置负载4个LED的参数
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;										//输出模式
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;   							//无上下拉
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9 | GPIO_Pin_10;				  //引脚编号
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;    							//输出速度为100MHZ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      							//推挽输出
  GPIO_Init(GPIOF, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_13 | GPIO_Pin_14;				  //引脚编号
  GPIO_Init(GPIOE, &GPIO_InitStructure);

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
		if( GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0)
		{
			/* Toggle LED */
			GPIO_ToggleBits(GPIOF, GPIO_Pin_9);
		}
	 
    //清除中断标志，提高可靠性
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

void EXTI2_IRQHandler(void)
{
	//获取中断线的中断状态
  if(EXTI_GetITStatus(EXTI_Line2) != RESET)
  {
		//消抖处理，提高可靠性
		delay_ms(5);
		
		//读取PE2管脚电平
		if( GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) == 0)
		{
			/* Toggle LED */
			GPIO_ToggleBits(GPIOF, GPIO_Pin_10);
		}
	 
    //清除中断标志，提高可靠性
    EXTI_ClearITPendingBit(EXTI_Line2);
  }
}

void EXTI3_IRQHandler(void)
{
	//获取中断线的中断状态
  if(EXTI_GetITStatus(EXTI_Line3) != RESET)
  {
		//消抖处理，提高可靠性
		delay_ms(5);
		
		//读取PE3管脚电平
		if( GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3) == 0)
		{
			/* Toggle LED */
			GPIO_ToggleBits(GPIOE, GPIO_Pin_13);
		}
	 
    //清除中断标志，提高可靠性
    EXTI_ClearITPendingBit(EXTI_Line3);
  }
}

void EXTI4_IRQHandler(void)
{
	//获取中断线的中断状态
  if(EXTI_GetITStatus(EXTI_Line4) != RESET)
  {
		//消抖处理，提高可靠性
		delay_ms(5);
		
		//读取PE4管脚电平
		if( GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) == 0)
		{
			/* Toggle LED */
			GPIO_ToggleBits(GPIOE, GPIO_Pin_14);
		}
	 
    //清除中断标志，提高可靠性
    EXTI_ClearITPendingBit(EXTI_Line4);
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
	EXTILine_Config();
	
  /* Infinite loop */
  for(;;)
  {
		
  }
}

/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
