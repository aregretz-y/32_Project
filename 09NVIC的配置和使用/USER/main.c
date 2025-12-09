/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/10/21
  * @brief   用于掌握内核中的NVIC外设的参数配置
  ******************************************************************************
  * @attention
	*				1.NVIC外设相关的驱动文件是misc.c和misc.h
	*				
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
  * @brief  GPIO的初始化
  * @param  None
  * @retval None
  */
void IOPin_Config(void)
{
	//1.定义变量
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//2.打开时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	//3.变量赋值
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_6;					//引脚编号
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;			//输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
	
	//4.初始化外设
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{     
	//中断优先级分组，采用分组4则表示抢占优先级的范围是0~15，子优先级的值固定为0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
  /* Infinite loop */
  for(;;)
  {
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
