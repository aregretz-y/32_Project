#ifndef __IIC_H
#define __IIC_H

#include "stm32f4xx.h"
#include "delay.h"
#include "sys.h"

void IIC_GPIOInit(void);

void IIC_Start(void);
void IIC_Stop(void);
void IIC_WriteByte(u8 WData);
u8 IIC_ReadByte(void);
u8 IIC_ReadAck(void);
void IIC_WriteAck(u8 Ack);

#endif

