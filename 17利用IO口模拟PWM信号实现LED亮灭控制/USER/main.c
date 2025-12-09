/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/10/24
  * @brief   这是一个使用IO口和延时函数去模拟PWM控制舵机转动
  ******************************************************************************
  * @attention
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
volatile int count = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  延时微秒函数
  * @param  nus:延时时间
  * @retval None
  */
void delay_us(u32 nus)
{
		SysTick->CTRL = 0; // Disable SysTick
		SysTick->LOAD = 168*nus - 1; //
		SysTick->VAL 	= 0; // Clear current value as well as count flag
		SysTick->CTRL = 5; // Enable SysTick timer with processor clock
		while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set
		SysTick->CTRL = 0; // Disable SysTick
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

void GPIOC_Config(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;										//输出模式
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;   							//无上下拉
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_6;				  							//引脚编号
	GPIO_InitStructure.GPIO_Speed 	= GPIO_High_Speed;    							//输出速度为100MHZ
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;      							//推挽输出

	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void) 
{     
	GPIOC_Config();
  /* Infinite loop */
	for(;;)
	{
		//打算生成脉冲信号的频率是50H2，所以时基是20ms
		GPIOC->BSRRL=GPIO_Pin_6;
		delay_ms(2);
		delay_us(500);	//2+0.5ms
		GPIOC->BSRRH =GPIO_Pin_6;
		delay_ms(17);
		delay_us(500);  //17.5ms

		delay_ms(1000);
		
		GPIOC->BSRRL=GPIO_Pin_6;
		delay_us(500);
		GPIOC->BSRRH =GPIO_Pin_6;
		delay_ms(19);
		delay_us(500);  //19.5ms

		delay_ms(1000);
	}
}

/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
