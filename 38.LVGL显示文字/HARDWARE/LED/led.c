#include "led.h"


void LED_GPIOInit(void)
{

	GPIO_InitTypeDef        GPIOInitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	GPIOInitStruct.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
	GPIOInitStruct.GPIO_Mode  = GPIO_Mode_OUT;//复用功能 IN OUT AF AN
	GPIOInitStruct.GPIO_OType = GPIO_OType_PP;//复用推挽输出 
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_100MHz;//最高速
	GPIOInitStruct.GPIO_PuPd  = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOF,&GPIOInitStruct);
	
	GPIOInitStruct.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOE,&GPIOInitStruct);
	
	//灯全灭 ResetBits  SetBits  WriteBit
	GPIO_WriteBit(GPIOF,GPIO_Pin_9  | GPIO_Pin_10,(BitAction)1);
	GPIO_WriteBit(GPIOE,GPIO_Pin_13 | GPIO_Pin_14,(BitAction)1);
}




