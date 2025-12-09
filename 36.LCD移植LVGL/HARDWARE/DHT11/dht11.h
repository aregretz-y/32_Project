#ifndef __DHT11_H
#define __DHT11_H


#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include <stdio.h>

extern unsigned char TH_RH[5];

int DHT11_Read(void);

#endif

