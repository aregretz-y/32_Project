/**
  ******************************************************************************
  * @file    main.c 
  * @author  3130822084@qq.com
  * @version V0.0.0
  * @date    2025/10/18
  * @brief   这是一个用于对PLL锁相环的分频系数M,N,P进行修改，以及对HSE_VALUE
							进行修改的案例，目的是确保系统时钟频率是168MHZ
  ******************************************************************************
  * @attention
	*          1.需要修改system_stm32
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

void Delay(void)//延时10ms
{
    unsigned int cnt = 134403;
		while(cnt--);
}

	void Camera_Config(void)
	{
		//1.初始化变量
		GPIO_InitTypeDef GPIO_InitStructure;
		
		//2.打开时钟
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		
		//3.变量赋值
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
		
		//4.初始化外设
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	
int main(void)
{
	Camera_Config();
	for(;;)
	{
		//置位操作
		GPIOC->BSRRL = GPIO_Pin_6;
		
		Delay();  // 延时一段时间
			
		//复位操作
		GPIOC->BSRRH = GPIO_Pin_6;
		
		Delay();  // 延时一段时间
	}

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
