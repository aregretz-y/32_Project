#ifndef __LIGHT_H
#define __LIGHT_H

#include "stm32f4xx.h"
#include "delay.h"
#include "sys.h"

extern u16 ADC3_Value[2];//全局变量声明不能初始化

void ADC3_IN5Init(void);
void ADC1_IN5Init(void);
#endif
