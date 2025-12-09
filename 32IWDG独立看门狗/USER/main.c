
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
	*				
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
  * @brief  看门狗的配置
  * @param  None
  * @retval None
  */
void IWDG_Config(void)
{
	
	//1.解除写保护
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	//2.配置分频值
	IWDG_SetPrescaler(IWDG_Prescaler_32);

	//3.配置重载值0-0x0FFF
	IWDG_SetReload(0x0FFF);
	
	//4.启动看门狗
	IWDG_Enable();
}


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
	IWDG_Config();
	uint16_t light_val = 0;
  /* Infinite loop */
  for(;;)
  {
		if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
		{
			printf("看门狗复位了\r\n");
			RCC_ClearFlag();
		}
		else
		{
			printf("用户复位了\r\n");
		
		}
		//开启ADC1软件转换
		ADC_SoftwareStartConv(ADC3);
		while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC) == RESET);
		light_val = ADC_GetConversionValue(ADC3);
		printf("light_val = %d\r\n",light_val);
		delay_ms(500);
		//喂狗操作
//		IWDG_ReloadCounter();
  }
}











/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
