#include "delay.h"


//us有上限 <=798915us
void delay_us(u32 tim_us)
{
	//选择SysTick时钟源--选择21Mhz  1/21us 21个脉冲刚好1us
	SysTick->CTRL &= ~(0x01<<2);

	//设置LOAD值
	SysTick->LOAD = tim_us*21-1;
	
	//清零VAL，LOAD会自动装载进VAL 这一步操作还可以清标志位
	SysTick->VAL = 0;
	
	//开启SysTick计数器
	SysTick->CTRL |= 0x01;
	
	//开始等待时间到了
	while(1)
	{
		if(SysTick->CTRL & (0x01<<16))
		{
			//能进来表示第16号位为1--时间到了
			break;
		}
	}
	
	//最好结束时关闭计数器
	SysTick->CTRL &= ~0x01;
}

//u8 a = 256;  1 0000 0000


//完成ms级别精准延时 <= 798ms
void delay_ms(u32 tim_ms)
{
	//选择SysTick时钟源--选择21Mhz  1/21us 21个脉冲刚好1us
	SysTick->CTRL &= ~(0x01<<2);

	//设置LOAD值
	SysTick->LOAD = tim_ms*21000-1;
	
	//清零VAL，LOAD会自动装载进VAL 这一步操作还可以清标志位
	SysTick->VAL = 0;
	
	//开启SysTick计数器
	SysTick->CTRL |= 0x01;
	
	//开始等待时间到了
	while(1)
	{
		if(SysTick->CTRL & (0x01<<16))
		{
			//能进来表示第16号位为1--时间到了
			break;
		}
	}
	
	//最好结束时关闭计数器
	SysTick->CTRL &= ~0x01;
	
}

//完成s级别精准延时---无上限
void delay_s(u32 tim_s)
{
	while(tim_s--)
	{
		delay_ms(500);
		delay_ms(500);
	}
}



