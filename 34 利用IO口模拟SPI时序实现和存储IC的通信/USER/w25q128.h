#ifndef __W25Q128_H__
#define __W25Q128_H__

#include "stm32f4xx.h"  //用户必须包含
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "delay.h"

#define sFLASH_SPI_PAGESIZE 256	

#define W25Q128_SCK_PORT   GPIOB
#define W25Q128_SCK_RCC    RCC_AHB1Periph_GPIOB
#define W25Q128_SCK_PIN    GPIO_Pin_3

#define W25Q128_MOSI_PORT  GPIOB
#define W25Q128_MOSI_RCC   RCC_AHB1Periph_GPIOB
#define W25Q128_MOSI_PIN   GPIO_Pin_5

#define W25Q128_MISO_PORT  GPIOB
#define W25Q128_MISO_RCC   RCC_AHB1Periph_GPIOB
#define W25Q128_MISO_PIN   GPIO_Pin_4

#define W25Q128_CS_PORT    GPIOB
#define W25Q128_CS_RCC     RCC_AHB1Periph_GPIOB
#define W25Q128_CS_PIN     GPIO_Pin_14


#define W25Q128_SCK(n)		 (n)?GPIO_SetBits(W25Q128_SCK_PORT,W25Q128_SCK_PIN):GPIO_ResetBits(W25Q128_SCK_PORT,W25Q128_SCK_PIN)
#define W25Q128_MOSI(n)		 (n)?GPIO_SetBits(W25Q128_MOSI_PORT,W25Q128_MOSI_PIN):GPIO_ResetBits(W25Q128_MOSI_PORT,W25Q128_MOSI_PIN)
#define W25Q128_CS(n)		   (n)?GPIO_SetBits(W25Q128_CS_PORT,W25Q128_CS_PIN):GPIO_ResetBits(W25Q128_CS_PORT,W25Q128_CS_PIN)

#define W25Q128_MISO_READ() GPIO_ReadInputDataBit(W25Q128_MISO_PORT,W25Q128_MISO_PIN)

/**
  * @brief  串行Flash芯片的初始化
  * @param  None
  * @retval None
  */
void W25Q128_Config(void);

/**
	* @brief  W25Q128发送字节
  * @param  None
	* @retval 返回的是从机发送的内容
  * @note   采用的SPI接口,是同步通信,打算采用模式0,MSB格式
  */
uint8_t W25Q128_SendByte(uint8_t byte);

/**
  * @brief  W25Q128读取ID
  * @param  None
  * @retval 返回厂商ID和设备ID  0xEF17
  */
uint16_t W25Q128_ReadID(void);

/**
  * @brief  W25Q128写使能
  * @param  None
  * @retval None
  */
void W25Q128_WriteEnable(void);

/**
  * @brief  W25Q128写失能
  * @param  None
  * @retval None
  */
void W25Q128_WriteDisable(void);

/**
  * @brief  W25Q128读取状态寄存器
  * @param  None
  * @retval None
  * @note   BUSY位置位表示忙碌，复位表示空闲
  */
void W25Q128_WaitForWriteEnd(void);

/**
  * @brief  W25Q128扇区擦除
  * @param  SectorAddr:待擦除的扇区地址
  * @retval None
  */
void W25Q128_SectorEarse(uint32_t SectorAddr);

/**
  * @brief  W25Q128读取数据
  * @param  pBuffer: 接收缓冲区
  * @param  ReadAddr: 指的是待读取的地址
  * @param  NumByteToRead: 待读取的字节数量
  * @retval None
  */
void W25Q128_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

/**
  * @brief  W25Q128页编程
  * @note   待写入的字节数量不能超过一页大小(256byte)
  * @param  pBuffer:数据缓冲区，存储待写入的数据
  * @param  WriteAddr: 指的是待写入的Flash地址
  * @param  NumByteToWrite: 待写入的字节数量，必须等于或者小于一页大小
  * @retval None
  */
void W25Q128_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

/**
  * @brief  W25Q128写入一个缓存区数据，可以超过一页
  * @note   待写入的字节数量不能超过一页大小(256byte)
  * @param  pBuffer:数据缓冲区，存储待写入的数据
  * @param  WriteAddr: 指的是待写入的Flash地址
  * @param  NumByteToWrite: 待写入的字节数量
  * @retval None
  */
	void W25Q128_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

#endif

