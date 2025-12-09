/**
  ******************************************************************************
  * @file    main.c 
  * @author  3130822084@qq.com
  * @version V0.0.0
  * @date    2025/10/17
  * @brief   这是一个基于STM32GPIO的点灯程序,使用寄存器开发
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

	#define RCC_AHB1ENR 		(*(volatile unsigned int *)(0x40023800+0x30)) //AHB1外设时钟使能寄存器
	#define GPIOF_MODER   	(*(volatile unsigned int *)(0x40021400+0x00)) //GPIO输出模式寄存器
	#define GPIOF_OTYPER  	(*(volatile unsigned int *)(0x40021400+0x04)) //GPIO 端口输出类型寄存器
	#define GPIOF_OSPEEDR   (*(volatile unsigned int *)(0x40021400+0x08)) //GPIO 端口输出速度寄存器
	#define GPIOF_PUPDR   	(*(volatile unsigned int *)(0x40021400+0x0C)) //GPIO端口上拉/下拉寄存器
	#define GPIOF_ODR   		(*(volatile unsigned int *)(0x40021400+0x14)) //GPIO 端口输出数据寄存器
		
	
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

	void LED_Config(void)
	{
			//1.打开外设时钟
			RCC_AHB1ENR |=  (1<<5);
			//2.设置GPIO模式（模式为输出、推挽、无上下拉、100MHZ）
		
			//2.1设置通用输出模式，01
			GPIOF_MODER &= ~(1<<19); 
			GPIOF_MODER |=  (1<<18);
		
			//2.2设置无上下拉，00
			GPIOF_PUPDR &= ~(1<<19);  
			GPIOF_PUPDR &= ~(1<<19); 
		
			//2.3设置100MHZ输出速度，11
			GPIOF_OSPEEDR |=  (1<<18);
			GPIOF_OSPEEDR |=  (1<<18);
		
			//2.4设置输出模式为推挽，0
			GPIOF_OTYPER &= ~(1<<9);
	}
	
int main(void)
{
	LED_Config();
  for(;;)
  {
			//设置输出数据寄存器为低电平
			GPIOF_ODR &= ~(1<<9);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
