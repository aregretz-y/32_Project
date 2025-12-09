#include "rtc.h"

#define SOURCE_LSE 0
#define SOURCE_LSI 1

#define RTC_CLK_SOURCE   SOURCE_LSE


void RTC_Config(void)
{
	RTC_InitTypeDef  RTC_InitStructure;
	RTC_TimeTypeDef  RTC_TimeStructure;
	RTC_DateTypeDef  RTC_DateStructure;
	
	//①使能PWR（power）模块时钟--PWR模块一些寄存器和RTC是相关
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	//②配置PWR_CR寄存器  关闭RTC寄存器写保护
	PWR_BackupAccessCmd(ENABLE);
	
	//开始配置RCC_BDCR使能RTC时钟   配置RTC寄存器
	//③开始配置RTC时钟
#if RTC_CLK_SOURCE==SOURCE_LSE
	RCC_LSEConfig(RCC_LSE_ON);//RCC_BDCR 第0位 LSEON位
	
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == 0)
	{}//等待LSE晶振就绪----RCC_BDCR 第1位 LSERDY位

	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//选择LSE作为RTC时钟源 RCC_BDCR 第8:9位
	
#elif RTC_CLK_SOURCE==SOURCE_LSI
	
#endif
		
	RCC_RTCCLKCmd(ENABLE);//使能RTC时钟   RCC_BDCR 第15号位   RTCEN位
	RTC_WaitForSynchro();//等待RTC内的日历和亚秒寄存器值同步到APB总线上的影子寄存器
	
	//④配置RTC数据寄存器和预分频器 UART
	RTC_InitStructure.RTC_AsynchPrediv = 127;//异步分频系数
	RTC_InitStructure.RTC_SynchPrediv = 255;//同步分频系数  32.726Khz/128/256 = 1Hz
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);
	
	//⑤配置时间日期
	/*设置日期: Friday January 11th 2013 */
	RTC_DateStructure.RTC_Year = 22;
	RTC_DateStructure.RTC_Month = 6;
	RTC_DateStructure.RTC_Date = 18;
	RTC_DateStructure.RTC_WeekDay = 6;
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);

	/* 设置时间：05h 20mn 00s AM */
	RTC_TimeStructure.RTC_H12     = RTC_H12_AM;//24时制  该值无意义
	RTC_TimeStructure.RTC_Hours   = 0x16;
	RTC_TimeStructure.RTC_Minutes = 0x18;
	RTC_TimeStructure.RTC_Seconds = 0x00; 
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
}

