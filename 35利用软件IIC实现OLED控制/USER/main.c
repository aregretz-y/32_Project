/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/11/12
  * @brief   这是利用软件IIC实现对OLEN的控制
  ******************************************************************************
  * @attention
	*					1.目前IIC引脚是：SDA 和 SCL
	*					2.SDA对应的引脚是PD6,SCL对应的引脚是PD7
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
#include "sys.h"
#include "oled.h"
#include "bmp.h"

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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//中断优先级分组，采用分组4则表示抢占优先级的范围是0~15，子优先级的值固定为0
  USART1_Config(115200); 	//PC
	
	OLED_Init();			//初始化OLED  
	OLED_Clear(); 
	
//	OLED_ShowCHinese(0,0,0);//我
//	OLED_ShowCHinese(18,2,1);//真
//	OLED_ShowCHinese(36,4,2);//的
//	OLED_ShowCHinese(54,6,5);//帅
//	
//	OLED_WR_Byte(0x26, OLED_CMD);        // 水平向右滚动
//	OLED_WR_Byte(0x00, OLED_CMD);        // 虚拟字节（固定为0x00）
//	OLED_WR_Byte(0x00, OLED_CMD);        // 起始页：页0
//	OLED_WR_Byte(0x07, OLED_CMD);        // 滚动速度：帧率2（可改）
//	OLED_WR_Byte(0x07, OLED_CMD);        // 结束页：页7（整个屏幕）
//	OLED_WR_Byte(0x00, OLED_CMD);        // 虚拟字节（固定为0x00）
//	OLED_WR_Byte(0xFF, OLED_CMD);        // 虚拟字节（固定为0xFF）
//	OLED_WR_Byte(0x2F, OLED_CMD);        // 激活滚动
//	

//	//设置亮度,采用双指令0x81 + 
//	OLED_WR_Byte(0x81, OLED_CMD);				//固定0x81
//	OLED_WR_Byte(0xFF, OLED_CMD);				//最亮 
//	delay_ms(5000);
//	OLED_WR_Byte(0x81, OLED_CMD);				//固定0x81
//	OLED_WR_Byte(0x00, OLED_CMD);				//最暗

  for(;;)
  {
		OLED_DrawBMP(0,0,128,8,BMP1);
		delay_ms(5);
//		OLED_Clear(); 
		OLED_DrawBMP(0,0,128,8,BMP2);
		delay_ms(5);
//		OLED_Clear(); 
		OLED_DrawBMP(0,0,128,8,BMP3);
		delay_ms(5);
//		OLED_Clear(); 
		OLED_DrawBMP(0,0,128,8,BMP4);
		delay_ms(5);
//		OLED_Clear(); 
		OLED_DrawBMP(0,0,128,8,BMP5);
		delay_ms(5);
//		OLED_Clear(); 
		OLED_DrawBMP(0,0,128,8,BMP6);
		delay_ms(5);
//		OLED_Clear(); 
	}

}






/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
