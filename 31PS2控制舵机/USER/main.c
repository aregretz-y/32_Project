
/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/11/04
  * @brief   这是利用ADC模数转换器把模拟信号转换成数字信号
  ******************************************************************************
  * @attention
  *				
	*				1.ADC模数转换器对应的GPIO引脚为PA5
	*				2.查阅数据手册得知PA5对应的规则通道是ADC1_IN5 or ADC2_IN5
	*				3.关于ADC的转换有3种方案:
	*											1.轮询式 :不好，降低实时性
	*											2.中断触发:不好，频繁中断会影响程序运行
	*											3.DMA:很好，高速传输，又降低CPU负担
	*				4.遥感的x轴对应引脚是PA4,对应的通道ADC1_IN4
	*								y轴对应引脚是PA6,对应的通道ADC2_IN6
	*								z轴对应引脚是PE6,使用GPIO输入模式，检测电平
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  //用户必须包含

#include "delay.h"
#include "uart.h"
#include "pot.h"
#include "sg90.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PDEADBAND  50 
#define NDEADBAND -50 
#define MIDBAND  1690 
/* Private macro -------------------------------------------------------------*/




/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{   

	//1.硬件初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  USART1_Config(115200); //PC
	PS2_Config();
	SG90_Config();
	uint16_t x_val = 0;
	uint16_t y_val = 0;
  /* Infinite loop */
  for(;;)
  {
		//开启ADC1软件转换
		ADC_SoftwareStartConv(ADC1);
		ADC_SoftwareStartConv(ADC2);
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
		while(ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET);
		x_val = ADC_GetConversionValue(ADC1);
		y_val = ADC_GetConversionValue(ADC2);
		printf("x_val = %d y_val = %d\r\n",x_val*3300/4095,y_val*3300/4095);
		delay_ms(500);
		if((x_val > MIDBAND + PDEADBAND || x_val > MIDBAND + NDEADBAND) || (y_val > MIDBAND + PDEADBAND || y_val > MIDBAND + NDEADBAND))
		{
			if(y_val >= MIDBAND + PDEADBAND)
			{
				TIM_SetCompare1(TIM3,2500);
			}
			else if(y_val <= MIDBAND + PDEADBAND)
			{
				TIM_SetCompare1(TIM3,500);
			}
		}
  }
}







/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
