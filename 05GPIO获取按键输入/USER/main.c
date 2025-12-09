/**
  ******************************************************************************
  * @file    main.c 
  * @author  3130822084@qq.com
  * @version V0.0.0
  * @date    2025/10/17
  * @brief   这是一个用于控制蜂鸣器的案例
  ******************************************************************************
  * @attention
	
	//控制蜂鸣器BEEP---PF8,高电平触发
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

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

	void BEEP_Config(void)
	{
		//1.初始化变量
		GPIO_InitTypeDef GPIO_InitStructure;
		
		//2.打开时钟
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
		
		//3.变量赋值
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
		
		//4.初始化外设
		GPIO_Init(GPIOF, &GPIO_InitStructure);
	}
	
	void KEY_Config()
	{
		//1.初始化变量
		GPIO_InitTypeDef GPIO_InitStructure;
		
		//2.打开时钟
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		
		//3.变量赋值
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
		
		//4.初始化外设
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	
int main(void)
{
	BEEP_Config();
	KEY_Config();
	
	for(;;)
	{
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0)
		{
				GPIO_SetBits(GPIOF,GPIO_Pin_8);
		}
		else
		{
				GPIO_ResetBits(GPIOF,GPIO_Pin_8);
		}
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
