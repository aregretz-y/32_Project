
/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/11/05
  * @brief   这是利用ADC实现光敏电阻数值的输出
  ******************************************************************************
  * @attention
  *				
	*				1.光敏电阻对应的GPIO引脚为PF7
	*				2.查阅数据手册得知PF7对应的规则通道是ADC3_IN5

  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  //用户必须包含

#include "delay.h"
#include "uart.h"
#include "pot.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
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
	LDR_Config();
	uint16_t light_val = 0;
  /* Infinite loop */
  for(;;)
  {
		//开启ADC1软件转换
		ADC_SoftwareStartConv(ADC3);
		while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC) == RESET);
		light_val = ADC_GetConversionValue(ADC3);
		printf("light_val = %d\r\n",light_val);
		delay_ms(500);
  }
}







/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
