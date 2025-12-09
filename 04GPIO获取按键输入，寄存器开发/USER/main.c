/**
  ******************************************************************************
  * @file    main.c 
  * @author  3130822084@qq.com
  * @version V0.0.0
  * @date    2025/10/17
  * @brief   这是一个使用GPIO获取按键输入的程序，使用寄存器开发
  ******************************************************************************
  * @attention
	
	//KEY0的引脚分布：KEY0---PA0
	//KEY0的电平逻辑：KEY0按下时，引脚获得低电平
	
	//蜂鸣器的引脚逻辑：BEEP---PF8
	//蜂鸣器的电平逻辑：获得高电平时响
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define RCC_AHB1ENR   	(*(volatile unsigned int *)(0x40023800+0x30))//AHB1外设时钟使能寄存器
	
#define GPIOA_MODER  		(*(volatile unsigned int *)(0x40020000+0x00))//GPIO 端口模式寄存器
#define GPIOA_PUPDR  		(*(volatile unsigned int *)(0x40020000+0x0C))//GPIO 端口上拉/下拉寄存器
#define GPIOA_IDR  	 		(*(volatile unsigned int *)(0x40020000+0x10))//GPIO 端口输入数据寄存器
	
#define GPIOF_MODER  		(*(volatile unsigned int *)(0x40021400+0x00))//GPIO 端口模式寄存器
#define GPIOF_OTYPER  	(*(volatile unsigned int *)(0x40021400+0x04))//GPIO 端口输出类型寄存器
#define GPIOF_OSPEEDR  	(*(volatile unsigned int *)(0x40021400+0x08))//GPIO 端口输出速度寄存器
#define GPIOF_PUPDR  		(*(volatile unsigned int *)(0x40021400+0x0C))//GPIO 端口上拉/下拉寄存器
#define GPIOF_ODR  			(*(volatile unsigned int *)(0x40021400+0x14))//GPIO 端口输出数据寄存器

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
		//1.打开时钟
		RCC_AHB1ENR |= (1<<5);
		
		//2.设置模式（推挽输出模式，无上下拉，100MHZ）
		GPIOF_MODER &= ~(1<<17);
		GPIOF_MODER |=  (1<<16);
		
		GPIOF_OTYPER &= ~(1<<8);
		
		GPIOF_OSPEEDR |=  (1<<17);
		GPIOF_OSPEEDR |=  (1<<16);
		
		GPIOF_PUPDR &= ~(1<<17);
    GPIOF_PUPDR &= ~(1<<16);
	}
	
	void KEY_Config(void)
	{
		//1.打开外设时钟
		RCC_AHB1ENR |= (1<<0);
		//设置输入模式,00
		GPIOA_MODER &= ~(1<<1);
		GPIOA_MODER &= ~(1<<0);
		//设置无上下拉
		GPIOA_PUPDR &= ~(1<<1);
		GPIOA_PUPDR &= ~(1<<0);
	}
	
int main(void)
{
	BEEP_Config();
	KEY_Config();
	for(;;)
	{
		//置位操作
		if(GPIOA_IDR & (1<<0))//32位取第0位
		{
			GPIOF_ODR &= ~(1<<8);//给蜂鸣器输出低电平
		}
		else
		{
			GPIOF_ODR |= (1<<8);//给蜂鸣器输出高电平
		}
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
