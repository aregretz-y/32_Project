#ifndef __DHT11_H__
#define __DHT11_H__

#include "stm32f4xx.h"  //用户必须包含
#include "delay.h"
#include <stdbool.h>
#include <stdio.h>


#define DHT11_PIN	  GPIO_Pin_9
#define DHT11_PORT  GPIOG


#define DHT11_SET(n)  (n)?GPIO_SetBits(DHT11_PORT,DHT11_PIN):GPIO_ResetBits(DHT11_PORT,DHT11_PIN)
#define DHT11_READ()  GPIO_ReadInputDataBit(DHT11_PORT,DHT11_PIN)

/**
  * @brief  DHT11的初始化
  * @param  None
  * @retval None
  */
void DHT11_Config(void);

/**
  * @brief  DHT11引脚模式设置
  * @param  GPIO_Mode：引脚模式
  * @retval None
  */
void DHT11_PinMode(GPIOMode_TypeDef GPIO_Mode);


/**
  * @brief  DHT11的开始信号
  * @param  None
  * @retval None
  */
void DHT11_Start(void);

/**
  * @brief  DHT11的响应信号
  * @param  None
  * @retval true 表示应答了  false 表示未应答
  */
bool DHT11_IsACK(void);

/**
  * @brief  DHT11的读取bit
  * @param  None
  * @retval 读取的bit的值 0000000 1 -- 0x01  0000000 0 -- 0x00
  */
uint8_t DHT11_ReadBit(void);

/**
  * @brief  DHT11的读取字节
  * @param  None
  * @retval 读取的字节
  */
uint8_t DHT11_ReadByte(void);

/**
  * @brief  DHT11的读取字节
  * @param  dhtbuf：用于存储数据的数组地址
  * @retval true 表示应答了  false 表示未应答
  */
bool DHT11_GetVal(uint8_t * dhtbuf);


#endif
