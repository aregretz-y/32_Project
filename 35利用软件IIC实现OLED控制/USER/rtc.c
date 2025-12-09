#include "rtc.h"

RTC_TimeTypeDef  	RTC_TimeStructure;
__IO uint32_t uwTimeDisplay;

uint8_t hour = 0;
uint8_t min = 0;
uint8_t sec = 0;

extern __IO uint8_t  recvflag;						 			//接收完成的标志
extern __IO uint8_t  initflag;						 	//获取时间完成的标志
extern __IO uint8_t  u3_recvbuf[BUFSIZE]; //UART3的接收缓冲区
extern __IO uint16_t u3_recvcnt;      //UART3的接收计数器

/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
void RTC_Config(void)
{
	RTC_InitTypeDef  	RTC_InitStructure;
  RTC_DateTypeDef 	RTC_DateStructure;
	
  NVIC_InitTypeDef  NVIC_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;
	
  //1.打开PWR外设的时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  //2.使能RTC备份域访问
  PWR_BackupAccessCmd(ENABLE);
    
	//3.开启LSE时钟
  RCC_LSEConfig(RCC_LSE_ON);

  //4.等待LSE就绪 
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

  //5.选择LSE作为RTC的时钟
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
  //6.使能RTC时钟
  RCC_RTCCLKCmd(ENABLE);

  //7.解除所有RTC寄存器的写保护
  RTC_WaitForSynchro();
  
  //8.配置RTC的预分频值和小时格式
  RTC_InitStructure.RTC_AsynchPrediv 	= 128-1;							//异步分频
  RTC_InitStructure.RTC_SynchPrediv 	= 256-1;							//同步分频
  RTC_InitStructure.RTC_HourFormat 		= RTC_HourFormat_24;  //24小时制
  RTC_Init(&RTC_InitStructure);
  
  //9.设置RTC日期
  RTC_DateStructure.RTC_Year 		= 0x25;								//BCD格式，年份
  RTC_DateStructure.RTC_Month 	= 0x11;								//BCD格式，月份
  RTC_DateStructure.RTC_Date 		= 0x06;								//BCD格式，日期
  RTC_DateStructure.RTC_WeekDay = 0x04;								//BCD格式，星期
  RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
  
  //10.设置RTC时间
	//  RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
	//  RTC_TimeStructure.RTC_Hours   = ihour/10*16+ihour%10;
	//  RTC_TimeStructure.RTC_Minutes = imin/10*16+imin%10;
	//  RTC_TimeStructure.RTC_Seconds = isec/10*16+isec%10; 
	//  RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
  
	//11.配置RTC唤醒中断
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	//12.配置RTC唤醒中断的EXTI中断线
  EXTI_ClearITPendingBit(EXTI_Line22);
	
  EXTI_InitStructure.EXTI_Line = EXTI_Line22;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	//13.关闭RTC唤醒定时器
	RTC_WakeUpCmd(DISABLE);
	
	//14.选择RTC唤醒时钟源 1HZ
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits); 
	
	//15.设置RTC唤醒重载值 1s
	RTC_SetWakeUpCounter(0);
	
	//16.选择RTC的中断源 唤醒中断
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	
	//17.清除RTC的唤醒中断标志
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//18.打开RTC唤醒定时器
  RTC_WakeUpCmd(ENABLE); 
}


/**
* @brief  处理时间日期时间
* @param  NULL  
* @retval true or false
* @note   
*/
void Time_Handle(void)
{
	char * p;
	p = strtok((char *)u3_recvbuf,":");
	if(p != NULL)
	{
		hour = atoi(p);
		printf("hour = %d\r\n",hour);
		RTC_TimeStructure.RTC_Hours   = hour/10*16+hour%10;
		p = strtok(NULL,":");
		if(p != NULL)
		{
			min = atoi(p);
			printf("min = %d\r\n",min);
			RTC_TimeStructure.RTC_Minutes   = min/10*16+min%10;
			p = strtok(NULL,":");
			if(p != NULL)
			{
				sec = atoi(p);
				printf("sec = %d\r\n",sec);
				RTC_TimeStructure.RTC_Seconds   = sec/10*16+sec%10;
			}
		}
	}
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure); 
}

/**
  * @brief  Display the current time.
  * @param  None
  * @retval None
  */
void RTC_TimeShow(void)
{
  /* Get the current Time */
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  /* Display time Format : hh:mm:ss */
  printf("%0.2d:%0.2d:%0.2d\r\n",RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
}


