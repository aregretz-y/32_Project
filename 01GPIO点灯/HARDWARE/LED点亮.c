/**
  ******************************************************************************
  * @file    main.c 
  * @author  3130822084@qq.com
  * @version V0.0.0
  * @date    2025/10/15
  * @brief   这是一个基于STM32GPIO的点灯程序
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

void Delay(__IO uint32_t nCount)
{
    for(; nCount != 0; nCount--);
}

	void LED_Config(void)
	{
		//1.初始化变量
		
		//2.打开时钟
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		
		//3.变量赋值
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
		
		//4.初始化外设
		GPIO_Init(GPIOF, &GPIO_InitStructure);
		
		//3.变量赋值
		GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_13 | GPIO_Pin_14;
		//4.初始化外设
		GPIO_Init(GPIOE, &GPIO_InitStructure);
		
	}
	
int main(void)
{
	LED_Config();
  for(;;)
  {
		//复位操作
		GPIOF->BSRRH = GPIO_Pin_9 | GPIO_Pin_10;
		GPIOE->BSRRH = GPIO_Pin_13 | GPIO_Pin_14;
		
		Delay(1000000);  // 延时一段时间
		
		//置位操作
		GPIOF->BSRRL = GPIO_Pin_9 | GPIO_Pin_10;
		GPIOE->BSRRL = GPIO_Pin_13 | GPIO_Pin_14;
		
		Delay(1000000);  //延时一段时间
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
