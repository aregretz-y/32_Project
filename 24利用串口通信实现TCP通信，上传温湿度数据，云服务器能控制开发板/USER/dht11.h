#ifndef __DHT11_H__
#define __DHT11_H__

#include "stm32f4xx.h"  //用户必须包含
#include "delay.h"
#include <stdbool.h>
#include <stdio.h>

#define DHT11_PIN  GPIO_Pin_9
#define DHT11_PORT GPIOG


#define DHT11_SET(n) (n)? GPIO_SetBits(DHT11_PORT,DHT11_PIN):GPIO_ResetBits(DHT11_PORT,DHT11_PIN)
#define DHT11_READ()  GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)


/**
	* @brief  初始化GPIOG--PG9为输出模式
	* @param  None
	* @retval None
	*/
void DHT11_Config(void);

/**
	* @brief  设置引脚工作模式
	* @param  GPIO_Mode：指的是要选择的工作模式
	* @retval None
	*/
void DHT11_PinMode(GPIOMode_TypeDef GPIO_Mode);

/**
	* @brief  发出起始信号
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
  * @brief  获取一个bit数据
  * @param  None
  * @retval uint8_t
  */
uint8_t DHT11_ReadBit(void);

/**
  * @brief  获取一个字节数据
  * @param  None
  * @retval uint8_t
  */
uint8_t DHT11_ReadByte(void);

/**
	* @brief  获取DHT11发送过来的数据
	* @param  dhtbuf:指的是存储数据的数组
  * @retval void
	*/
bool DHT11_GetVal(uint8_t *dhtbuf);


#endif
