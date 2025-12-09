#ifndef __SPI_H
#define __SPI_H

#include "stm32f4xx.h"
#include "delay.h"
#include "sys.h"

#define SPI_SCK   PBout(7)
#define SPI_MOSI  PAout(4)
#define SPI_MISO  PAin(2)//OLED这个引脚未引出 不管
#define SPI_CS    PCout(9)
#define OLED_DC   PCout(7)
#define OLED_RES  PGout(15) 

void SPI_GPIOInit(void); 
u8  SPI0_ReadWriteByte(u8 WData);

#endif
