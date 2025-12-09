#ifndef __IIC_H__
#define __IIC_H__

#include "stm32f4xx.h"  //用户必须包含
#include "delay.h"
#include <stdbool.h>

#define IIC_SDA_PORT   GPIOD
#define IIC_SDA_RCC    RCC_AHB1Periph_GPIOD
#define IIC_SDA_PIN    GPIO_Pin_7

#define IIC_SCL_PORT  GPIOD
#define IIC_SCL_RCC   RCC_AHB1Periph_GPIOD
#define IIC_SCL_PIN   GPIO_Pin_6

#define IIC_SDA(n)		 (n)?GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN):GPIO_ResetBits(IIC_SDA_PORT,IIC_SDA_PIN)
#define IIC_SCL(n)		 (n)?GPIO_SetBits(IIC_SCL_PORT,IIC_SCL_PIN):GPIO_ResetBits(IIC_SCL_PORT,IIC_SCL_PIN)

#define IIC_SDA_READ() GPIO_ReadInputDataBit(IIC_SDA_PORT,IIC_SDA_PIN)

/**
	* @brief  IIC引脚初始化
	* @param  None
	* @retval None
	*/
void IIC_Config(void);

/**
	* @brief  设置引脚工作模式
	* @param  GPIO_Mode：指的是要选择的工作模式
	* @retval None
	*/
void IIC_SDAPinMode(GPIOMode_TypeDef GPIO_Mode);


/**
  * @brief  IIC的开始信号
  * @param  None
  * @retval None
  */
void IIC_Start(void);

/**
  * @brief  IIC发送数据
  * @param  uint8_t 指的是待发送的字节   
  * @retval None
  */
void IIC_Send(uint8_t byte);

/**
  * @brief  IIC判断从机应答
  * @param  None   
  * @retval true 表示应答 false 表示不应答
  */
bool IIC_IsACK(void);

/**
  * @brief  IIC的停止条件
  * @param  None
  * @retval None
  */
void IIC_Stop(void);

#endif



