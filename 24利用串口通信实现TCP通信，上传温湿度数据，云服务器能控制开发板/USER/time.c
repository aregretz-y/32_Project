#include "time.h"

__IO uint16_t count = 0;           //定时次数
uint8_t dhtbuf[5] = {0};

/**
  * @brief  配置定时器TIM6定时1s
  * @param  None
  * @retval None
  */
void TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  //1.打开时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	//2.初始化结构体，赋值并初始化
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;//设置分频值
  TIM_TimeBaseStructure.TIM_Period    = 10000-1;//设置计数次数
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //不二次分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//设置递增模式
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

  //3.配置NVIC，赋值并初始化
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	//4.选择中断源 更新事件：计数器上溢
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	
	//5.启动定时器
	TIM_Cmd(TIM6, ENABLE);
}

/**
  * @brief  定义定时器TIM6的ISR
  * @param  None
  * @retval None
  */
void TIM6_DAC_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
  {
		count++;
		if(count >= 50)
		{
			count = 0;
			//向服务器发送心跳包
			USART3_SendStr("ping\r\n");
		}
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	}
}

/**
  * @brief  配置定时器TIM3定时1s
  * @param  None
  * @retval None
  */
void TIM3_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  //1.打开时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//2.初始化结构体，赋值并初始化
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;//设置分频值
  TIM_TimeBaseStructure.TIM_Period    = 10000-1;//设置计数次数
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //不二次分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//设置递增模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  //3.配置NVIC，赋值并初始化
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	//4.选择中断源 更新事件：计数器上溢
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	//5.启动定时器
	TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  定义定时器TIM3的ISR
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
		DHT11_GetVal(dhtbuf);
		char sendbuf[256] = "\0";
		sprintf(sendbuf,"cmd=2&uid=a64a8c06bd5f4df8ba03c205a36cca7e&topic=DHT11&msg=stemp = %d humi = %d %%RH\r\n",dhtbuf[2],dhtbuf[0]);
		USART3_SendStr(sendbuf);
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}


