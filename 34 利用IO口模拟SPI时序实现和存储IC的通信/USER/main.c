/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/11/7
  * @brief   这是利用IO口模拟SPI时序，实现和存储IC的通信
  ******************************************************************************
  * @attention
  *					1.目前SPI采用的四线制：MOSI MISO SCK CS
	*    			2.引脚描述：
	*											MOSI--PB5 (输出模式)  
	*											MISO--PB4 (输入模式)
	*											SCK --PB3 (输出模式)
	*											CS  --PB14(输出模式)
	*					3.W25Q128存储IC支持模式0和模式3，采用模式0（SCK空闲低电平，第1个边沿）
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  //用户必须包含
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "delay.h"
#include "uart.h"
#include "rtc.h"
#include "dht11.h"
#include "w25q128.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/



/* Private macro -------------------------------------------------------------*/
__IO uint8_t count = 0;
extern __IO uint32_t uwTimeDisplay;
__IO uint8_t  timeflag = 0;

/**
  * @brief  配置TIM3定时2ms
  * @param  None
  * @retval None
  */
void TIM3_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

  //1.打开时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//2.配置时基---1s
	TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1;     // 分频
	TIM_TimeBaseStructure.TIM_Period = 10000 - 1;     // 周期：2ms
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//3.配置NVIC
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;	//抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE); 

	//3.启动定时器
	TIM_Cmd(TIM3, ENABLE);
}


/**
  * @brief  TIM3的ISR
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)   //TIM3 每 1s 中断一次
{   
  
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		count++;
		uwTimeDisplay = 1;
        if(count>5)           //每5s 获取一次温湿度
        {
						timeflag = 1;
						
            count = 0;
        }
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
		}   
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{   
		//1.硬件初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//中断优先级分组，采用分组4则表示抢占优先级的范围是0~15，子优先级的值固定为0
  USART1_Config(115200); 	//PC
	W25Q128_Config();
	DHT11_Config();					//DHT11
	RTC_Config();

	//2.读取ID
	printf("W25Q128 ID = %#X\r\n",W25Q128_ReadID()); //0xEF17
	
  for(;;)
  {
		if(timeflag == 1)
		{
			timeflag = 0;
			DHT11_GetVal(dhtbuf);
			sprintf(dhtstr,"tmep = %d℃&humi = %d%%RH",dhtbuf[2],dhtbuf[0]);
			printf("%s\r\n",dhtstr);
		}
		if(uwTimeDisplay)
		{
			uwTimeDisplay = 0;
			RTC_TimeShow();
		}
	}

}






/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
