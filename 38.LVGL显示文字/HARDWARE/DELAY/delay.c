#include "delay.h"

#if (USER_OS == 0) //说明不用OS

/**
  * @brief  微秒延时
  * @param  None
  * @retval None
  */
void delay_us(u32 nus)
{
	SysTick->CTRL = 0; 		// Disable SysTick
	SysTick->LOAD = nus * 21 - 1; // 计数次数 = nus * 21 - 1
	SysTick->VAL 	= 0; 		// Clear current value as well as count flag
	SysTick->CTRL = 1; 		// Enable SysTick timer with 21MHZ clock
	while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set
	SysTick->CTRL = 0; 		// Disable SysTick
}

/**
  * @brief  毫秒延时
  * @param  None
  * @retval None
  */
void delay_ms(u32 nms)
{
	while(nms--)
	{
		delay_us(1000);
	}
}

#else //说明使用OS

/**
  * @brief  非阻塞微秒延时
  * @param  None
  * @retval None
  */
void delay_us(u32 nus)
{	
	u32 sum = 0; //记录和
	u32 t1 = 0,t2 = 0;
	
	t1 = SysTick->VAL; //获取第1次val
	
	for(;;)
	{
		t2 = SysTick->VAL; //获取第2次val
		
		if(t1 != t2)
		{
			if(t1 > t2)
			{
				sum += t1 - t2;
			}
			else
			{
				sum += t1 + SysTick->LOAD - t2;
			}
			
			if(sum >= nus * 168)
				break;
				
			t1 = t2;
		}
	}
}

/**
  * @brief  非阻塞毫秒延时
  * @param  None
  * @retval None
  */
void delay_ms(u32 nms)
{
	while(nms--)
	{
		delay_us(1000);
	}
}

/**
  * @brief  非阻塞秒延时
  * @param  None
  * @retval None
  */
void delay_s(u32 ns)
{
	while(ns--)
	{
		delay_ms(1000);
	}
}

#endif
