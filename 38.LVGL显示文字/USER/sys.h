#ifndef __SYS_H
#define __SYS_H
#include "stm32f4xx.h"

#define PAout(n) *(unsigned int *)((((GPIOA_BASE-0x40000000)+0x14)*8+n)*4+0x42000000)
#define PBout(n) *(unsigned int *)((((GPIOB_BASE-0x40000000)+0x14)*8+n)*4+0x42000000)
#define PCout(n) *(unsigned int *)((((GPIOC_BASE-0x40000000)+0x14)*8+n)*4+0x42000000)
#define PDout(n) *(unsigned int *)((((GPIOD_BASE-0x40000000)+0x14)*8+n)*4+0x42000000)
#define PEout(n) *(unsigned int *)((((GPIOE_BASE-0x40000000)+0x14)*8+n)*4+0x42000000)	
#define PFout(n) *(unsigned int *)((((GPIOF_BASE-0x40000000)+0x14)*8+n)*4+0x42000000)
#define PGout(n) *(unsigned int *)((((GPIOG_BASE-0x40000000)+0x14)*8+n)*4+0x42000000)
#define PHout(n) *(unsigned int *)((((GPIOH_BASE-0x40000000)+0x14)*8+n)*4+0x42000000)
#define PIout(n) *(unsigned int *)((((GPIOI_BASE-0x40000000)+0x14)*8+n)*4+0x42000000)

#define PAin(n) *(unsigned int *)((((GPIOA_BASE-0x40000000)+0x10)*8+n)*4+0x42000000)
#define PBin(n) *(unsigned int *)((((GPIOB_BASE-0x40000000)+0x10)*8+n)*4+0x42000000)
#define PCin(n) *(unsigned int *)((((GPIOC_BASE-0x40000000)+0x10)*8+n)*4+0x42000000)
#define PDin(n) *(unsigned int *)((((GPIOD_BASE-0x40000000)+0x10)*8+n)*4+0x42000000)
#define PEin(n) *(unsigned int *)((((GPIOE_BASE-0x40000000)+0x10)*8+n)*4+0x42000000)	
#define PFin(n) *(unsigned int *)((((GPIOF_BASE-0x40000000)+0x10)*8+n)*4+0x42000000)
#define PGin(n) *(unsigned int *)((((GPIOG_BASE-0x40000000)+0x10)*8+n)*4+0x42000000)
#define PHin(n) *(unsigned int *)((((GPIOH_BASE-0x40000000)+0x10)*8+n)*4+0x42000000)
#define PIin(n) *(unsigned int *)((((GPIOI_BASE-0x40000000)+0x10)*8+n)*4+0x42000000)


#endif
